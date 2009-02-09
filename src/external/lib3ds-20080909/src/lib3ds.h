/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_H
#define INCLUDED_LIB3DS_H
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

#include <stddef.h>

#ifndef LIB3DSAPI
    #ifdef _MSC_VER
        #ifdef LIB3DS_EXPORTS
            #define LIB3DSAPI __declspec(dllexport)
        #else               
            #define LIB3DSAPI __declspec(dllimport)
        #endif           
    #else
        #define LIB3DSAPI
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup api API */
/** @{ */

typedef enum Lib3dsIoSeek {
    LIB3DS_SEEK_SET     = 0,
    LIB3DS_SEEK_CUR     = 1,
    LIB3DS_SEEK_END     = 2
} Lib3dsIoSeek;

typedef enum Lib3dsLogLevel {
    LIB3DS_LOG_ERROR    = 0,
    LIB3DS_LOG_WARN     = 1,
    LIB3DS_LOG_INFO     = 2,
    LIB3DS_LOG_DEBUG    = 3
} Lib3dsLogLevel;

typedef struct Lib3dsIo {
    void*   impl;
    void*   self;
    long    (*seek_func) (void *self, long offset, Lib3dsIoSeek origin);
    long    (*tell_func) (void *self);
    size_t  (*read_func) (void *self, void *buffer, size_t size);
    size_t  (*write_func)(void *self, const void *buffer, size_t size);
    void    (*log_func)  (void *self, Lib3dsLogLevel level, int indent, const char *msg);
} Lib3dsIo;

/* Atmosphere settings */
typedef struct Lib3dsAtmosphere {
    int         use_fog;
    float       fog_color[3];
    int         fog_background;
    float       fog_near_plane;
    float       fog_near_density;
    float       fog_far_plane;
    float       fog_far_density;
    int         use_layer_fog;
    unsigned    layer_fog_flags;
    float       layer_fog_color[3];
    float       layer_fog_near_y;
    float       layer_fog_far_y;
    float       layer_fog_density;
    int         use_dist_cue;
    int         dist_cue_background;     /* bool */
    float       dist_cue_near_plane;
    float       dist_cue_near_dimming;
    float       dist_cue_far_plane;
    float       dist_cue_far_dimming;
} Lib3dsAtmosphere;

/* Background settings */
typedef struct Lib3dsBackground {
    int         use_bitmap;
    char        bitmap_name[64];
    int         use_solid;
    float       solid_color[3];
    int         use_gradient;
    float       gradient_percent;
    float       gradient_top[3];
    float       gradient_middle[3];
    float       gradient_bottom[3];
} Lib3dsBackground;

/* Shadow settings */
typedef struct Lib3dsShadow {
    short       map_size;           /* Global shadow map size that ranges from 10 to 4096 */
    float       low_bias;           /* Global shadow low bias */
    float       hi_bias;            /* Global shadow hi bias */
    float       filter;             /* Global shadow filter that ranges from 1 (lowest) to 10 (highest) */
    float       ray_bias;           /* Global raytraced shadow bias */
} Lib3dsShadow;

/* Layout view types */
typedef enum Lib3dsViewType {
    LIB3DS_VIEW_NOT_USED   = 0,
    LIB3DS_VIEW_TOP        = 1,
    LIB3DS_VIEW_BOTTOM     = 2,
    LIB3DS_VIEW_LEFT       = 3,
    LIB3DS_VIEW_RIGHT      = 4,
    LIB3DS_VIEW_FRONT      = 5,
    LIB3DS_VIEW_BACK       = 6,
    LIB3DS_VIEW_USER       = 7,
    LIB3DS_VIEW_SPOTLIGHT  = 18,
    LIB3DS_VIEW_CAMERA     = 65535
} Lib3dsViewType;

/* Layout styles */
typedef enum Lib3dsLayoutStyle {
    LIB3DS_LAYOUT_SINGLE                    = 0,
    LIB3DS_LAYOUT_TWO_PANE_VERT_SPLIT       = 1,
    LIB3DS_LAYOUT_TWO_PANE_HORIZ_SPLIT      = 2,
    LIB3DS_LAYOUT_FOUR_PANE                 = 3,
    LIB3DS_LAYOUT_THREE_PANE_LEFT_SPLIT     = 4,
    LIB3DS_LAYOUT_THREE_PANE_BOTTOM_SPLIT   = 5,
    LIB3DS_LAYOUT_THREE_PANE_RIGHT_SPLIT    = 6,
    LIB3DS_LAYOUT_THREE_PANE_TOP_SPLIT      = 7,
    LIB3DS_LAYOUT_THREE_PANE_VERT_SPLIT     = 8,
    LIB3DS_LAYOUT_THREE_PANE_HORIZ_SPLIT    = 9,
    LIB3DS_LAYOUT_FOUR_PANE_LEFT_SPLIT      = 10,
    LIB3DS_LAYOUT_FOUR_PANE_RIGHT_SPLIT     = 11
} Lib3dsLayoutStyle;

