#ifndef __OCME_MEM_DEC__
#define __OCME_MEM_DEC__

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <limits>
#include <map>
#include <list>
#include <typeinfo>
#include "../utils/string_serialize.h"
#include "../utils/timing.h"
#include "type_traits.h"
#include "cache_policy.h"
#include "../utils/logging.h"
#include "../utils/std_util.h"
#include "../utils/memory_debug.h"
#include "../utils/release_assert.h"
#include <limits>
#ifdef SIMPLE_DB
#include "../ooc_vector/simpledb.h"
#endif
/* **********************************************************************
the classes Chain and OOCEnv implement very large vector like containers (Chain)
that do not fit in memory.

The idea is that a Chain is stored as a list of chunks, which are trasparently
stored in-core and out-of-core memory.
If needed, a chain may be entirely loaded in a contiguous array.
Main functions:
Chain<TYPE>::AddElem(TYPE e)
TYPE & operator Chain<TYPE>::operator[](const int & i);
Chain<TYPE>::DeleteElem(const int &i )

The class OOCEnv is simply an allocator of all the chains.
Main functions:

template <class TYPE>
Chain<TYPE> *  GetChain( std::string name , bool ifnot_create = false);

ChainBase * NewChain(std::string & type );

template <class TYPE>
void DeleteChain( Chain<TYPE> * chain );


*************************************************************************/




/*
This class implements the cache policy for otpimizing memory use.
In the current implementation uses a LRU strategy with Chunk granularity 
with priority set to loading time.
It means that the N chunks actually stored are the last N that where loaded.
TODO: implement a version that use access time to the chunk (not loading time)
as priority.
*/


struct CachePolicy; //mere forward declaration
struct ChainBase {
	virtual ~ChainBase() {};
        virtual std::string & Key()							= 0;
        virtual void GetValue(const unsigned int & pos, void * )                             = 0;
        virtual void SetValue(const unsigned int & pos, void * )                             = 0;

        virtual unsigned int   Size()const					= 0;
	virtual void Resize(const unsigned int&) 					= 0;
	virtual void UnloadAllChunks() 								= 0;
	virtual void RemoveAllChunks()								= 0;
	virtual void PointedBy(const int & )						= 0;
	virtual void Compact(std::vector<unsigned int> &deleted)	= 0;
	virtual void SetChunkSize(unsigned int blocksize)			= 0;
	virtual void AddChunk()										= 0;
	virtual	void LoadChunk(const int & ic)						= 0;
	virtual	void UnloadChunk(const int & ic)					= 0;
	virtual	int SaveChunk(const int & ic)						= 0;
	virtual	bool IsLoadedChunk(const int & ic)					= 0;
	virtual	int FetchTime(const int & ic)						= 0;
	virtual void FreeChunk(const int & ic)						= 0;
        virtual unsigned int  ChunkSize() const				= 0;
	virtual CachePolicy *& CachePol()							= 0;
	virtual void * & ExtMemHnd()								= 0;
	virtual void SaveData()										= 0;
	virtual std::string & Type()								= 0;
	virtual int SizeOfMem()										= 0;
	virtual char * Serialize(char *)							= 0;
	virtual char * DeSerialize(char*)							= 0;
};



template <class TYPE>
struct Chain: public ChainBase{
		// private
	~Chain(){
		Delete(chunks);
                assert(this->pointedby == 0);
	}

		struct Params{
			int chunkSize;			// size of the chunk (in number of elements)
			int blockSize;			// usable ram memory (in MB)
		} params;

		std::string type;			// string name of the TYPE

		bool saveOnce;				// tell if the chunks will be saved only once

		std::string & Type(){return type;}

		std::vector<TYPE> incore;	// when a chain is fully loaded in memory, this is the place where it's stored 
									
		CachePolicy * cache_policy;	// implements a cache policy for the chunks 
		void		* extMemHnd;	// handle to extMemHandle of the lcm (UGLY)

