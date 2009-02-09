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
fog_read(Lib3dsAtmosphere *at, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;

    lib3ds_chunk_read_start(&c, CHK_FOG, io);

    at->fog_near_plane = lib3ds_io_read_float(io);
    at->fog_near_density = lib3ds_io_read_float(io);
    at->fog_far_plane = lib3ds_io_read_float(io);
    at->fog_far_density = lib3ds_io_read_float(io);
    lib3ds_chunk_read_tell(&c, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_LIN_COLOR_F: {
                int i;
                for (i = 0; i < 3; ++i) {
                    at->fog_color[i] = lib3ds_io_read_float(io);
                }
            }
            break;

            case CHK_COLOR_F:
                break;

            case CHK_FOG_BGND: {
                at->fog_background = TRUE;
            }
            break;

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


static void
layer_fog_read(Lib3dsAtmosphere *at, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;
    int have_lin = FALSE;

    lib3ds_chunk_read_start(&c, CHK_LAYER_FOG, io);

    at->layer_fog_near_y = lib3ds_io_read_float(io);
    at->layer_fog_far_y = lib3ds_io_read_float(io);
    at->layer_fog_density = lib3ds_io_read_float(io);
    at->layer_fog_flags = lib3ds_io_read_dword(io);
    lib3ds_chunk_read_tell(&c, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_LIN_COLOR_F:
                lib3ds_io_read_rgb(io, at->layer_fog_color);
                have_lin = TRUE;
                break;

            case CHK_COLOR_F:
                lib3ds_io_read_rgb(io, at->layer_fog_color);
                break;

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


static void
distance_cue_read(Lib3dsAtmosphere *at, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;

    lib3ds_chunk_read_start(&c, CHK_DISTANCE_CUE, io);

    at->dist_cue_near_plane = lib3ds_io_read_float(io);
    at->dist_cue_near_dimming = lib3ds_io_read_float(io);
    at->dist_cue_far_plane = lib3ds_io_read_float(io);
    at->dist_cue_far_dimming = lib3ds_io_read_float(io);
    lib3ds_chunk_read_tell(&c, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_DCUE_BGND: {
                at->dist_cue_background = TRUE;
            }
            break;

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


void
lib3ds_atmosphere_read(Lib3dsAtmosphere *atmosphere, Lib3dsIo *io) {
    Lib3dsChunk c;

    lib3ds_chunk_read(&c, io);
    switch (c.chunk) {
        case CHK_FOG: {
            lib3ds_chunk_read_reset(&c, io);
            fog_read(atmosphere, io);
            break;
        }

        case CHK_LAYER_FOG: {
            lib3ds_chunk_read_reset(&c, io);
            layer_fog_read(atmosphere, io);
            break;
        }

        case CHK_DISTANCE_CUE: {
            lib3ds_chunk_read_reset(&c, io);
            distance_cue_read(atmosphere, io);
            break;
        }

        case CHK_USE_FOG: {
            atmosphere->use_fog = TRUE;
            break;
        }

        case CHK_USE_LAYER_FOG: {
            atmosphere->use_layer_fog = TRUE;
            break;
        }

        case CHK_USE_DISTANCE_CUE: {
            atmosphere->use_dist_cue = TRUE;
            break;
        }
    }
}


void
lib3ds_atmosphere_write(Lib3dsAtmosphere *atmosphere, Lib3dsIo *io) {
    if (atmosphere->use_fog) { /*---- LIB3DS_FOG ----*/
        Lib3dsChunk c;
        c.chunk = CHK_FOG;
        lib3ds_chunk_write_start(&c, io);

        lib3ds_io_write_float(io, atmosphere->fog_near_plane);
        lib3ds_io_write_float(io, atmosphere->fog_near_density);
        lib3ds_io_write_float(io, atmosphere->fog_far_plane);
        lib3ds_io_write_float(io, atmosphere->fog_far_density);
        {
            Lib3dsChunk c;
            c.chunk = CHK_COLOR_F;
            c.size = 18;
            lib3ds_chunk_write(&c, io);
            lib3ds_io_write_rgb(io, atmosphere->fog_color);
        }
        if (atmosphere->fog_background) {
            Lib3dsChunk c;
            c.chunk = CHK_FOG_BGND;
            c.size = 6;
            lib3ds_chunk_write(&c, io);
        }

        lib3ds_chunk_write_end(&c, io);
    }

    if (atmosphere->use_layer_fog) { /*---- LIB3DS_LAYER_FOG ----*/
        Lib3dsChunk c;
        c.chunk = CHK_LAYER_FOG;
        c.size = 40;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, atmosphere->layer_fog_near_y);
        lib3ds_io_write_float(io, atmosphere->layer_fog_far_y);
        lib3ds_io_write_float(io, atmosphere->layer_fog_near_y);
        lib3ds_io_write_dword(io, atmosphere->layer_fog_flags);
        {
            Lib3dsChunk c;
            c.chunk = CHK_COLOR_F;
            c.size = 18;
            lib3ds_chunk_write(&c, io);
            lib3ds_io_write_rgb(io, atmosphere->fog_color);
        }
    }

    if (atmosphere->use_dist_cue) { /*---- LIB3DS_DISTANCE_CUE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_DISTANCE_CUE;
        lib3ds_chunk_write_start(&c, io);

        lib3ds_io_write_float(io, atmosphere->dist_cue_near_plane);
        lib3ds_io_write_float(io, atmosphere->dist_cue_near_dimming);
        lib3ds_io_write_float(io, atmosphere->dist_cue_far_plane);
        lib3ds_io_write_float(io, atmosphere->dist_cue_far_dimming);
        if (atmosphere->dist_cue_background) {
            Lib3dsChunk c;
            c.chunk = CHK_DCUE_BGND;
            c.size = 6;
            lib3ds_chunk_write(&c, io);
        }

        lib3ds_chunk_write_end(&c, io);
    }

    if (atmosphere->use_fog) { /*---- LIB3DS_USE_FOG ----*/
        Lib3dsChunk c;
        c.chunk = CHK_USE_FOG;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (atmosphere->use_layer_fog) { /*---- LIB3DS_USE_LAYER_FOG ----*/
        Lib3dsChunk c;
        c.chunk = CHK_USE_LAYER_FOG;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (atmosphere->use_dist_cue) { /*---- LIB3DS_USE_DISTANCE_CUE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_USE_V_GRADIENT;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }
}


