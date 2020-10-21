#include "../ooc_vector/ooc_chains.h"
#include "BorderIndex_chunk.h"

#define _COMPRESS_BorderIndex_
#ifdef _COMPRESS_BorderIndex_
#include <bcl/src/bcl.h>

template <> unsigned int  Chain<BorderIndex>::Chunk::
SizeOfMem(){ return sizeof(BorderIndex)*this->capacity;}

template <> unsigned int  Chain<BorderIndex>::Chunk::
SizeOfDisk(){ return size_of_disk;}

template < > void  Chain<BorderIndex>::Chunk::
Written(unsigned  char * & buf){ delete [] buf; buf  = 0;}

template < > unsigned int  Chain<BorderIndex>::Chunk::
Write(unsigned  char * & buf ){ 
	const unsigned int insize = this->SizeOfMem();
	unsigned int worstcase_outsize = ceil(257.f/256.f * (insize+1));

	buf = new unsigned  char[worstcase_outsize]; 
	this->size_of_disk = RLE_Compress((unsigned char*)this->buffer, insize, (unsigned char*) &buf[0],worstcase_outsize );
 	return this->size_of_disk;
}

 
template < > void  Chain<BorderIndex>::Chunk::
Read(unsigned char *buf, unsigned char * here  ){ 
	if(!here) this->buffer =  new BorderIndex[this->capacity];
	unsigned int outsize = RLE_Uncompress((unsigned char*)&buf [0],this->size_of_disk,(here)?here:(unsigned char*)this->buffer,this->SizeOfMem());
	assert(outsize == this->SizeOfMem());
	delete [] buf;
}

template <>  float  Chain<BorderIndex>::Chunk::
CompressionRatio(){
	return 0.5;// crappy estimation of rle over BorderIndex
}


template < > void  Chain<BorderIndex>::Chunk::
AllocMem(unsigned  char * & buf,unsigned char * here ){
	// ignore "here"
	buf = (unsigned char*) new unsigned char [this->size_of_disk];
};

template < > void  Chain<BorderIndex>::Chunk::
DeAllocMem(){delete [] this->buffer;this->buffer=0;}

#else


template <> unsigned int  Chain<BorderIndex>::Chunk::
SizeOfMem(){ return sizeof(BorderIndex)*this->capacity;}

template <> unsigned int  Chain<BorderIndex>::Chunk::
SizeOfDisk(){ return size_of_disk;}

template < > void  Chain<BorderIndex>::Chunk::
Written(unsigned  char * & buffer){}

template < > unsigned int  Chain<BorderIndex>::Chunk::
Write(unsigned char * & buffer ){ 
	unsigned char *ptr = buffer; 
	buffer = (unsigned char*) this->buffer;
	return this->size*sizeof(BorderIndex);
}

 
template < > void  Chain<BorderIndex>::Chunk::
Read(unsigned char *buffer ){ 
	unsigned char *ptr = buffer; 
	this->buffer = (BorderIndex*)buffer;
}

template < > void  Chain<BorderIndex>::Chunk::
AllocMem(unsigned char * & buf ){this->buffer = new BorderIndex[this->capacity];buf = (unsigned  char*) this->buffer;};

template < > void  Chain<BorderIndex>::Chunk::
DeAllocMem(){delete [] this->buffer;this->buffer=0;}
#endif