/* Layout view settings */
typedef struct Lib3dsView {
    int         type;
    unsigned    axis_lock;
    short       position[2];
    short       size[2];
    float       zoom;
    float       center[3];
    float       horiz_angle;
    float       vert_angle;
    char        camera[11];
} Lib3dsView;

#define LIB3DS_LAYOUT_MAX_VIEWS 32

/* Viewport and default view settings */
typedef struct Lib3dsViewport {
    int             layout_style;
    int             layout_active;
    int             layout_swap;
    int             layout_swap_prior;
    int             layout_swap_view;
    unsigned short  layout_position[2];
    unsigned short  layout_size[2];
    int             layout_nviews;
    Lib3dsView      layout_views[LIB3DS_LAYOUT_MAX_VIEWS];
    int             default_type;
    float           default_position[3];
    float           default_width;
    float           default_horiz_angle;
    float           default_vert_angle;
    float           default_roll_angle;
    char            default_camera[64];
} Lib3dsViewport;

/* Material texture map flags */
typedef enum Lib3dsTextureMapFlags {
    LIB3DS_TEXTURE_DECALE       = 0x0001,
    LIB3DS_TEXTURE_MIRROR       = 0x0002,
    LIB3DS_TEXTURE_NEGATE       = 0x0008,
    LIB3DS_TEXTURE_NO_TILE      = 0x0010,
    LIB3DS_TEXTURE_SUMMED_AREA  = 0x0020,
    LIB3DS_TEXTURE_ALPHA_SOURCE = 0x0040,
    LIB3DS_TEXTURE_TINT         = 0x0080,
    LIB3DS_TEXTURE_IGNORE_ALPHA = 0x0100,
    LIB3DS_TEXTURE_RGB_TINT     = 0x0200
} Lib3dsTextureMapFlags;

/* Material texture map */
typedef struct Lib3dsTextureMap {
    unsigned    user_id;
    void*       user_ptr;
    char        name[64];
    unsigned    flags;
    float       percent;
    float       blur;
    float       scale[2];
    float       offset[2];
    float       rotation;
    float       tint_1[3];
    float       tint_2[3];
    float       tint_r[3];
    float       tint_g[3];
    float       tint_b[3];
} Lib3dsTextureMap;

/* Auto reflection texture map flags */
typedef enum Lib3dsAutoReflMapFlags {
    LIB3DS_AUTOREFL_USE                     = 0x0001,
    LIB3DS_AUTOREFL_READ_FIRST_FRAME_ONLY   = 0x0004,
    LIB3DS_AUTOREFL_FLAT_MIRROR             = 0x0008 
} Lib3dsAutoReflMapFlags;

/* Material shading type */
typedef enum Lib3dsShading {
    LIB3DS_SHADING_WIRE_FRAME = 0,
    LIB3DS_SHADING_FLAT       = 1, 
    LIB3DS_SHADING_GOURAUD    = 2, 
    LIB3DS_SHADING_PHONG      = 3, 
    LIB3DS_SHADING_METAL      = 4
} Lib3dsShading; 

