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
#include <lib3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef _MSC_VER
#pragma warning ( disable : 4996 )
#endif


/**
    @example 3dsdump.c
    A utility to display information about the content of a 3DS file.
*/


static void
help() {
    fprintf(stderr,
            "The 3D Studio File Format Library - 3dsdump\n"
            "Copyright (C) 1996-2007 by Jan Eric Kyprianidis <www.kyprianidis.com>\n"
            "All rights reserved.\n"
            "\n"
            "Syntax: 3dsdump [options] filename [options]\n"
            "\n"
            "Options:\n"
            "  -h           This help\n"
            "  -d=level     Set log level (0=ERROR, 1=WARN, 2=INFO, 3=DEBUG)\n"
            "  -m           Dump materials\n"
            "  -t           Dump trimeshes\n"
            "  -i           Dump instances\n"
            "  -c           Dump cameras\n"
            "  -l           Dump lights\n"
            "  -n           Dump node hierarchy\n"
            "  -w=filename  Write new 3ds file to disk\n"
            "\n"
           );
    exit(1);
}


typedef enum Flags {
    LIB3DSDUMP_MATERIALS  = 0x0004,
    LIB3DSDUMP_TRIMESHES  = 0x0008,
    LIB3DSDUMP_INSTANCES  = 0x0010,
    LIB3DSDUMP_CAMERAS    = 0x0020,
    LIB3DSDUMP_LIGHTS     = 0x0040,
    LIB3DSDUMP_NODES      = 0x0080
} Flags;


static const char* filename = 0;
static const char* output = 0;
static unsigned flags = 0;
static int  log_level = LIB3DS_LOG_INFO;


static void
parse_args(int argc, char **argv) {
    int i;

    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
                help();
            } else if ((argv[i][1] == 'd') && (argv[i][2] == '='))  {
                log_level =  atoi(&argv[i][3]);
            } else if (argv[i][1] == 'm') {
                flags |= LIB3DSDUMP_MATERIALS;
            } else if (argv[i][1] == 't') {
                flags |= LIB3DSDUMP_TRIMESHES;
            } else if (argv[i][1] == 'i') {
                flags |= LIB3DSDUMP_INSTANCES;
            } else if (argv[i][1] == 'c') {
                flags |= LIB3DSDUMP_CAMERAS;
            } else if (argv[i][1] == 'l') {
                flags |= LIB3DSDUMP_LIGHTS;
            } else if (argv[i][1] == 'n') {
                flags |= LIB3DSDUMP_NODES;
            } else if ((argv[i][1] == 'w') && (argv[i][2] == '='))  {
                output =  &argv[i][3];
            } else {
                help();
            }
        } else {
            if (filename) {
                help();
            }
            filename = argv[i];
        }
    }
    if (!filename) {
        help();
    }
}


static long
fileio_seek_func(void *self, long offset, Lib3dsIoSeek origin) {
    FILE *f = (FILE*)self;
    int o;
    switch (origin) {
        case LIB3DS_SEEK_SET:
            o = SEEK_SET;
            break;

        case LIB3DS_SEEK_CUR:
            o = SEEK_CUR;
            break;

        case LIB3DS_SEEK_END:
            o = SEEK_END;
            break;
    }
    return (fseek(f, offset, o));
}


static long
fileio_tell_func(void *self) {
    FILE *f = (FILE*)self;
    return(ftell(f));
}


static size_t
fileio_read_func(void *self, void *buffer, size_t size) {
    FILE *f = (FILE*)self;
    return (fread(buffer, 1, size, f));
}


static size_t
fileio_write_func(void *self, const void *buffer, size_t size) {
    FILE *f = (FILE*)self;
    return (fwrite(buffer, 1, size, f));
}


static void 
fileio_log_func(void *self, Lib3dsLogLevel level, int indent, const char *msg)
{
    static const char * level_str[] = {
        "ERROR", "WARN", "INFO", "DEBUG"
    };
    if (log_level >=  level) {
        int i;
        printf("%5s : ", level_str[level]);
        for (i = 1; i < indent; ++i) printf("\t");
        printf("%s\n", msg);
    }
}


static void
matrix_dump(float matrix[4][4]) {
    int i, j;

    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            printf("%f ", matrix[j][i]);
        }
        printf("\n");
    }
}


