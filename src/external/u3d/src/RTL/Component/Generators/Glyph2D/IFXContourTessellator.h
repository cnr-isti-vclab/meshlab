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

/**
  @file	IFXContourTesselator.h

		Header file for the font classes.
*/

#ifndef __IFXCONTOURTESSELATOR_INTERFACES_H__
#define __IFXCONTOURTESSELATOR_INTERFACES_H__

#include "IFXUnknown.h"
#include "IFXSimpleList.h"
#include "IFXContour.h"
#include "IFXContourExtruder.h"
#include "IFXMesh.h"
#include "IFXMeshGroup.h"

// {139C63D9-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXContourTessellator,
0x139c63d9, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

struct SIFXTessellatorProperties 
{
  EIFXGlyphFacing eFacing;
  EIFXGlyphNormalOrientation eNormalOrientation;
  BOOL bNormals;
  F64 fDepth;

  BOOL bInFacing;
  BOOL bOutFacing;
  BOOL bWindClockWise;
  BOOL bInverseNormals;

#ifdef CONTOURTESSELLATOR_BUILD_OPENGL_TESTCODE
  GLuint uDisplayList;
#endif
};


/**
This class tessellates a set of contours.  It is used by IFXGlyph3DGenerator.
It is an internal API.  See IFXGlyph3DGenerator for documentation.
*/
class IFXContourTessellator:public IFXUnknown 
{
public:
  virtual IFXRESULT IFXAPI  Tessellate(SIFXTessellatorProperties* pTessellatorProperties, IFXSimpleList* pGlyphList, IFXMeshGroup** ppMeshGroup)=0;
  virtual IFXRESULT IFXAPI  Tessellate(SIFXTessellatorProperties* pTessellatorProperties, IFXContour* pGlyph, IFXMesh** ppMesh)=0;
  virtual IFXRESULT Tessellate
  (
   IFXSimpleList* pGlyphList,
   SIFXTessellatorProperties* pTessellatorPropertiesFront,
   IFXMeshGroup** ppMeshGroupFront,
   SIFXTessellatorProperties* pTessellatorPropertiesBack,
   IFXMeshGroup** ppMeshGroupBack
  )=0;
};

#endif