/** Material */
typedef struct Lib3dsMaterial {
    unsigned            user_id;
    void*               user_ptr;
    char                name[64];           /* Material name */
    float               ambient[3];         /* Material ambient reflectivity */
    float               diffuse[3];         /* Material diffuse reflectivity */
    float               specular[3];        /* Material specular reflectivity */
    float               shininess;          /* Material specular exponent */
    float               shin_strength;
    int                 use_blur;
    float               blur;
    float               transparency;
    float               falloff;
    int                 is_additive;
    int                 self_illum_flag; /* bool */
    float               self_illum;
    int                 use_falloff;
    int                 shading;
    int                 soften;         /* bool */
    int                 face_map;       /* bool */
    int                 two_sided;      /* Material visible from back */
    int                 map_decal;      /* bool */
    int                 use_wire;
    int                 use_wire_abs;
    float               wire_size;
    Lib3dsTextureMap    texture1_map;
    Lib3dsTextureMap    texture1_mask;
    Lib3dsTextureMap    texture2_map;
    Lib3dsTextureMap    texture2_mask;
    Lib3dsTextureMap    opacity_map;
    Lib3dsTextureMap    opacity_mask;
    Lib3dsTextureMap    bump_map;
    Lib3dsTextureMap    bump_mask;
    Lib3dsTextureMap    specular_map;
    Lib3dsTextureMap    specular_mask;
    Lib3dsTextureMap    shininess_map;
    Lib3dsTextureMap    shininess_mask;
    Lib3dsTextureMap    self_illum_map;
    Lib3dsTextureMap    self_illum_mask;
    Lib3dsTextureMap    reflection_map;
    Lib3dsTextureMap    reflection_mask;
    unsigned            autorefl_map_flags;
    int                 autorefl_map_anti_alias;  /* 0=None, 1=Low, 2=Medium, 3=High */
    int                 autorefl_map_size;
    int                 autorefl_map_frame_step;
} Lib3dsMaterial;

/** Object flags for cameras, lights and meshes */
typedef enum Lib3dsObjectFlags {
    LIB3DS_OBJECT_HIDDEN          = 0x01, 
    LIB3DS_OBJECT_VIS_LOFTER      = 0x02, 
    LIB3DS_OBJECT_DOESNT_CAST     = 0x04, 
    LIB3DS_OBJECT_MATTE           = 0x08, 
    LIB3DS_OBJECT_DONT_RCVSHADOW  = 0x10, 
    LIB3DS_OBJECT_FAST            = 0x20, 
    LIB3DS_OBJECT_FROZEN          = 0x40 
} Lib3dsObjectFlags;

/** Camera object */
typedef struct Lib3dsCamera {
    unsigned    user_id;
    void*       user_ptr;
    char        name[64];
    unsigned    object_flags; /*< @see Lib3dsObjectFlags */ 
    float       position[3];
    float       target[3];
    float       roll;
    float       fov;
    int         see_cone;
    float       near_range;
    float       far_range;
} Lib3dsCamera; 

/** Light object */
typedef struct Lib3dsLight {
    unsigned    user_id;
    void*       user_ptr;
    char        name[64];
    unsigned    object_flags; 
    int         spot_light;     /* bool */
    int         see_cone;
    float       color[3];
    float       position[3];
    float       target[3];
    float       roll;
    int         off;              /* bool */
    float       outer_range;
    float       inner_range;
    float       multiplier;
    /*const char**  excludes;*/
    float       attenuation;
    int         rectangular_spot;   /* bool */
    int         shadowed;           /* bool */
    float       shadow_bias;
    float       shadow_filter;
    int         shadow_size;
    float       spot_aspect;
    int         use_projector;
    char        projector[64];
    int         spot_overshoot;      /* bool */
    int         ray_shadows;         /* bool */
    float       ray_bias;
    float       hotspot;
    float       falloff;
} Lib3dsLight; 

/* Texture map projection */
typedef enum {
  LIB3DS_MAP_NONE           = -1,
  LIB3DS_MAP_PLANAR         = 0,
  LIB3DS_MAP_CYLINDRICAL    = 1,
  LIB3DS_MAP_SPHERICAL      = 2
} Lib3dsMapType;

/**  Meaning of Lib3dsFace::flags. ABC are points of the current face 
    (A: is 1st vertex, B is 2nd vertex, C is 3rd vertex) */
typedef enum Lib3dsFaceFlags {
  LIB3DS_FACE_VIS_AC    = 0x01,       /**< Bit 0: Edge visibility AC */
  LIB3DS_FACE_VIS_BC    = 0x02,       /**< Bit 1: Edge visibility BC */
  LIB3DS_FACE_VIS_AB    = 0x04,       /**< Bit 2: Edge visibility AB */
  LIB3DS_FACE_WRAP_U    = 0x08,       /**< Bit 3: Face is at tex U wrap seam */
  LIB3DS_FACE_WRAP_V    = 0x10,       /**< Bit 4: Face is at tex V wrap seam */
  LIB3DS_FACE_SELECT_3  = (1<<13),    /**< Bit 13: Selection of the face in selection 3*/
  LIB3DS_FACE_SELECT_2  = (1<<14),    /**< Bit 14: Selection of the face in selection 2*/
  LIB3DS_FACE_SELECT_1  = (1<<15),    /**< Bit 15: Selection of the face in selection 1*/
} Lib3dsFaceFlags;

