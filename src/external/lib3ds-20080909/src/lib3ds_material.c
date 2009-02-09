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
initialize_texture_map(Lib3dsTextureMap *map) {
    map->flags = 0x10;
    map->percent = 1.0f;
    map->scale[0] = 1.0f;
    map->scale[1] = 1.0f;
}


/*!
 * Creates and returns a new, empty Lib3dsMaterial object.
 *
 * Initial value of the material is a shiny grey.
 *
 * \return A pointer to the Lib3dsMaterial structure.
 *  If the structure cannot be allocated, NULL is returned.
 */
Lib3dsMaterial*
lib3ds_material_new(const char* name) {
    Lib3dsMaterial *mat;

    mat = (Lib3dsMaterial*)calloc(sizeof(Lib3dsMaterial), 1);
    if (!mat) {
        return(0);
    }

    if (name) {
        strcpy(mat->name, name);
    }
    mat->ambient[0] = mat->ambient[1] = mat->ambient[2] = 0.588235f;
    mat->diffuse[0] = mat->diffuse[1] = mat->diffuse[2] = 0.588235f;
    mat->specular[0] = mat->specular[1] = mat->specular[2] = 0.898039f;
    mat->shininess = 0.1f;
    mat->wire_size = 1.0f;
    mat->shading = 3;

    initialize_texture_map(&mat->texture1_map);
    initialize_texture_map(&mat->texture1_mask);
    initialize_texture_map(&mat->texture2_map);
    initialize_texture_map(&mat->texture2_mask);
    initialize_texture_map(&mat->opacity_map);
    initialize_texture_map(&mat->opacity_mask);
    initialize_texture_map(&mat->bump_map);
    initialize_texture_map(&mat->bump_mask);
    initialize_texture_map(&mat->specular_map);
    initialize_texture_map(&mat->specular_mask);
    initialize_texture_map(&mat->shininess_map);
    initialize_texture_map(&mat->shininess_mask);
    initialize_texture_map(&mat->self_illum_map);
    initialize_texture_map(&mat->self_illum_mask);
    initialize_texture_map(&mat->reflection_map);
    initialize_texture_map(&mat->reflection_mask);

    return(mat);
}


void
lib3ds_material_free(Lib3dsMaterial *material) {
    memset(material, 0, sizeof(Lib3dsMaterial));
    free(material);
}


