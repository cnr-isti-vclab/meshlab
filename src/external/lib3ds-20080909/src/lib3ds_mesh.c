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


/*!
 * Create and return a new empty mesh object.
 *
 * Mesh is initialized with the name and an identity matrix; all
 * other fields are zero.
 *
 * See Lib3dsFaceFlag for definitions of per-face flags.
 *
 * \param name Mesh name.  Must not be NULL.  Must be < 64 characters.
 *
 * \return mesh object or NULL on error.
 */
Lib3dsMesh*
lib3ds_mesh_new(const char *name) {
    Lib3dsMesh *mesh;

    assert(name);
    assert(strlen(name) < 64);

    mesh = (Lib3dsMesh*) calloc(sizeof(Lib3dsMesh), 1);
    if (!mesh) {
        return (0);
    }
    strcpy(mesh->name, name);
    lib3ds_matrix_identity(mesh->matrix);
    mesh->map_type = LIB3DS_MAP_NONE;
    return (mesh);
}


/*!
 * Free a mesh object and all of its resources.
 *
 * \param mesh Mesh object to be freed.
 */
void
lib3ds_mesh_free(Lib3dsMesh *mesh) {
    lib3ds_mesh_resize_vertices(mesh, 0, 0, 0);
    lib3ds_mesh_resize_faces(mesh, 0);
    memset(mesh, 0, sizeof(Lib3dsMesh));
    free(mesh);
}


void
lib3ds_mesh_resize_vertices(Lib3dsMesh *mesh, int nvertices, int use_texcos, int use_flags) {
    assert(mesh);
    mesh->vertices = lib3ds_util_realloc_array(mesh->vertices, mesh->nvertices, nvertices, 3 * sizeof(float));
    mesh->texcos = lib3ds_util_realloc_array(
        mesh->texcos, 
        mesh->texcos? mesh->nvertices : 0, 
        use_texcos? nvertices : 0, 
        2 * sizeof(float)
    );
    mesh->vflags = lib3ds_util_realloc_array(
        mesh->vflags, 
        mesh->vflags? mesh->nvertices : 0, 
        use_flags? nvertices : 0, 
        2 * sizeof(float)
    );
    mesh->nvertices = nvertices;
}


void 
lib3ds_mesh_resize_faces(Lib3dsMesh *mesh, int nfaces) {
    int i;
    assert(mesh);
    mesh->faces = lib3ds_util_realloc_array(mesh->faces, mesh->nfaces, nfaces, sizeof(Lib3dsFace));
    for (i = mesh->nfaces; i < nfaces; ++i) {
        mesh->faces[i].material = -1;
    }
    mesh->nfaces = nfaces;
}


/*!
 * Find the bounding box of a mesh object.
 *
 * \param mesh The mesh object
 * \param bmin Returned bounding box
 * \param bmax Returned bounding box
 */
void
lib3ds_mesh_bounding_box(Lib3dsMesh *mesh, float bmin[3], float bmax[3]) {
    int i;
    bmin[0] = bmin[1] = bmin[2] = FLT_MAX;
    bmax[0] = bmax[1] = bmax[2] = -FLT_MAX;

    for (i = 0; i < mesh->nvertices; ++i) {
        lib3ds_vector_min(bmin, mesh->vertices[i]);
        lib3ds_vector_max(bmax, mesh->vertices[i]);
    }
}


void
lib3ds_mesh_calculate_face_normals(Lib3dsMesh *mesh, float (*face_normals)[3]) {
    int i;

    if (!mesh->nfaces) {
        return;
    }
    for (i = 0; i < mesh->nfaces; ++i) {
        lib3ds_vector_normal(
            face_normals[i],
            mesh->vertices[mesh->faces[i].index[0]],
            mesh->vertices[mesh->faces[i].index[1]],
            mesh->vertices[mesh->faces[i].index[2]]
        );
    }
}


typedef struct Lib3dsFaces {
    struct Lib3dsFaces *next;
    int index;
    float normal[3];
} Lib3dsFaces;


/*!
 * Calculates the vertex normals corresponding to the smoothing group
 * settings for each face of a mesh.
 *
 * \param mesh      A pointer to the mesh to calculate the normals for.
 * \param normals   A pointer to a buffer to store the calculated
 *                  normals. The buffer must have the size:
 *                  3*3*sizeof(float)*mesh->nfaces.
 *
 * To allocate the normal buffer do for example the following:
 * \code
 *  Lib3dsVector *normals = malloc(3*3*sizeof(float)*mesh->nfaces);
 * \endcode
 *
 * To access the normal of the i-th vertex of the j-th face do the
 * following:
 * \code
 *   normals[3*j+i]
 * \endcode
 */