typedef struct Lib3dsFace {
    unsigned short  index[3];
    unsigned short  flags;
    int             material;
    unsigned        smoothing_group;
} Lib3dsFace; 

/* Triangular mesh object */
typedef struct Lib3dsMesh {
    unsigned        user_id;
    void*           user_ptr;
    char            name[64];            /**< Mesh name. Don't use more than 8 characters  */
    unsigned        object_flags;        /**< @see Lib3dsObjectFlags */ 
    int             color;               /**< Index to editor palette [0..255] */
    float           matrix[4][4];        /**< Transformation matrix for mesh data */
    unsigned short  nvertices;           /**< Number of vertices in vertex array (max. 65535) */
    float           (*vertices)[3];
    float           (*texcos)[2];
    unsigned short* vflags;
    unsigned short  nfaces;              /**< Number of faces in face array (max. 65535) */
    Lib3dsFace*     faces;               
    char            box_front[64];
    char            box_back[64];
    char            box_left[64];
    char            box_right[64];
    char            box_top[64];
    char            box_bottom[64];
    int             map_type;
    float           map_pos[3];
    float           map_matrix[4][4];
    float           map_scale;
    float           map_tile[2];
    float           map_planar_size[2];
    float           map_cylinder_height;
} Lib3dsMesh; 

typedef enum Lib3dsNodeType {
    LIB3DS_NODE_AMBIENT_COLOR   = 0,
    LIB3DS_NODE_MESH_INSTANCE   = 1,
    LIB3DS_NODE_CAMERA          = 2,
    LIB3DS_NODE_CAMERA_TARGET   = 3,
    LIB3DS_NODE_OMNILIGHT       = 4,
    LIB3DS_NODE_SPOTLIGHT       = 5,
    LIB3DS_NODE_SPOTLIGHT_TARGET= 6
} Lib3dsNodeType;

typedef enum Lib3dsNodeFlags{
    LIB3DS_NODE_HIDDEN           = 0x000800,
    LIB3DS_NODE_SHOW_PATH        = 0x010000,
    LIB3DS_NODE_SMOOTHING        = 0x020000,
    LIB3DS_NODE_MOTION_BLUR      = 0x100000,
    LIB3DS_NODE_MORPH_MATERIALS  = 0x400000
} Lib3dsNodeFlags;

typedef struct Lib3dsNode {
    unsigned            user_id;
    void*               user_ptr;
    struct Lib3dsNode*  next;
    struct Lib3dsNode*  childs;
    struct Lib3dsNode*  parent;
    Lib3dsNodeType      type;
    unsigned short      node_id;            /**< 0..65535 */
    char                name[64];
    unsigned            flags;
    float               matrix[4][4];
} Lib3dsNode;

typedef enum Lib3dsKeyFlags {
    LIB3DS_KEY_USE_TENS         = 0x01,
    LIB3DS_KEY_USE_CONT         = 0x02,
    LIB3DS_KEY_USE_BIAS         = 0x04,
    LIB3DS_KEY_USE_EASE_TO      = 0x08,
    LIB3DS_KEY_USE_EASE_FROM    = 0x10
} Lib3dsKeyFlags;

typedef struct Lib3dsKey {
    int         frame;
    unsigned    flags;
    float       tens;
    float       cont;
    float       bias;
    float       ease_to;
    float       ease_from;
    float       value[4];
} Lib3dsKey;

typedef enum Lib3dsTrackType {
    LIB3DS_TRACK_BOOL   = 0,
    LIB3DS_TRACK_FLOAT  = 1,
    LIB3DS_TRACK_VECTOR = 3,
    LIB3DS_TRACK_QUAT   = 4
} Lib3dsTrackType;

typedef enum {
    LIB3DS_TRACK_REPEAT   = 0x0001,
    LIB3DS_TRACK_SMOOTH   = 0x0002,
    LIB3DS_TRACK_LOCK_X   = 0x0008,
    LIB3DS_TRACK_LOCK_Y   = 0x0010,
    LIB3DS_TRACK_LOCK_Z   = 0x0020,
    LIB3DS_TRACK_UNLINK_X = 0x0100,
    LIB3DS_TRACK_UNLINK_Y = 0x0200,
    LIB3DS_TRACK_UNLINK_Z = 0x0400
} Lib3dsTrackFlags;

typedef struct Lib3dsTrack {
    unsigned        flags;
    Lib3dsTrackType type; 
    int             nkeys;
    Lib3dsKey*      keys;   
} Lib3dsTrack;

