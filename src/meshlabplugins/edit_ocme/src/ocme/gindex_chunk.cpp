#include "../ooc_vector/ooc_chains.h"
#include "gindex_chunk.h"

#define _COMPRESS_GINDEX_
#ifdef _COMPRESS_GINDEX_
#include <bcl/src/bcl.h>

template <> unsigned int  Chain<GIndex>::Chunk::
SizeOfMem(){ return sizeof(GIndex)*this->capacity;}

template <> unsigned int  Chain<GIndex>::Chunk::
SizeOfDisk(){ return size_of_disk;}

template < > void  Chain<GIndex>::Chunk::
Written(unsigned  char * & buf){ delete [] buf; buf  = 0;}

template < > unsigned int  Chain<GIndex>::Chunk::
Write(unsigned  char * & buf ){ 
	const unsigned int insize = this->SizeOfMem();
	unsigned int worstcase_outsize = ceil(257.f/256.f * (insize+1));

	buf = new unsigned  char[worstcase_outsize]; 
	this->size_of_disk = RLE_Compress((unsigned char*)this->buffer, insize, (unsigned char*) &buf[0],worstcase_outsize );
 	return this->size_of_disk;
}

 
template < > void  Chain<GIndex>::Chunk::
Read(unsigned char *buf, unsigned char * here  ){ 
	if(!here) this->buffer =  new GIndex[this->capacity];
	unsigned int outsize = RLE_Uncompress((unsigned char*)&buf [0],this->size_of_disk,(here)?here:(unsigned char*)this->buffer,this->SizeOfMem());
	assert(outsize == this->SizeOfMem());
	delete [] buf;
}

template <>  float  Chain<GIndex>::Chunk::
CompressionRatio(){
	return 0.5;// crappy estimation of rle over GIndex
}


template < > void  Chain<GIndex>::Chunk::
AllocMem(unsigned  char * & buf,unsigned char * here ){
	// ignore "here"
	buf = (unsigned char*) new unsigned char [this->size_of_disk];
};

template < > void  Chain<GIndex>::Chunk::
DeAllocMem(){delete [] this->buffer;this->buffer=0;}

#else


template <> unsigned int  Chain<GIndex>::Chunk::
SizeOfMem(){ return sizeof(GIndex)*this->capacity;}

template <> unsigned int  Chain<GIndex>::Chunk::
SizeOfDisk(){ return size_of_disk;}

template < > void  Chain<GIndex>::Chunk::
Written(unsigned  char * & buffer){}

template < > unsigned int  Chain<GIndex>::Chunk::
Write(unsigned char * & buffer ){ 
	unsigned char *ptr = buffer; 
	buffer = (unsigned char*) this->buffer;
	return this->size*sizeof(GIndex);
}

 
template < > void  Chain<GIndex>::Chunk::
Read(unsigned char *buffer ){ 
	unsigned char *ptr = buffer; 
	this->buffer = (GIndex*)buffer;
}

template < > void  Chain<GIndex>::Chunk::
AllocMem(unsigned char * & buf ){this->buffer = new GIndex[this->capacity];buf = (unsigned  char*) this->buffer;};

template < > void  Chain<GIndex>::Chunk::
DeAllocMem(){delete [] this->buffer;this->buffer=0;}
#endif
