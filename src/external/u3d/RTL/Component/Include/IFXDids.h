//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
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

@file  IFXDids.h

	The header for the IFXDataElement identifiers.                           */

#ifndef IFXDIDS_H
#define IFXDIDS_H

#include "IFXUnknown.h"

extern const U32 IFX_DID_RENDERABLE;
extern const U32 IFX_DID_UNKNOWN;
extern const U32 IFX_DID_BOUND;

// DataElements Identifiers
IFXDEFINE_GUID(DID_IFXRenderableGroup,
0xab8373ae, 0xd306, 0x4b61, 0xa0, 0x1f, 0x2, 0xe0, 0xdb, 0x80, 0x4f, 0x27);

IFXDEFINE_GUID(DID_IFXRenderableGroupBounds,
0x2d15f6a3, 0x23ac, 0x4152, 0xbd, 0x4, 0x6f, 0xa0, 0xd1, 0x76, 0x1d, 0x2d);

IFXDEFINE_GUID(DID_IFXBoundFrame,
0xa984e964, 0xf025, 0x4a4c, 0x81, 0x9e, 0xa9, 0x10, 0x75, 0x35, 0x5b, 0xb4);

IFXDEFINE_GUID(DID_IFXBoundFrameBounds,
0x2f20f7ef, 0xc56b, 0x4e57, 0xaa, 0xcf, 0x2a, 0x46, 0x63, 0x2c, 0xf4, 0xa2);

IFXDEFINE_GUID(DID_IFXLightSet,
0xdadedf6a, 0xe55d, 0x4503, 0x9a, 0x89, 0x4f, 0x66, 0x2d, 0x7a, 0xb, 0xbc);

IFXDEFINE_GUID(DID_IFXNeighborMesh,
0xbab95c22, 0x2766, 0x40c1, 0x9c, 0xd0, 0x0, 0xa, 0x26, 0xae, 0x4d, 0xd);

IFXDEFINE_GUID(DID_IFXCLODController,
0x7D6F77D2, 0x2987, 0x4ADC, 0xAA, 0x1F, 0x3D, 0x0D, 0x71, 0x7D, 0xE4, 0x66);

IFXDEFINE_GUID(DID_IFXNeighborResController,
0x251b367e, 0x8c6f, 0x4dde, 0x8b, 0xc6, 0xac, 0x5e, 0x9, 0x22, 0x57, 0xee);

IFXDEFINE_GUID(DID_IFXTransform,
0x8c643112, 0xabef, 0x4797, 0xa1, 0xf6, 0x27, 0x77, 0xf7, 0xfb, 0xc, 0x9e);

IFXDEFINE_GUID(DID_IFXFrustum,
0xec603f6a, 0x22, 0x4531, 0xa3, 0x60, 0xb2, 0xc2, 0xe3, 0x59, 0x22, 0xfa);

IFXDEFINE_GUID(DID_IFXViewSize,
0xae53324, 0x4bdc, 0x45a4, 0x8f, 0x4a, 0xe9, 0xa4, 0xeb, 0x5e, 0xf3, 0xe4);

IFXDEFINE_GUID(DID_IFXSimulationTime,
0xaec394f4, 0x5824, 0x48fe, 0xa6, 0x82, 0x40, 0xcc, 0x58, 0x52, 0x23, 0x92);

IFXDEFINE_GUID(DID_IFXVertexWeights,
0x6c988a53, 0x2d98, 0x4304, 0xa2, 0x4a, 0xfa, 0x88, 0x91, 0x71, 0xc3, 0x42);

IFXDEFINE_GUID(DID_IFXBoneMatrices,
0xcf05d76c, 0xdd3b, 0x4235, 0x95, 0x7a, 0x3e, 0x7f, 0x66, 0xb6, 0xb4, 0x69);

IFXDEFINE_GUID(DID_IFXBonesManager,
0xf28336b7, 0x9876, 0x4c7e, 0x8c, 0xea, 0x3, 0xb9, 0xcd, 0x47, 0x5f, 0xe6);

