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
	@file	IFXContourExtruder.h
			
			Header file for the Contour extruder classes.
	@note
			This class controls the appearance of extruded 3D models. 
			It is an internal API.
			See IFXGlyph3DGenerator for documentation.
*/

#ifndef __IFXCONTOUREXTRUDER_INTERFACES_H__
#define __IFXCONTOUREXTRUDER_INTERFACES_H__


#include "IFXUnknown.h"
#include "IFXSimpleList.h"
#include "IFXContour.h"
#include "IFXMesh.h"
#include "IFXMeshGroup.h"

// external declarations
// {139C63D7-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXContourExtruder,
0x139c63d7, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

enum EIFXGlyphFacing
{
  IFXGlyphInFacing,
  IFXGlyphOutFacing,
  IFXGlyphBothFacing
};

enum EIFXGlyphNormalOrientation 
{
  IFXGlyphRegularNormals,
  IFXGlyphInverseNormals
};

enum EIFXGlyphBevelType 
{
  IFXGlyphBevelMiter=0,
  IFXGlyphBevelRound=1
};

enum EIFXGlyphBevelFace 
{
  IFXGlyphBevelFaceFront=1,
  IFXGlyphBevelFaceBack=2,
  IFXGlyphBevelFaceBoth=3
};

struct SIFXExtruderProperties 
{
  F32  fExtrusionDepth;
  U32  uDepthSteps;
  BOOL bNormals;

  BOOL bBevel;
  F32  fBevelDepth;
  EIFXGlyphBevelType eBevelType;
  EIFXGlyphBevelFace eBevelFace;

  EIFXGlyphFacing eFacing;
  EIFXGlyphNormalOrientation eNormalOrientation;
  BOOL bWindClockWise;

#ifdef CONTOUREXTRUDER_BUILD_OPENGL_TESTCODE
  GLuint  uDisplayList;
#endif
};

class IFXContourExtruder:public IFXUnknown 
{
public:
  virtual IFXRESULT IFXAPI  Extrude(SIFXExtruderProperties* pExtruderProperties, IFXSimpleList* pGlyphList, IFXMeshGroup** ppMeshGroup)=0;
  virtual IFXRESULT IFXAPI  Extrude(SIFXExtruderProperties* pExtruderProperties, IFXContour* pGlyph, IFXMesh** ppMesh)=0;
};

#endif
