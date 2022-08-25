#ifndef __MEMORY_DBG
#define __MEMORY_DBG

#include <vector>
#include "std_util.h"

#ifdef WIN32
	#define _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

//#define NO_BERKELEY
struct MemDbgBase{
	static bool CheckHeap(int level){level = 0;return true;};
};

#ifdef WIN32
struct MemDbg: public MemDbgBase{
	static int & Level() {static int   level = 0; return level;}
	static bool CheckHeap(int level){ 
                #ifdef WIN32
		return (level>Level())?(0!=_CrtCheckMemory()):true;
		#else
		return true;
		#endif
	}
	static void SetBreakAlloc(int value ){
                #ifdef WIN32
		_CrtSetBreakAlloc(value );
		#endif
		value = 0;}
	static void DumpMemoryLeaks(){
                #ifdef WIN32
		_CrtDumpMemoryLeaks();
		#endif
	}

	static std::vector<_CrtMemState> &  NV(){static std::vector<_CrtMemState> name_value;return   name_value;}

	static _CrtMemState & MemState(){static _CrtMemState cms; return cms; }
	static bool & All(){static bool allocated = false; return allocated;}
	static void SetPoint(unsigned int   n){
		n=n;
		if(!All())  NV().resize(128);
                #ifdef WIN32
		_CrtMemCheckpoint(& NV()[n]);
		#endif
	}
	static unsigned int MemFromPoint(unsigned int p){
                #ifdef WIN32
		_CrtMemCheckpoint(&MemState());
		#endif
		return MemState().lTotalCount -  NV()[p].lTotalCount;
	}
	static void End(){Delete(NV());}
};
#else
#define  _CrtMemState int
struct MemDbg: public MemDbgBase{
	static bool & All(){static bool allocated = false; return allocated;}
	static int & Level() {static int  level = 0; return level;}
	static bool CheckHeap(int level){ return true;level = 0; }
	static void SetBreakAlloc(int value ){value=0; }
	static void DumpMemoryLeaks(){ }
	static std::vector<rusage> &  NV(){static std::vector<rusage> name_value;return   name_value;}


static unsigned int MemFromPoint(unsigned int p){
		int res;
		rusage ru;
		res = getrusage(RUSAGE_SELF,&ru);
		return ru.ru_maxrss - NV()[p].ru_maxrss;
}

static void SetPoint(unsigned int   n){
		n=n;
		if(!All())  {NV().resize(128);All() = true;}
		int res;
		rusage ru;
		res = getrusage(RUSAGE_SELF,&ru);
		NV()[n].ru_maxrss = ru.ru_maxrss;
}
static void End(){Delete(NV());}

};
#endif

#endif