void
lib3ds_mesh_calculate_vertex_normals(Lib3dsMesh *mesh, float (*normals)[3]) {
    Lib3dsFaces **fl;
    Lib3dsFaces *fa;
    int i, j;

    if (!mesh->nfaces) {
        return;
    }

    fl = calloc(sizeof(Lib3dsFaces*), mesh->nvertices);
    fa = malloc(sizeof(Lib3dsFaces) * 3 * mesh->nfaces);

    for (i = 0; i < mesh->nfaces; ++i) {
        for (j = 0; j < 3; ++j) {
            Lib3dsFaces* l = &fa[3*i+j];
            float p[3], q[3], n[3];
            float len, weight;

            l->index = i;
            l->next = fl[mesh->faces[i].index[j]];
            fl[mesh->faces[i].index[j]] = l;

            lib3ds_vector_sub(p, mesh->vertices[mesh->faces[i].index[j<2? j + 1 : 0]], mesh->vertices[mesh->faces[i].index[j]]);
            lib3ds_vector_sub(q, mesh->vertices[mesh->faces[i].index[j>0? j - 1 : 2]], mesh->vertices[mesh->faces[i].index[j]]);
            lib3ds_vector_cross(n, p, q);
            len = lib3ds_vector_length(n);
            if (len > 0) {       
                weight = (float)atan2(len, lib3ds_vector_dot(p, q));
                lib3ds_vector_scalar_mul(l->normal, n, weight / len);
            } else {
                lib3ds_vector_zero(l->normal);
            }
        }
    }

    for (i = 0; i < mesh->nfaces; ++i) {
        Lib3dsFace *f = &mesh->faces[i];
        for (j = 0; j < 3; ++j) {
            float n[3];
            Lib3dsFaces *p;
            Lib3dsFace *pf;

            assert(mesh->faces[i].index[j] < mesh->nvertices);

            if (f->smoothing_group) {
                unsigned smoothing_group = f->smoothing_group;

                lib3ds_vector_zero(n);
                for (p = fl[mesh->faces[i].index[j]]; p; p = p->next) {
                    pf = &mesh->faces[p->index];
                    if (pf->smoothing_group & f->smoothing_group)
                        smoothing_group |= pf->smoothing_group;
                }

                for (p = fl[mesh->faces[i].index[j]]; p; p = p->next) {
                    pf = &mesh->faces[p->index];                
                    if (smoothing_group & pf->smoothing_group) {
                        lib3ds_vector_add(n, n, p->normal);
                    }
                }
            } else {
                lib3ds_vector_copy(n, fa[3*i+j].normal);
            }

            lib3ds_vector_normalize(n);
            lib3ds_vector_copy(normals[3*i+j], n);
        }
    }

    free(fa);
    free(fl);
}


static void
face_array_read(Lib3dsFile *file, Lib3dsMesh *mesh, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;
    int i;
    uint16_t nfaces;

    lib3ds_chunk_read_start(&c, CHK_FACE_ARRAY, io);

    lib3ds_mesh_resize_faces(mesh, 0);
    nfaces = lib3ds_io_read_word(io);
    if (nfaces) {
        lib3ds_mesh_resize_faces(mesh, nfaces);
        for (i = 0; i < nfaces; ++i) {
            mesh->faces[i].index[0] = lib3ds_io_read_word(io);
            mesh->faces[i].index[1] = lib3ds_io_read_word(io);
            mesh->faces[i].index[2] = lib3ds_io_read_word(io);
            mesh->faces[i].flags = lib3ds_io_read_word(io);
        }
        lib3ds_chunk_read_tell(&c, io);

        while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
            switch (chunk) {
                case CHK_MSH_MAT_GROUP: {
                    char name[64];
                    unsigned n;
                    unsigned i;
                    int index;
                    int material;

                    lib3ds_io_read_string(io, name, 64);
                    material = lib3ds_file_material_by_name(file, name);

                    n = lib3ds_io_read_word(io);
                    for (i = 0; i < n; ++i) {
                        index = lib3ds_io_read_word(io);
                        if (index < mesh->nfaces) {
                            mesh->faces[index].material = material;
                        } else {
                            // TODO warning
                        }
                    }
                    break;
                }

                case CHK_SMOOTH_GROUP: {
                    int i;
                    for (i = 0; i < mesh->nfaces; ++i) {
                        mesh->faces[i].smoothing_group = lib3ds_io_read_dword(io);
                    }
                    break;
                }

                case CHK_MSH_BOXMAP: {
                    lib3ds_io_read_string(io, mesh->box_front, 64);
                    lib3ds_io_read_string(io, mesh->box_back, 64);
                    lib3ds_io_read_string(io, mesh->box_left, 64);
                    lib3ds_io_read_string(io, mesh->box_right, 64);
                    lib3ds_io_read_string(io, mesh->box_top, 64);
                    lib3ds_io_read_string(io, mesh->box_bottom, 64);
                    break;
                }

                default:
                    lib3ds_chunk_unknown(chunk,io);
            }
        }

    }
    lib3ds_chunk_read_end(&c, io);
}


