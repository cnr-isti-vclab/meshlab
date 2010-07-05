
#ifndef _CHAIN_MEM_BERKELEYDB_
#define _CHAIN_MEM_BERKELEYDB_


#ifndef NO_BERKELEY

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "berkeleyDb.hpp"
#include "../ooc_vector/ooc_chains.h"
#include "../../utils/timing.h"


template <class TYPE> int Chain<TYPE>::
SaveChunk( typename Chain<TYPE>::Chunk & ck ){
	TIM::Begin(13);
	unsigned   char * local_buffer = 0;			

	if(ck.savedOnce && this->saveOnce) return 0 ;

 	RAssert(MemDbg::CheckHeap(0));

	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
	std::string & name = this->GetKey(chunk_order);
	Dbt key((void*)name.c_str(), (u_int32_t)strlen(name.c_str()) + 1);

	unsigned int siz = ck.Write(local_buffer);
    Dbt data(local_buffer,  siz);	data.set_flags(DB_DBT_USERMEM);
    ((BerkeleyDb*)extMemHnd)->getDb().put(NULL, &key, &data, 0);
	ck.Written(local_buffer);

	if(ck.size == this->params.chunkSize ) 
		ck.savedOnce = true;

	TIM::End(13);

	return  ck.SizeOfMem();
}

template <class TYPE> void Chain<TYPE>::
RemoveChunk( typename Chain<TYPE>::Chunk & ck ){
 	RAssert(MemDbg::CheckHeap(0));
	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
	std::string & name = this->GetKey(chunk_order);

	Dbt key((void*)name.c_str(), (u_int32_t)strlen(name.c_str()) + 1);
    ((BerkeleyDb*)extMemHnd)->getDb().del(NULL, &key,  0);
}


template <class TYPE> void Chain<TYPE>::
FetchChunk( typename  Chain<TYPE>::Chunk & ck ){
 	RAssert(MemDbg::CheckHeap(1));
	TIM::Begin(12);


	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
	std::string & name = this->GetKey(chunk_order);

 	Dbt key((void*)name.c_str(), (u_int32_t)strlen(name.c_str()) + 1);


	unsigned char * local_buffer;

 	ck.AllocMem(local_buffer);

	Dbt data(local_buffer , ck.SizeOfDisk());

	data.set_flags(DB_DBT_USERMEM);

	data.set_ulen(ck.SizeOfDisk());
	int res = ((BerkeleyDb*)extMemHnd)->getDb().get(NULL, &key, &data, 0);
	if(res != 0)
		printf("record not found"); 

	ck.Read(local_buffer); 

	TIM::End(12);
		
}

template <class TYPE> void Chain<TYPE>::
LoadAll(){
	incore.resize(this->Size());
	for(unsigned int ci = 0; ci < chunks.size(); ++ci)
	{
		typename Chain<TYPE>::Chunk & ck = chunks[ci];
		unsigned char * ptr = (unsigned char*) &this->incore[ci*this->params.chunkSize]; 
		if(ck.buffer){
			memcpy(ptr,ck.buffer,sizeof(TYPE)*ck.size);
		}else
		{
	 		RAssert(MemDbg::CheckHeap(1));
		
			unsigned char * local_buffer = NULL;
			const unsigned int & chunk_order =  &ck-&(*chunks.begin());
			std::string & name = this->GetKey(chunk_order);
			Dbt key((void*)name.c_str(), (u_int32_t)strlen(name.c_str()) + 1);

	 		ck.AllocMem(local_buffer,ptr);

			Dbt data(local_buffer , ck.SizeOfDisk());
			data.set_flags(DB_DBT_USERMEM);

			data.set_ulen(ck.SizeOfDisk());
			int res = ((BerkeleyDb*)extMemHnd)->getDb().get(NULL, &key, &data, 0);
			RAssert(res==0);

			ck.Read(local_buffer,ptr);
		}
	}
}


#else

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

#endif // NO_BERKELEY

#endif //_CHAIN_MEM_BERKELEYDB_