typedef struct Lib3dsAmbientColorNode {
    Lib3dsNode      base;
    float           color[3];
    Lib3dsTrack     color_track;
} Lib3dsAmbientColorNode;

typedef struct Lib3dsMeshInstanceNode {
    Lib3dsNode      base;
    float           pivot[3];
    char            instance_name[64];
    float           bbox_min[3];
    float           bbox_max[3];
    int             hide;
    float           pos[3];
    float           rot[4];
    float           scl[3];
    float           morph_smooth;
    char            morph[64];
    Lib3dsTrack     pos_track;
    Lib3dsTrack     rot_track;
    Lib3dsTrack     scl_track;
    Lib3dsTrack     hide_track;
} Lib3dsMeshInstanceNode;

typedef struct Lib3dsCameraNode {
    Lib3dsNode      base;
    float           pos[3];
    float           fov;
    float           roll;
    Lib3dsTrack     pos_track;
    Lib3dsTrack     fov_track;
    Lib3dsTrack     roll_track;
} Lib3dsCameraNode;

typedef struct Lib3dsTargetNode {
    Lib3dsNode      base;
    float           pos[3];
    Lib3dsTrack     pos_track;
} Lib3dsTargetNode;

typedef struct Lib3dsOmnilightNode {
    Lib3dsNode      base;
    float           pos[3];
    float           color[3];
    Lib3dsTrack     pos_track;
    Lib3dsTrack     color_track;
} Lib3dsOmnilightNode;

typedef struct Lib3dsSpotlightNode {
    Lib3dsNode      base;
    float           pos[3];
    float           color[3];
    float           hotspot;
    float           falloff;
    float           roll;
    Lib3dsTrack     pos_track;
    Lib3dsTrack     color_track;
    Lib3dsTrack     hotspot_track;
    Lib3dsTrack     falloff_track;
    Lib3dsTrack     roll_track;
} Lib3dsSpotlightNode;

typedef struct Lib3dsFile {
    unsigned            user_id;
    void*               user_ptr;
    unsigned            mesh_version;
    unsigned            keyf_revision;
    char                name[12+1];
    float               master_scale;
    float               construction_plane[3];
    float               ambient[3];
    Lib3dsShadow        shadow;
    Lib3dsBackground    background;
    Lib3dsAtmosphere    atmosphere;
    Lib3dsViewport      viewport;
    Lib3dsViewport      viewport_keyf;
    int                 frames;
    int                 segment_from;
    int                 segment_to;
    int                 current_frame;
    int                 materials_size;
    int                 nmaterials;         
    Lib3dsMaterial**    materials;        
    int                 cameras_size;  
    int                 ncameras;                      
    Lib3dsCamera**      cameras;
    int                 lights_size;
    int                 nlights;                      
    Lib3dsLight**       lights;
    int                 meshes_size;
    int                 nmeshes;                      
    Lib3dsMesh**        meshes;                         
    Lib3dsNode*         nodes;
} Lib3dsFile; 