		CachePolicy * &CachePol(){ return cache_policy;}
		void * & ExtMemHnd(){return extMemHnd;}								  

	public:
		Chain():size(0),pointedby(0),saveOnce(false){SetChunkSize(params.blockSize);}
                Chain(std::string _name):saveOnce(false),key(_name),size(0),pointedby(0){SetChunkSize(params.blockSize);}

		/* Set the size of the chunk in terms of number of elements*/
		void SetChunkSize( unsigned int blocksize);

	struct Chunk{
		Chunk():buffer(0),fetch_time(0),savedOnce(false){
#ifdef SIMPLE_DB
			pos.SetVoid();
#endif
		}
			void Init(const int & _capacity){
				buffer = new TYPE[_capacity];size = 0; capacity = _capacity;}
			int capacity;								// how many elements can be stored in this chunk
			int size;									// [to remove] how many elements are currently stored in this chunk
			TYPE * buffer;								// buffer where the data are kept (== NULL  ->not in memory, != NULL -> loaded
			void Dump(void * buffer, int & size); //save the Chunk (NOT its data)
			int fetch_time;								// the last time it was loaded
			bool savedOnce;								// if the chunk has been saved at least once
			unsigned int size_of_disk;					// size on disk
#ifdef SIMPLE_DB	 
			SimpleDb::Index pos;
#endif
			/* query         */
			bool IsLoaded(){return buffer!=NULL;}

			/* serialization */
			char * Serialize(char * ptr);
			char * DeSerialize(char * ptr);

			/* read/write */
			unsigned int SizeOfDisk();
			unsigned int SizeOfMem();
			void Written( char * & buffer);
			unsigned int Write( char * & buffer);
			void Read(  char * buffer,  char * here = NULL);
			static float CompressionRatio();

			/* allocation */
			void AllocMem( char *& buf, char * here = NULL);
			void DeAllocMem();
		};
public:
		std::string  key;  // it must be unique

		std::string & Key(){return key;}

		std::vector< Chunk >  chunks;	// sequence of chunks of this chain
		
		// number of elements in the chain
		unsigned int size;

		// number of pointers to this 
		// This form of smart pointing in only for the pointers kept by the cache_policy class
		// because when a chain is deleted there can be reference to it from the lru_queue
		int pointedby;

		// access function
		unsigned int  Size() const { return size;}

		// returns the memory required to fetch a chunk
		unsigned int  ChunkSize() const { return params.chunkSize * sizeof(TYPE);}

		// add an element to the chain
		unsigned int AddElem(const TYPE & elem); 

		// add an empty element to the chain
		unsigned int AddElem( ); 
		
		// Compact the vector taking into account the vector of deleted element passed
		void Compact(std::vector<unsigned int> &deleted);

		// compute the remapping of a vector of size elements after that "Compact" will be applied
		void  BuildRemap(std::vector<unsigned int> &deleted,std::vector<unsigned int> &remap);



		// ----------------------------------- Management of the chunks ------------------------------ 
			
		// save a specific chunk [*]
		int SaveChunk(Chunk & ck);
		int SaveChunk(const int & ci);

		// say if a specific chunk is loaded in memory
		bool IsLoadedChunk(const int & ic);

		// remove a specific chunk [*]
		void RemoveChunk(Chunk & ck);

		// remove an interval of chunks
		void RemoveChunks(const unsigned int & from, const unsigned int &  to);

		// remove all the chunks
		void RemoveAllChunks();

		// load a specific chunk [*] from external memory straighaway
		void FetchChunk(Chunk & ck);

		// return the time the chunk ic was fetched
		int FetchTime(const int & ic){return (static_cast<unsigned int>(ic)>=chunks.size())?0:chunks[ic].fetch_time;}

		// Load a specific chunk invoking cache policy
		void LoadChunk(const int & ic);

		// free a specific chunk [*] straigth away
		void FreeChunk(Chunk & ck);
		void FreeChunk(const int & ci){FreeChunk(chunks[ci]);}

