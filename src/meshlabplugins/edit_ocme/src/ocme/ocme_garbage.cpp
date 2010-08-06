#include "ocme.h"
#include "multiple_std_iterator.h"


void OCME::RemoveDeletedFaces(  std::vector<Cell*> & cells){
	std::vector<Cell*>::iterator ci;
	for(ci = cells.begin(); ci != cells.end(); ++ci){
		MarkTouched((*ci)->key);
		(*ci) ->ecd->deleted_face.SetAsVectorOfMarked();	
		(*ci)->face->Compact( (*ci)->ecd->deleted_face.marked_elements);
		(*ci) ->ecd->deleted_face.Clear();	
	}
}


void OCME::RemoveDeletedVertices(    std::vector<Cell*>  cs){

	/* 
	NOTE: this function as it is works on the assumption that the dependent cells of 
	the set cs that DO NOT belong to cs DO NOT have deleted faces or vertices.
	This is guaranteed when the function is called at the end of Commit (see ocme_commit.h)
	but not in general (at the moment of this writing there are no other calls)
	*/
	std::vector<Cell*>   dep_cells;
	std::vector<Cell*>::const_iterator ci;

#ifdef _DEBUG
	kernelSetMark++;								// UGLY**FACTORIZE	
	for(   ci = cs.begin(); ci != cs.end(); ++ci ) {
		(*ci)->ecd->is_in_kernel = FBool(&kernelSetMark);// UGLY**FACTORIZE	
		(*ci)->ecd->is_in_kernel = true;
	}
	ComputeDependentCells(cs,dep_cells);
	CheckDependentSet(dep_cells);
#endif


	// set the cells in cs as the one of the kernel (they will be added with the dependent ones below)
	kernelSetMark++;								// UGLY**FACTORIZE	
	for(   ci = cs.begin(); ci != cs.end(); ++ci ) {
		(*ci)->ecd->is_in_kernel = FBool(&kernelSetMark);// UGLY**FACTORIZE	
		(*ci)->ecd->is_in_kernel = true;
	}
	ComputeDependentCells(cs,dep_cells);


#ifdef _DEBUG
CheckDependentSet(dep_cells);
#endif
for(ci  = cs.begin(); ci != cs.end(); ++ci){
    this->CheckExternalRefAreExternal(*ci);
    this->CheckExternalsCoherent(*ci);
    }

	// Add to cs the dependent cells (and remove duplicated)
	cs.insert(cs.end(),dep_cells.begin(),dep_cells.end());
	RemoveDuplicates(cs); // this should not be necessary ( a cell is implicitly dependent on itself)

	lgn->Append("phase 1");

#ifdef _DEBUG
for(ci  = cs.begin(); ci != cs.end(); ++ci){
this->CheckExternalRefAreExternal(*ci);
this->CheckExternalsCoherent(*ci);
}
#endif

	/// Phase 1. a) Propagate the information on deleted vertices 
	///			 b) Build a remapping for all the cells

	/* (a)
		During the Commit operation, a vertex can be deleted but other cells
		may refere to it because is external in those cells. In this cycle
		we make sure that also the deleted vertices that are externals are marked. 
	*/

	/*	(b)
		for each cell keeps a vector that tells  where 
		the element (the vertex) [i] is gone after the vector
		of elements (vertices) has been compacted. 
	*/

	std::map<Cell*,std::vector<unsigned int> * > remap_v,remap_e;
	std::map<Cell*,std::vector<GIndex> * > new_exts;
	typedef std::map<Cell*,std::vector<unsigned int> * >::iterator RmIte;
	typedef std::map<Cell*,std::vector<GIndex> * >::iterator GiIte;
	typedef std::pair<RmIte,bool > RmIteBool;

	// set the access mode to the bool value for kernel cells and vector mode for non kernel
	for(   ci = cs.begin(); ci != cs.end(); ++ci ) 
		if( (*ci)->ecd->is_in_kernel())
			{
				(*ci) ->ecd->deleted_vertex.SetAsVectorOfBool();	
				(*ci) ->ecd->deleted_face.SetAsVectorOfBool();
				(*ci) ->ecd->deleted_externals.SetAsVectorOfBool();
			}
			else
			{
				(*ci) ->ecd->deleted_vertex.SetAsVectorOfMarked();	
				(*ci) ->ecd->deleted_face.SetAsVectorOfMarked();	
				(*ci) ->ecd->deleted_externals.SetAsVectorOfMarked();
			}

		lgn->Append("phase 1a");

	//(a)
	for(   ci = cs.begin(); ci != cs.end(); ++ci ){
		MarkTouched((*ci)->key);
		Cell * cell = (*ci);
		cell->ecd->deleted_face.SetAsVectorOfMarked();
		RAssert(cell->ecd->deleted_face.marked_elements.empty());

		this->CheckFaceVertDeletions(cell);	
#ifdef _DEBUG
this->CheckExternalRefAreExternal(*ci);
this->CheckExternalsCoherent(*ci);
#endif  

		Chain<OVertex> * vchain = cell->vert;

		RAssert(cell->ecd->is_in_kernel() ||( !cell->ecd->is_in_kernel() && cell->ecd->deleted_vertex.marked_elements.empty()));

		Cell * ext_cell;
		int ext_i;
		for(unsigned int vi = 0; vi < vchain->Size(); ++vi)
			if( (*vchain)[vi].isExternal)
			{
				 
				/* find where is the external vertex */
				unsigned int ei = (*vchain)[vi].GetIndexToExternal();

			    assert(ei < (*((*ci)->externalReferences)).Size());
				GIndex gi  = (*((*ci)->externalReferences))[ei];
				 
				this->FindExternalVertexCellIndex( gi ,ext_cell,ext_i);

#ifdef _DEBUG
if(ext_cell == cell)
        this->FindExternalVertexCellIndex( gi ,ext_cell,ext_i);
#endif
				if(ext_cell->ecd->is_in_kernel())						// is one of the cells passed as "cs" (otherwise we are not interested)
					if( ext_cell->ecd->deleted_vertex.IsMarked(ext_i))	// if the vertex is deleted
					{
						 cell->ecd->deleted_vertex.SetMarked(vi,true);	// mark it as deleted also in this cell
					}
			}
		this->CheckFaceVertDeletions(cell);	

	}
	 

	//(b)
	for(   ci = cs.begin(); ci != cs.end(); ++ci ){
		Cell * cell = (*ci);
		Chain<OVertex> * vchain = cell->vert;
		std::vector<unsigned int> * cell_remap = new std::vector<unsigned int> ();
		RmIteBool ite_bool = 
			remap_v.insert(std::pair<Cell*,std::vector<unsigned int>* >	( cell,cell_remap));

		cell->ecd->deleted_vertex.SetAsVectorOfMarked();

		std::sort(cell->ecd->deleted_vertex.marked_elements.begin(),cell->ecd->deleted_vertex.marked_elements.end());
		vchain->BuildRemap( cell->ecd->deleted_vertex.marked_elements, *cell_remap);			// build the remapping from the current 

#ifdef _DEBUG
		Chain<OFace> *   fchain = cell->face;

		this->CheckFaceVertDeletions(cell);	
		cell->ecd->deleted_vertex.SetAsVectorOfBool();
		cell->ecd->deleted_face.SetAsVectorOfBool();
		for(unsigned int fi = 0; fi < fchain->Size();++fi)
			if(!cell->ecd->deleted_face.IsMarked(fi))
		{
			OFace tmp = (*fchain)[fi];	
			RAssert(!cell->ecd->deleted_vertex.IsMarked(tmp[0]));
			RAssert(!cell->ecd->deleted_vertex.IsMarked(tmp[1]));
			RAssert(!cell->ecd->deleted_vertex.IsMarked(tmp[2]));
		}
		
		for(unsigned int ti = 0 ; ti < vchain->Size(); ++ ti)
			if(!cell->ecd->deleted_vertex.IsMarked(ti))
				{
					RAssert((*cell_remap)[ti]<vchain->Size());
					RAssert((*cell_remap)[ti]>=0);
				}
		
		cell->ecd->deleted_face.SetAsVectorOfMarked();
		cell->ecd->deleted_vertex.SetAsVectorOfMarked();
#endif
	}

	 

        /* Phase 2. Adjust all the references and compact external vertices
        / 		(a) adjust the references to the vertex from the faces
        / 		(b) compute which external references will be removed
        / 		(c) build the remapping of externals
        */
	RmIte  res;
	for( ci = cs.begin(); ci != cs.end(); ++ci){
		Cell * cell = *ci;

#ifdef _DEBUG
this->CheckExternalRefAreExternal(cell);
this->CheckExternalsCoherent(cell);
#endif

		Chain<OFace> *   fchain = cell->face;
		Chain<OVertex> * vchain = cell->vert;
		cell->ecd->deleted_externals.SetAsVectorOfBool();

		//(a)
		// find the remap_v vector
		res = remap_v.find(cell);
		assert(res!=remap_v.end());
		std::vector<unsigned int> & vec = *(*res).second;
		for(unsigned int fi = 0; fi < fchain->Size();++fi){
                        OFace   f = (*fchain)[fi];
			for(int vi = 0 ; vi < 3; ++vi){
                                (*fchain)[fi][vi] = vec[f[vi]];
                                RAssert((*fchain)[fi][vi]>=0);
                                RAssert((*fchain)[fi][vi]< vchain->Size());
			}
		}

#ifdef _DEBUG
this->CheckExternalRefAreExternal(cell);
this->CheckExternalsCoherent(cell);
#endif
		
		//(b)
		cell->ecd->deleted_vertex.SetAsVectorOfBool();
		cell->ecd->deleted_externals.SetAsVectorOfMarked();

		for( unsigned int vi = 0; vi < (*vchain).Size(); ++vi)
			if((*vchain)[vi].isExternal){
				if(cell->ecd->deleted_vertex.IsMarked(vi)) 
					cell->ecd->deleted_externals.SetMarked((*vchain)[vi].GetIndexToExternal(),true);
				}
		
		//(c) 
		std::vector<unsigned int> * cell_remap = new std::vector<unsigned int> ();
		RmIteBool ite_bool =  remap_e.insert(std::pair<Cell*,std::vector<unsigned int>* >	( cell,cell_remap));

		std::sort(cell->ecd->deleted_externals.marked_elements.begin(),cell->ecd->deleted_externals.marked_elements.end());
 		cell->externalReferences->BuildRemap(cell->ecd->deleted_externals.marked_elements,*cell_remap);

#ifdef _DEBUG
this->CheckExternalRefAreExternal(cell);
this->CheckExternalsCoherent(cell);
#endif

	}

		lgn->Append("phase 3a");

	/// Phase 3.  
	///		(a) build the new externalReferences
	///		(b) assign the new externalReferences
	///     (c) compact vert and externalReferences

	/// (a)
	for( ci = cs.begin(); ci != cs.end(); ++ci){
		std::vector<GIndex> * newExternals = new std::vector<GIndex>();
		(*newExternals).resize((*ci)->externalReferences->Size());
		new_exts.insert(std::pair<Cell*,std::vector<GIndex>* >	( *ci,newExternals));

		for(unsigned int ei = 0; ei < (*ci)->externalReferences->Size(); ++ei){
			GIndex gi = this->FindExternalVertexGIndex( (*(*ci)->externalReferences)[ei]);
			Cell*c = GetCell(gi.ck,false);
			assert(c);
			(*newExternals)[ei]  = gi ;
			res = remap_v.find(c);							 
                        if(res != remap_v.end())				// if the external in c have been remapped
				(*newExternals)[ei].i =(*(*res).second)[gi.i];	// update the position
		}
	}
		lgn->Append("phase 3bc");

	/// (b)
	for( ci = cs.begin(); ci != cs.end(); ++ci){
		Chain<OVertex> * vchain = (*ci)->vert;
		// copy the new values of the external references
		GiIte ne = new_exts.find(*ci);
		assert(ne!= new_exts.end());
		assert((*ci)->externalReferences->Size() == (*ci)->externalReferences->Size());
		for(unsigned int ei = 0; ei < (*ci)->externalReferences->Size(); ++ei){
			(*(*ci)->externalReferences)[ei] = (*(*ne).second)[ei];
		}
		// update the vertex references to the external references
		res = remap_e.find(*ci);
		assert(res != remap_e.end());
		const std::vector<unsigned int > & vec = *((*res).second);
		assert(vec.size() == (*ci)->externalReferences->Size());
		for(unsigned int vi = 0; vi < (*vchain).Size(); ++vi)
			if((*vchain)[vi].isExternal){
				((*vchain)[vi]).SetIndexToExternal(vec[((*vchain)[vi]).GetIndexToExternal()]);
			}

		///(c)
		// compact the external references
		(*ci)->ecd->deleted_externals.SetAsVectorOfMarked();
		(*ci)->externalReferences->Compact( (*ci)->ecd->deleted_externals.marked_elements);
		(*ci)->ecd->deleted_externals.Clear();

		// compact the vertices
		(*ci)->ecd->deleted_vertex.SetAsVectorOfMarked();
		(*ci)->vert->Compact( (*ci)->ecd->deleted_vertex.marked_elements);
		(*ci)->ecd->deleted_vertex.Clear();

		this->CheckVertexToExternalRef(*ci);
		}

		lgn->Append("phase 4");

        /* Phase 4. Adjust the GIndex in the external references of the cells
           in the dependent set of all the cells that have had their vertex conteiner compacted
        */

	std::map<Cell*,std::vector<unsigned int> * >::iterator mi;
	++kernelSetMark;
	for(   ci = cs.begin(); ci != cs.end(); ++ci ){
		(*ci)->ecd->is_in_kernel = FBool(&kernelSetMark);// UGLY**FACTORIZE	
		(*ci)->ecd->is_in_kernel = true;
	}
	
	ComputeDependentCells(cs,dep_cells);
	dep_cells.insert(dep_cells.end(),cs.begin(),cs.end());
	RemoveDuplicates(dep_cells);

	for(   ci = dep_cells.begin(); ci != dep_cells.end(); ++ci )
		if(!(*ci)->ecd->is_in_kernel())
		{
			assert(remap_v.find(*ci) == remap_v.end());
			Chain<GIndex> & er = (*(*ci)->externalReferences);
			for(unsigned int ei = 0; ei < er.Size(); ++ei){
				Cell * ec = GetCell(er[ei].ck,false);
				assert(ec);
				mi = remap_v.find(ec);
				if(mi != remap_v.end())
					er[ei].i = (*(*mi).second)[er[ei].i];
			}
		}

#ifdef _DEBUG
	for(   ci = cs.begin(); ci != cs.end(); ++ci ) 
		this->CheckVertexToExternalRef(*ci);

	this->CheckDependentSet(dep_cells);
	CellsIterator aci;
	for(aci = this->cells.begin(); aci != this->cells.end(); ++aci){
			Chain<GIndex> & er = (*((*aci).second)->externalReferences);
			for(unsigned int ei = 0; ei < er.Size(); ++ei){
				Cell * ec = GetCell(er[ei].ck,false);
				assert(ec);
				mi = remap_v.find(ec);
				if(mi != remap_v.end())
				{unsigned int  h = 0;
					for(; h < dep_cells.size(); ++h)
						if(dep_cells[h] == (*aci).second) break;
					assert(h <  dep_cells.size());
				}
			}
	}

	for(   ci = dep_cells.begin(); ci != dep_cells.end(); ++ci )
 		 	this->CheckExternalsCoherent(*ci);
 #endif
        lgn->Append("cleanup");

	// clean up temporary data
	for(mi = remap_v.begin(); mi != remap_v.end(); ++mi) 
		delete (*mi).second;
	for(mi = remap_e.begin(); mi != remap_e.end(); ++mi) 
		delete (*mi).second;

	GiIte gi_ite;
	for(gi_ite = new_exts.begin(); gi_ite != new_exts.end(); ++gi_ite)
		delete (*gi_ite).second;

	kernelSetMark++;								
		lgn->Append("end commit");


#ifdef _DEBUG
	for( ci = cs.begin(); ci != cs.end(); ++ci){
	this->CheckExternalsCoherent(*ci);
	this->CheckExternalRefAreExternal(*ci);
}
#endif

}