IFXDEFINE_GUID(DID_IFXBoneWeights,
0xc7adbab7, 0xaba0, 0x4b48, 0x96, 0x73, 0xb6, 0xc6, 0xe8, 0xaa, 0xe3, 0xa1);

IFXDEFINE_GUID(DID_IFXMatrixSet,
0xb43c0625, 0x3040, 0x4779, 0xab, 0xba, 0x11, 0x85, 0x31, 0xe7, 0xb2, 0x84);

IFXDEFINE_GUID(DID_IFXSkeleton,
0x6ebd1e2f, 0x6e47, 0x49bf, 0xa7, 0x50, 0x63, 0x65, 0x2a, 0xd2, 0xff, 0x66);

IFXDEFINE_GUID(DID_IFXTexture, 
0x9bed642f, 0x3c75, 0x48a1, 0x84, 0x4a, 0xa6, 0x3b, 0xaa, 0xa1, 0x47, 0x2e);

IFXDEFINE_GUID(DTS_IFXRenderable,
0x2392F9C9, 0x3761, 0x44E8, 0x99, 0xA5, 0xF7, 0x17, 0x09, 0xE3, 0x7D, 0x67);

IFXDEFINE_GUID(DTS_IFXBound,
0x407e2c7, 0x53e6, 0x468c, 0xa1, 0x58, 0xe8, 0x17, 0xa9, 0x19, 0xd8, 0x51);

// No data is associated with this Did, 
// it is a place holder did for modifiers that have no direct output
// but need to have inputs. 
IFXDEFINE_GUID(DID_IFXNullOutput,
0x911B50BA, 0x633E, 0x4032, 0x85, 0xFC, 0xE1, 0xF5, 0x8B, 0x01, 0xC2, 0x53);


// Element Attribute bits. 
// Output change bits
//	Renderable
extern const U32 IFX_RENDERABLE_CB_SHADERLIST;
extern const U32 IFX_RENDERABLE_CB_BOUNDSPHERE;
extern const U32 IFX_RENDERABLE_CB_RESERVED0;
extern const U32 IFX_RENDERABLE_CB_RESERVED1;
extern const U32 IFX_RENDERABLE_CB_RESERVED2;
extern const U32 IFX_RENDERABLE_CB_RESERVED3;
extern const U32 IFX_RENDERABLE_CB_RESERVED4;
extern const U32 IFX_RENDERABLE_CB_RESERVED5;
extern const U32 IFX_RENDERABLE_CB_ALL;

// MeshGroup	
extern const U32 IFX_MESHGROUP_CB_FACECOUNT;
extern const U32 IFX_MESHGROUP_CB_FACES;
extern const U32 IFX_MESHGROUP_CB_VERTEXCOUNT;
extern const U32 IFX_MESHGROUP_CB_POSITIONS;
extern const U32 IFX_MESHGROUP_CB_NORMALS;
extern const U32 IFX_MESHGROUP_CB_DIFFUSECOLORS;
extern const U32 IFX_MESHGROUP_CB_SPECULARCOLORS;
extern const U32 IFX_MESHGROUP_CB_NUMMESHES;
extern const U32 IFX_MESHGROUP_CB_TRANSFORM;

extern const U32 IFX_MESHGROUP_CB_TEXCOORD0;
extern const U32 IFX_MESHGROUP_CB_TEXCOORD1;
extern const U32 IFX_MESHGROUP_CB_TEXCOORD2;
extern const U32 IFX_MESHGROUP_CB_TEXCOORD3;
extern const U32 IFX_MESHGROUP_CB_TEXCOORD4;
extern const U32 IFX_MESHGROUP_CB_TEXCOORD5;
extern const U32 IFX_MESHGROUP_CB_TEXCOORD6;
extern const U32 IFX_MESHGROUP_CB_TEXCOORD7;
extern const U32 IFX_MESHGROUP_CB_TEXCOORDS;		
extern const U32 IFX_MESHGROUP_CB_ALL;

#endif