		// unload a specific chunk [*] implementing cache policy
		void UnloadChunk(const int & ic){FreeChunk(chunks[ic]);}

		//  unload all the chunks
		void UnloadAllChunks();

		// ------------------------------------ END management of the chunks ----------------------------	



		// add/decrease the number of pointers to this class
		void PointedBy(const int & inc);

		// add a new chunk
		void AddChunk();

		// get the key of the i-th chunk
		std::string  GetKey(unsigned int chunk_order);

		// close and free the buffer, optionally save before closing [*]
		void Close(bool saveAnd = true);

                //  access by reference to the i-th element of the chain  when is chunked
		inline TYPE & operator [](const unsigned int & i);

                // access by value to the i-th element of the chain  when is chunked
								inline TYPE  operator []  (const unsigned int & i) const;

                // access to the i-th element of the chain  when is incore
		TYPE & At(const int & i);

                // copy the i-th element in to a given memory location
                void GetValue(const unsigned int & pos, void * dst);

                // copy the value from the given memory location to the i-th element
                void SetValue(const unsigned int & pos, void * dst);

                // resize the container
		void Resize(const unsigned int &  n);

		// load all the data of the chain in this->incore
		void LoadAll();

		// free incore 
		void FreeAll(){Delete(incore);};

		// save the data
		void SaveData(); 

		/* serialization */
		int SizeOfMem();
		char * Serialize(char * buffer);
		char * DeSerialize(char * buffer);

};


 
/*
This class is the interface to use the Chain. It implements the creation/deletion of chains.
*/

class OOCEnv {
	//private:
public:
        OOCEnv(){ InitDefaultNameTypeBinders();cache_policy = new CachePolicy(); params.blockSizeBytes = 1024; }
	~OOCEnv(){ 
		ChainIterator ci;
		for(ci = chains.begin(); ci != chains.end(); ++ci)
			delete (*ci).second;
		delete cache_policy;
	}

	struct Params{
		int blockSizeBytes;	 
	} params;


public: 
	
	// private
	std::map<std::string,ChainBase* > chains;	// all the chains 

	typedef  std::map<std::string,ChainBase* >::iterator ChainIterator;

        CachePolicy * cache_policy;
        
	// is_file_owned=true means the OOC_env is responsible for opening /cosing the file
	bool is_file_owned;

public:
	typedef ChainBase * CallBack( const std::string & key );

        //CallBack * CreateFromString_UserTypes;
	// get an existing chain (optionally create it) 
	template <class TYPE>
	Chain<TYPE> *  GetChain( std::string name , bool ifnot_create = false);

	// get an existing chain
	ChainBase * GetChain( std::string name);

	// create a new chain
	ChainBase * CreateFromString(const std::string & key, const  std::string & type );

	// remove an existing chain
	void  RemoveChain(const std::string & key);
		
	// close and free buffers, optionally save before closing
	void Close(bool saveAnd = true);

	// *** UGLY *** patch...refactorize this stuff
	// this is called to delete everything...the chain of this OOCEnv will not bu used 
	// enymore, so just dealloc everything and close the database
	void TrashEverything();

        /*----------------------- Type Traits ------------------------------------------*/

        struct NameTypeBinder_Base{
            virtual  std::string  Name()  = 0;
            virtual ChainBase  * NewChain() = 0;
            virtual unsigned int SizeOfType() = 0;
						virtual std::string TypeID() = 0;
            /* serialization */
            virtual int SizeOfDisk() = 0;
            virtual int SizeOfMem() = 0;
            virtual char * Serialize(char * buffer) = 0;
            virtual char * DeSerialize(char * buffer) = 0;
        };

		std::map<std::string,NameTypeBinder_Base*> binders;
		typedef std::map<std::string,NameTypeBinder_Base*>::iterator BoundsIterator ;

        template <class TYPE>
                struct NameTypeBind: public NameTypeBinder_Base{
                    NameTypeBind(){}
					NameTypeBind(const char *  name){
                        _name = std::string(name);
                        _size_of = sizeof(TYPE);
                    }
                    std::string  Name()   {return _name;}
                    ChainBase  * NewChain(){return new Chain<TYPE>();}
                    unsigned int SizeOfType() {return _size_of;}

