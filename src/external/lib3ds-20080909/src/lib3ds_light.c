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


Lib3dsLight*
lib3ds_light_new(const char *name) {
    Lib3dsLight *light;

    assert(name);
    assert(strlen(name) < 64);

    light = (Lib3dsLight*)calloc(sizeof(Lib3dsLight), 1);
    if (!light) {
        return(0);
    }
    strcpy(light->name, name);
    return(light);
}


void
lib3ds_light_free(Lib3dsLight *light) {
    memset(light, 0, sizeof(Lib3dsLight));
    free(light);
}


static void
spotlight_read(Lib3dsLight *light, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;
    int i;

    lib3ds_chunk_read_start(&c, CHK_DL_SPOTLIGHT, io);

    light->spot_light = TRUE;
    for (i = 0; i < 3; ++i) {
        light->target[i] = lib3ds_io_read_float(io);
    }
    light->hotspot = lib3ds_io_read_float(io);
    light->falloff = lib3ds_io_read_float(io);
    lib3ds_chunk_read_tell(&c, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_DL_SPOT_ROLL: 
                light->roll = lib3ds_io_read_float(io);
                break;

            case CHK_DL_SHADOWED: {
                light->shadowed = TRUE;
                break;
            }

            case CHK_DL_LOCAL_SHADOW2: {
                light->shadow_bias = lib3ds_io_read_float(io);
                light->shadow_filter = lib3ds_io_read_float(io);
                light->shadow_size = lib3ds_io_read_intw(io);
                break;
            }

            case CHK_DL_SEE_CONE: {
                light->see_cone = TRUE;
                break;
            }

            case CHK_DL_SPOT_RECTANGULAR: {
                light->rectangular_spot = TRUE;
                break;
            }

            case CHK_DL_SPOT_ASPECT: {
                light->spot_aspect = lib3ds_io_read_float(io);
                break;
            }

            case CHK_DL_SPOT_PROJECTOR: {
                light->use_projector = TRUE;
                lib3ds_io_read_string(io, light->projector, 64);
                break;
            }

            case CHK_DL_SPOT_OVERSHOOT: {
                light->spot_overshoot = TRUE;
                break;
            }

            case CHK_DL_RAY_BIAS: {
                light->ray_bias = lib3ds_io_read_float(io);
                break;
            }

            case CHK_DL_RAYSHAD: {
                light->ray_shadows = TRUE;
                break;
            }

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


void
lib3ds_light_read(Lib3dsLight *light, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;

    lib3ds_chunk_read_start(&c, CHK_N_DIRECT_LIGHT, io);

    {
        int i;
        for (i = 0; i < 3; ++i) {
            light->position[i] = lib3ds_io_read_float(io);
        }
    }
    lib3ds_chunk_read_tell(&c, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_COLOR_F: {
                int i;
                for (i = 0; i < 3; ++i) {
                    light->color[i] = lib3ds_io_read_float(io);
                }
                break;
            }

            case CHK_DL_OFF: 
                light->off = TRUE;
                break;

            case CHK_DL_OUTER_RANGE: 
                light->outer_range = lib3ds_io_read_float(io);
                break;

            case CHK_DL_INNER_RANGE: 
                light->inner_range = lib3ds_io_read_float(io);
                break;

            case CHK_DL_MULTIPLIER: 
                light->multiplier = lib3ds_io_read_float(io);
                break;

            case CHK_DL_EXCLUDE: {
                /* FIXME: */
                lib3ds_chunk_unknown(chunk, io);
                break;
            }

            case CHK_DL_ATTENUATE: 
                light->attenuation = lib3ds_io_read_float(io);
                break;

            case CHK_DL_SPOTLIGHT: {
                lib3ds_chunk_read_reset(&c, io);
                spotlight_read(light, io);
                break;
            }

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


void
lib3ds_light_write(Lib3dsLight *light, Lib3dsIo *io) {
    Lib3dsChunk c;

    c.chunk = CHK_N_DIRECT_LIGHT;
    lib3ds_chunk_write_start(&c, io);

    lib3ds_io_write_vector(io, light->position);
    { /*---- LIB3DS_COLOR_F ----*/
        Lib3dsChunk c;
        c.chunk = CHK_COLOR_F;
        c.size = 18;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_rgb(io, light->color);
    }
    if (light->off) { /*---- LIB3DS_DL_OFF ----*/
        Lib3dsChunk c;
        c.chunk = CHK_DL_OFF;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }
    { /*---- LIB3DS_DL_OUTER_RANGE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_DL_OUTER_RANGE;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, light->outer_range);
    }
    { /*---- LIB3DS_DL_INNER_RANGE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_DL_INNER_RANGE;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, light->inner_range);
    }
    { /*---- LIB3DS_DL_MULTIPLIER ----*/
        Lib3dsChunk c;
        c.chunk = CHK_DL_MULTIPLIER;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, light->multiplier);
    }
    if (light->attenuation) { /*---- LIB3DS_DL_ATTENUATE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_DL_ATTENUATE;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (light->spot_light) {
        Lib3dsChunk c;

        c.chunk = CHK_DL_SPOTLIGHT;
        lib3ds_chunk_write_start(&c, io);

        lib3ds_io_write_vector(io, light->target);
        lib3ds_io_write_float(io, light->hotspot);
        lib3ds_io_write_float(io, light->falloff);

        { /*---- LIB3DS_DL_SPOT_ROLL ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_SPOT_ROLL;
            c.size = 10;
            lib3ds_chunk_write(&c, io);
            lib3ds_io_write_float(io, light->roll);
        }
        if (light->shadowed) { /*---- LIB3DS_DL_SHADOWED ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_SHADOWED;
            c.size = 6;
            lib3ds_chunk_write(&c, io);
        }
        if ((fabs(light->shadow_bias) > LIB3DS_EPSILON) ||
            (fabs(light->shadow_filter) > LIB3DS_EPSILON) ||
            (light->shadow_size != 0)) { /*---- LIB3DS_DL_LOCAL_SHADOW2 ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_LOCAL_SHADOW2;
            c.size = 16;
            lib3ds_chunk_write(&c, io);
            lib3ds_io_write_float(io, light->shadow_bias);
            lib3ds_io_write_float(io, light->shadow_filter);
            lib3ds_io_write_intw(io, light->shadow_size);
        }
        if (light->see_cone) { /*---- LIB3DS_DL_SEE_CONE ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_SEE_CONE;
            c.size = 6;
            lib3ds_chunk_write(&c, io);
        }
        if (light->rectangular_spot) { /*---- LIB3DS_DL_SPOT_RECTANGULAR ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_SPOT_RECTANGULAR;
            c.size = 6;
            lib3ds_chunk_write(&c, io);
        }
        if (fabs(light->spot_aspect) > LIB3DS_EPSILON) { /*---- LIB3DS_DL_SPOT_ASPECT ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_SPOT_ASPECT;
            c.size = 10;
            lib3ds_chunk_write(&c, io);
            lib3ds_io_write_float(io, light->spot_aspect);
        }
        if (light->use_projector) { /*---- LIB3DS_DL_SPOT_PROJECTOR ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_SPOT_PROJECTOR;
            c.size = 10;
            lib3ds_chunk_write(&c, io);
            lib3ds_io_write_string(io, light->projector);
        }
        if (light->spot_overshoot) { /*---- LIB3DS_DL_SPOT_OVERSHOOT ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_SPOT_OVERSHOOT;
            c.size = 6;
            lib3ds_chunk_write(&c, io);
        }
        if (fabs(light->ray_bias) > LIB3DS_EPSILON) { /*---- LIB3DS_DL_RAY_BIAS ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_RAY_BIAS;
            c.size = 10;
            lib3ds_chunk_write(&c, io);
            lib3ds_io_write_float(io, light->ray_bias);
        }
        if (light->ray_shadows) { /*---- LIB3DS_DL_RAYSHAD ----*/
            Lib3dsChunk c;
            c.chunk = CHK_DL_RAYSHAD;
            c.size = 6;
            lib3ds_chunk_write(&c, io);
        }
        lib3ds_chunk_write_end(&c, io);
    }

    lib3ds_chunk_write_end(&c, io);
}


