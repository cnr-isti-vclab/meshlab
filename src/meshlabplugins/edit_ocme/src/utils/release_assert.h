#ifndef OCME_ASSERT
#define OCME_ASSERT
#include <assert.h>
#include <string>
#include "logging.h"


#ifdef _RELEASE_ASSERT_
#define ABC(x)   #x
#define RAssert(E)  {if(!(E)) { sprintf(lgn->Buf(),"%s failed !!",  ABC(E)  );lgn->Push();};}
#else
#define RAssert(E)   assert(E) 
#endif
#endif 
 