extern LIB3DSAPI Lib3dsFile* lib3ds_file_open(const char *filename);
extern LIB3DSAPI int lib3ds_file_save(Lib3dsFile *file, const char *filename);
extern LIB3DSAPI Lib3dsFile* lib3ds_file_new();
extern LIB3DSAPI void lib3ds_file_free(Lib3dsFile *file);
extern LIB3DSAPI void lib3ds_file_eval(Lib3dsFile *file, float t);
extern LIB3DSAPI int lib3ds_file_read(Lib3dsFile *file, Lib3dsIo *io);
extern LIB3DSAPI int lib3ds_file_write(Lib3dsFile *file, Lib3dsIo *io);
extern LIB3DSAPI void lib3ds_file_reserve_materials(Lib3dsFile *file, int size, int force);
extern LIB3DSAPI void lib3ds_file_insert_material(Lib3dsFile *file, Lib3dsMaterial *material, int index);
extern LIB3DSAPI void lib3ds_file_remove_material(Lib3dsFile *file, int index);
extern LIB3DSAPI int lib3ds_file_material_by_name(Lib3dsFile *file, const char *name);
extern LIB3DSAPI void lib3ds_file_reserve_cameras(Lib3dsFile *file, int size, int force);
extern LIB3DSAPI void lib3ds_file_insert_camera(Lib3dsFile *file, Lib3dsCamera *camera, int index);
extern LIB3DSAPI void lib3ds_file_remove_camera(Lib3dsFile *file, int index);
extern LIB3DSAPI int lib3ds_file_camera_by_name(Lib3dsFile *file, const char *name);
extern LIB3DSAPI void lib3ds_file_reserve_lights(Lib3dsFile *file, int size, int force);
extern LIB3DSAPI void lib3ds_file_insert_light(Lib3dsFile *file, Lib3dsLight *light, int index);
extern LIB3DSAPI void lib3ds_file_remove_light(Lib3dsFile *file, int index);
extern LIB3DSAPI int lib3ds_file_light_by_name(Lib3dsFile *file, const char *name);
extern LIB3DSAPI void lib3ds_file_reserve_meshes(Lib3dsFile *file, int size, int force);
extern LIB3DSAPI void lib3ds_file_insert_mesh(Lib3dsFile *file, Lib3dsMesh *mesh, int index);
extern LIB3DSAPI void lib3ds_file_remove_mesh(Lib3dsFile *file, int index);
extern LIB3DSAPI int lib3ds_file_mesh_by_name(Lib3dsFile *file, const char *name);
extern LIB3DSAPI Lib3dsMesh* lib3ds_file_mesh_for_node(Lib3dsFile *file, Lib3dsNode *node);
extern LIB3DSAPI Lib3dsNode* lib3ds_file_node_by_name(Lib3dsFile *file, const char* name, Lib3dsNodeType type);
extern LIB3DSAPI Lib3dsNode* lib3ds_file_node_by_id(Lib3dsFile *file, unsigned short node_id);
extern LIB3DSAPI void lib3ds_file_append_node(Lib3dsFile *file, Lib3dsNode *node, Lib3dsNode *parent);
extern LIB3DSAPI void lib3ds_file_insert_node(Lib3dsFile *file, Lib3dsNode *node, Lib3dsNode *at);
extern LIB3DSAPI void lib3ds_file_remove_node(Lib3dsFile *file, Lib3dsNode *node);
extern LIB3DSAPI void lib3ds_file_minmax_node_id(Lib3dsFile *file, unsigned short *min_id, unsigned short *max_id);
extern LIB3DSAPI void lib3ds_file_create_nodes_for_meshes(Lib3dsFile *file);


/**
    This function computes the bounding box of meshes, cameras 
    and lights defined in the 3D editor.

    \param file             The Lib3dsFile object to be examined.
    \param include_meshes   Include meshes in bounding box calculation.
    \param include_cameras  Include cameras in bounding box calculation.
    \param include_lights   Include lights in bounding box calculation.
    \param bmin             Returned minimum x,y,z values.
    \param bmax             Returned maximum x,y,z values.
 */
extern LIB3DSAPI void lib3ds_file_bounding_box_of_objects(
    Lib3dsFile *file, 
    int include_meshes, 
    int include_cameras,
    int include_lights,
    float bmin[3],
    float bmax[3]);

/**
    This function computes the bounding box of mesh, camera 
    and light instances defined in the Keyframer.
    
    \param file             The Lib3dsFile object to be examined.
    \param include_meshes   Include meshes in bounding box calculation.
    \param include_cameras  Include cameras in bounding box calculation.
    \param include_lights   Include lights in bounding box calculation.
    \param bmin             Returned minimum x,y,z values.
    \param bmax             Returned maximum x,y,z values.
    \param matrix           A matrix describing the coordinate system to
                            calculate the bounding box in.
 */
extern LIB3DSAPI void lib3ds_file_bounding_box_of_nodes(
    Lib3dsFile *file, 
    int include_meshes, 
    int include_cameras, 
    int include_lights, 
    float bmin[3], 
    float bmax[3], 
    float matrix[4][4]);

extern LIB3DSAPI Lib3dsMaterial* lib3ds_material_new(const char *name);
extern LIB3DSAPI void lib3ds_material_free(Lib3dsMaterial *material);
extern LIB3DSAPI Lib3dsCamera* lib3ds_camera_new(const char *name);
extern LIB3DSAPI void lib3ds_camera_free(Lib3dsCamera *mesh);
extern LIB3DSAPI Lib3dsLight* lib3ds_light_new(const char *name);
extern LIB3DSAPI void lib3ds_light_free(Lib3dsLight *mesh);
extern LIB3DSAPI Lib3dsMesh* lib3ds_mesh_new(const char *name);
extern LIB3DSAPI void lib3ds_mesh_free(Lib3dsMesh *mesh);
extern LIB3DSAPI void lib3ds_mesh_resize_vertices(Lib3dsMesh *mesh, int nvertices, int use_texcos, int use_flags);
extern LIB3DSAPI void lib3ds_mesh_resize_faces(Lib3dsMesh *mesh, int nfaces);
extern LIB3DSAPI void lib3ds_mesh_bounding_box(Lib3dsMesh *mesh, float bmin[3], float bmax[3]);
extern LIB3DSAPI void lib3ds_mesh_calculate_face_normals(Lib3dsMesh *mesh, float (*face_normals)[3]);
extern LIB3DSAPI void lib3ds_mesh_calculate_vertex_normals(Lib3dsMesh *mesh, float (*normals)[3]);