                    /* serialization */
                    int SizeOfDisk(){return SizeOfMem();};
                    int SizeOfMem();
                    char * Serialize(char * buffer);
                    char * DeSerialize(char * buffer);
										std::string TypeID(){ return typeid(TYPE).name();}
                 private:
                    std::string _name;
                    unsigned int _size_of;
        };

		typedef std::pair<std::string,NameTypeBinder_Base*>  StringTypeBound;

		struct NameUnknownTypeBind: public NameTypeBind<char>{
            char * DeSerialize(char * buffer){return NameTypeBind<char>::DeSerialize(buffer);};
        };

        template <class TYPE>
		void AddNameTypeBound(const char *  name){
			assert(name);
			assert( strcmp(name,"")!=0); // you cannot bound a type to an empty string
			BoundsIterator bi = binders.find(name);
			if(bi!=binders.end())
				assert(typeid(TYPE).name() == ((*bi).second)->TypeID() ); // the name was previously bound to a dirrefent type
			else{
				NameTypeBind<TYPE>  * newbound = new NameTypeBind<TYPE>(name);
				binders.insert( StringTypeBound(std::string(name),newbound));
			}
		}


		void AddNameUnknownTypeBound(NameUnknownTypeBind tb){
			 BoundsIterator i;
			  for(i = binders.begin(); i != binders.end(); ++i)
				  if(tb.Name()==(*i).second->Name()){
					  if(tb.SizeOfType()!=(*i).second->SizeOfType())
                      {
						sprintf(lgn->Buf(),"the name %s with size %d is already used with a type of size %d",tb.Name().c_str(),tb.SizeOfType(),(*i).second->SizeOfType());
                        lgn->Push();
                        exit(0);
                      }else
						  return;
				  }
              sprintf(lgn->Buf(),"the name %s has not been declared. This is not (yet) admitted",tb.Name().c_str());
              lgn->Push();
              exit(0);
		}


		template <class TYPE>
            std::string NameOf(){

			BoundsIterator bi;
			for(bi = binders.begin(); bi != binders.end(); ++bi)
				if (typeid(TYPE).name() == ((*bi).second->TypeID() ))
					return (*bi).second->Name();
			return std::string();
        }

        void InitDefaultNameTypeBinders();
        /*----------------------- creation/opening/closing -----#------------------------*/

	// create a new  OOCEnv in the filename
	void Create(const char * name);

	// create a new chain in a existing database
	void Create(void  * handle);

	// open an existing chainmem from filename
	void Open(const char * name);

	// open an existing chainmem from handle
	void Open(void * handle);

	// save the Allocation Table
	int SaveAT();

	// load the Allocation Table
	void LoadAT();

	// save all the chunks that are currently load in memory
	void SaveData();

	/* serialization */
	int SizeOfDisk(){return SizeOfMem();};
	int SizeOfMem();
	char * Serialize(char * buffer);
	char * DeSerialize(char * buffer);

	/*	UGLY
		OOCEnv was intended to became a singleton , i.e. a unique instance manage all the large container
		stuff. Unfortunaly berkeley does not free disk space and therefore if you want to allocate temporary
		containers the best thing if to create a separate instance of OOCEnv, which require some kind of handle
		like this one....
	*/
	void  * extMemHnd;
};


