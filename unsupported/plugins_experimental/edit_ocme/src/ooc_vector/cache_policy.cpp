#include <algorithm>
#include "../utils/logging.h"
#include "cache_policy.h"
#include "ooc_chains.h"

/* -------------------------------------------------------------------------- */
/* -------------------------  CachePolicy implementation -------------------- */
/* -------------------------------------------------------------------------- */

CachePolicy::CachePolicy():edit_mode(false),memory_limit(500*(1<<20)),memory_occupied(0){}

void CachePolicy::UnloadOneChunk(bool and_save){
		pop_heap(lru_queue.begin(),lru_queue.end());
		ChunkRef  ch = lru_queue.back();
		lru_queue.pop_back();


		if(ch.chain->IsLoadedChunk(ch.chunk_id)){
                        if(and_save) ch.chain->SaveChunk(ch.chunk_id);
			ch.chain->FreeChunk(ch.chunk_id);
		}
		ch.chain->PointedBy(-1);
		memory_occupied-=ch.chain->ChunkSize();
                STAT::Inc(N_CLEANEDCH);
}

void CachePolicy::NeedChunk(ChainBase * cb ){
	//sprintf(lgn->Buf()buf," need   (%d,%d,%d,%d)");

	const int &  needed_memory = cb->ChunkSize();
	while(memory_occupied + needed_memory > memory_limit){ // need to unfetch something
		// unload the chunk (unless someone else did it)
		/* *UGLY* 
		if a chunk is removed (because say  a cell is removed) the cache_policy does not know
		about, so the memory is assumed to be occupied by that chunk also when it is not.
		TODO: 
		- incorporate a notification to cache_policy that a chunk has been removed into
		the FreeChunk functions
		- add a clear heap to get rid of those references to chunks that have been alrady freed but are 
		still referred in lru_queue
		*/
		UnloadOneChunk();
	}
}

void CachePolicy::FlushAllChunks(bool and_save){
	sprintf(lgn->Buf(),"Flushing %d chunks",lru_queue.size());
	lgn->Push();
	while(!lru_queue.empty())
		UnloadOneChunk(and_save);
	lgn->Append("done",true); 

}


void CachePolicy::ChunkLoaded(ChainBase * cb, const int  & ic ){
	memory_occupied+=cb->ChunkSize();
	lru_queue.push_back(ChunkRef(cb,ic));
	cb->PointedBy(1);
	push_heap(lru_queue.begin(),lru_queue.end());
}

bool CachePolicy:: ChunkRef::
		 operator<(const ChunkRef & o) const 
                        {return chain->FetchTime(chunk_id)  > o.chain->FetchTime(o.chunk_id);}