static void
viewport_dump(Lib3dsViewport *vp) {
    Lib3dsView *view;
    int i;
    assert(vp);

    printf("  viewport:\n");
    printf("    layout:\n");
    printf("      style:       %d\n", vp->layout_style);
    printf("      active:      %d\n", vp->layout_active);
    printf("      swap:        %d\n", vp->layout_swap);
    printf("      swap_prior:  %d\n", vp->layout_swap_prior);
    printf("      position:    %d,%d\n", vp->layout_position[0], vp->layout_position[1]);
    printf("      size:        %d,%d\n", vp->layout_size[0], vp->layout_size[1]);
    printf("      views:       %ld\n", vp->layout_nviews);

    for (i = 0; i < vp->layout_nviews; ++i) {
        view = &vp->layout_views[i];

        printf("        view %d:\n", i);
        printf("          type:         %d\n", view->type);
        printf("          axis_lock:    %d\n", view->axis_lock);
        printf("          position:     (%d,%d)\n", view->position[0], view->position[1]);
        printf("          size:         (%d,%d)\n", view->size[0], view->size[1]);
        printf("          zoom:         %g\n", view->zoom);
        printf("          center:       (%g,%g,%g)\n", view->center[0], view->center[1], view->center[2]);
        printf("          horiz_angle:  %g\n", view->horiz_angle);
        printf("          vert_angle:   %g\n", view->vert_angle);
        printf("          camera:       %s\n", view->camera);
    }

    printf("    default:\n");
    printf(" type:         %d\n", vp->default_type);
    printf(" position:     (%g,%g,%g)\n", vp->default_position[0], vp->default_position[1], vp->default_position[2]);
    printf(" width:        %g\n", vp->default_width);
    printf(" horiz_angle:  %g\n", vp->default_horiz_angle);
    printf(" vert_angle:   %g\n", vp->default_vert_angle);
    printf(" roll_angle:   %g\n", vp->default_roll_angle);
    printf(" camera:       %s\n", vp->default_camera);
}


static void
texture_dump(const char *maptype, Lib3dsTextureMap *texture) {
    assert(texture);
    if (strlen(texture->name) == 0) {
        return;
    }
    printf("  %s:\n", maptype);
    printf("    name:          %s\n", texture->name);
    printf("    flags:         %X\n", (unsigned)texture->flags);
    printf("    percent:       %f\n", texture->percent);
    printf("    blur:          %f\n", texture->blur);
    printf("    scale:         (%f, %f)\n", texture->scale[0], texture->scale[1]);
    printf("    offset:        (%f, %f)\n", texture->offset[0], texture->offset[1]);
    printf("    rotation:      %f\n", texture->rotation);
    printf("    tint_1:        (%f, %f, %f)\n",
        texture->tint_1[0], texture->tint_1[1], texture->tint_1[2]);
    printf("    tint_2:        (%f, %f, %f)\n",
        texture->tint_2[0], texture->tint_2[1], texture->tint_2[2]);
    printf("    tint_r:        (%f, %f, %f)\n",
        texture->tint_r[0], texture->tint_r[1], texture->tint_r[2]);
    printf("    tint_g:        (%f, %f, %f)\n",
        texture->tint_g[0], texture->tint_g[1], texture->tint_g[2]);
    printf("    tint_b:        (%f, %f, %f)\n",
        texture->tint_b[0], texture->tint_b[1], texture->tint_b[2]);
}


