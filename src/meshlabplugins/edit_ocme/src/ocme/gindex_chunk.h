#ifndef _OCME_GINDEX_CHUNK_
#define _OCME_GINDEX_CHUNK_

#include "cell.h"


template <> unsigned int  Chain<GIndex>::Chunk::SizeOfMem();

template <> unsigned int  Chain<GIndex>::Chunk::SizeOfDisk();

template <> void  Chain<GIndex>::Chunk::
Written(unsigned char * & buffer);

template <> unsigned int  Chain<GIndex>::Chunk::
Write(unsigned  char * & buffer );
 
template <> void  Chain<GIndex>::Chunk::
Read(unsigned char *buffer,unsigned char *here );

template <>  float  Chain<GIndex>::Chunk::
CompressionRatio();

template <> void  Chain<GIndex>::Chunk::
AllocMem(unsigned char * & buf,unsigned char * here);
template <> void  Chain<GIndex>::Chunk::
DeAllocMem();
#endif