/* -------------------------------------------------------------------------- */
/* -------------------------  Chain implementation -------------------------- */
/* -------------------------------------------------------------------------- */
template <class TYPE> 
void Chain<TYPE>:: SetChunkSize( unsigned int blocksize)	
{
	params.blockSize = blocksize; 
	// as suggested by the berkeley manual we set the size of the chunks so that a page contains 4 chunks
#ifndef NO_BERKELEY
	params.chunkSize = static_cast<int>(  ( (params.blockSize - 68)*0.25 - key.length()-10  ) / (sizeof(TYPE)*Chunk::CompressionRatio()));
#else
	params.chunkSize = blocksize / sizeof(TYPE);
#endif
	if(params.chunkSize == 0 ){
		// if params.chunkSize==0 it means your data is way too big for the blocksize expected
		// In this case we give a default value of
		params.chunkSize = 1;
		sprintf(lgn->Buf(),"%s is %d bytes, too big for the blocksize of %d", this->Key().c_str(),sizeof(TYPE),params.blockSize);
	}
}


template <class TYPE> unsigned int  Chain<TYPE>::
AddElem(const TYPE & elem){
	RAssert(MemDbg::CheckHeap(0));
	// takes the current active chunk 
	unsigned int chi = size/params.chunkSize ;

	if( (size % params.chunkSize)==0) 
		AddChunk();						// add a new chunk	
	 
	Chunk & ck = chunks [ chi ]; 

	if(!ck.IsLoaded()){
		LoadChunk(chi);
	}

	((TYPE*)ck.buffer)[ck.size++] = elem;	// write the element

	size++;
	return size-1;
} 

template <class TYPE> unsigned int  Chain<TYPE>::AddElem(){return AddElem(TYPE());}
 
template <class TYPE> 
void Chain<TYPE>::AddChunk( ){
	assert(MemDbg::CheckHeap(0));
	cache_policy->NeedChunk(this);						// tell the CachePolicy we re going to load this chunk
	chunks.resize(chunks.size()+1);	
	Chunk & c = chunks.back();
	c.Init(this->params.chunkSize);
	cache_policy->ChunkLoaded(this,chunks.size()-1);	// tell the CachePolicy we have loaded a chunk	
}

template <class TYPE> 
std::string  Chain<TYPE>::GetKey(unsigned int chunk_order){
        char buffer[255];
	sprintf(buffer,"%d",chunk_order);
	return (this-> key + std::string("_") + std::string(buffer));
}

template <class TYPE> 
void Chain<TYPE>::LoadChunk(const int & ic){	
 	RAssert(MemDbg::CheckHeap(0));
	cache_policy->NeedChunk(this);						// tell the CachePolicy we re going to load this chunk
        FetchChunk(chunks[ic]);							// When NeedChunk returns (after possibly unloading other chunks, load the chunk
	cache_policy->ChunkLoaded(this,ic);					// tell the CachePolicy we have loaded a chunk	
}

template <class TYPE> 
void  Chain<TYPE>::FreeChunk( Chunk & ck ){
	RAssert(MemDbg::CheckHeap(0));
	RAssert(ck.buffer);
	ck.DeAllocMem();
}

template <class TYPE> 
void Chain<TYPE>::RemoveChunks(const unsigned int & from, const unsigned int &  to){
	for(unsigned int i = from; i < to; ++i){
		if(chunks[i].IsLoaded()) FreeChunk(chunks[i]);
		RemoveChunk(chunks[i]);
	}
	chunks.erase( chunks.begin() + from,chunks.begin() + to );
}

template <class TYPE> 
void Chain<TYPE>::RemoveAllChunks(){
	RemoveChunks(0,chunks.size());
}

template <class TYPE> 
void Chain<TYPE>::PointedBy(const int & inc){
	pointedby+=inc;
}

template <class TYPE> 
int Chain<TYPE>::SaveChunk(const int & ci){ 
	return SaveChunk(chunks[ci]);
}
template <class TYPE> 
bool Chain<TYPE>::IsLoadedChunk(const int & ci){ 
	if(static_cast<unsigned int>(ci) >= chunks.size()) return false;
	return chunks[ci].IsLoaded();
}   

template <class TYPE> 
void  Chain<TYPE>::Resize(const unsigned int & n ){
	/* resize the chain */
	std::vector<unsigned int> toDel;
	if(n<this->Size())															
	{																		// downsize		
		for(unsigned int y = n; y < this->Size();++y) toDel.push_back(y);
		this->Compact(toDel);
	}
	else
		for(unsigned int i  = this->Size(); i < n; ++i) AddElem(TYPE());	// upsize
}

