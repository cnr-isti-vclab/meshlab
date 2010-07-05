#ifndef __CHUNK_MEM_DISK_
#define __CHUNK_MEM_DISK_


#ifndef NO_BERKELEY

#include "../utils/timing.h"
#include "../../ooc_vector/ooc_chains.h"
#include "berkeleydb.hpp"

template <class TYPE> int Chain<TYPE>::
SaveChunk( typename Chain<TYPE>::Chunk & ck ){
	TIM::Begin(13);
	unsigned   char * local_buffer = 0;			

	if(ck.savedOnce && this->saveOnce) return 0 ;

 	RAssert(MemDbg::CheckHeap(0));

	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
        std::string  name = this->GetKey(chunk_order);
	Dbt key((void*)name.c_str(), (u_int32_t)strlen(name.c_str()) + 1);

	unsigned int siz = ck.Write(local_buffer);

	Dbt data(local_buffer,  siz);
	data.set_flags(DB_DBT_USERMEM);
	((BerkeleyDb*)extMemHnd)->getDb().put(NULL, &key, &data, 0);

	ck.Written(local_buffer);

	if(ck.size == this->params.chunkSize ) 
		ck.savedOnce = true;
	else
		STAT::Inc(N_SAVED_PART_CH);

	TIM::End(13);
    STAT::Inc(N_SAVEDCH);

	return  ck.SizeOfMem();
}

template <class TYPE> void Chain<TYPE>::
RemoveChunk( typename Chain<TYPE>::Chunk & ck ){
 	RAssert(MemDbg::CheckHeap(0));
	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
        std::string  name = this->GetKey(chunk_order);

	Dbt key((void*)name.c_str(), (u_int32_t)strlen(name.c_str()) + 1);
    ((BerkeleyDb*)extMemHnd)->getDb().del(NULL, &key,  0);
    STAT::Inc(N_REMOVEDCH);
}


template <class TYPE> void Chain<TYPE>::
FetchChunk( typename  Chain<TYPE>::Chunk & ck ){
 	RAssert(MemDbg::CheckHeap(1));
	TIM::Begin(12);

	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
        std::string name = this->GetKey(chunk_order);

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
        ck.fetch_time = clock();

	TIM::End(12);
	STAT::Inc(N_LOADEDCH);
		
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
                        std::string  name = this->GetKey(chunk_order);
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


#include "../utils/timing.h"
#include "../../ooc_vector/ooc_chains.h"
#include "../simpledb.h"
#include <limits.h>

template <class TYPE> int Chain<TYPE>::
SaveChunk( typename Chain<TYPE>::Chunk & ck ){
	TIM::Begin(13);
	unsigned   char * local_buffer = 0;

	if(ck.savedOnce && this->saveOnce) return 0 ;

	RAssert(MemDbg::CheckHeap(0));

	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
	std::string  key = this->GetKey(chunk_order);

	unsigned int siz = ck.Write(local_buffer);

	if(!ck.pos.Void() )
		((SimpleDb*)extMemHnd)->Put(ck.pos, local_buffer, siz);
	else
		ck.pos = ((SimpleDb*)extMemHnd)->Put(key, local_buffer, siz);

	ck.Written(local_buffer);

	if(ck.size == this->params.chunkSize )
		ck.savedOnce = true;
	else
		STAT::Inc(N_SAVED_PART_CH);

	TIM::End(13);
	STAT::Inc(N_SAVEDCH);
	return  ck.SizeOfMem();
}

template <class TYPE> void Chain<TYPE>::
RemoveChunk( typename Chain<TYPE>::Chunk & ck ){
	RAssert(MemDbg::CheckHeap(0));
	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
	std::string  name = this->GetKey(chunk_order);

	((SimpleDb*)extMemHnd)->Del(name);
}


template <class TYPE> void Chain<TYPE>::
FetchChunk( typename  Chain<TYPE>::Chunk & ck ){
	RAssert(MemDbg::CheckHeap(1));
	TIM::Begin(12);


	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
	std::string key = this->GetKey(chunk_order);

	unsigned char * local_buffer;

	ck.AllocMem(local_buffer);

	SimpleDb::Index res;

	if(!ck.pos.Void())
          res = ((SimpleDb*)extMemHnd)->Get(ck.pos, local_buffer,ck.SizeOfDisk());
	else{
	  res = ((SimpleDb*)extMemHnd)->Get(key, local_buffer, ck.SizeOfDisk());
	  ck.pos = res;
	}
	if(res.Void() )
		printf("record not found");

	ck.Read(local_buffer);

	TIM::End(12);
	STAT::Inc(N_LOADEDCH);
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
			std::string  key = this->GetKey(chunk_order);

			ck.AllocMem(local_buffer,ptr);

			SimpleDb::Index res = ((SimpleDb*)extMemHnd)->Get(key, local_buffer, ck.SizeOfDisk());
                        RAssert(!res.Void());

			ck.Read(local_buffer,ptr);
		}
	}
}


#endif

#endif
