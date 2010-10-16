#include "ocme_definition.h"

void OCME::Verify(){ 

	/* */
	CellsIterator ci;

	{
		lgn->Append("CHECK IF: ecd/rd exists for each cell");lgn->Push();
		for(ci = cells.begin(); ci != cells.end(); ++ci){
			RAssert( (*ci).second->ecd);
			RAssert( (*ci).second->rd);
		}
	}

	{
		lgn->Append("CHECK IF: load/unload work");lgn->Push();
		for(ci = cells.begin(); ci != cells.end(); ++ci){
			Chain<OVertex> * vert = (*ci).second->vert;
			Chain<OFace> * face = (*ci).second->face;
			vert->LoadAll();
			face->LoadAll();
			vert->FreeAll();
			face->FreeAll();
		}
	}

	{
		lgn->Append("CHECK IF: faces point to vertex inside the cells");lgn->Push();
		for(ci = cells.begin(); ci != cells.end(); ++ci){
			Chain<OVertex> * vert = (*ci).second->vert;
			Chain<OFace> * face = (*ci).second->face;

			for(unsigned int fi = 0 ; fi < face->Size();++fi)
				for(unsigned int vi = 0 ; vi <3;++vi) 
					RAssert((*face)[fi][vi] < vert->Size());
		}
	}

	{
		lgn->Append("CHECK IF: dependent cells exist and dependence is symmetric");lgn->Push();
		std::set<CellKey>::iterator cki;
		for(ci = cells.begin(); ci != cells.end(); ++ci)
			for(cki = (*ci).second->dependence_set.begin(); cki != (*ci).second->dependence_set.end(); ++cki)
				{	
					Cell *  c = GetCell((*cki),false);		
					if(!c){
						sprintf(lgn->Buf(),"%d %d %d %d does not exist",(*cki).x,(*cki).y,(*cki).z,(*cki).h);
						lgn->Push();
					}else
					{
						if(c->dependence_set.find((*ci).second->key)== c->dependence_set.end())
							sprintf(lgn->Buf(),"%d %d %d %d Asymmetric",(*cki).x,(*cki).y,(*cki).z,(*cki).h);
					}


				}
	}

	{
		lgn->Append("CHECK IF: pointers to boundary (uniquely) refer to existing vertices");lgn->Push();
		std::set<CellKey>::iterator cki;
		for(ci = cells.begin(); ci != cells.end(); ++ci){
			std::set<unsigned int> bv;
			Chain<OVertex> * vert = (*ci).second->vert;
			Chain<BorderIndex > * border = (*ci).second->border;
			for(unsigned int bi = 0 ; bi < border->Size();++bi)
				{	
					if((*border)[bi].vi > vert->Size()){
						sprintf(lgn->Buf(),"(vi,bi)= (%d,%d) , vs = %d",(*border)[bi].vi,(*border)[bi].bi,vert->Size());
						lgn->Push();
					}
					
					std::pair<std::set<unsigned int>::iterator, bool > res =  bv.insert((*border)[bi].vi);

					if(!res.second){
						Cell * c = (*ci).second;
						sprintf(lgn->Buf(),"%d %d %d %d :: ii %d,  vi %d , bv.size() %d",
							c->key.x,c->key.y,c->key.z,c->key.h,
							bi,(*border)[bi].vi,bv.size());
						lgn->Push();

					}
				}
		}
	}


}

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

bool OCME::CheckFaceVertexAdj(Cell *c){
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

bool OCME:: CheckDependentSet(std::vector<Cell*> &  ){return true;

}

bool OCME:: BorderExists(Cell* c,unsigned int vi){
	for(unsigned int vii = 0; vii < c->border->Size(); ++vii)
		if((*c->border)[vii].vi == vi) 
			return  true;
	return false;
}
