#include "ocme_disk_loader.h"
#include "ocme_definition.h"
#include "ocme_extract.h"
#include <vcg/complex/algorithms/update/normal.h>


int CellDisk::get(CellToken * /*token*/){
	return 1;
}
int CellDisk::drop(CellToken * /*token*/){
	return 1;
}
int CellDisk::size(CellToken * /*token*/){
	return 1;
}

/* the problem with c->SizeInRAM is that it changes during a commit,
	 while it must be same when entering and exiting the cache
*/
int CellRAM::get(CellToken *token){
	Cell * c =  ocme->GetCell(token->ck);
	ocme->ExtractContainedFacesFromASingleCell(token->ck,c->rd->Mesh());
        vcg::tri::UpdateNormals<vcgMesh>::PerVertexPerFace(c->rd->Mesh());
	token->sizeinRAM =    c->SizeInRAM();
	return token->sizeinRAM;
}

int CellRAM::drop(CellToken *token){
	Cell * c =  ocme->GetCell(token->ck);
	c->rd->ClearMesh();
	return token->sizeinRAM;
}

int CellRAM::size(CellToken *token){
	Cell * c =  ocme->GetCell(token->ck);
	return c->SizeInRAM();
}

int CellVideo::get(CellToken * /*token*/){return 1;}
int CellVideo::drop(CellToken * /*token*/){return 1;}
int CellVideo::size(CellToken * /*token*/){return 1;}



RenderCachesController::RenderCachesController() {

	cellRAM.setCapacity((1<<20)*25);
	//cellRAM.setCapacity(cellRAM.ocme->renderParams.memory_limit_in_core);
//	cellVideo.setCapacity(cellVideo.ocme->renderParams.memory_limit_video);
//	cellDisk.setCapacity(100);
	cellVideo.setInputCache(&cellRAM);

	cellRAM.setInputCache(&cellDisk);

	cellRAM.setObjectName("cellRAM");
	cellVideo.setObjectName("cellVideo");

	controller.addCache(&cellDisk);

	controller.addCache(&cellRAM);
	//controller.addCache(&cellVideo);


}

void RenderCachesController::Req(Cell *c, float _priority){
	if(!c->rd->renderCacheToken){
		c->rd->renderCacheToken = new CellToken(c->key,_priority);
		controller.addToken(c->rd->renderCacheToken);
	}else
		c->rd->renderCacheToken->setPriority(_priority);
}

void RenderCachesController::Finish(){
		controller.finish();
		cellRAM.ocme->cells_to_render.clear();
		for(OCME::CellsIterator		ci  = cellRAM.ocme->cells.begin();
															ci != cellRAM.ocme->cells.end();
															++ci)
				if((*ci).second->rd->renderCacheToken!=0){
						delete (*ci).second->rd->renderCacheToken;
						(*ci).second->rd->renderCacheToken = 0;
				}
}
