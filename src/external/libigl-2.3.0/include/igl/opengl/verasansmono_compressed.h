#ifndef IGL_OPENGL_VERASANSMONO_COMPRESSED_H
#define IGL_OPENGL_VERASANSMONO_COMPRESSED_H

#include <igl/opengl/../igl_inline.h>

namespace igl
{
namespace opengl
{

IGL_INLINE void decompress_verasansmono_atlas(unsigned char* _fontatlas);

}

}

#ifndef IGL_STATIC_LIBRARY
#  include "verasansmono_compressed.cpp"
#endif

#endif