void
lib3ds_mesh_read(Lib3dsFile *file, Lib3dsMesh *mesh, Lib3dsIo *io) {
    Lib3dsChunk c;
    uint16_t chunk;

    lib3ds_chunk_read_start(&c, CHK_N_TRI_OBJECT, io);

    while ((chunk = lib3ds_chunk_read_next(&c, io)) != 0) {
        switch (chunk) {
            case CHK_MESH_MATRIX: {
                int i, j;

                lib3ds_matrix_identity(mesh->matrix);
                for (i = 0; i < 4; i++) {
                    for (j = 0; j < 3; j++) {
                        mesh->matrix[i][j] = lib3ds_io_read_float(io);
                    }
                }
                break;
            }

            case CHK_MESH_COLOR: {
                mesh->color = lib3ds_io_read_byte(io);
                break;
            }

            case CHK_POINT_ARRAY: {
                int i;
                uint16_t nvertices = lib3ds_io_read_word(io);
                lib3ds_mesh_resize_vertices(mesh, nvertices, mesh->texcos != NULL, mesh->vflags != NULL);
                for (i = 0; i < mesh->nvertices; ++i) {
                    lib3ds_io_read_vector(io, mesh->vertices[i]);
                }
                break;
            }

            case CHK_POINT_FLAG_ARRAY: {
                int i;
                uint16_t nflags = lib3ds_io_read_word(io);
                uint16_t nvertices = (mesh->nvertices >= nflags)? mesh->nvertices : nflags;
                lib3ds_mesh_resize_vertices(mesh, nvertices, mesh->texcos != NULL, 1);
                for (i = 0; i < nflags; ++i) {
                    mesh->vflags[i] = lib3ds_io_read_word(io);
                }
                break;
            }

            case CHK_FACE_ARRAY: {
                lib3ds_chunk_read_reset(&c, io);
                face_array_read(file, mesh, io);
                break;
            }

            case CHK_MESH_TEXTURE_INFO: {
                int i, j;

                //FIXME: mesh->map_type = lib3ds_io_read_word(io);

                for (i = 0; i < 2; ++i) {
                    mesh->map_tile[i] = lib3ds_io_read_float(io);
                }
                for (i = 0; i < 3; ++i) {
                    mesh->map_pos[i] = lib3ds_io_read_float(io);
                }
                mesh->map_scale = lib3ds_io_read_float(io);

                lib3ds_matrix_identity(mesh->map_matrix);
                for (i = 0; i < 4; i++) {
                    for (j = 0; j < 3; j++) {
                        mesh->map_matrix[i][j] = lib3ds_io_read_float(io);
                    }
                }
                for (i = 0; i < 2; ++i) {
                    mesh->map_planar_size[i] = lib3ds_io_read_float(io);
                }
                mesh->map_cylinder_height = lib3ds_io_read_float(io);
                break;
            }

            case CHK_TEX_VERTS: {
                int i;
                uint16_t ntexcos = lib3ds_io_read_word(io);
                uint16_t nvertices = (mesh->nvertices >= ntexcos)? mesh->nvertices : ntexcos;;
                if (!mesh->texcos) {
                    lib3ds_mesh_resize_vertices(mesh, nvertices, 1, mesh->vflags != NULL);
                }
                for (i = 0; i < ntexcos; ++i) {
                    mesh->texcos[i][0] = lib3ds_io_read_float(io);
                    mesh->texcos[i][1] = lib3ds_io_read_float(io);
                }
                break;
            }

            default:
                lib3ds_chunk_unknown(chunk, io);
        }
    }

    if (lib3ds_matrix_det(mesh->matrix) < 0.0) {
        /* Flip X coordinate of vertices if mesh matrix
           has negative determinant */
        float inv_matrix[4][4], M[4][4];
        float tmp[3];
        int i;

        lib3ds_matrix_copy(inv_matrix, mesh->matrix);
        lib3ds_matrix_inv(inv_matrix);

        lib3ds_matrix_copy(M, mesh->matrix);
        lib3ds_matrix_scale(M, -1.0f, 1.0f, 1.0f);
        lib3ds_matrix_mult(M, M, inv_matrix);

        for (i = 0; i < mesh->nvertices; ++i) {
            lib3ds_vector_transform(tmp, M, mesh->vertices[i]);
            lib3ds_vector_copy(mesh->vertices[i], tmp);
        }
    }

    lib3ds_chunk_read_end(&c, io);
}