static void
material_dump(Lib3dsMaterial *material) {
    assert(material);
    printf("  name:            %s\n", material->name);
    printf("  ambient:         (%f, %f, %f)\n",
        material->ambient[0], material->ambient[1], material->ambient[2]);
    printf("  diffuse:         (%f, %f, %f)\n",
        material->diffuse[0], material->diffuse[1], material->diffuse[2]);
    printf("  specular:        (%f, %f, %f)\n",
        material->specular[0], material->specular[1], material->specular[2]);
    printf("  shininess:       %f\n", material->shininess);
    printf("  shin_strength:   %f\n", material->shin_strength);
    printf("  use_blur:        %s\n", material->use_blur ? "yes" : "no");
    printf("  blur:            %f\n", material->blur);
    printf("  falloff:         %f\n", material->falloff);
    printf("  is_additive:     %s\n", material->is_additive ? "yes" : "no");
    printf("  use_falloff:     %s\n", material->use_falloff ? "yes" : "no");
    printf("  self_illum_flag: %s\n", material->self_illum_flag ? "yes" : "no");
    printf("  self_illum:      %f\n", material->self_illum);
    printf("  shading:         %d\n", material->shading);
    printf("  soften:          %s\n", material->soften ? "yes" : "no");
    printf("  face_map:        %s\n", material->face_map ? "yes" : "no");
    printf("  two_sided:       %s\n", material->two_sided ? "yes" : "no");
    printf("  map_decal:       %s\n", material->map_decal ? "yes" : "no");
    printf("  use_wire:        %s\n", material->use_wire ? "yes" : "no");
    printf("  use_wire_abs:    %s\n", material->use_wire_abs ? "yes" : "no");
    printf("  wire_size:       %f\n", material->wire_size);
    texture_dump("texture1_map", &material->texture1_map);
    texture_dump("texture1_mask", &material->texture1_mask);
    texture_dump("texture2_map", &material->texture2_map);
    texture_dump("texture2_mask", &material->texture2_mask);
    texture_dump("opacity_map", &material->opacity_map);
    texture_dump("opacity_mask", &material->opacity_mask);
    texture_dump("bump_map", &material->bump_map);
    texture_dump("bump_mask", &material->bump_mask);
    texture_dump("specular_map", &material->specular_map);
    texture_dump("specular_mask", &material->specular_mask);
    texture_dump("shininess_map", &material->shininess_map);
    texture_dump("shininess_mask", &material->shininess_mask);
    texture_dump("self_illum_map", &material->self_illum_map);
    texture_dump("self_illum_mask", &material->self_illum_mask);
    texture_dump("reflection_map", &material->reflection_map);
    texture_dump("reflection_mask", &material->reflection_mask);
    printf("  autorefl_map:\n");
    printf("    flags          %X\n", (unsigned)material->autorefl_map_flags);
    printf("    level          %d\n", (int)material->autorefl_map_anti_alias);
    printf("    size           %d\n", (int)material->autorefl_map_size);
    printf("    frame_step     %d\n", (int)material->autorefl_map_frame_step);
    printf("\n");
}


static void
camera_dump(Lib3dsCamera *camera) {
    assert(camera);
    printf("  name:       %s\n", camera->name);
    printf("  position:   (%f, %f, %f)\n",
        camera->position[0], camera->position[1], camera->position[2]);
    printf("  target      (%f, %f, %f)\n",
        camera->target[0], camera->target[1], camera->target[2]);
    printf("  roll:       %f\n", camera->roll);
    printf("  fov:        %f\n", camera->fov);
    printf("  see_cone:   %s\n", camera->see_cone ? "yes" : "no");
    printf("  near_range: %f\n", camera->near_range);
    printf("  far_range:  %f\n", camera->far_range);
    printf("\n");
}


static void
light_dump(Lib3dsLight *light) {
    assert(light);
    printf("  name:             %s\n", light->name);
    printf("  spot_light:       %s\n", light->spot_light ? "yes" : "no");
    printf("  see_cone:         %s\n", light->see_cone ? "yes" : "no");
    printf("  color:            (%f, %f, %f)\n",
        light->color[0], light->color[1], light->color[2]);
    printf("  position          (%f, %f, %f)\n",
        light->position[0], light->position[1], light->position[2]);
    printf("  target              (%f, %f, %f)\n",
        light->target[0], light->target[1], light->target[2]);
    printf("  roll:             %f\n", light->roll);
    printf("  off:              %s\n", light->off ? "yes" : "no");
    printf("  outer_range:      %f\n", light->outer_range);
    printf("  inner_range:      %f\n", light->inner_range);
    printf("  multiplier:       %f\n", light->multiplier);
    printf("  attenuation:      %f\n", light->attenuation);
    printf("  rectangular_spot: %s\n", light->rectangular_spot ? "yes" : "no");
    printf("  shadowed:         %s\n", light->shadowed ? "yes" : "no");
    printf("  shadow_bias:      %f\n", light->shadow_bias);
    printf("  shadow_filter:    %f\n", light->shadow_filter);
    printf("  shadow_size:      %d\n", light->shadow_size);
    printf("  spot_aspect:      %f\n", light->spot_aspect);
    printf("  use_projector:    %s\n", light->use_projector ? "yes" : "no");
    printf("  projector:        %s\n", light->projector);
    printf("  spot_overshoot:   %d\n", (int)light->spot_overshoot);
    printf("  ray_shadows:      %s\n", light->ray_shadows ? "yes" : "no");
    printf("  ray_bias:         %f\n", light->ray_bias);
    printf("  hotspot:         %f\n", light->hotspot);
    printf("  falloff:         %f\n", light->falloff);
    printf("\n");
}


