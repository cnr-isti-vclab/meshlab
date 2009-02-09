/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_IMPL_H
#define INCLUDED_LIB3DS_IMPL_H
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

#include "lib3ds.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <setjmp.h>
#include <stdarg.h>

#ifdef _MSC_VER
#pragma warning ( disable : 4996 )
#endif

#ifndef _MSC_VER
#include <stdint.h>
#else
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define LIB3DS_EPSILON (1e-5)
#define LIB3DS_PI 3.14159265358979323846
#define LIB3DS_TWOPI (2.0*LIB3DS_PI)
#define LIB3DS_HALFPI (LIB3DS_PI/2.0)
#define LIB3DS_RAD_TO_DEG(x) ((180.0/LIB3DS_PI)*(x))
#define LIB3DS_DEG_TO_RAD(x) ((LIB3DS_PI/180.0)*(x))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Lib3dsChunks {
  CHK_NULL_CHUNK             =0x0000,
  CHK_M3DMAGIC               =0x4D4D,    /*3DS file*/
  CHK_SMAGIC                 =0x2D2D,    
  CHK_LMAGIC                 =0x2D3D,    
  CHK_MLIBMAGIC              =0x3DAA,    /*MLI file*/
  CHK_MATMAGIC               =0x3DFF,    
  CHK_CMAGIC                 =0xC23D,    /*PRJ file*/
  CHK_M3D_VERSION            =0x0002,
  CHK_M3D_KFVERSION          =0x0005,

  CHK_COLOR_F                =0x0010,
  CHK_COLOR_24               =0x0011,
  CHK_LIN_COLOR_24           =0x0012,
  CHK_LIN_COLOR_F            =0x0013,
  CHK_INT_PERCENTAGE         =0x0030,
  CHK_FLOAT_PERCENTAGE       =0x0031,

  CHK_MDATA                  =0x3D3D,
  CHK_MESH_VERSION           =0x3D3E,
  CHK_MASTER_SCALE           =0x0100,
  CHK_LO_SHADOW_BIAS         =0x1400,
  CHK_HI_SHADOW_BIAS         =0x1410,
  CHK_SHADOW_MAP_SIZE        =0x1420,
  CHK_SHADOW_SAMPLES         =0x1430,
  CHK_SHADOW_RANGE           =0x1440,
  CHK_SHADOW_FILTER          =0x1450,
  CHK_RAY_BIAS               =0x1460,
  CHK_O_CONSTS               =0x1500,
  CHK_AMBIENT_LIGHT          =0x2100,
  CHK_BIT_MAP                =0x1100,
  CHK_SOLID_BGND             =0x1200,
  CHK_V_GRADIENT             =0x1300,
  CHK_USE_BIT_MAP            =0x1101,
  CHK_USE_SOLID_BGND         =0x1201,
  CHK_USE_V_GRADIENT         =0x1301,
  CHK_FOG                    =0x2200,
  CHK_FOG_BGND               =0x2210,
  CHK_LAYER_FOG              =0x2302,
  CHK_DISTANCE_CUE           =0x2300,
  CHK_DCUE_BGND              =0x2310,
  CHK_USE_FOG                =0x2201,
  CHK_USE_LAYER_FOG          =0x2303,
  CHK_USE_DISTANCE_CUE       =0x2301,

  CHK_MAT_ENTRY              =0xAFFF,
  CHK_MAT_NAME               =0xA000,
  CHK_MAT_AMBIENT            =0xA010,
  CHK_MAT_DIFFUSE            =0xA020,
  CHK_MAT_SPECULAR           =0xA030,
  CHK_MAT_SHININESS          =0xA040,
  CHK_MAT_SHIN2PCT           =0xA041,
  CHK_MAT_TRANSPARENCY       =0xA050,
  CHK_MAT_XPFALL             =0xA052,
  CHK_MAT_USE_XPFALL         =0xA240,
  CHK_MAT_REFBLUR            =0xA053,
  CHK_MAT_SHADING            =0xA100,
  CHK_MAT_USE_REFBLUR        =0xA250,
  CHK_MAT_SELF_ILLUM         =0xA080,
  CHK_MAT_TWO_SIDE           =0xA081,
  CHK_MAT_DECAL              =0xA082,
  CHK_MAT_ADDITIVE           =0xA083,
  CHK_MAT_SELF_ILPCT         =0xA084,
  CHK_MAT_WIRE               =0xA085,
  CHK_MAT_FACEMAP            =0xA088,
  CHK_MAT_PHONGSOFT          =0xA08C,
  CHK_MAT_WIREABS            =0xA08E,
  CHK_MAT_WIRE_SIZE          =0xA087,
  CHK_MAT_TEXMAP             =0xA200,
  CHK_MAT_SXP_TEXT_DATA      =0xA320,
  CHK_MAT_TEXMASK            =0xA33E,
  CHK_MAT_SXP_TEXTMASK_DATA  =0xA32A,
  CHK_MAT_TEX2MAP            =0xA33A,
  CHK_MAT_SXP_TEXT2_DATA     =0xA321,
  CHK_MAT_TEX2MASK           =0xA340,
  CHK_MAT_SXP_TEXT2MASK_DATA =0xA32C,
  CHK_MAT_OPACMAP            =0xA210,
  CHK_MAT_SXP_OPAC_DATA      =0xA322,
  CHK_MAT_OPACMASK           =0xA342,
  CHK_MAT_SXP_OPACMASK_DATA  =0xA32E,
  CHK_MAT_BUMPMAP            =0xA230,
  CHK_MAT_SXP_BUMP_DATA      =0xA324,
  CHK_MAT_BUMPMASK           =0xA344,
  CHK_MAT_SXP_BUMPMASK_DATA  =0xA330,
  CHK_MAT_SPECMAP            =0xA204,
  CHK_MAT_SXP_SPEC_DATA      =0xA325,
  CHK_MAT_SPECMASK           =0xA348,
  CHK_MAT_SXP_SPECMASK_DATA  =0xA332,
  CHK_MAT_SHINMAP            =0xA33C,
  CHK_MAT_SXP_SHIN_DATA      =0xA326,
  CHK_MAT_SHINMASK           =0xA346,
  CHK_MAT_SXP_SHINMASK_DATA  =0xA334,
  CHK_MAT_SELFIMAP           =0xA33D,
  CHK_MAT_SXP_SELFI_DATA     =0xA328,
  CHK_MAT_SELFIMASK          =0xA34A,
  CHK_MAT_SXP_SELFIMASK_DATA =0xA336,
  CHK_MAT_REFLMAP            =0xA220,
  CHK_MAT_REFLMASK           =0xA34C,
  CHK_MAT_SXP_REFLMASK_DATA  =0xA338,
  CHK_MAT_ACUBIC             =0xA310,
  CHK_MAT_MAPNAME            =0xA300,
  CHK_MAT_MAP_TILING         =0xA351,
  CHK_MAT_MAP_TEXBLUR        =0xA353,
  CHK_MAT_MAP_USCALE         =0xA354,
  CHK_MAT_MAP_VSCALE         =0xA356,
  CHK_MAT_MAP_UOFFSET        =0xA358,
  CHK_MAT_MAP_VOFFSET        =0xA35A,
  CHK_MAT_MAP_ANG            =0xA35C,
  CHK_MAT_MAP_COL1           =0xA360,
  CHK_MAT_MAP_COL2           =0xA362,
  CHK_MAT_MAP_RCOL           =0xA364,
  CHK_MAT_MAP_GCOL           =0xA366,
  CHK_MAT_MAP_BCOL           =0xA368,

  CHK_NAMED_OBJECT           =0x4000,
  CHK_N_DIRECT_LIGHT         =0x4600,
  CHK_DL_OFF                 =0x4620,
  CHK_DL_OUTER_RANGE         =0x465A,
  CHK_DL_INNER_RANGE         =0x4659,
  CHK_DL_MULTIPLIER          =0x465B,
  CHK_DL_EXCLUDE             =0x4654,
  CHK_DL_ATTENUATE           =0x4625,
  CHK_DL_SPOTLIGHT           =0x4610,
  CHK_DL_SPOT_ROLL           =0x4656,
  CHK_DL_SHADOWED            =0x4630,
  CHK_DL_LOCAL_SHADOW2       =0x4641,
  CHK_DL_SEE_CONE            =0x4650,
  CHK_DL_SPOT_RECTANGULAR    =0x4651,
  CHK_DL_SPOT_ASPECT         =0x4657,
  CHK_DL_SPOT_PROJECTOR      =0x4653,
  CHK_DL_SPOT_OVERSHOOT      =0x4652,
  CHK_DL_RAY_BIAS            =0x4658,
  CHK_DL_RAYSHAD             =0x4627,
  CHK_N_CAMERA               =0x4700,
  CHK_CAM_SEE_CONE           =0x4710,
  CHK_CAM_RANGES             =0x4720,
  CHK_OBJ_HIDDEN             =0x4010,
  CHK_OBJ_VIS_LOFTER         =0x4011,
  CHK_OBJ_DOESNT_CAST        =0x4012,
  CHK_OBJ_DONT_RCVSHADOW     =0x4017,
  CHK_OBJ_MATTE              =0x4013,
  CHK_OBJ_FAST               =0x4014,
  CHK_OBJ_PROCEDURAL         =0x4015,
  CHK_OBJ_FROZEN             =0x4016,
  CHK_N_TRI_OBJECT           =0x4100,
  CHK_POINT_ARRAY            =0x4110,
  CHK_POINT_FLAG_ARRAY       =0x4111,
  CHK_FACE_ARRAY             =0x4120,
  CHK_MSH_MAT_GROUP          =0x4130,
  CHK_SMOOTH_GROUP           =0x4150,
  CHK_MSH_BOXMAP             =0x4190,
  CHK_TEX_VERTS              =0x4140,
  CHK_MESH_MATRIX            =0x4160,
  CHK_MESH_COLOR             =0x4165,
  CHK_MESH_TEXTURE_INFO      =0x4170,

  CHK_KFDATA                 =0xB000,
  CHK_KFHDR                  =0xB00A,
  CHK_KFSEG                  =0xB008,
  CHK_KFCURTIME              =0xB009,
  CHK_AMBIENT_NODE_TAG       =0xB001,
  CHK_OBJECT_NODE_TAG        =0xB002,
  CHK_CAMERA_NODE_TAG        =0xB003,
  CHK_TARGET_NODE_TAG        =0xB004,
  CHK_LIGHT_NODE_TAG         =0xB005,
  CHK_L_TARGET_NODE_TAG      =0xB006,
  CHK_SPOTLIGHT_NODE_TAG     =0xB007,
  CHK_NODE_ID                =0xB030,
  CHK_NODE_HDR               =0xB010,
  CHK_PIVOT                  =0xB013,
  CHK_INSTANCE_NAME          =0xB011,
  CHK_MORPH_SMOOTH           =0xB015,
  CHK_BOUNDBOX               =0xB014,
  CHK_POS_TRACK_TAG          =0xB020,
  CHK_COL_TRACK_TAG          =0xB025,
  CHK_ROT_TRACK_TAG          =0xB021,
  CHK_SCL_TRACK_TAG          =0xB022,
  CHK_MORPH_TRACK_TAG        =0xB026,
  CHK_FOV_TRACK_TAG          =0xB023,
  CHK_ROLL_TRACK_TAG         =0xB024,
  CHK_HOT_TRACK_TAG          =0xB027,
  CHK_FALL_TRACK_TAG         =0xB028,
  CHK_HIDE_TRACK_TAG         =0xB029,

  CHK_POLY_2D                = 0x5000,
  CHK_SHAPE_OK               = 0x5010,
  CHK_SHAPE_NOT_OK           = 0x5011,
  CHK_SHAPE_HOOK             = 0x5020,
  CHK_PATH_3D                = 0x6000,
  CHK_PATH_MATRIX            = 0x6005,
  CHK_SHAPE_2D               = 0x6010,
  CHK_M_SCALE                = 0x6020,
  CHK_M_TWIST                = 0x6030,
  CHK_M_TEETER               = 0x6040,
  CHK_M_FIT                  = 0x6050,
  CHK_M_BEVEL                = 0x6060,
  CHK_XZ_CURVE               = 0x6070,
  CHK_YZ_CURVE               = 0x6080,
  CHK_INTERPCT               = 0x6090,
  CHK_DEFORM_LIMIT           = 0x60A0,

  CHK_USE_CONTOUR            = 0x6100,
  CHK_USE_TWEEN              = 0x6110,
  CHK_USE_SCALE              = 0x6120,
  CHK_USE_TWIST              = 0x6130,
  CHK_USE_TEETER             = 0x6140,
  CHK_USE_FIT                = 0x6150,
  CHK_USE_BEVEL              = 0x6160,

  CHK_DEFAULT_VIEW           = 0x3000,
  CHK_VIEW_TOP               = 0x3010,
  CHK_VIEW_BOTTOM            = 0x3020,
  CHK_VIEW_LEFT              = 0x3030,
  CHK_VIEW_RIGHT             = 0x3040,
  CHK_VIEW_FRONT             = 0x3050,
  CHK_VIEW_BACK              = 0x3060,
  CHK_VIEW_USER              = 0x3070,
  CHK_VIEW_CAMERA            = 0x3080,
  CHK_VIEW_WINDOW            = 0x3090,

  CHK_VIEWPORT_LAYOUT_OLD    = 0x7000,
  CHK_VIEWPORT_DATA_OLD      = 0x7010,
  CHK_VIEWPORT_LAYOUT        = 0x7001,
  CHK_VIEWPORT_DATA          = 0x7011,
  CHK_VIEWPORT_DATA_3        = 0x7012,
  CHK_VIEWPORT_SIZE          = 0x7020,
  CHK_NETWORK_VIEW           = 0x7030
} Lib3dsChunks;