extern LIB3DSAPI Lib3dsNode* lib3ds_node_new(Lib3dsNodeType type);
extern LIB3DSAPI Lib3dsAmbientColorNode* lib3ds_node_new_ambient_color(float color0[3]);
extern LIB3DSAPI Lib3dsMeshInstanceNode* lib3ds_node_new_mesh_instance(Lib3dsMesh *mesh, const char* instance_name, float pos0[3], float scl0[3], float rot0[4]);
extern LIB3DSAPI Lib3dsCameraNode* lib3ds_node_new_camera(Lib3dsCamera *camera);
extern LIB3DSAPI Lib3dsTargetNode* lib3ds_node_new_camera_target(Lib3dsCamera *camera);
extern LIB3DSAPI Lib3dsOmnilightNode* lib3ds_node_new_omnilight(Lib3dsLight *light);
extern LIB3DSAPI Lib3dsSpotlightNode* lib3ds_node_new_spotlight(Lib3dsLight *light);
extern LIB3DSAPI Lib3dsTargetNode* lib3ds_node_new_spotlight_target(Lib3dsLight *light);
extern LIB3DSAPI void lib3ds_node_free(Lib3dsNode *node);
extern LIB3DSAPI void lib3ds_node_eval(Lib3dsNode *node, float t);
extern LIB3DSAPI Lib3dsNode* lib3ds_node_by_name(Lib3dsNode *node, const char* name, Lib3dsNodeType type);
extern LIB3DSAPI Lib3dsNode* lib3ds_node_by_id(Lib3dsNode *node, unsigned short node_id);

extern LIB3DSAPI Lib3dsTrack* lib3ds_track_new(Lib3dsTrackType type, int nkeys);
extern LIB3DSAPI void lib3ds_track_free(Lib3dsTrack *track);
extern LIB3DSAPI void lib3ds_track_resize(Lib3dsTrack *track, int nkeys);
extern LIB3DSAPI void lib3ds_track_eval_bool(Lib3dsTrack *track, int *b, float t);
extern LIB3DSAPI void lib3ds_track_eval_float(Lib3dsTrack *track, float *f, float t);
extern LIB3DSAPI void lib3ds_track_eval_vector(Lib3dsTrack *track, float v[3], float t);
extern LIB3DSAPI void lib3ds_track_eval_quat(Lib3dsTrack *track, float q[4], float t);

/** 
    Calculates the ease in/out function. See Lib3dsKey for details. 
    
    \param fp           Previous frame number.
    \param fc           Current frame number.
    \param fn           Next frame number. 
    \param ease_from    Ease in value [0, 1.0] 
    \param ease_to      Ease out value [0, 1.0]
*/
extern LIB3DSAPI float lib3ds_math_ease(
    float fp,
    float fc,
    float fn,
    float ease_from,
    float ease_to);

/**  
    Computes a point on a n-dimensional cubic hermite spline. 
    \param v   
        [out] Result 
    \param a
        [in] First point of the spline.
    \param p   
        [in] Tangent at the first point of the spline.
    \param q
        [in] Tangent at the second point of the spline.
    \param b
        [in] Second point of the spline.
    \param n
        [in] Dimension
    \param t
        [in] Parameter value [0...1] 
*/
extern LIB3DSAPI void lib3ds_math_cubic_interp(
    float *v, 
    float *a,
    float *p,
    float *q,
    float *b,
    int n,
    float t);

extern LIB3DSAPI void lib3ds_vector_make(
    float c[3],
    float x,
    float y,
    float z);

/** 
    Sets all components of a vector to zero. 
    \param c
        The Pointer to the vector.
*/
extern LIB3DSAPI void lib3ds_vector_zero(
    float c[3]);

/** 
    Copies all components of a vector to another vector. 
    \param dst
        [out] The destination vector.
    \param src
        [in] The source vector.
*/
extern LIB3DSAPI void lib3ds_vector_copy( 
    float dst[3],  
    float src[3]);

