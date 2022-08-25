1 {
 a\
/* -*- c -*- */\
#ifndef INCLUDED_LIB3DS_CHUNKTABLE_H\
#define INCLUDED_LIB3DS_CHUNKTABLE_H\
/*\
 * The 3D Studio File Format Library\
 * Copyright (C) 1996-2007 by Jan Eric Kyprianidis <www.kyprianidis.com>\
 * All rights reserved.\
 *\
 * This program is  free  software;  you can redistribute it and/or modify it\
 * under the terms of the  GNU Lesser General Public License  as published by\
 * the  Free Software Foundation;  either version 2.1 of the License,  or (at\
 * your option) any later version.\
 *\
 * This  program  is  distributed in  the  hope that it will  be useful,  but\
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY\
 * or  FITNESS FOR A  PARTICULAR PURPOSE.  See the  GNU Lesser General Public\
 * License for more details.\
 *\
 * You should  have received  a copy of the GNU Lesser General Public License\
 * along with  this program;  if not, write to the  Free Software Foundation,\
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\
 *\
 * $Id: chunktable.sed,v 1.7 2007/06/14 09:59:10 jeh Exp $\
 */\
\
#ifndef INCLUDED_LIB3DS_CHUNK_H\
#include <lib3ds/chunk.h>\
#endif\
\
#ifdef __cplusplus\
extern "C" {\
#endif\
\
typedef struct _Lib3dsChunkTable {\
    Lib3dsDword chunk;\
    const char* name;\
} Lib3dsChunkTable;\
\
static Lib3dsChunkTable lib3ds_chunk_table[]={
}
/^ *LIB3DS_/ {
  s/ *\([0-9A-Z_]*\).*/  {\1, "\1"},/
  p
}
$ {
 i\
  {0,0}\
};\
\
#ifdef __cplusplus\
};\
#endif\
#endif\

}
d




