#ifndef __OOC_VECTOR__
#define __OOC_VECTOR__

#include "ooc_chains.h"
#include "./berkeleydb/ooc_chains_berkeleydb.hpp"


template <class T>
struct ooc_vector{
private:
	// pointer to the chain
	Chain<T> * chain;
	OOCEnv * env;

public:
	
	// here the difference with a std::vector. The destruction of the ooc_vector object does not
	// actually destroy the Chain object, because it is just a handle to it.
	~ooc_vector(){}

	/// create or reassign the ooc_vector
	void assign(OOCEnv & _env,    char *   name, bool ifnotexists_create = true){

		env = & _env;
		chain = _env.GetChain<T>(name,ifnotexists_create);
	}

	/// actually destroy the ooc_vector. The data will be lost forever
	void destroy(){RAssert(env);RAssert(chain); env->RemoveChain(chain->key);}

	/// resize
	const void resize(const unsigned int & sz){chain->Resize(sz);}

	/// returns the size of the vector
	const  unsigned int  size() const { RAssert(chain);return chain->Size();}

	/// access to the i-th element of the vector
	T & operator [](const int & i) {RAssert(chain);return (*chain)[i];};  

	/// erase the elements corresponding to the indexes passed
	void erase(std::vector<unsigned int> &deleted){RAssert(chain);chain->Compact(deleted);}

	/// add an element to the back of the vector
	void push_back(const T  & elem){ RAssert(chain); chain->AddElem(elem);}

};


#endif