static void
point_array_write(Lib3dsMesh *mesh, Lib3dsIo *io) {
    Lib3dsChunk c;
    int i;

    c.chunk = CHK_POINT_ARRAY;
    c.size = 8 + 12 * mesh->nvertices;
    lib3ds_chunk_write(&c, io);

    lib3ds_io_write_word(io, (uint16_t) mesh->nvertices);

    if (lib3ds_matrix_det(mesh->matrix) >= 0.0f) {
        for (i = 0; i < mesh->nvertices; ++i) {
            lib3ds_io_write_vector(io, mesh->vertices[i]);
        }
    } else {
        /* Flip X coordinate of vertices if mesh matrix
           has negative determinant */
        float inv_matrix[4][4], M[4][4];
        float tmp[3];

        lib3ds_matrix_copy(inv_matrix, mesh->matrix);
        lib3ds_matrix_inv(inv_matrix);
        lib3ds_matrix_copy(M, mesh->matrix);
        lib3ds_matrix_scale(M, -1.0f, 1.0f, 1.0f);
        lib3ds_matrix_mult(M, M, inv_matrix);

        for (i = 0; i < mesh->nvertices; ++i) {
            lib3ds_vector_transform(tmp, M, mesh->vertices[i]);
            lib3ds_io_write_vector(io, tmp);
        }
    }
}


static void
flag_array_write(Lib3dsMesh *mesh, Lib3dsIo *io) {
    Lib3dsChunk c;
    int i;

    if (!mesh->vflags) {
        return;
    }

    c.chunk = CHK_POINT_FLAG_ARRAY;
    c.size = 8 + 2 * mesh->nvertices;
    lib3ds_chunk_write(&c, io);

    lib3ds_io_write_word(io, (uint16_t) mesh->nvertices);
    for (i = 0; i < mesh->nvertices; ++i) {
        lib3ds_io_write_word(io, mesh->vflags[i]);
    }
}


static void
face_array_write(Lib3dsFile *file, Lib3dsMesh *mesh, Lib3dsIo *io) {
    Lib3dsChunk c;

    if (mesh->nfaces == 0) {
        return;
    }
    c.chunk = CHK_FACE_ARRAY;
    lib3ds_chunk_write_start(&c, io);

    {
        int i;

        lib3ds_io_write_word(io, (uint16_t) mesh->nfaces);
        for (i = 0; i < mesh->nfaces; ++i) {
            lib3ds_io_write_word(io, mesh->faces[i].index[0]);
            lib3ds_io_write_word(io, mesh->faces[i].index[1]);
            lib3ds_io_write_word(io, mesh->faces[i].index[2]);
            lib3ds_io_write_word(io, mesh->faces[i].flags);
        }
    }

    {
        /*---- MSH_CHK_MAT_GROUP ----*/
        Lib3dsChunk c;
        int i, j;
        uint16_t num;
        char *matf = ((Lib3dsIoImpl*)io->impl)->tmp_mem = calloc(sizeof(char), mesh->nfaces);
        assert(matf);

        for (i = 0; i < mesh->nfaces; ++i) {
            if (!matf[i] && (mesh->faces[i].material >= 0) && (mesh->faces[i].material < file->nmaterials)) {
                matf[i] = 1;
                num = 1;

                for (j = i + 1; j < mesh->nfaces; ++j) {
                    if (mesh->faces[i].material == mesh->faces[j].material) ++num;
                }

                c.chunk = CHK_MSH_MAT_GROUP;
                c.size = 6 + (uint32_t)strlen(file->materials[mesh->faces[i].material]->name) + 1 + 2 + 2 * num;
                lib3ds_chunk_write(&c, io);
                lib3ds_io_write_string(io, file->materials[mesh->faces[i].material]->name);
                lib3ds_io_write_word(io, num);
                lib3ds_io_write_word(io, (uint16_t) i);

                for (j = i + 1; j < mesh->nfaces; ++j) {
                    if (mesh->faces[i].material == mesh->faces[j].material) {
                        lib3ds_io_write_word(io, (uint16_t) j);
                        matf[j] = 1;
                    }
                }
            }
        }
        ((Lib3dsIoImpl*)io->impl)->tmp_mem = NULL;
        free(matf);
    }

    {
        /*---- SMOOTH_GROUP ----*/
        Lib3dsChunk c;
        int i;

        c.chunk = CHK_SMOOTH_GROUP;
        c.size = 6 + 4 * mesh->nfaces;
        lib3ds_chunk_write(&c, io);

        for (i = 0; i < mesh->nfaces; ++i) {
            lib3ds_io_write_dword(io, mesh->faces[i].smoothing_group);
        }
    }

    {
        /*---- MSH_BOXMAP ----*/
        Lib3dsChunk c;

        if (strlen(mesh->box_front) ||
            strlen(mesh->box_back) ||
            strlen(mesh->box_left) ||
            strlen(mesh->box_right) ||
            strlen(mesh->box_top) ||
            strlen(mesh->box_bottom)) {

            c.chunk = CHK_MSH_BOXMAP;
            lib3ds_chunk_write_start(&c, io);

            lib3ds_io_write_string(io, mesh->box_front);
            lib3ds_io_write_string(io, mesh->box_back);
            lib3ds_io_write_string(io, mesh->box_left);
            lib3ds_io_write_string(io, mesh->box_right);
            lib3ds_io_write_string(io, mesh->box_top);
            lib3ds_io_write_string(io, mesh->box_bottom);

            lib3ds_chunk_write_end(&c, io);
        }
    }

    lib3ds_chunk_write_end(&c, io);
}