static void
color_read(float rgb[3], Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;
    int have_lin = FALSE;

    lib3ds_chunk_read_start(&c, 0, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_LIN_COLOR_24: {
                int i;
                for (i = 0; i < 3; ++i) {
                    rgb[i] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                }
                have_lin = TRUE;
                break;
            }

            case CHK_COLOR_24: {
                /* gamma corrected color chunk
                   replaced in 3ds R3 by LIN_COLOR_24 */
                if (!have_lin) {
                    int i;
                    for (i = 0; i < 3; ++i) {
                        rgb[i] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                    }
                }
                break;
            }

            case CHK_LIN_COLOR_F: {
                int i;
                for (i = 0; i < 3; ++i) {
                    rgb[i] = lib3ds_io_read_float(io);
                }
                have_lin = TRUE;
                break;
            }

            case CHK_COLOR_F: {
                if (!have_lin) {
                    int i;
                    for (i = 0; i < 3; ++i) {
                        rgb[i] = lib3ds_io_read_float(io);
                    }
                }
                break;
            }

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


static void
int_percentage_read(float *p, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;

    lib3ds_chunk_read_start(&c, 0, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_INT_PERCENTAGE: {
                int16_t i = lib3ds_io_read_intw(io);
                *p = (float)(1.0 * i / 100.0);
                break;
            }

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


static void
texture_map_read(Lib3dsTextureMap *map, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;

    lib3ds_chunk_read_start(&c, 0, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_INT_PERCENTAGE: {
                map->percent = 1.0f * lib3ds_io_read_intw(io) / 100.0f;
                break;
            }

            case CHK_MAT_MAPNAME: {
                lib3ds_io_read_string(io, map->name, 64);
                lib3ds_io_log(io, LIB3DS_LOG_INFO, "  NAME=%s", map->name);
                break;
            }

            case CHK_MAT_MAP_TILING: {
                map->flags = lib3ds_io_read_word(io);
                break;
            }

            case CHK_MAT_MAP_TEXBLUR: 
                map->blur = lib3ds_io_read_float(io);
                break;

            case CHK_MAT_MAP_USCALE:
                map->scale[0] = lib3ds_io_read_float(io);
                break;

            case CHK_MAT_MAP_VSCALE: {
                map->scale[1] = lib3ds_io_read_float(io);
                break;
            }
            case CHK_MAT_MAP_UOFFSET: {
                map->offset[0] = lib3ds_io_read_float(io);
                break;
            }

            case CHK_MAT_MAP_VOFFSET: {
                map->offset[1] = lib3ds_io_read_float(io);
                break;
            }

            case CHK_MAT_MAP_ANG: {
                map->rotation = lib3ds_io_read_float(io);
                break;
            }

            case CHK_MAT_MAP_COL1: {
                map->tint_1[0] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_1[1] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_1[2] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                break;
            }

            case CHK_MAT_MAP_COL2: {
                map->tint_2[0] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_2[1] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_2[2] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                break;
            }

            case CHK_MAT_MAP_RCOL: {
                map->tint_r[0] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_r[1] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_r[2] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                break;
            }

            case CHK_MAT_MAP_GCOL: {
                map->tint_g[0] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_g[1] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_g[2] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                break;
            }

            case CHK_MAT_MAP_BCOL: {
                map->tint_b[0] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_b[1] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                map->tint_b[2] = 1.0f * lib3ds_io_read_byte(io) / 255.0f;
                break;
            }

            default:
                lib3ds_chunk_unknown(chunk,io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


void
lib3ds_material_read(Lib3dsMaterial *material, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;

    assert(material);
    lib3ds_chunk_read_start(&c, CHK_MAT_ENTRY, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_MAT_NAME: {
                lib3ds_io_read_string(io, material->name, 64);
                lib3ds_io_log(io, LIB3DS_LOG_INFO, "  NAME=%s", material->name);
                break;
            }

            case CHK_MAT_AMBIENT: {
                lib3ds_chunk_read_reset(&c, io);
                color_read(material->ambient, io);
                break;
            }

            case CHK_MAT_DIFFUSE: {
                lib3ds_chunk_read_reset(&c, io);
                color_read(material->diffuse, io);
                break;
            }

            case CHK_MAT_SPECULAR: {
                lib3ds_chunk_read_reset(&c, io);
                color_read(material->specular, io);
                break;
            }

            case CHK_MAT_SHININESS: {
                lib3ds_chunk_read_reset(&c, io);
                int_percentage_read(&material->shininess, io);
                break;
            }

            case CHK_MAT_SHIN2PCT: {
                lib3ds_chunk_read_reset(&c, io);
                int_percentage_read(&material->shin_strength, io);
                break;
            }

            case CHK_MAT_TRANSPARENCY: {
                lib3ds_chunk_read_reset(&c, io);
                int_percentage_read(&material->transparency, io);
                break;
            }

            case CHK_MAT_XPFALL: {
                lib3ds_chunk_read_reset(&c, io);
                int_percentage_read(&material->falloff, io);
                break;
            }

            case CHK_MAT_SELF_ILPCT: {
                lib3ds_chunk_read_reset(&c, io);
                int_percentage_read(&material->self_illum, io);
                break;
            }

            case CHK_MAT_USE_XPFALL: {
                material->use_falloff = TRUE;
                break;
            }

            case CHK_MAT_REFBLUR: {
                lib3ds_chunk_read_reset(&c, io);
                int_percentage_read(&material->blur, io);
                break;
            }

            case CHK_MAT_USE_REFBLUR: {
                material->use_blur = TRUE;
                break;
            }

            case CHK_MAT_SHADING: {
                material->shading = lib3ds_io_read_intw(io);
                break;
            }

            case CHK_MAT_SELF_ILLUM: {
                material->self_illum_flag = TRUE;
                break;
            }

            case CHK_MAT_TWO_SIDE: {
                material->two_sided = TRUE;
                break;
            }

            case CHK_MAT_DECAL: {
                material->map_decal = TRUE;
                break;
            }

            case CHK_MAT_ADDITIVE: {
                material->is_additive = TRUE;
                break;
            }

            case CHK_MAT_FACEMAP: {
                material->face_map = TRUE;
                break;
            }

            case CHK_MAT_PHONGSOFT: {
                material->soften = TRUE;
                break;
            }

            case CHK_MAT_WIRE: {
                material->use_wire = TRUE;
                break;
            }

            case CHK_MAT_WIREABS: {
                material->use_wire_abs = TRUE;
                break;
            }
            case CHK_MAT_WIRE_SIZE: {
                material->wire_size = lib3ds_io_read_float(io);
                break;
            }

            case CHK_MAT_TEXMAP: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->texture1_map, io);
                break;
            }

            case CHK_MAT_TEXMASK: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->texture1_mask, io);
                break;
            }

            case CHK_MAT_TEX2MAP: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->texture2_map, io);
                break;
            }

            case CHK_MAT_TEX2MASK: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->texture2_mask, io);
                break;
            }

            case CHK_MAT_OPACMAP: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->opacity_map, io);
                break;
            }

            case CHK_MAT_OPACMASK: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->opacity_mask, io);
                break;
            }

            case CHK_MAT_BUMPMAP: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->bump_map, io);
                break;
            }
            case CHK_MAT_BUMPMASK: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->bump_mask, io);
                break;
            }
            case CHK_MAT_SPECMAP: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->specular_map, io);
                break;
            }

            case CHK_MAT_SPECMASK: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->specular_mask, io);
                break;
            }

            case CHK_MAT_SHINMAP: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->shininess_map, io);
                break;
            }

            case CHK_MAT_SHINMASK: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->shininess_mask, io);
                break;
            }

            case CHK_MAT_SELFIMAP: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->self_illum_map, io);
                break;
            }

            case CHK_MAT_SELFIMASK: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->self_illum_mask, io);
                break;
            }

            case CHK_MAT_REFLMAP: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->reflection_map, io);
                break;
            }

            case CHK_MAT_REFLMASK: {
                lib3ds_chunk_read_reset(&c, io);
                texture_map_read(&material->reflection_mask, io);
                break;
            }

            case CHK_MAT_ACUBIC: {
                lib3ds_io_read_intb(io);
                material->autorefl_map_anti_alias = lib3ds_io_read_intb(io);
                material->autorefl_map_flags = lib3ds_io_read_intw(io);
                material->autorefl_map_size = lib3ds_io_read_intd(io);
                material->autorefl_map_frame_step = lib3ds_io_read_intd(io);
                break;
            }

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


