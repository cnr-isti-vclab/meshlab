
#ifndef _CHAIN_MEM_SIMPLEDB_
#define _CHAIN_MEM_SIMPLEDB_

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "berkeleyDb.hpp"
#include "../chain_mem.h"

extern  SimpleDb  * simpledb;

template <  class  HandleChunkType> 
void OOCEnv<   HandleChunkType>:: 
Create( const char * name ){
	simpledb = new SimpleDb(std::string(""), std::string(name),true);
}

template <  typename   HandleChunkType> 
void OOCEnv<   HandleChunkType>:: 
Open( const char * name ){
	simpledb = new SimpleDb(std::string(""), std::string(name),false);
}


template <  typename   HandleChunkType> 
void OOCEnv<   HandleChunkType>:: 
Close( bool andsave){
	if(andsave){
		SaveAT();
		SaveData();
	}
	delete simpledb;
}


template <class TYPE> void Chain<TYPE>::
SaveChunk( typename Chain<TYPE>::Chunk & ck ){
	RAssert(ck.IsLoaded());
    simpledb->SaveSegment(ck.handle.key,(char*)ck.buffer,sizeof(TYPE)*ck.capacity);
}
 
template <class TYPE> void Chain<TYPE>::
RemoveChunk( typename Chain<TYPE>::Chunk & ck ){
 	RAssert(MemDbg::CheckHeap(0));
	/* NOT IMPLEMENTED */
}

template <class TYPE> void Chain<TYPE>::
FetchChunk( typename  Chain<TYPE>::Chunk & ck ){
 	RAssert(MemDbg::CheckHeap(1));
	ck.buffer = new TYPE[ck.capacity];
	lgn->chunks_mem+=sizeof(TYPE)*ck.capacity;
	simpledb->LoadSegment(ck.handle.key.c_str(), (char*)ck.buffer);
}

template <  typename   HandleChunkType> 
void OOCEnv<   HandleChunkType>::
SaveAT( ){

	int siz = this->SizeOf();			// size of the Allocation Table 
	char * buf = new char[siz];			// allocate memory
#ifdef _DEBUG
	char * res = 
#endif
	this->Serialize(buf);	// serialize
#ifdef _DEBUG
	RAssert(res-buf == siz);
#endif

	char title[65];
	sprintf(&title[0],"%s","CHAINMEM_ALLOCATION_TABLE");
	 
    
#ifdef _DEBUG
    int result = 
#endif
		simpledb->SaveSegment(std::string(title), buf,siz);
	delete buf;
#ifdef _DEBUG
	RAssert(result==0);
#endif
}

template <  typename   HandleChunkType> 
void OOCEnv<   HandleChunkType>::
LoadAT( ){
	char title[65];
	int size;
	sprintf(&title[0],"%s","CHAINMEM_ALLOCATION_TABLE");
	char * buf = new char[simpledb->SizeSegment(std::string(title))];
	
	simpledb->LoadSegment(std::string(title), buf);
#ifdef _DEBUG	
	char * ptr = 
#endif
	this->DeSerialize(buf);
	size = this->SizeOf();
#ifdef _DEBUG
	RAssert(ptr-buf==size);
#endif
}


#endif //_CHAIN_MEM_SIMPLEDB