typedef struct Lib3dsChunk {
    uint16_t chunk;
    uint32_t size;
    uint32_t end;
    uint32_t cur;
} Lib3dsChunk; 

extern void lib3ds_chunk_read(Lib3dsChunk *c, Lib3dsIo *io);
extern void lib3ds_chunk_read_start(Lib3dsChunk *c, uint16_t chunk, Lib3dsIo *io);
extern void lib3ds_chunk_read_tell(Lib3dsChunk *c, Lib3dsIo *io);
extern uint16_t lib3ds_chunk_read_next(Lib3dsChunk *c, Lib3dsIo *io);
extern void lib3ds_chunk_read_reset(Lib3dsChunk *c, Lib3dsIo *io);
extern void lib3ds_chunk_read_end(Lib3dsChunk *c, Lib3dsIo *io);
extern void lib3ds_chunk_write(Lib3dsChunk *c, Lib3dsIo *io);
extern void lib3ds_chunk_write_start(Lib3dsChunk *c, Lib3dsIo *io);
extern void lib3ds_chunk_write_end(Lib3dsChunk *c, Lib3dsIo *io);
extern const char* lib3ds_chunk_name(uint16_t chunk);
extern void lib3ds_chunk_unknown(uint16_t chunk, Lib3dsIo *io);

