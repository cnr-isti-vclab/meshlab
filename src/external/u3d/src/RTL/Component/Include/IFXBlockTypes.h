//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
#ifndef IFXBLOCKTYPES_H__
#define IFXBLOCKTYPES_H__

#include "IFXDataTypes.h"

//***************************************************************************
//  Constants
//***************************************************************************

// U3D block types

/** 
For first edition and second edition, the version is a 32-bit field 
and the current version is zero (0x00000000).
For encoders that have not been validated, 
the file version must have first bit set.  
It is recommended that this be done by setting the most significant byte to 0xFF.

For third edition, the version is split into a 16-bit major version
and a 16-bit minor version.  The current major version is zero (0x0000).
The current minor version for the U3D Sample Software 1.2 Gold release is 0x0100.
The U3D Sample Software 1.2 Gold ignores the least significant byte 
of the minor version.  If significant modifications are made to 
the U3D file encoding methods (but still compliant to the ECMA-363 
U3D File Format Standard), it is expected the current minor version
could be incremented in the least significant byte to identify the 
change without causing those files to be rejected by applications using
the unmodified U3D Sample Software 1.2 Gold libraries.

Previous versions of the U3D Sample Software will reject 
U3D files with a non-zero minor version.  To disable use of the
minor version field, comment out #define for IFX_SUPPORT_MINOR_VERSION.
*/
// #define IFX_SUPPORT_MINOR_VERSION
static const U32 FileHeader_VersionFirstEditionBeta	= 0xFF000020;
static const U32 FileHeader_VersionFirstEdition		= 0x00000000;
static const U32 FileHeader_Version_1_2_Gold		= 0x00000100;
#ifndef IFX_SUPPORT_MINOR_VERSION
static const U32 FileHeader_VersionCurrent			= FileHeader_VersionFirstEdition;
#else
static const U32 FileHeader_VersionCurrent			= FileHeader_Version_1_2_Gold;
#endif

static const U32 BlockType_FileHeaderU3D			= 0x00443355;
static const U32 BlockType_FileReferenceU3D			= 0xFFFFFF12;
static const U32 BlockType_FileModifierChainU3D		= 0xFFFFFF14;
static const U32 BlockType_FilePriorityUpdateU3D	= 0xFFFFFF15;
static const U32 BlockType_FileNewObjectTypeU3D		= 0xFFFFFF16;

static const U32 BlockType_NodeGroupU3D		= 0xFFFFFF21;
static const U32 BlockType_NodeModelU3D		= 0xFFFFFF22;
static const U32 BlockType_NodeLightU3D		= 0xFFFFFF23;
static const U32 BlockType_NodeViewU3D		= 0xFFFFFF24;

static const U32 BlockType_GeneratorCLODMeshU3D							= 0xFFFFFF31;
static const U32 BlockType_GeneratorCLODBaseMeshContinuationU3D			= 0xFFFFFF3B;
static const U32 BlockType_GeneratorCLODProgressiveMeshContinuationU3D	= 0xFFFFFF3C;
static const U32 BlockType_GeneratorPointSetU3D							= 0xFFFFFF36;
static const U32 BlockType_GeneratorPointSetContinuationU3D				= 0xFFFFFF3E;
static const U32 BlockType_GeneratorLineSetU3D					        = 0xFFFFFF37;
static const U32 BlockType_GeneratorLineSetContinuationU3D				= 0xFFFFFF3F;

static const U32 BlockType_Modifier2DGlyphU3D		= 0xFFFFFF41;
static const U32 BlockType_ModifierSubdivisionU3D	= 0xFFFFFF42;
static const U32 BlockType_ModifierAnimationU3D		= 0xFFFFFF43;
static const U32 BlockType_ModifierBoneWeightsU3D	= 0xFFFFFF44;
static const U32 BlockType_ModifierShadingU3D		= 0xFFFFFF45;
static const U32 BlockType_ModifierCLODU3D		= 0xFFFFFF46;

static const U32 BlockType_ResourceLightU3D					= 0xFFFFFF51;
static const U32 BlockType_ResourceViewU3D					= 0xFFFFFF52;
static const U32 BlockType_ResourceLitTextureShaderU3D		= 0xFFFFFF53;
static const U32 BlockType_ResourceMaterialU3D				= 0xFFFFFF54;
static const U32 BlockType_ResourceTextureU3D				= 0xFFFFFF55;
static const U32 BlockType_ResourceTextureContinuationU3D	= 0xFFFFFF5C;
static const U32 BlockType_ResourceMotionU3D				= 0xFFFFFF56;

#endif
