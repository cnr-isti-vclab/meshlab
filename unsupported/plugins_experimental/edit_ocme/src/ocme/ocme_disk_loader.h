#ifndef _OCME_DISKLOADER_
#define _OCME_DISKLOADER_

#include "cell.h"


#include <cache/cache.h>
#include <cache/controller.h>

struct OCME;

/*
  A token is a whole cell (next version: tokens with finer granularity: single chunks of vertices)
  */
struct CellToken : public Token<float> {
	CellToken(CellKey _ck, float _priority):ck(_ck){this->setPriority(_priority);}
	CellKey ck;
	int sizeinRAM;
};

class RenderCache: public Cache<CellToken>{
public:	OCME * ocme;
};

class CellDisk: public RenderCache {
 public:
  int get(CellToken *token);
  int drop(CellToken *token);
  int size(CellToken *token) ;
};

class CellRAM: public RenderCache {
 public:
  int get(CellToken *token);
  int drop(CellToken *token);
  int size(CellToken *token) ;
};

class CellVideo: public RenderCache {
 public:
  int get(CellToken *token);
  int drop(CellToken *token);
  int size(CellToken *token);
};


class RenderCachesController {
public:
	RenderCachesController() ;
	~RenderCachesController() {};
	void Req(Cell * c , float priority);
	void Init(OCME * o){ cellRAM.ocme = cellVideo.ocme = o;}
	void Start(){controller.start();}
	void Finish();
	Controller<CellToken> controller;

	CellDisk cellDisk;
	CellRAM cellRAM;
	CellVideo cellVideo;

};


#endif