typedef struct Lib3dsIoImpl {
    jmp_buf jmpbuf;
    int log_indent;
    void *tmp_mem;
    Lib3dsNode *tmp_node;
} Lib3dsIoImpl;

extern void lib3ds_io_setup(Lib3dsIo *io);
extern void lib3ds_io_cleanup(Lib3dsIo *io);

extern long lib3ds_io_seek(Lib3dsIo *io, long offset, Lib3dsIoSeek origin);
extern long lib3ds_io_tell(Lib3dsIo *io);
extern size_t lib3ds_io_read(Lib3dsIo *io, void *buffer, size_t size);
extern size_t lib3ds_io_write(Lib3dsIo *io, const void *buffer, size_t size);
extern void lib3ds_io_log(Lib3dsIo *io, Lib3dsLogLevel level, const char *format, ...);
extern void lib3ds_io_log_indent(Lib3dsIo *io, int indent);
extern void lib3ds_io_read_error(Lib3dsIo *io);
extern void lib3ds_io_write_error(Lib3dsIo *io);

extern uint8_t lib3ds_io_read_byte(Lib3dsIo *io);
extern uint16_t lib3ds_io_read_word(Lib3dsIo *io);
extern uint32_t lib3ds_io_read_dword(Lib3dsIo *io);
extern int8_t lib3ds_io_read_intb(Lib3dsIo *io);
extern int16_t lib3ds_io_read_intw(Lib3dsIo *io);
extern int32_t lib3ds_io_read_intd(Lib3dsIo *io);
extern float lib3ds_io_read_float(Lib3dsIo *io);
extern void lib3ds_io_read_vector(Lib3dsIo *io, float v[3]);
extern void lib3ds_io_read_rgb(Lib3dsIo *io, float rgb[3]);
extern void lib3ds_io_read_string(Lib3dsIo *io, char *s, int buflen);

