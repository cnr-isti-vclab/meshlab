//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************

/** 
	@file	IFXEnums.h

			Defines basic IFX constants
*/

#ifndef IFX_ENUMS_H
#define IFX_ENUMS_H

#include "IFXAttributes.h"
#include "IFXDataTypes.h"

typedef U32 IFXenum;

//=========================
// IFXRender Defines
//=========================
#define IFX_MAX_NUM_INDICES		(0xFFFF*3)
#define IFX_MAX_NUM_VERTICES	(0xFFFF)


// Lights
#define IFX_MAX_LIGHTS		0x0008
#define IFX_LIGHT0			0x0100
#define IFX_LIGHT_GLOBAL_AMBIENT	0x0130
#define IFX_AMBIENT			0x0130
#define IFX_DIRECTIONAL		0x0131
#define IFX_POINT			0x0132
#define IFX_SPOT			0x0133
#define IFX_LIGHTING		0x0134

// Textures
#define IFX_MAX_TEXUNITS	0x0008
#define IFX_TEXUNIT0		0x0200

#define IFX_TEXTURE_1D		0x0240
#define IFX_TEXTURE_2D		0x0241
#define IFX_TEXTURE_3D		0x0242
#define IFX_TEXTURE_CUBE	0x0250
#define IFX_TEXTURE_CUBE_PX	0x0251
#define IFX_TEXTURE_CUBE_NX	0x0252
#define IFX_TEXTURE_CUBE_PY	0x0253
#define IFX_TEXTURE_CUBE_NY	0x0254
#define IFX_TEXTURE_CUBE_PZ	0x0255
#define IFX_TEXTURE_CUBE_NZ	0x0256

#define IFXTEXTURESOURCE_UNDEFINED					0x0289
#define IFXTEXTURESOURCE_DECODED					0x0290
#define IFXTEXURESOURCE_OTHER						0x0291
#define IFXTEXTURECOMPRESSIONSTATE_FORCEDECOMPRESS	0x0292
#define IFXTEXTURECOMPRESSIONSTATE_FORCECOMPRESSED	0x0293

#define IFX_SELECT_ARG0		0x0000
#define IFX_ADD             0x0001
#define IFX_MODULATE        0x0002
#define IFX_INTERPOLATE		0x0003
#define IFX_MODULATE2X      0x0004
#define IFX_MODULATE4X      0x0005
#define IFX_ADDSIGNED       0x0006   
#define IFX_ADDSIGNED2X     0x0007   
#define IFX_DOTPRODUCT3     0x0008

// Texture Unit Input Options
#define IFX_INCOMING		0x0330
#define IFX_DIFFUSE			0x0331
#define IFX_TEXTURE			0x0332
#define IFX_CONSTANT		0x0333

// Texture Unit Input modifiers
#define IFX_ALPHA			0x0040
#define IFX_INVERSE			0x0080

// Filtering Options
#define IFX_NEAREST			0x0340
#define IFX_LINEAR			0x0341
// Mipmode can use IFX_NONE as well

// Texture Coordinate Gen Options
#define IFX_NONE						0x0351
#define IFX_TEXGEN_REFLECTION_SPHERE	0x0352

// The rest of these produce 3 component texture 
// coordinates.  Good for projecting 2D textures
// or cubemaps.
#define IFX_TEXGEN_VIEWPOSITION			0x0353
#define IFX_TEXGEN_VIEWNORMAL			0x0354
#define IFX_TEXGEN_VIEWREFLECTION		0x0355
#define IFX_TEXGEN_WORLDPOSITION		0x0356
#define IFX_TEXGEN_WORLDNORMAL			0x0357
#define IFX_TEXGEN_WORLDREFLECTION		0x0358
#define IFX_TEXGEN_LOCALPOSITION		0x0359
#define IFX_TEXGEN_LOCALNORMAL			0x035A
#define IFX_TEXGEN_LOCALREFLECTION		0x035B

// Image Format Options
#define IFX_BGRA			0x0390
#define IFX_RGBA			0x0391
#define IFX_RGBA_8888		0x0001
#define IFX_RGBA_8880		0x0002
#define IFX_RGBA_5650		0x0003
#define IFX_RGBA_5550		0x0004
#define IFX_RGBA_5551		0x0005
#define IFX_RGBA_4444		0x0006
#define IFX_TEXCOMPRESS_1	0x1040
#define IFX_TEXCOMPRESS_2	0x1080
#define IFX_TEXCOMPRESS_3	0x1100
#define IFX_TEXCOMPRESS_4	0x1200
#define IFX_TEXCOMPRESS_5	0x1400
#define IFX_FMT_DEFAULT		0x0000

// Depth-Stencil Format Options
#define IFX_DEPTH_D16S0		0x2001
#define IFX_DEPTH_D15S1		0x2002
#define IFX_DEPTH_D24S8		0x2004
#define IFX_DEPTH_D24S4		0x2008
#define IFX_DEPTH_D32S0		0x2010

// Projection Mode Options
#define IFX_PERSPECTIVE3	0x0000 // 3-point perspective projection
#define IFX_ORTHOGRAPHIC	0x0002 // ortho projection
#define IFX_PERSPECTIVE2	0x0004 // 2-point perspective projection
#define IFX_PERSPECTIVE1	0x0006 // 1-point perspective projection

#define IFX_SCREENPIXELS	0x0000 // screen position units expressed in screen pixels
#define IFX_PERCENTDIMEN	0x0001 // screen position units expressed as percentage of screen dimensions

