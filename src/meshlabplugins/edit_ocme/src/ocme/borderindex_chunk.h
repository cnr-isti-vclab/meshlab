#ifndef _OCME_GINDEX_CHUNK_
#define _OCME_GINDEX_CHUNK_

#include "cell.h"


template <> unsigned int  Chain<BorderIndex>::Chunk::SizeOfMem();

template <> unsigned int  Chain<BorderIndex>::Chunk::SizeOfDisk();

template <> void  Chain<GIndex>::Chunk::
Written(unsigned char * & buffer);

template <> unsigned int  Chain<BorderIndex>::Chunk::
Write(unsigned  char * & buffer );
 
template <> void  Chain<BorderIndex>::Chunk::
Read(unsigned char *buffer,unsigned char *here );

template <>  float  Chain<BorderIndex>::Chunk::
CompressionRatio();

template <> void  Chain<BorderIndex>::Chunk::
AllocMem(unsigned char * & buf,unsigned char * here);
template <> void  Chain<BorderIndex>::Chunk::
DeAllocMem();
#endif
