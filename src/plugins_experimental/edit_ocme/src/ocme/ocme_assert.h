#ifndef OCME_ASSERT
#define OCME_ASSERT
#include "logging.h"
struct Assert{
	Assert(){AssertLevel() = 0;}
	static int & AssertLevel(  ){static int lev =0; return lev;}

	Assert(bool expr, int lev, char*msg){
			if(lev >= AssertLevel)
#ifndef NDEBUG
		assert(expr);
#else
		if(!expr){
			sprintf(lgn->Buf(),"%s failed",msg);
			lgn->Push();
		}

#endif
	}

};

#endif