static void
color_write(float rgb[3], Lib3dsIo *io) {
    Lib3dsChunk c;

    c.chunk = CHK_COLOR_24;
    c.size = 9;
    lib3ds_chunk_write(&c, io);
    lib3ds_io_write_byte(io, (uint8_t)floor(255.0*rgb[0] + 0.5));
    lib3ds_io_write_byte(io, (uint8_t)floor(255.0*rgb[1] + 0.5));
    lib3ds_io_write_byte(io, (uint8_t)floor(255.0*rgb[2] + 0.5));

    c.chunk = CHK_LIN_COLOR_24;
    c.size = 9;
    lib3ds_chunk_write(&c, io);
    lib3ds_io_write_byte(io, (uint8_t)floor(255.0*rgb[0] + 0.5));
    lib3ds_io_write_byte(io, (uint8_t)floor(255.0*rgb[1] + 0.5));
    lib3ds_io_write_byte(io, (uint8_t)floor(255.0*rgb[2] + 0.5));
}


static void
int_percentage_write(float p, Lib3dsIo *io) {
    Lib3dsChunk c;

    c.chunk = CHK_INT_PERCENTAGE;
    c.size = 8;
    lib3ds_chunk_write(&c, io);
    lib3ds_io_write_intw(io, (uint8_t)floor(100.0*p + 0.5));
}


