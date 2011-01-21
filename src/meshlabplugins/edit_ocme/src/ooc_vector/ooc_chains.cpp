#include "ooc_chains.h"

#ifdef SIMPLE_DB
#include "./io/ooc_chains.hpp"
#else
#include "./io/ooc_chains_kcdb.hpp"
#endif

/* -------------------------------------------------------------------------- */
/* ------------------------- OOCEnv implementation ----------------------------- */
/* -------------------------------------------------------------------------- */

void
OOCEnv::TrashEverything(){
	cache_policy->FlushAllChunks(false);
	Close(false);
}

void
OOCEnv ::InitDefaultNameTypeBinders(){
	this->AddNameTypeBound<float>("float");
	this->AddNameTypeBound<int>("int");
	this->AddNameTypeBound<char>("char");
	this->AddNameTypeBound<short>("short");
	this->AddNameTypeBound<double>("double");
	this->AddNameTypeBound<unsigned int>("unsigned int");
	this->AddNameTypeBound<unsigned short>("unsigned short");
	this->AddNameTypeBound<unsigned char>("unsigned char");
}


ChainBase * 
OOCEnv ::CreateFromString(const std::string & key, const std::string & type ){
	ChainBase * newchain;
	   BoundsIterator  i;
        for(i = binders.begin(); i != binders.end(); ++i)
			if((*i).second->Name() == type) {newchain = (*i).second->NewChain();break;}

	RAssert(i != binders.end()); // none of the admitted types

	newchain->Key () = key;
	newchain->Type() = type;
	newchain->CachePol() = this->cache_policy;
	newchain->ExtMemHnd() = this->extMemHnd;
	newchain->SetChunkSize(params.blockSizeBytes);
	return newchain;	 

}


ChainBase   *
OOCEnv  ::
GetChain( std::string name ){
	ChainIterator ci;
	ci = chains.find(name);
	if( ci != chains.end())
		return (*ci).second;

	return NULL; // it does not exists  
}

void
OOCEnv  ::
RemoveChain( const std::string  & name ){
	 ChainBase * c = GetChain(name);
	 RAssert( c != NULL);
	 c->RemoveAllChunks();		// remove all the chunks
	 chains.erase(name);	// remove the chain from the map of all the chains
	 c->PointedBy(-1);		// delete the datastructure
//	 SaveAT();				// save the updated allocation table
 }



void  OOCEnv ::
SaveData(){
	cache_policy->FlushAllChunks();
		//for( ChainIterator ci = chains.begin(); ci != chains.end(); ++ci)
		//(*ci).second->SaveData();		 
}


/*   SERIALIZATION  */
/* OOCEnv */
int  OOCEnv ::
SizeOfMem(){
	BoundsIterator bi;
	int siz = 0;
	siz += sizeof(Params);
        siz += sizeof(unsigned int); // number of binders
		for(bi = binders.begin(); bi != binders.end(); ++bi) siz +=  (*bi).second->SizeOfMem();
	siz += sizeof(int); // number of chains
	for( ChainIterator ci = chains.begin(); ci != chains.end(); ++ci)
		siz +=(*ci).second->SizeOfMem();
	return siz;
}

char *  OOCEnv ::
Serialize(char  * buffer){
		BoundsIterator bi;
        char * ptr = buffer;
        memcpy(ptr,&params,sizeof(Params) );ptr+=sizeof(Params) ;

        *(unsigned int*)ptr = binders.size(); ptr+=sizeof(unsigned int);
		for(bi = binders.begin(); bi != binders.end(); ++bi) ptr  =  (*bi).second->Serialize(ptr);

        unsigned int n_chain = (int)chains.size();
		memcpy(ptr,&n_chain,sizeof(int));ptr+=sizeof(int);
	
		for( ChainIterator ci = chains.begin(); ci != chains.end(); ++ci)
			ptr = (*ci).second->Serialize(ptr);
		return ptr;
}

char *  OOCEnv ::
DeSerialize(char  * buffer){
        unsigned int i;

	char * ptr = buffer;
	memcpy(&params,ptr,sizeof(Params) );ptr+=sizeof(Params) ;

        unsigned int n_binders  = *(unsigned int*)ptr; ptr+=sizeof(unsigned int);
        for(i = 0; i < n_binders; ++i) {
            NameUnknownTypeBind tb;
            ptr  =  tb.DeSerialize(ptr);
			AddNameUnknownTypeBound(tb);
        }

	int n_chain;
	memcpy(&n_chain,ptr,sizeof(int));ptr+=sizeof(int);

	std::string key,type;
	std::pair<ChainIterator,bool> ins;

	unsigned int mem; 
	for( int ic = 0; ic <n_chain;++ic){
		MemDbg::SetPoint(0);
		char * ptrB = ptr;																			// the pointer is saved in ptrB and then passed to DeSerialize
		ptr = ::DeSerialize(ptr,key);																// This is done to read "name" and "type" an to be able to create a chain
		ptr = ::DeSerialize(ptr,type);
		ins = chains.insert(std::pair<std::string,ChainBase*>(key,CreateFromString(key,type)));		// here the chain is allocated
		ptr = (*(ins.first)).second->DeSerialize(ptrB);												// Here DeSerialize with the saved poiner is called
		mem = MemDbg::MemFromPoint(0);
	}

	return ptr;
}