static void
mesh_dump(Lib3dsMesh *mesh) {
    int i;
    float p[3];

    assert(mesh);
    printf("  %s vertices=%ld faces=%ld\n",
        mesh->name,
        mesh->nvertices,
        mesh->nfaces);
    printf("  matrix:\n");
    matrix_dump(mesh->matrix);
    printf("  vertices (x, y, z, u, v):\n");
    for (i = 0; i < mesh->nvertices; ++i) {
        lib3ds_vector_copy(p, mesh->vertices[i]);
        printf("    %10.5f %10.5f %10.5f", p[0], p[1], p[2]);
        if (mesh->texcos) {
            printf("%10.5f %10.5f", mesh->texcos[i][0], mesh->texcos[i][1]);
        }
        printf("\n");
    }
    printf("  facelist:\n");
    for (i = 0; i < mesh->nfaces; ++i) {
        printf("    %4d %4d %4d  flags:%X  smoothing:%X  material:\"%d\"\n",
            mesh->faces[i].index[0],
            mesh->faces[i].index[1],
            mesh->faces[i].index[2],
            mesh->faces[i].flags,
            mesh->faces[i].smoothing_group,
            mesh->faces[i].material
            );
    }
}


static void
dump_instances(Lib3dsNode *node, const char* parent) {
    Lib3dsNode *p;
    char name[255];

    strcpy(name, parent);
    strcat(name, ".");
    strcat(name, node->name);
    if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
        Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode*)node;
        printf("  %s : %s\n", name, n->instance_name);
    }
    for (p = node->childs; p != 0; p = p->next) {
        dump_instances(p, parent);
    }
}


static const char* node_names_table[] = {
    "Ambient",
    "Mesh",
    "Camera",
    "Camera Target",
    "Omnilight",
    "Spotlight",
    "Spotlight Target"
};


static void
node_dump(Lib3dsNode *node, int level) {
    Lib3dsNode *p;
    char l[128];

    assert(node);
    memset(l, ' ', 2*level);
    l[2*level] = 0;

    if (node->type == LIB3DS_NODE_MESH_INSTANCE) {
        Lib3dsMeshInstanceNode *n = (Lib3dsMeshInstanceNode*)node; 
        printf("%s%s [%s] (%s)\n",
            l,
            node->name,
            n->instance_name,
            node_names_table[node->type]
        );
    } else {
        printf("%s%s (%s)\n",
            l,
            node->name,
            node_names_table[node->type]
        );
    }

    for (p = node->childs; p != 0; p = p->next) {
        node_dump(p, level + 1);
    }
}


int
main(int argc, char **argv) {
    FILE *file;
    Lib3dsFile *f = 0;
    Lib3dsIo io;
    int result;
    int i;

    parse_args(argc, argv);

    file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "***ERROR***\nFile not found: %s\n", filename);
        exit(1);
    }

    f = lib3ds_file_new();
 
    memset(&io, 0, sizeof(io));
    io.self = file;
    io.seek_func = fileio_seek_func;
    io.tell_func = fileio_tell_func;
    io.read_func = fileio_read_func;
    io.write_func = fileio_write_func;
    io.log_func = fileio_log_func;

    result =  lib3ds_file_read(f, &io);

    fclose(file);
 
    if (!result) {
        fprintf(stderr, "***ERROR***\nLoading file failed: %s\n", filename);
        exit(1);
    }

    if (flags & LIB3DSDUMP_MATERIALS) {
        printf("Dumping materials:\n");
        for (i = 0; i < f->nmaterials; ++i) material_dump(f->materials[i]);
        printf("\n");
    }
    if (flags & LIB3DSDUMP_TRIMESHES) {
        printf("Dumping meshes:\n");
        for (i = 0; i < f->nmeshes; ++i) mesh_dump(f->meshes[i]);
        printf("\n");
    }
    if (flags & LIB3DSDUMP_INSTANCES) {
        Lib3dsNode *p;
        printf("Dumping instances:\n");
        for (p = f->nodes; p != 0; p = p->next) {
            dump_instances(p, "");
        }
        printf("\n");
    }
    if (flags & LIB3DSDUMP_CAMERAS) {
        printf("Dumping cameras:\n");
        for (i = 0; i < f->ncameras; ++i) camera_dump(f->cameras[i]);
        printf("\n");
    }
    if (flags & LIB3DSDUMP_LIGHTS) {
        printf("Dumping lights:\n");
        for (i = 0; i < f->nlights; ++i) light_dump(f->lights[i]);
        printf("\n");
    }
    if (flags & LIB3DSDUMP_NODES) {
        Lib3dsNode *p;
        printf("Dumping node hierarchy:\n");
        for (p = f->nodes; p != 0; p = p->next) {
            node_dump(p, 1);
        }
        printf("\n");
    }
    if (output) {
        if (!lib3ds_file_save(f, output)) {
            printf("***ERROR**** Writing %s\n", output);
        }
    }

    lib3ds_file_free(f);
    return 0;
}