template <class TYPE> 
void  Chain<TYPE>::UnloadAllChunks( ){
	/* The chain is cleared, we don't have to save the chunks */
	for(unsigned int ic  = 0; ic < chunks.size(); ++ic) FreeChunk(ic);
	size = 0;
}

template <class TYPE> 
void Chain<TYPE>::Compact(std::vector<unsigned int> &deleted){
        TYPE tmp;

	if(deleted.empty()) return;
	 
	/* compact the vector */
	unsigned int	siz = 0,	//	index of the last non deleted element being written , finally new size of the chain
                    cnt = 0;	//	index of the non deleted element being copied (moved)

    ::RemoveDuplicates(deleted); 
	RAssert(deleted.back() < size);
	for(unsigned int di = 0; di < deleted.size(); ++di,++cnt)
            for(; cnt < deleted[di]; ++cnt){
                        tmp = (*this)[cnt];
                        (*this)[siz++] = tmp;
                    }
	
	// copy the elements beyond the last one contained in deleted
        for(; cnt < size; ++cnt){
                tmp =  (*this)[cnt];
                (*this)[siz++] = tmp;
            }

	/* update the new size */
	RAssert(siz == size-deleted.size());
	size = siz;

	if(size == 0)
		RemoveChunks(0, chunks.size());
	else{
		/* UGLY? maybe..** update the lenght of the last chunk */
		int chi = ( (size-1)/params.chunkSize);				// find in which chunk the element i is  kept
		int cksize  = (size-1)%params.chunkSize;			// find its position inside the chunk
		RAssert(chi < chunks.size());
		chunks [ chi ].size = cksize + 1;
		RemoveChunks( chi+1, chunks.size());
		RAssert((size-1)/params.chunkSize==chunks.size()-1);
	}
	 
}

template <class TYPE> 
void  Chain<TYPE>::BuildRemap(std::vector<unsigned int> &deleted,std::vector<unsigned int> &remap){
        unsigned int di = 0;
        unsigned int id = 0;

        ::RemoveDuplicates(deleted);
        remap.resize(this->size);

	for( id = 0; id < deleted.size(); ){
		for(; di < deleted[id]; ++di)
			remap[di] = di-id;
		remap[di] = std::numeric_limits<unsigned int>::max();
		++id;	// next deleted element
		++di;	// skip deleted element
	}

	for(; di < size; ++di)
		remap[di] = di-id;
}


/* ******************   DATA ACCESS  ******************* */

template <class TYPE> 
TYPE  & Chain<TYPE>::operator [] (const unsigned int & i){
 	if(  incore.empty() ){										// it is is not entirely loaded

        STAT::Inc(N_ACCESSES);
		const unsigned int & chi = (i/params.chunkSize);		// find in which chunk the element i is  kept
		const unsigned int & id  = i%params.chunkSize;			// find its position inside the chunk

		RAssert( i<size);

#ifdef _DEBUG
		if(chunks.size() <= chi){
			sprintf(lgn->Buf(),"failed chunks.size() > chi %d %d %d",i,chi,id);
			lgn->Push();
			sprintf(lgn->Buf(),"params.chunkSize  %d",params.chunkSize);
			lgn->Push();
			sprintf(lgn->Buf(),"n chunks  %d",chunks.size());
			lgn->Push();
		}
#endif
		if(!chunks[chi].IsLoaded())						//if the chunk is not loaded
			LoadChunk( chi);							//load the chunk
		return ( (chunks[chi].buffer) )[id];	//return the element
 	}else  
 		return  incore[i] ;				//return the element
}

