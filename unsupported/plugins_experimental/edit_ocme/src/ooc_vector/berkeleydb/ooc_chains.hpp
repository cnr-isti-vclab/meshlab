#ifndef __CHUNK_MEM_DISK_
#define __CHUNK_MEM_DISK_
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
		ck.pos = ((SimpleDb*)extMemHnd)->PutSingle(key,ck.pos, local_buffer, siz);
	else
		ck.pos = ((SimpleDb*)extMemHnd)->PutSingle(key, local_buffer, siz);

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
