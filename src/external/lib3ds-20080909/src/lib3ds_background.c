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


static void
solid_bgnd_read(Lib3dsBackground *background, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;
    int have_lin = FALSE;

    lib3ds_chunk_read_start(&c, CHK_SOLID_BGND, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_LIN_COLOR_F:
                lib3ds_io_read_rgb(io, background->solid_color);
                have_lin = TRUE;
                break;

            case CHK_COLOR_F:
                lib3ds_io_read_rgb(io, background->solid_color);
                break;

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


static void
v_gradient_read(Lib3dsBackground *background, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;
    int index[2];
    float col[2][3][3];
    int have_lin = 0;

    lib3ds_chunk_read_start(&c, CHK_V_GRADIENT, io);

    background->gradient_percent = lib3ds_io_read_float(io);
    lib3ds_chunk_read_tell(&c, io);

    index[0] = index[1] = 0;
    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_COLOR_F:
                lib3ds_io_read_rgb(io, col[0][index[0]]);
                index[0]++;
                break;

            case CHK_LIN_COLOR_F:
                lib3ds_io_read_rgb(io, col[1][index[1]]);
                index[1]++;
                have_lin = 1;
                break;

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }
    {
        int i;
        for (i = 0; i < 3; ++i) {
            background->gradient_top[i] = col[have_lin][0][i];
            background->gradient_middle[i] = col[have_lin][1][i];
            background->gradient_bottom[i] = col[have_lin][2][i];
        }
    }
    lib3ds_chunk_read_end(&c, io);
}


void
lib3ds_background_read(Lib3dsBackground *background, Lib3dsIo *io) {
    Lib3dsChunk c;

    lib3ds_chunk_read(&c, io);
    switch (c.chunk) {
        case CHK_BIT_MAP: {
            lib3ds_io_read_string(io, background->bitmap_name, 64);
            break;
        }

        case CHK_SOLID_BGND: {
            lib3ds_chunk_read_reset(&c, io);
            solid_bgnd_read(background, io);
            break;
        }

        case CHK_V_GRADIENT: {
            lib3ds_chunk_read_reset(&c, io);
            v_gradient_read(background, io);
            break;
        }

        case CHK_USE_BIT_MAP: {
            background->use_bitmap = TRUE;
            break;
        }

        case CHK_USE_SOLID_BGND: {
            background->use_solid = TRUE;
            break;
        }

        case CHK_USE_V_GRADIENT: {
            background->use_gradient = TRUE;
            break;
        }
    }
}


static void
colorf_write(float rgb[3], Lib3dsIo *io) {
    Lib3dsChunk c;

    c.chunk = CHK_COLOR_F;
    c.size = 18;
    lib3ds_chunk_write(&c, io);
    lib3ds_io_write_rgb(io, rgb);

    c.chunk = CHK_LIN_COLOR_F;
    c.size = 18;
    lib3ds_chunk_write(&c, io);
    lib3ds_io_write_rgb(io, rgb);
}


static int
colorf_defined(float rgb[3]) {
    int i;
    for (i = 0; i < 3; ++i) {
        if (fabs(rgb[i]) > LIB3DS_EPSILON) {
            break;
        }
    }
    return(i < 3);
}


void
lib3ds_background_write(Lib3dsBackground *background, Lib3dsIo *io) {
    if (strlen(background->bitmap_name)) { /*---- LIB3DS_BIT_MAP ----*/
        Lib3dsChunk c;
        c.chunk = CHK_BIT_MAP;
        c.size = 6 + 1 + (uint32_t)strlen(background->bitmap_name);
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_string(io, background->bitmap_name);
    }

    if (colorf_defined(background->solid_color)) { /*---- LIB3DS_SOLID_BGND ----*/
        Lib3dsChunk c;
        c.chunk = CHK_SOLID_BGND;
        c.size = 42;
        lib3ds_chunk_write(&c, io);
        colorf_write(background->solid_color, io);
    }

    if (colorf_defined(background->gradient_top) ||
        colorf_defined(background->gradient_middle) ||
        colorf_defined(background->gradient_bottom)) { /*---- LIB3DS_V_GRADIENT ----*/
        Lib3dsChunk c;
        c.chunk = CHK_V_GRADIENT;
        c.size = 118;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, background->gradient_percent);
        colorf_write(background->gradient_top, io);
        colorf_write(background->gradient_middle, io);
        colorf_write(background->gradient_bottom, io);
    }

    if (background->use_bitmap) { /*---- LIB3DS_USE_BIT_MAP ----*/
        Lib3dsChunk c;
        c.chunk = CHK_USE_BIT_MAP;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (background->use_solid) { /*---- LIB3DS_USE_SOLID_BGND ----*/
        Lib3dsChunk c;
        c.chunk = CHK_USE_SOLID_BGND;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (background->use_gradient) { /*---- LIB3DS_USE_V_GRADIENT ----*/
        Lib3dsChunk c;
        c.chunk = CHK_USE_V_GRADIENT;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }
}

