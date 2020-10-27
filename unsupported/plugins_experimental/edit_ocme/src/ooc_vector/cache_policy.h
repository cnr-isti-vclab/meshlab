#ifndef _CACHE_POLICY_
#define _CACHE_POLICY_


#include <vector>
/*
This class implements the cache policy for otpimizing memory use.
In the current implementation uses a LRU strategy with Chunk granularity 
with priority set to loading time.
It means that the N chunks actually stored are the last N that where loaded.
TODO: implement a version that use access time to the chunk (not loading time)
as priority.
*/
struct Logging;
extern Logging * lgn;
struct ChainBase;
struct CachePolicy{
	CachePolicy();
	CachePolicy(int limit):edit_mode(false),memory_limit(limit),memory_occupied(0){}

	struct ChunkRef{
		ChunkRef(ChainBase *_chain, int _chunk_id):chain(_chain),chunk_id(_chunk_id){};
		ChainBase *chain;
		int chunk_id;
		bool operator<(const ChunkRef & o) const ;
	};

	// edit mode: if edit_mode == true the chunks are saved prior to be freed
	bool edit_mode;

	// amount of usable memory  (in Bytes)
	unsigned int memory_limit;

	// amount of occupied memory  (in Bytes) 
	unsigned int memory_occupied;

	// queue of loaded chunks
	std::vector<ChunkRef> lru_queue;

	// this function is called by Chain to communicate that a chunk needs to be loaded
	void NeedChunk(ChainBase * cb);	

	// unload the first chunk in the lru queue
	void UnloadOneChunk(bool and_save = true);

	// this function is called by Chain to communicate that a chunk  has been loaded
	void ChunkLoaded(ChainBase * cb, const int  & ic);	

	// this function is called to empty the cache, saving all the chunks
	void FlushAllChunks(bool and_save = true);
};

#endif
