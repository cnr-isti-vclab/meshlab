#ifndef __CHUNK_MEM_DISK_
#define __CHUNK_MEM_DISK_
#include "../utils/timing.h"
#include "../../ooc_vector/ooc_chains.h"
#include <limits.h>

#include <kcpolydb.h>

template <class TYPE> int Chain<TYPE>::
SaveChunk( typename Chain<TYPE>::Chunk & ck ){
	TIM::Begin(13);
	char * local_buffer = 0;

	if(ck.savedOnce && this->saveOnce) return 0 ;

	RAssert(MemDbg::CheckHeap(0));

	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
	std::string  key = this->GetKey(chunk_order);

	unsigned int siz = ck.Write(local_buffer);

	
	((kyotocabinet::PolyDB*)extMemHnd)->set(key.c_str(),key.length(), local_buffer, siz);

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

	((kyotocabinet::PolyDB*)extMemHnd)->remove(name);
}


template <class TYPE> void Chain<TYPE>::
FetchChunk( typename  Chain<TYPE>::Chunk & ck ){
	RAssert(MemDbg::CheckHeap(1));
	TIM::Begin(12);

	const unsigned int & chunk_order =  &ck-&(*chunks.begin());
	std::string key = this->GetKey(chunk_order);

        char * chunk_buffer, *local_buffer;

        ck.AllocMem(chunk_buffer);

	size_t size_buffer;
	local_buffer   = ((kyotocabinet::PolyDB*)extMemHnd)->get(key.c_str(),key.length(), &size_buffer);

	RAssert(ck.SizeOfDisk() == size_buffer);
 
        if(local_buffer==0 )
		printf("record not found");

        memcpy(chunk_buffer,local_buffer,sizeof(char)*size_buffer);
        ck.Read(chunk_buffer);

        delete [] local_buffer;

	TIM::End(12);
	STAT::Inc(N_LOADEDCH);
}

template <class TYPE> void Chain<TYPE>::
LoadAll(){
	incore.resize(this->Size());
	for(unsigned int ci = 0; ci < chunks.size(); ++ci)
	{
		typename Chain<TYPE>::Chunk & ck = chunks[ci];
		char * ptr = (char*) &this->incore[ci*this->params.chunkSize];
		if(ck.buffer){
			memcpy(ptr,ck.buffer,sizeof(TYPE)*ck.size);
		}else
		{
			RAssert(MemDbg::CheckHeap(1));

                         char  * chunk_buffer,* local_buffer = NULL;
			const unsigned int & chunk_order =  &ck-&(*chunks.begin());
			std::string  key = this->GetKey(chunk_order);

                        ck.AllocMem(chunk_buffer,ptr);

			size_t size_buffer;
                        local_buffer =((kyotocabinet::PolyDB*)extMemHnd)->get(key.c_str(),key.length(),& size_buffer);
                        RAssert(local_buffer);
                        memcpy(chunk_buffer,local_buffer,sizeof(char)*size_buffer);
                        delete [] local_buffer;

                        ck.Read(local_buffer,ptr); // does nothing
		}
	}
}


#endif
