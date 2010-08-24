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

bool OCME:: CheckDependentSet(std::vector<Cell*> &  dep){return true;

}