extern void lib3ds_io_write_byte(Lib3dsIo *io, uint8_t b);
extern void lib3ds_io_write_word(Lib3dsIo *io, uint16_t w);
extern void lib3ds_io_write_dword(Lib3dsIo *io, uint32_t d);
extern void lib3ds_io_write_intb(Lib3dsIo *io, int8_t b);
extern void lib3ds_io_write_intw(Lib3dsIo *io, int16_t w);
extern void lib3ds_io_write_intd(Lib3dsIo *io, int32_t d);
extern void lib3ds_io_write_float(Lib3dsIo *io, float l);
extern void lib3ds_io_write_vector(Lib3dsIo *io, float v[3]);
extern void lib3ds_io_write_rgb(Lib3dsIo *io, float rgb[3]);
extern void lib3ds_io_write_string(Lib3dsIo *io, const char *s);

extern void lib3ds_atmosphere_read(Lib3dsAtmosphere *atmosphere, Lib3dsIo *io);
extern void lib3ds_atmosphere_write(Lib3dsAtmosphere *atmosphere, Lib3dsIo *io);
extern void lib3ds_background_read(Lib3dsBackground *background, Lib3dsIo *io);
extern void lib3ds_background_write(Lib3dsBackground *background, Lib3dsIo *io);
extern void lib3ds_shadow_read(Lib3dsShadow *shadow, Lib3dsIo *io);
extern void lib3ds_shadow_write(Lib3dsShadow *shadow, Lib3dsIo *io);
extern void lib3ds_viewport_read(Lib3dsViewport *viewport, Lib3dsIo *io);
extern void lib3ds_viewport_write(Lib3dsViewport *viewport, Lib3dsIo *io);
extern void lib3ds_material_read(Lib3dsMaterial *material, Lib3dsIo *io);
extern void lib3ds_material_write(Lib3dsMaterial *material, Lib3dsIo *io);
extern void lib3ds_camera_read(Lib3dsCamera *camera, Lib3dsIo *io);
extern void lib3ds_camera_write(Lib3dsCamera *camera, Lib3dsIo *io);
extern void lib3ds_light_read(Lib3dsLight *light, Lib3dsIo *io);
extern void lib3ds_light_write(Lib3dsLight *light, Lib3dsIo *io);
extern void lib3ds_mesh_read(Lib3dsFile *file, Lib3dsMesh *mesh, Lib3dsIo *io);
extern void lib3ds_mesh_write(Lib3dsFile *file, Lib3dsMesh *mesh, Lib3dsIo *io);
extern void lib3ds_track_read(Lib3dsTrack *track, Lib3dsIo *io);
extern void lib3ds_track_write(Lib3dsTrack *track, Lib3dsIo *io);
extern void lib3ds_node_read(Lib3dsNode *node, Lib3dsIo *io);
extern void lib3ds_node_write(Lib3dsNode *node, uint16_t node_id, uint16_t parent_id, Lib3dsIo *io);

typedef void (*Lib3dsFreeFunc)(void *ptr);

extern void* lib3ds_util_realloc_array(void *ptr, int old_size, int new_size, int element_size);
extern void lib3ds_util_reserve_array(void ***ptr, int *n, int *size, int new_size, int force, Lib3dsFreeFunc free_func);
extern void lib3ds_util_insert_array(void ***ptr, int *n, int *size, void *element, int index);
extern void lib3ds_util_remove_array(void ***ptr, int *n, int index, Lib3dsFreeFunc free_func);

#ifdef __cplusplus
}
#endif
#endif


