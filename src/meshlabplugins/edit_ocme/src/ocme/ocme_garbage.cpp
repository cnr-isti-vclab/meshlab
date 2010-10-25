#include "ocme.h"
#include "multiple_std_iterator.h"


void OCME::RemoveDeletedFaces(  std::vector<Cell*> & cucells){
	std::vector<Cell*>::iterator ci;
	for(ci = cucells.begin(); ci != cucells.end(); ++ci){
		MarkTouched((*ci)->key);
		(*ci) ->ecd->deleted_face.SetAsVectorOfMarked();

		/* remove the faces (indices) */
		(*ci)->face->Compact( (*ci)->ecd->deleted_face.marked_elements);

		/* remove corresponding per face attribute (color, normal, whatever... */
		Cell::StringChainMap::iterator fai;
		for(fai = (*ci)->perFace_attributes.begin(); fai != (*ci)->perFace_attributes.end(); ++fai)
			(*fai).second->Compact((*ci)->ecd->deleted_face.marked_elements);

		(*ci) ->ecd->deleted_face.Clear();	
	}
}

void OCME::RemoveDeletedBorder(std::vector<Cell*> &  cucells){
	std::vector<Cell*>::iterator ci;
	for(ci = cucells.begin(); ci != cucells.end(); ++ci){
		/* here deleted_border contains the references to vertices that are no more border because 
		they are not referred in other cells. Now we add also those that have been removed */
		(*ci) ->ecd->deleted_border.SetAsVectorOfMarked();	
		(*ci) ->ecd->deleted_vertex.SetAsVectorOfBool();

		for(unsigned int i = 0; i < (*ci)->border->Size(); ++i)
			if( (*ci) ->ecd->deleted_vertex.IsMarked( (*(*ci)->border)[i].vi ))
				(*ci) ->ecd->deleted_border.SetMarked(i,true);

		(*ci)->border->Compact( (*ci)->ecd->deleted_border.marked_elements);
		(*ci) ->ecd->deleted_border.Clear();	
	}
}

void OCME::RemoveDeletedVertices(    std::vector<Cell*>  cs){
        std::vector<Cell*>::const_iterator ci;
        //
        for(ci = cs.begin(); ci != cs.end(); ++ci){
            std::vector<unsigned int>    remap;
            Chain<BorderIndex> * bchain = (*ci)->border;
            Chain<OVertex> * vchain = (*ci)->vert;
            Chain<OFace> * fchain = (*ci)->face;

			(*ci)->ecd->deleted_vertex.SetAsVectorOfMarked();
            vchain->BuildRemap( (*ci)->ecd->deleted_vertex.marked_elements, remap);
            vchain->Compact((*ci)->ecd->deleted_vertex.marked_elements);

			/* remove corresponding per vertex attribute (color, normal, whatever... */
			Cell::StringChainMap::iterator vai;
			for( vai = (*ci)->perVertex_attributes.begin(); vai != (*ci)->perVertex_attributes.end(); ++vai)
				(*vai).second->Compact((*ci)->ecd->deleted_vertex.marked_elements);

            (*ci)->ecd->deleted_vertex.Clear();

            for(unsigned int fi = 0; fi < fchain->Size(); ++fi)
                for(unsigned int  i = 0;  i < 3 ; ++ i){
                    int ind = (*fchain)[fi][i];
                    assert(remap[ind] < vchain->Size());
                    (*fchain)[fi][i] = remap[(*fchain)[fi][i]];
				}
            for(unsigned int ii = 0; ii < bchain->Size(); ++ii)
                    (*bchain)[ii].vi = remap[(*bchain)[ii].vi];
		}
}


void OCME::RemoveEmptyCells(  ){
                std::vector<Cell*> toremove;
		CellsIterator ci;
		for(ci = cells.begin(); ci != cells.end(); ++ci)
				if((*ci).second->IsEmpty())
                                    toremove.push_back((*ci).second);
                for(unsigned int i = 0; i < toremove.size(); ++i)
                    RemoveCell(toremove[i]->key);
}

void OCME::RemoveCell(const CellKey & key){
	Cell * c = GetCell(key,false);
	RAssert(c!=NULL);
	RAssert(c->IsEmpty());

	std::map<std::string, ChainBase *  >::iterator ai;

	//sprintf(lgn->Buf(),"removing cell %d %d %d %d",key.x,key.y,key.z,key.h);
	//lgn->Push();

	// remove all the chains from OOCEnv
	for(ai = c->elements.begin(); ai != c->elements.end(); ++ai )  
		 oce.RemoveChain(NameOfChain(key,(*ai).first));
	 
 	 for(ai = c->perVertex_attributes.begin(); ai != c->perVertex_attributes.end(); ++ai ) 
		oce.RemoveChain(NameOfChain(key,(*ai).first));

	 for(ai = c->perFace_attributes.begin(); ai != c->perFace_attributes.end(); ++ai ) 
 		oce.RemoveChain(NameOfChain(key,(*ai).first));

	 std::set<CellKey>::iterator di;
	 for(di = c ->dependence_set.begin(); di != c->dependence_set.end();++di){
		 Cell * dc = GetCell(*di,false);
		 if(dc){
			 RAssert(dc->dependence_set.find(key)!=dc->dependence_set.end());
			 dc->dependence_set.erase(key);
		 }
	 }

	cells.erase(key);		// remove from the multigrid map

	MarkRemoved(key);
	delete c;				// delete the data structure
}

