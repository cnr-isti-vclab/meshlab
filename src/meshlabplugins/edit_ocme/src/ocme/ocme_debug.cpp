#include "ocme_definition.h"

bool OCME::CheckFaceVertDeletions(Cell *c){ return true;

	c->ecd->deleted_face.SetAsVectorOfBool();
	c->ecd->deleted_vertex.SetAsVectorOfBool();
	for(unsigned int fi = 0; fi < c->face->Size(); ++fi)
		if(!c->ecd->deleted_face.IsMarked(fi))
			{
				OFace of = (*(c->face))[fi];
				for(unsigned int vpi = 0; vpi < 3; ++vpi)
					RAssert(!c->ecd->deleted_vertex.IsMarked(of[vpi]));
			}
	return true;
}

bool OCME::CheckFaceVertexAdj(Cell *c){return true;

	for(unsigned int fi = 0; fi < c->face->Size(); ++fi){

		OFace of = (*(c->face))[fi];
 
		for(unsigned int vpi = 0; vpi < 3; ++vpi){
 			if( (*(c->vert)).Size() <=  of[vpi])	
			{ 
				sprintf(lgn->Buf(),"wrong face-vert adj  cell: %d %d %d %d", c->key);lgn->Push(); 
				sprintf(lgn->Buf(),"wrong face-vert adj  face: %d", fi);lgn->Push(); 
				sprintf(lgn->Buf(),"wrong face-vert adj  ov[]: %d %d %d ",of[0],of[1],of[2]);lgn->Push(); 
				
				
				return false;}
//			if( 0 >  of[vpi] )						
//			{ lgn->Append("negative face-vert adj ");lgn->Push(); return false;}
		}
	}
	return true;
}

bool OCME::CheckExternalVertexAdj(Cell *c){ return true;
	Cell * cell;
	GIndex gi;
	 int id;
	for(unsigned int  vi = 0 ; vi < c->vert->Size(); ++vi){
		OVertex ov = (*(c->vert))[vi];
		if(ov.isExternal){
			RAssert(ov.GetIndexToExternal()>=0);
			gi = (*(c->externalReferences))[ov.GetIndexToExternal()];
			RAssert(gi.i>=0);
			this->FindExternalVertexCellIndex(gi,cell,id);
			RAssert(cell);
		}
	}
	
	return true;}

bool OCME::CheckVertexToExternalRef(Cell *c){ return true;
	GIndex gi;
	 std::vector<unsigned int> ref;
	 for(unsigned int  vi = 0; vi < c->vert->Size(); ++vi) {
		OVertex ov = (*(c->vert))[vi];
		if(ov.isExternal)
			ref.push_back( ov.GetIndexToExternal());
	 }
	std::sort(ref.begin(),ref.end());
	std::vector<unsigned int >::iterator  newEnd = std::unique(ref.begin(),ref.end());
	assert(newEnd ==ref.end());

	return true;
	
}

bool OCME::CheckExternalRefAreExternal(Cell *cell){ return true;
	GIndex gi;
	 std::vector<unsigned int> ref;
	 for(unsigned int  vi = 0; vi < cell->vert->Size(); ++vi) {
		 if((*(cell->vert))[vi].isExternal){
			assert((*(cell->vert))[vi].P()[1]==0.0);
			assert((*(cell->vert))[vi].P()[2]==0.0);
		 }
	 }
	 for(unsigned int  vi = 0; vi < cell->externalReferences->Size(); ++vi) {
		Cell * ext_cell;  int ext_i;OVertex v;
		 this->FindExternalVertexCellIndex( (*(cell->externalReferences))[vi] ,ext_cell,ext_i);
		 if( ext_cell == cell ){							// if it is external it cannot be the same cell
			 {this->FindExternalVertexCellIndex( (*(cell->externalReferences))[vi] ,ext_cell,ext_i);
			  RAssert(ext_cell != cell);	
                          cell->ecd->vert2externals.clear();
                          cell->ComputeVert2Externals();
                          std::map<unsigned int , unsigned int>::iterator ite = cell->ecd->vert2externals.find(vi);
			  v = (*ext_cell->vert)[ext_i];
                                assert (0);
				}
		 }
	 }
	 
	return true;
}

bool OCME:: CheckDependentSet(std::vector<Cell*> &  dep){return true;
	std::set<Cell*> depset;
	std::set<Cell*>::iterator id;
	for( std::vector<Cell*>::iterator i= dep.begin(); i != dep.end(); ++i)
		depset.insert(*i);

	for(CellsIterator mi = cells.begin(); mi != cells.end(); ++mi){
		Cell * c  =  (*mi).second;
		for(unsigned int ei = 0; ei < (*(c->externalReferences)).Size(); ++ ei ){
			Cell * ext = GetCell((*(c->externalReferences))[ei].ck,false);
			assert(ext);		
			if(ext->ecd->is_in_kernel()){
				id = depset.find(c);
				RAssert(id != depset.end());
				if(id == depset.end())
					return false;	// c has a reference to a kernel cell BUT it is not in the dependent set
			}
		}	
	}
	return true;
}

bool OCME:: CheckExternalsCoherent( Cell*c){return true;
c=c;
	//for(unsigned int i = 0; i < c->externalReferences->Size(); ++i){
	//	GIndex gi = (*(c->externalReferences))[i];
	//	vcg::Point3f p = gi.p;
	//	Cell* ec  = GetCell(gi.ck,false);
	//	assert(ec);
	//	vcg::Point3f p1 = (*(ec->vert))[gi.i].P();
	//	if ( p != p1 ){
	//		Cell * extcell;   int exti;
	//		this->FindExternalVertexCellIndex(gi,extcell,exti);
	//		vcg::Point3f p2 = (*(extcell	->vert ))[exti].P();
	//			if ( p == p2)
	//				return false;
	//		return false;
	//	}
	//}
	return true;
}