static void
texco_array_write(Lib3dsMesh *mesh, Lib3dsIo *io) {
    Lib3dsChunk c;
    int i;

    if (!mesh->texcos) {
        return;
    }
     
    c.chunk = CHK_TEX_VERTS;
    c.size = 8 + 8 * mesh->nvertices;
    lib3ds_chunk_write(&c, io);

    lib3ds_io_write_word(io, mesh->nvertices);
    for (i = 0; i < mesh->nvertices; ++i) {
        lib3ds_io_write_float(io, mesh->texcos[i][0]);
        lib3ds_io_write_float(io, mesh->texcos[i][1]);
    }
}


void
lib3ds_mesh_write(Lib3dsFile *file, Lib3dsMesh *mesh, Lib3dsIo *io) {
    Lib3dsChunk c;

    c.chunk = CHK_N_TRI_OBJECT;
    lib3ds_chunk_write_start(&c, io);

    point_array_write(mesh, io);
    texco_array_write(mesh, io);

    if (mesh->map_type != LIB3DS_MAP_NONE) {   /*---- LIB3DS_MESH_TEXTURE_INFO ----*/
        Lib3dsChunk c;
        int i, j;

        c.chunk = CHK_MESH_TEXTURE_INFO;
        c.size = 92;
        lib3ds_chunk_write(&c, io);

        lib3ds_io_write_word(io, mesh->map_type);

        for (i = 0; i < 2; ++i) {
            lib3ds_io_write_float(io, mesh->map_tile[i]);
        }
        lib3ds_io_write_vector(io, mesh->map_pos);
        lib3ds_io_write_float(io, mesh->map_scale);

        for (i = 0; i < 4; i++) {
            for (j = 0; j < 3; j++) {
                lib3ds_io_write_float(io, mesh->map_matrix[i][j]);
            }
        }
        for (i = 0; i < 2; ++i) {
            lib3ds_io_write_float(io, mesh->map_planar_size[i]);
        }
        lib3ds_io_write_float(io, mesh->map_cylinder_height);
    }

    flag_array_write(mesh, io);

    {
        /*---- LIB3DS_MESH_MATRIX ----*/
        Lib3dsChunk c;
        int i, j;

        c.chunk = CHK_MESH_MATRIX;
        c.size = 54;
        lib3ds_chunk_write(&c, io);
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 3; j++) {
                lib3ds_io_write_float(io, mesh->matrix[i][j]);
            }
        }
    }

    if (mesh->color) {   /*---- LIB3DS_MESH_COLOR ----*/
        Lib3dsChunk c;

        c.chunk = CHK_MESH_COLOR;
        c.size = 7;
        lib3ds_chunk_write(&c, io);
        lib3ds_io_write_byte(io, mesh->color);
    }
    
    face_array_write(file, mesh, io);

    lib3ds_chunk_write_end(&c, io);
}