// Fog Options
#define IFX_FOG				0x0500
#define IFX_FOG_LINEAR		0x0000
#define IFX_FOG_EXP			0x0001
#define IFX_FOG_EXP2		0x0002

#define IFX_DEPTH_TEST		0x0600
#define IFX_DEPTH_WRITE		0x0601
#define IFX_FB_BLEND		0x0602
#define IFX_FB_ALPHA_TEST	0x0603
#define IFX_FB_ADD			0x0604
#define IFX_FB_MULT			0x0605
#define IFX_FB_ALPHA_BLEND	0x0606
#define IFX_FB_INV_ALPHA_BLEND 0x0607
#define IFX_FB_COPY			0x0608
#define IFX_FB_INVISIBLE	0x0609
#define IFX_FB_ADD_BLEND	0x06AA
#define IFX_CULL			0x060A
#define IFX_CULL_NONE		0x060B
#define IFX_CULL_CW			0x060C
#define IFX_CULL_CCW		0x060D
#define IFX_NEVER			0x0610
#define IFX_LESS			0x0611
#define IFX_GREATER			0x0612
#define IFX_EQUAL			0x0613
#define IFX_NOT_EQUAL		0x0614
#define IFX_LEQUAL			0x0615
#define IFX_GEQUAL			0x0616
#define IFX_ALWAYS			0x0617
#define IFX_KEEP			0x0620
#define IFX_ZERO			0x0621
#define IFX_REPLACE			0x0622
#define IFX_INCR			0x0623
#define IFX_DECR			0x0624
#define IFX_INVERT			0x0625
#define IFX_STENCIL			0x0626
#define IFX_DEPTH_OFFSET	0x0630
#define IFX_VSYNC			0x0631
#define IFX_SMOOTH			0x0632
#define IFX_FLAT			0x0633
#define IFX_FILLED			0x0634
#define IFX_WIREFRAME		0x0635
#define IFX_POINTS			0x0636

#define IFX_DEPTH_BUFFER	0x0100
#define IFX_COLOR_BUFFER	0x4000
#define IFX_STENCIL_BUFFER	0x0400

#define IFX_CLIP_BOTTOM		0x0001
#define IFX_CLIP_TOP		0x0002
#define IFX_CLIP_RIGHT		0x0004
#define IFX_CLIP_LEFT		0x0008
#define IFX_CLIP_FRONT		0x0010
#define IFX_CLIP_BACK		0x0020
#define IFX_CLIP_ALL		0x003F

#define IFX_FRUSTUM_BOTTOM	0x0000
#define IFX_FRUSTUM_TOP		0x0001
#define IFX_FRUSTUM_RIGHT	0x0002
#define IFX_FRUSTUM_LEFT	0x0003
#define IFX_FRUSTUM_FRONT	0x0004
#define IFX_FRUSTUM_BACK	0x0005

// Mesh Attributes
#define IFX_MESH_POSITION		0x0000
#define IFX_MESH_NORMAL			0x0001
#define IFX_MESH_DIFFUSE_COLOR	0x0002
#define IFX_MESH_SPECULAR_COLOR	0x0003
#define IFX_MESH_TC0			0x0004
#define IFX_MESH_FACE			(IFX_MESH_TC0+IFX_MAX_TEXUNITS+0)
#define IFX_MESH_LINE			(IFX_MESH_TC0+IFX_MAX_TEXUNITS+1)
#define IFX_MESH_RENDER_TC0		(IFX_MESH_TC0+IFX_MAX_TEXUNITS+2)
#define IFX_MESH_NUM_ATTRIBUTES	(IFX_MESH_RENDER_TC0+IFX_MAX_TEXUNITS)
#define IFX_MESH_POINT    	    (IFX_MESH_TC0+IFX_MAX_TEXUNITS+3)

typedef IFXAttributes<IFX_MESH_NUM_ATTRIBUTES> IFXMeshAttributes;

// Performance Data
#define IFX_NUM_FACES			0x2001
#define IFX_NUM_VERTICES		0x2002
#define IFX_NUM_MESHES			0x2003
#define IFX_NUM_RENDER_CALLS	0x2004

// AntiAliasing modes
#define IFX_AA_2X			0x0001
#define IFX_AA_3X			0x0002
#define IFX_AA_4X			0x0003
#define IFX_AA_4X_SW		0x0010
#define IFX_AA_DEFAULT		0x0200
#define IFX_AA_DISABLED		0x0000
#define IFX_AA_ENABLED		0x0001

// Overlays / Backdrops
#define IFX_VIEW_OVERLAY	0x0000
#define IFX_VIEW_BACKDROP	0x0001
#define IFX_VIEW_NUM_LAYERS	0x0002

// Culling
#define IFX_FRONT			0x0404
#define IFX_BACK			0x0405

// UV Generators
#define IFX_UV_VERTEX		0x0000
#define	IFX_UV_NORMAL		0x0001

#define IFX_UV_PLANAR		0x0000
#define IFX_UV_SPHERICAL	0x0001
#define IFX_UV_CYLINDRICAL	0x0002
#define IFX_UV_REFLECTION	0x0003
#define IFX_UV_NONE			0x0004
#define IFX_UV_NUM_MODES	0x0005

#define IFX_SHADE_STYLE_GRADIENT		0x0000
#define IFX_SHADE_STYLE_TOON			0x0001
#define IFX_SHADE_STYLE_BLACK_AND_WHITE	0x0002

#define IFX_SHADER_STANDARD		0x0000
#define IFX_SHADER_PAINTER		0x0001
#define IFX_SHADER_ENGRAVER		0x0002
#define IFX_SHADER_NEWSPRINT	0x0003

#endif