template <class TYPE>
TYPE    Chain<TYPE>::operator [] (const unsigned int & i)const {
        if(  incore.empty() ){										// it is is not entirely loaded

                const unsigned int & chi = (i/params.chunkSize);		// find in which chunk the element i is  kept
                const unsigned int & id  = i%params.chunkSize;			// find its position inside the chunk

                RAssert( i>=0);
                RAssert( i<size);
#ifdef _DEBUG
                if(chunks.size() <= chi){
                        sprintf(lgn->Buf(),"failed chunks.size() > chi %d %d %d",i,chi,id);
                        lgn->Push();
                        sprintf(lgn->Buf(),"params.chunkSize  %d",params.chunkSize);
                        lgn->Push();
                        sprintf(lgn->Buf(),"n chunks  %d",chunks.size());
                        lgn->Push();
                }
#endif
                if(!chunks[chi].IsLoaded())						//if the chunk is not loaded
                        LoadChunk( chi);							//load the chunk
                return ( (chunks[chi].buffer) )[id];	//return the element
        }else
                return  incore[i] ;				//return the element
}
template <class TYPE> 
TYPE  & Chain<TYPE>::At(const int & i){
	RAssert(i <  incore.size());
	return  incore[i] ;
}

template <class TYPE>
        void Chain<TYPE>::GetValue(const unsigned int & pos, void * dst){*(TYPE*)dst = (*this)[pos];}

template <class TYPE>
        void Chain<TYPE>::SetValue(const unsigned int & pos, void * src){(*this)[pos] = *(TYPE*)src;}


/* ******************   DATA SAVING  ******************* */
 
template <class TYPE> 
void  Chain<TYPE>::SaveData(){
		for(unsigned int i = 0; i < chunks.size(); ++i) 
			if( chunks[i].IsLoaded() ) {
				SaveChunk(chunks[i]);
				UnloadChunk(i);
			}
		}


/*  SERIALIZATION  */

/* Chain */  
template <class TYPE> 
int  Chain<TYPE>:: SizeOfMem( ){
	int siz = 0;
	siz += ::SizeOf(this->key);		// length of the string
	siz += ::SizeOf(this->type);	// length of the string
	siz += sizeof(int);							// size of the chain
	siz += sizeof(int);							// number or chunks
	siz += sizeof(unsigned int) * chunks.size();// one unsigned int per chunk (size_of_disk)	
	return siz;
}

 
template <class TYPE>char *  Chain<TYPE>::
Serialize( char * buffer){
	char * ptr = buffer;	
	ptr = ::Serialize(this->key,ptr); 
	ptr = ::Serialize(this->type,ptr);
	memcpy(ptr,&size,sizeof(int));		ptr+=sizeof(int);	// number of elements of the chain
	unsigned int n_chunk = (int) chunks.size();
	memcpy(ptr,&n_chunk,sizeof (int) ); ptr+=sizeof(int);	// number of chunk of the chain
	for(unsigned int ci = 0; ci < n_chunk; ++ ci)
		ptr = chunks[ci].Serialize(ptr);
	return ptr;
}

 
template <class TYPE> char * Chain<TYPE>::
DeSerialize( char * buffer){
	char * ptr = buffer;	
	
	ptr  = ::DeSerialize(ptr,this->key);
	ptr  = ::DeSerialize(ptr,this->type);

	
	memcpy(&size,ptr,sizeof(int));	ptr+=sizeof(int); // number of elements of the chain
	int n_chunk;
	memcpy(&n_chunk,ptr,sizeof (int) ); ptr+=sizeof(int); // number of chunk of the chain
	if(n_chunk == 0) return ptr;

	chunks.reserve(n_chunk);
	for(int ic = 0; ic < n_chunk; ++ic){
	 chunks.push_back(Chunk());

	 // set capacity and size of the chunks
	chunks.back().capacity = this->params.chunkSize;
	chunks.back().size =  this->params.chunkSize;
	ptr = chunks.back().DeSerialize(ptr);
	}
	if(this->Size()%this->params.chunkSize!=0) chunks.back().size = this->Size()%this->params.chunkSize;

	return ptr;
}

/* Chunk */ 
template <class TYPE> char *   Chain<TYPE>::Chunk::
Serialize(char * ptr){
	*(unsigned int*) ptr = this->size_of_disk;
	return ptr+4;
}