void OCME::RemoveUnreferencedExternal(  std::vector<Cell*> & cells){
	if(0) cells.clear();
	assert(0); // not implemented yet
}

void OCME::RemoveEmptyCells(  ){
		CellsIterator ci;
		for(ci = cells.begin(); ci != cells.end(); ++ci)
				if((*ci).second->IsEmpty())
						RemoveCell((*ci).second->key);
}

void OCME::RemoveCell(const CellKey & key){
	Cell * c = GetCell(key,false);
	assert(c!=NULL);
	assert(c->IsEmpty());

	std::map<std::string, ChainBase *  >::iterator ai;

	sprintf(lgn->Buf(),"cell %d %d %d %d",key.x,key.y,key.z,key.h);
	lgn->Push();

	// remove all the chains from OOCEnv
	lgn->Append("remove element");
	for(ai = c->elements.begin(); ai != c->elements.end(); ++ai )  
		 oce.RemoveChain(NameOfChain(key,(*ai).first));
	 
	lgn->Append("remove pva");
 	 for(ai = c->perVertex_attributes.begin(); ai != c->perVertex_attributes.end(); ++ai ) 
		oce.RemoveChain(NameOfChain(key,(*ai).first));

 	lgn->Append("remove pfa");
	 for(ai = c->perFace_attributes.begin(); ai != c->perFace_attributes.end(); ++ai ) 
 		oce.RemoveChain(NameOfChain(key,(*ai).first));

	 std::set<CellKey>::iterator di;
	 for(di = c ->dependence_set.begin(); di != c->dependence_set.end();++di){
		 Cell * dc = GetCell(*di,false);
		 RAssert(dc);
		 RAssert(dc->dependence_set.find(key)!=dc->dependence_set.end());
		 dc->dependence_set.erase(key);
	 }

 	lgn->Append("erase");
	cells.erase(key);		// remove from the multigrid map

	MarkRemoved(key);
	delete c;				// delete the data structure
}

