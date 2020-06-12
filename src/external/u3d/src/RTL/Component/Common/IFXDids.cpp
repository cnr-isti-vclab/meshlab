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
/*
@file  IFXDids.cpp

	The definition of the IFXDataElement identifiers.                        */

//#define IFX_INIT_GUID

#include "IFXCOM.h"
#include "IFXDids.h"

const U32 IFX_DID_RENDERABLE   = 0x01;

const U32 IFX_DID_UNKNOWN      = 0x02;

const U32 IFX_DID_BOUND        = 0x04;

// Element Attribute bits. 
// Output change bits
//	Renderable
const U32 IFX_RENDERABLE_CB_SHADERLIST	= 1u<<24;
const U32 IFX_RENDERABLE_CB_BOUNDSPHERE	= 1u<<25;
const U32 IFX_RENDERABLE_CB_RESERVED0	= 1u<<26;
const U32 IFX_RENDERABLE_CB_RESERVED1	= 1u<<27;
const U32 IFX_RENDERABLE_CB_RESERVED2	= 1u<<28;
const U32 IFX_RENDERABLE_CB_RESERVED3	= 1u<<29;
const U32 IFX_RENDERABLE_CB_RESERVED4	= 1u<<30;
const U32 IFX_RENDERABLE_CB_RESERVED5	= 1u<<31;
const U32 IFX_RENDERABLE_CB_ALL		= 0xFF000000;

// MeshGroup	
const U32 IFX_MESHGROUP_CB_FACECOUNT  	= 1u<<1;
const U32 IFX_MESHGROUP_CB_VERTEXCOUNT	= 1u<<2;
const U32 IFX_MESHGROUP_CB_POSITIONS	= 1u<<3;
const U32 IFX_MESHGROUP_CB_NORMALS		= 1u<<4;
const U32 IFX_MESHGROUP_CB_DIFFUSECOLORS	= 1u<<5;
const U32 IFX_MESHGROUP_CB_SPECULARCOLORS	= 1u<<6;
const U32 IFX_MESHGROUP_CB_NUMMESHES	= 1u<<7;
	
const U32 IFX_MESHGROUP_CB_TEXCOORD0	= 1u<<8;
const U32 IFX_MESHGROUP_CB_TEXCOORD1	= 1u<<9;
const U32 IFX_MESHGROUP_CB_TEXCOORD2	= 1u<<10;
const U32 IFX_MESHGROUP_CB_TEXCOORD3	= 1u<<11;
const U32 IFX_MESHGROUP_CB_TEXCOORD4	= 1u<<12;
const U32 IFX_MESHGROUP_CB_TEXCOORD5	= 1u<<13;
const U32 IFX_MESHGROUP_CB_TEXCOORD6	= 1u<<14;
const U32 IFX_MESHGROUP_CB_TEXCOORD7	= 1u<<15;
const U32 IFX_MESHGROUP_CB_TEXCOORDS	= 0xFF<<8;		
const U32 IFX_MESHGROUP_CB_FACES	  	= 1u<<16;
const U32 IFX_MESHGROUP_CB_TRANSFORM	= 1u<<17;
const U32 IFX_MESHGROUP_CB_ALL			= 0x00FFFFFF;