/** 
    Negates all components of a vector. 
    \param c
        The Pointer to the vector.
*/
extern LIB3DSAPI void lib3ds_vector_neg(
    float c[3]);

extern LIB3DSAPI void lib3ds_vector_make(float c[3], float x, float y, float z);
extern LIB3DSAPI void lib3ds_vector_zero(float c[3]);
extern LIB3DSAPI void lib3ds_vector_add(float c[3], float a[3], float b[3]);
extern LIB3DSAPI void lib3ds_vector_sub(float c[3], float a[3], float b[3]);
extern LIB3DSAPI void lib3ds_vector_scalar_mul(float c[3], float a[3], float k);
extern LIB3DSAPI void lib3ds_vector_cross(float c[3], float a[3], float b[3]);
extern LIB3DSAPI float lib3ds_vector_dot(float a[3], float b[3]);
extern LIB3DSAPI float lib3ds_vector_length(float c[3]);
extern LIB3DSAPI void lib3ds_vector_normalize(float c[3]);
extern LIB3DSAPI void lib3ds_vector_normal(float n[3], float a[3], float b[3], float c[3]);
extern LIB3DSAPI void lib3ds_vector_min(float c[3], float a[3]);
extern LIB3DSAPI void lib3ds_vector_max(float c[3], float a[3]);
extern LIB3DSAPI void lib3ds_vector_transform(float c[3], float m[4][4], float a[3]);

extern LIB3DSAPI void lib3ds_quat_identity(float c[4]);
extern LIB3DSAPI void lib3ds_quat_copy(float dest[4], float src[4]);
extern LIB3DSAPI void lib3ds_quat_axis_angle(float c[4], float axis[3], float angle);
extern LIB3DSAPI void lib3ds_quat_neg(float c[4]);
extern LIB3DSAPI void lib3ds_quat_cnj(float c[4]);
extern LIB3DSAPI void lib3ds_quat_mul(float c[4], float a[4], float b[4]);
extern LIB3DSAPI void lib3ds_quat_scalar(float c[4], float k);
extern LIB3DSAPI void lib3ds_quat_normalize(float c[4]);
extern LIB3DSAPI void lib3ds_quat_inv(float c[4]);
extern LIB3DSAPI float lib3ds_quat_dot(float a[4], float b[4]);
extern LIB3DSAPI float lib3ds_quat_norm(float c[4]);
extern LIB3DSAPI void lib3ds_quat_ln(float c[4]);
extern LIB3DSAPI void lib3ds_quat_ln_dif(float c[4], float a[4], float b[4]);
extern LIB3DSAPI void lib3ds_quat_exp(float c[4]);
extern LIB3DSAPI void lib3ds_quat_slerp(float c[4], float a[4], float b[4], float t);
extern LIB3DSAPI void lib3ds_quat_squad(float c[4], float a[4], float p[4], float q[4], float b[4], float t);
extern LIB3DSAPI void lib3ds_quat_tangent(float c[4], float p[4], float q[4], float n[4]);

extern LIB3DSAPI void lib3ds_matrix_zero(float m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_identity(float  m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_copy(float dest[4][4], float src[4][4]);
extern LIB3DSAPI void lib3ds_matrix_neg(float m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_transpose(float m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_add(float m[4][4], float a[4][4], float b[4][4]);
extern LIB3DSAPI void lib3ds_matrix_sub(float m[4][4], float a[4][4], float b[4][4]);
extern LIB3DSAPI void lib3ds_matrix_mult(float m[4][4], float a[4][4], float b[4][4]);
extern LIB3DSAPI void lib3ds_matrix_scalar(float m[4][4], float k);
extern LIB3DSAPI float lib3ds_matrix_det(float m[4][4]);
extern LIB3DSAPI int lib3ds_matrix_inv(float m[4][4]);
extern LIB3DSAPI void lib3ds_matrix_translate(float m[4][4], float x, float y, float z);
extern LIB3DSAPI void lib3ds_matrix_scale(float m[4][4], float x, float y, float z);
extern LIB3DSAPI void lib3ds_matrix_rotate_quat(float m[4][4], float q[4]);
extern LIB3DSAPI void lib3ds_matrix_rotate(float m[4][4], float angle, float ax, float ay, float az);
extern LIB3DSAPI void lib3ds_matrix_camera(float m[4][4], float pos[3], float tgt[3], float roll);

/** @} */
#ifdef __cplusplus
}
#endif
#endif

