#ifndef __TIMING__
#define __TIMING__

#include <vector>
#include <time.h>

struct TIM{

	static std::vector<unsigned int> &  T(){static std::vector<unsigned int> t; return   t;}
	static std::vector<unsigned int> &  ST(){static std::vector<unsigned int> st; return   st;}

	static void Begin(unsigned int   n){
		if(T().size() < n+1)  
		{T().resize(n+1); ST().resize(n+1); }
		 ST()[n] = clock();
		 
	}
	static void End(unsigned int p){T()[p]+=(clock()-ST()[p]);}
	static unsigned int Total(unsigned int p){return (p<T().size())?T()[p]:0;}

}; 

struct STAT{

    static std::vector<unsigned int> &  _V(){static std::vector<unsigned int> v; return   v;}

    static void Begin(unsigned int n){_V().resize(n);Reset();}
    static void Reset(){for(unsigned int i=0; i <_V().size();++i) _V()[i]=0;}

    static void Inc(unsigned int i){ ++(_V()[i]);}
    static unsigned int V(unsigned int i){return _V()[i];}
};

const unsigned int N_STAT = 7;
const unsigned int N_SAVEDCH= 0;
const unsigned int N_LOADEDCH= 1;
const unsigned int N_CREATEDCH= 2;
const unsigned int N_REMOVEDCH= 3;
const unsigned int N_ACCESSES = 4;
const unsigned int N_CLEANEDCH = 5;
const unsigned int N_SAVED_PART_CH = 6;


#endif
