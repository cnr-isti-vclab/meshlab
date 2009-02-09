/*
    Copyright (C) 2008 by Jan Eric Kyprianidis <www.kyprianidis.com>
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
#include <lib3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <string.h>


#ifdef _MSC_VER
#pragma warning ( disable : 4996 )
#endif


/**
    @example 3ds2obj.cpp
    This example shows how to convert a 3DS file to a Wavefront OBJ file.
*/


static void
help() {
    fprintf(stderr,
        "Syntax: 3ds2obj 3ds-file [obj-file] [mtl-file]\n"
    );
    exit(1);     
}


static const char* input = 0;
static char* obj_file = 0;
static char* mtl_file = 0;
static int max_vertices = 0;
static int max_texcos = 0;
static int max_normals = 0;


void parse_args(int argc, char **argv) {
    int i;

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
                help();
            } else {
                help();
            }
        } else {
            if (!input) {
                input = argv[i];
            } else if (!obj_file) {
                obj_file = argv[i];
            } else if (!mtl_file) {
                mtl_file = argv[i];
            } else {
                help();
            }
        }
    }

    if (input) {
        size_t input_len = strlen(input);
        if ((input_len > 4) && (strcmp(input + input_len - 4, ".3ds") == 0)) {
            if (!obj_file) {
                obj_file = (char*)malloc(sizeof(char*) * (input_len + 1));
                strcpy(obj_file, input);
                strcpy(obj_file + input_len - 4, ".obj");
            }
            if (!mtl_file) {
                mtl_file = (char*)malloc(sizeof(char*) * (input_len + 1));
                strcpy(mtl_file, input);
                strcpy(mtl_file + input_len - 4, ".mtl");
            }
        }
    }

    if (!input || !obj_file) {
        help();
    }
}


void write_mtl(FILE *mtl, Lib3dsFile *f) {
    int i, j;

    fprintf(mtl, "# Wavefront material file\n");
    fprintf(mtl, "# Converted by 3ds2obj\n");
    fprintf(mtl, "# http://www.lib3ds.org\n\n");

    {
        int unique = 1;
        for (i = 0; i < f->nmaterials; ++i) {
            char *p;
            for (p = f->materials[i]->name; *p; ++p) {
                if (!isalnum(*p) && (*p != '_')) *p = '_';
            }

            for (j = 0; j < i; ++j) {
                if (strcmp(f->materials[i]->name, f->materials[j]->name) == 0) {
                    unique = 0;
                    break;
                }
            }
            if (!unique)
                break;
        }
        if (!unique) {
            for (i = 0; i < f->nmaterials; ++i) {
                sprintf(f->materials[i]->name, "mat_%d", i);
            }
        }
    }

    for (i = 0; i < f->nmaterials; ++i) {
        Lib3dsMaterial *m = f->materials[i];
        fprintf(mtl, "newmtl %s\n", m->name);
        fprintf(mtl, "Ka %f %f %f\n", m->ambient[0], m->ambient[1], m->ambient[2]);
        fprintf(mtl, "Kd %f %f %f\n", m->diffuse[0], m->diffuse[1], m->diffuse[2]);
        fprintf(mtl, "Ks %f %f %f\n", m->specular[0], m->specular[1], m->specular[2]);
        fprintf(mtl, "illum 2\n");
        fprintf(mtl, "Ns %f\n", pow(2, 10 * m->shininess + 1));
        fprintf(mtl, "d %f\n", 1.0 - m->transparency);
        fprintf(mtl, "map_Kd %s\n", m->texture1_map.name);
        fprintf(mtl, "map_bump %s\n", m->bump_map.name);
        fprintf(mtl, "map_d %s\n", m->opacity_map.name);
        fprintf(mtl, "refl %s\n", m->reflection_map.name);
        fprintf(mtl, "map_KS %s\n", m->specular_map.name);
        fprintf(mtl, "\n");
    }
}