template <class TYPE> char * Chain<TYPE>::Chunk::
DeSerialize(char * ptr){
	this->size_of_disk = *(unsigned int*) ptr;
	return ptr+4;
}

template <class TYPE>unsigned int  Chain<TYPE>::Chunk::
SizeOfMem(){ return sizeof(TYPE)*this->size;}

template <class TYPE> unsigned int  Chain<TYPE>::Chunk::
SizeOfDisk(){ return sizeof(TYPE)*this->size;}


template <class TYPE> void  Chain<TYPE>::Chunk::
Written(  char * & buffer){buffer=buffer;}

template <class TYPE> unsigned int  Chain<TYPE>::Chunk::
Write(char * & buffer ){ 
	buffer = ( char*) this->buffer;
	this->size_of_disk = this->SizeOfMem();
	return this->size_of_disk;
}

 
template <class TYPE> void  Chain<TYPE>::Chunk::
Read( char *buf, char *here ){ 
	if(!here) 
		this->buffer = (TYPE*)buf;
}

template <class TYPE> float   Chain<TYPE>::Chunk::			
CompressionRatio(){return 1.0;};

template <class TYPE> void  Chain<TYPE>::Chunk::
AllocMem( char * & buf, char * here ){
	if(!here){
		this->buffer = new TYPE[this->capacity];	// allocate the internal memory of the chunk
		buf = ( char*) this->buffer;		// return
	}else
		buf = here;									// simply copy the passed pointer
};

template <class TYPE> void  Chain<TYPE>::Chunk::
DeAllocMem(){RAssert(this->buffer!=0); delete [] this->buffer;this->buffer=0;}




/* -------------------------------------------------------------------------- */
/* ------------------------- OOCEnv (templated )implementation ------------------ */
/* -------------------------------------------------------------------------- */

template <class TYPE>
Chain<TYPE>   *
OOCEnv  ::
GetChain( std::string name , bool create){
	ChainIterator ci;
	std::pair<ChainIterator, bool> ci_ins;	

	ci = chains.find(name);
	if( ci != chains.end())
		return (Chain<TYPE> *)(*ci).second;


	if(create){ 
		ci_ins = chains.insert(std::pair<std::string,ChainBase*> (name, new Chain<TYPE>(name)));
		ChainBase* & inserted = (*(ci_ins.first)).second;
                inserted->Type() = NameOf<TYPE>();
		(( Chain<TYPE>*)inserted)->params.blockSize = params.blockSizeBytes;
		(( Chain<TYPE>*)inserted)->SetChunkSize(params.blockSizeBytes);
		inserted->CachePol() = this->cache_policy;
		inserted->ExtMemHnd() = this->extMemHnd;
		//inserted->AddChunk();
		return  (Chain<TYPE> *) inserted;// it does not exists and it was asked to create it
	}

	return 0; // it does not exists  
}

/* -------------------------------------------------------------------------- */
/* ------------------------- OOCEnv:: (templated )implementation ------------------ */
/* -------------------------------------------------------------------------- */

template <class TYPE>
int
OOCEnv::NameTypeBind<TYPE>::SizeOfMem(){
    return sizeof(unsigned int)+// _size_of
           SizeOf(this->Name());
};

template <class TYPE>
char *
OOCEnv::NameTypeBind<TYPE>::Serialize(char * buffer){
    char * ptr = buffer;
    *(unsigned int*)ptr = this->SizeOfType(); ptr+=sizeof(unsigned int);
    ptr = ::Serialize(this->Name().c_str(),ptr);
    return ptr;
}

template <class TYPE>
char *
OOCEnv::NameTypeBind<TYPE>::DeSerialize(char * buffer){
        char * ptr = buffer;
       this->_size_of =  *(unsigned int*)ptr; ptr+=sizeof(unsigned int);
       ptr = ::DeSerialize(ptr,this->_name);
       return ptr;
};

#endif