static void
texture_map_write(uint16_t chunk, Lib3dsTextureMap *map, Lib3dsIo *io) {
    Lib3dsChunk c;

    if (strlen(map->name) == 0) {
        return;
    }
    c.chunk = chunk;
    lib3ds_chunk_write_start(&c, io);

    int_percentage_write(map->percent, io);

    { /*---- CHK_MAT_MAPNAME ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAPNAME;
        c.size = 6 + (uint32_t)strlen(map->name) + 1;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_string(io, map->name);
    }

    { /*---- CHK_MAT_MAP_TILING ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_TILING;
        c.size = 8;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_word(io, (uint16_t)map->flags);
    }

    { /*---- CHK_MAT_MAP_TEXBLUR ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_TEXBLUR;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, map->blur);
    }

    { /*---- CHK_MAT_MAP_USCALE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_USCALE;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, map->scale[0]);
    }

    { /*---- CHK_MAT_MAP_VSCALE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_VSCALE;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, map->scale[1]);
    }

    { /*---- CHK_MAT_MAP_UOFFSET ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_UOFFSET;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, map->offset[0]);
    }

    { /*---- CHK_MAT_MAP_VOFFSET ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_VOFFSET;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, map->offset[1]);
    }

    { /*---- CHK_MAT_MAP_ANG ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_ANG;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, map->rotation);
    }

    { /*---- CHK_MAT_MAP_COL1 ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_COL1;
        c.size = 9;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_1[0] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_1[1] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_1[2] + 0.5));
    }

    { /*---- CHK_MAT_MAP_COL2 ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_COL2;
        c.size = 9;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_2[0] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_2[1] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_2[2] + 0.5));
    }

    { /*---- CHK_MAT_MAP_RCOL ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_RCOL;
        c.size = 9;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_r[0] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_r[1] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_r[2] + 0.5));
    }

    { /*---- CHK_MAT_MAP_GCOL ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_GCOL;
        c.size = 9;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_g[0] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_g[1] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_g[2] + 0.5));
    }

    { /*---- CHK_MAT_MAP_BCOL ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_MAP_BCOL;
        c.size = 9;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_b[0] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_b[1] + 0.5));
        lib3ds_io_write_byte(io, (uint8_t)floor(255.0*map->tint_b[2] + 0.5));
    }

    lib3ds_chunk_write_end(&c, io);
}


void
lib3ds_material_write(Lib3dsMaterial *material, Lib3dsIo *io) {
    Lib3dsChunk c;

    c.chunk = CHK_MAT_ENTRY;
    lib3ds_chunk_write_start(&c, io);

    { /*---- CHK_MAT_NAME ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_NAME;
        c.size = 6 + (uint32_t)strlen(material->name) + 1;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_string(io, material->name);
    }

    { /*---- CHK_MAT_AMBIENT ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_AMBIENT;
        c.size = 24;
        lib3ds_chunk_write(&c, io);
        color_write(material->ambient, io);
    }

    { /*---- CHK_MAT_DIFFUSE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_DIFFUSE;
        c.size = 24;
        lib3ds_chunk_write(&c, io);
        color_write(material->diffuse, io);
    }

    { /*---- CHK_MAT_SPECULAR ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_SPECULAR;
        c.size = 24;
        lib3ds_chunk_write(&c, io);
        color_write(material->specular, io);
    }

    { /*---- CHK_MAT_SHININESS ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_SHININESS;
        c.size = 14;
        lib3ds_chunk_write(&c, io);
        int_percentage_write(material->shininess, io);
    }

    { /*---- CHK_MAT_SHIN2PCT ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_SHIN2PCT;
        c.size = 14;
        lib3ds_chunk_write(&c, io);
        int_percentage_write(material->shin_strength, io);
    }

    { /*---- CHK_MAT_TRANSPARENCY ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_TRANSPARENCY;
        c.size = 14;
        lib3ds_chunk_write(&c, io);
        int_percentage_write(material->transparency, io);
    }

    { /*---- CHK_MAT_XPFALL ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_XPFALL;
        c.size = 14;
        lib3ds_chunk_write(&c, io);
        int_percentage_write(material->falloff, io);
    }

    if (material->use_falloff) { /*---- CHK_MAT_USE_XPFALL ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_USE_XPFALL;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    { /*---- CHK_MAT_SHADING ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_SHADING;
        c.size = 8;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_intw(io, material->shading);
    }

    { /*---- CHK_MAT_REFBLUR ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_REFBLUR;
        c.size = 14;
        lib3ds_chunk_write(&c, io);
        int_percentage_write(material->blur, io);
    }

    if (material->use_blur) { /*---- CHK_MAT_USE_REFBLUR ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_USE_REFBLUR;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (material->self_illum_flag) { /*---- CHK_MAT_SELF_ILLUM ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_SELF_ILLUM;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (material->two_sided) { /*---- CHK_MAT_TWO_SIDE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_TWO_SIDE;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (material->map_decal) { /*---- CHK_MAT_DECAL ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_DECAL;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (material->is_additive) { /*---- CHK_MAT_ADDITIVE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_ADDITIVE;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (material->use_wire) { /*---- CHK_MAT_WIRE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_WIRE;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (material->use_wire_abs) { /*---- CHK_MAT_WIREABS ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_WIREABS;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    { /*---- CHK_MAT_WIRE_SIZE ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_WIRE_SIZE;
        c.size = 10;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_float(io, material->wire_size);
    }

    if (material->face_map) { /*---- CHK_MAT_FACEMAP ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_FACEMAP;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    if (material->soften) { /*---- CHK_MAT_PHONGSOFT ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_PHONGSOFT;
        c.size = 6;
        lib3ds_chunk_write(&c, io);
    }

    texture_map_write(CHK_MAT_TEXMAP, &material->texture1_map, io);
    texture_map_write(CHK_MAT_TEXMASK, &material->texture1_mask, io);
    texture_map_write(CHK_MAT_TEX2MAP, &material->texture2_map, io);
    texture_map_write(CHK_MAT_TEX2MASK, &material->texture2_mask, io);
    texture_map_write(CHK_MAT_OPACMAP, &material->opacity_map, io);
    texture_map_write(CHK_MAT_OPACMASK, &material->opacity_mask, io);
    texture_map_write(CHK_MAT_BUMPMAP, &material->bump_map, io);
    texture_map_write(CHK_MAT_BUMPMASK, &material->bump_mask, io);
    texture_map_write(CHK_MAT_SPECMAP, &material->specular_map, io);
    texture_map_write(CHK_MAT_SPECMASK, &material->specular_mask, io);
    texture_map_write(CHK_MAT_SHINMAP, &material->shininess_map, io);
    texture_map_write(CHK_MAT_SHINMASK, &material->shininess_mask, io);
    texture_map_write(CHK_MAT_SELFIMAP, &material->self_illum_map, io);
    texture_map_write(CHK_MAT_SELFIMASK, &material->self_illum_mask, io);
    texture_map_write(CHK_MAT_REFLMAP,  &material->reflection_map, io);
    texture_map_write(CHK_MAT_REFLMASK,  &material->reflection_mask, io);

    { /*---- CHK_MAT_ACUBIC ----*/
        Lib3dsChunk c;
        c.chunk = CHK_MAT_ACUBIC;
        c.size = 18;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_intb(io, 0);
        lib3ds_io_write_intb(io, material->autorefl_map_anti_alias);
        lib3ds_io_write_intw(io, material->autorefl_map_flags);
        lib3ds_io_write_intd(io, material->autorefl_map_size);
        lib3ds_io_write_intd(io, material->autorefl_map_frame_step);
    }

    lib3ds_chunk_write_end(&c, io);
}