void write_mesh(FILE *o, Lib3dsFile *f, Lib3dsMeshInstanceNode *node) {
    float (*orig_vertices)[3];
    int export_texcos;
    int export_normals;
    int i, j;
    Lib3dsMesh *mesh;
        
    mesh = lib3ds_file_mesh_for_node(f, (Lib3dsNode*)node);
    if (!mesh || !mesh->vertices) return;

    fprintf(o, "# object %s\n", node->base.name);
    fprintf(o, "g %s\n", node->instance_name[0]? node->instance_name : node->base.name);

    orig_vertices = (float(*)[3])malloc(sizeof(float) * 3 * mesh->nvertices);
    memcpy(orig_vertices, mesh->vertices, sizeof(float) * 3 * mesh->nvertices);
     {
         float inv_matrix[4][4], M[4][4];
         float tmp[3];
         int i;
 
         lib3ds_matrix_copy(M, node->base.matrix);
         lib3ds_matrix_translate(M, -node->pivot[0], -node->pivot[1], -node->pivot[2]);
         lib3ds_matrix_copy(inv_matrix, mesh->matrix);
         lib3ds_matrix_inv(inv_matrix);
         lib3ds_matrix_mult(M, M, inv_matrix);
 
         for (i = 0; i < mesh->nvertices; ++i) {
             lib3ds_vector_transform(tmp, M, mesh->vertices[i]);
             lib3ds_vector_copy(mesh->vertices[i], tmp);
         }
     }

    export_texcos = (mesh->texcos != 0);
    export_normals = (mesh->faces != 0);

    for (i = 0; i < mesh->nvertices; ++i) {
        fprintf(o, "v %f %f %f\n", mesh->vertices[i][0], 
                                   mesh->vertices[i][1], 
                                   mesh->vertices[i][2]);
    }
    fprintf(o, "# %d vertices\n", mesh->nvertices);

    if (export_texcos) {
        for (i = 0; i < mesh->nvertices; ++i) {
            fprintf(o, "vt %f %f\n", mesh->texcos[i][0], 
                                     mesh->texcos[i][1]);
        }
        fprintf(o, "# %d texture vertices\n", mesh->nvertices);
    }

    if (export_normals) {
        float (*normals)[3] = (float(*)[3])malloc(sizeof(float) * 9 * mesh->nfaces);
        lib3ds_mesh_calculate_vertex_normals(mesh, normals);
        for (i = 0; i < 3 * mesh->nfaces; ++i) {
            fprintf(o, "vn %f %f %f\n", normals[i][0],
                                        normals[i][1],
                                        normals[i][2]);
        }
        free(normals);  
        fprintf(o, "# %d normals\n", 3 * mesh->nfaces);
    }

    {
        int mat_index = -1;
        for (i = 0; i < mesh->nfaces; ++i) {
            if (mat_index != mesh->faces[i].material) {
                mat_index = mesh->faces[i].material;
                if (mat_index != -1) {
                    fprintf(o, "usemtl %s\n", f->materials[mat_index]->name);
                }
            }

            fprintf(o, "f ");
            for (j = 0; j < 3; ++j) {
                fprintf(o, "%d", mesh->faces[i].index[j] + max_vertices + 1);
                if (export_texcos) {
                    fprintf(o, "/%d", mesh->faces[i].index[j] + max_texcos + 1);
                } else if (export_normals) {
                    fprintf(o, "/");
                }
                if (export_normals) {
                    fprintf(o, "/%d", 3 * i + j + max_normals + 1);
                }
                if (j < 3) {
                    fprintf(o, " ");
                }
            }
            fprintf(o, "\n");
        }
    }

    max_vertices += mesh->nvertices;
    if (export_texcos) 
        max_texcos += mesh->nvertices;
    if (export_normals) 
        max_normals += 3 * mesh->nfaces;
    
    memcpy(mesh->vertices, orig_vertices, sizeof(float) * 3 * mesh->nvertices);
    free(orig_vertices);
}


void write_nodes(FILE *o, Lib3dsFile *f, Lib3dsNode *first_node) {
    Lib3dsNode *p;
    for (p = first_node; p; p = p->next) {
        if (p->type == LIB3DS_NODE_MESH_INSTANCE) {
            write_mesh(o, f, (Lib3dsMeshInstanceNode*)p);
            write_nodes(o, f, p->childs);
        }
    }
}


int main(int argc, char **argv) {
    Lib3dsFile *f;
    parse_args(argc, argv);

    f = lib3ds_file_open(input);
    if (!f) {
        fprintf(stderr, "***ERROR***\nLoading file failed: %s\n", input);
        exit(1);
    }

    if (mtl_file) {
        FILE *mtl = fopen(mtl_file, "wt");
        if (!mtl) {
            fprintf(stderr, "***ERROR***\nCreating output file failed: %s\n", mtl_file);
            exit(1);
        }
        write_mtl(mtl, f);
        fclose(mtl);
    }

    {
        FILE *obj = fopen(obj_file, "wt");
        if (!obj) {
            fprintf(stderr, "***ERROR***\nCreating output file failed: %s\n", obj_file);
            exit(1);
        }

        if (!f->nodes)
            lib3ds_file_create_nodes_for_meshes(f);
        lib3ds_file_eval(f, 0);
        
        fprintf(obj, "# Wavefront OBJ file\n");
        fprintf(obj, "# Converted by 3ds2obj\n");
        fprintf(obj, "# http://www.lib3ds.org\n\n");
        if (mtl_file) {
            fprintf(obj, "mtllib %s\n", mtl_file);
        }

        write_nodes(obj, f, f->nodes);
        fclose(obj);
    }

    lib3ds_file_free(f);
    return 0;
}
