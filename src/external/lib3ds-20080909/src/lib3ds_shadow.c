/*
    Copyright (C) 1996-2008 by Jan Eric Kyprianidis <www.kyprianidis.com>
    All rights reserved.
    
    This program is free  software: you can redistribute it and/or modify 
    it under the terms of the GNU Lesser General Public License as published 
    by the Free Software Foundation, either version 2.1 of the License, or 
    (at your option) any later version.

    Thisprogram  is  distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
    GNU Lesser General Public License for more details.
    
    You should  have received a copy of the GNU Lesser General Public License
    along with  this program; If not, see <http://www.gnu.org/licenses/>. 
*/
#include "lib3ds_impl.h"


void
lib3ds_shadow_read(Lib3dsShadow *shadow, Lib3dsIo *io) {
    Lib3dsChunk c;

    lib3ds_chunk_read(&c, io);
    switch (c.chunk) {
        case CHK_SHADOW_MAP_SIZE: {
            shadow->map_size = lib3ds_io_read_intw(io);
            break;
        }

        case CHK_LO_SHADOW_BIAS: {
            shadow->low_bias = lib3ds_io_read_float(io);
            break;
        }

        case CHK_HI_SHADOW_BIAS: {
            shadow->hi_bias = lib3ds_io_read_float(io);
            break;
        }

        case CHK_SHADOW_FILTER: {
            shadow->filter = lib3ds_io_read_float(io);
            break;
        }

        case CHK_RAY_BIAS: {
            shadow->ray_bias = lib3ds_io_read_float(io);
            break;
        }
    }
}


void
lib3ds_shadow_write(Lib3dsShadow *shadow, Lib3dsIo *io) {
    if (fabs(shadow->low_bias) > LIB3DS_EPSILON) { /*---- CHK_LO_SHADOW_BIAS ----*/
        Lib3dsChunk c;
        c.chunk = CHK_LO_SHADOW_BIAS;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, shadow->low_bias);
    }

    if (fabs(shadow->hi_bias) > LIB3DS_EPSILON) { /*---- CHK_HI_SHADOW_BIAS ----*/
        Lib3dsChunk c;
        c.chunk = CHK_HI_SHADOW_BIAS;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, shadow->hi_bias);
    }

    if (shadow->map_size) { /*---- CHK_SHADOW_MAP_SIZE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_SHADOW_MAP_SIZE;
        c.size = 8;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_intw(io, shadow->map_size);
    }

    if (fabs(shadow->filter) > LIB3DS_EPSILON) { /*---- CHK_SHADOW_FILTER ----*/
        Lib3dsChunk c;
        c.chunk = CHK_SHADOW_FILTER;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, shadow->filter);
    }
    if (fabs(shadow->ray_bias) > LIB3DS_EPSILON) { /*---- CHK_RAY_BIAS ----*/
        Lib3dsChunk c;
        c.chunk = CHK_RAY_BIAS;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, shadow->ray_bias);
    }
}

