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
	@file IFXGlyph3DGenerator.h

		Header file for the 3D glyph generator classes.

	@note
		This class provides an interface to construct a 3D extruded model from a set of 2D contours.
		It can be used to generate a string of 3D Text. The contour drawing interface matches up 
		with the true type font spec.  Structures are used to control the appearance of the 
		generated models. It is an internal API.  See IFXModifier3DGlyphGen.h for documentation.
*/

#ifndef IFXGLYPH3DGENERATOR_H
#define IFXGLYPH3DGENERATOR_H


#include "IFXUnknown.h"
#include "IFXContourExtruder.h"
#include "IFXContourTessellator.h"
#include "IFXGlyph2DCommands.h"

class IFXNeighborMesh;

// {139C63DC-109C-11d4-A120-00A0C9A0F93B}
IFXDEFINE_GUID(IID_IFXGlyph3DGenerator,
0x139c63dc, 0x109c, 0x11d4, 0xa1, 0x20, 0x0, 0xa0, 0xc9, 0xa0, 0xf9, 0x3b);

struct SIFXGlyphGeneratorParams 
{
	// 3D Glyph Generator
	F64		fDefaultGlyphWidth;
	F64		fDefaultGlyphHeight;
	F64		fDefaultGlyphSpacing;
};

struct SIFXGlyphCombineProperties 
{
	BOOL bFrontCap;
	EIFXGlyphFacing	eFrontCapFacing;
	BOOL bBackCap;
	EIFXGlyphFacing	eBackCapFacing;
	U32 uNumberOfCurveSteps;
};

struct SIFXGlyphMeshParams 
{
	//tesselator flags
	BOOL	bTessellate;
	SIFXTessellatorProperties sTessellator;

	//extruder flags
	BOOL	bExtrude;
	SIFXExtruderProperties sExtruder;

	//combine extruder and tesselator mesh flags
	BOOL	bCombine;
	SIFXGlyphCombineProperties sCombiner;

	IFXMeshGroup*	 pMeshGroup;
	IFXNeighborMesh* pNeighborMesh;

#ifdef GLYPHGENERATOR_BUILD_OPENGL_TESTCODE
	GLuint		uWireFrameDisplayList;
	GLuint		uExtrusionDisplayList[100];
	GLuint		uTessellatorDisplayLists[100];
	U32			uNumExtrusionLists;
	U32			uNumTessellatorLists;
#endif
};


class IFXGlyph3DGenerator:public IFXUnknown 
{
public:
	virtual IFXRESULT IFXAPI  Initialize(SIFXGlyphGeneratorParams* pParams)=0;	
	virtual IFXRESULT IFXAPI  BuildMesh(SIFXGlyphMeshParams* pInitInfo)=0;

	// exposure of contour generator fuctions
	virtual IFXRESULT IFXAPI  StartPath()=0;
	virtual IFXRESULT IFXAPI  LineTo(F64 fX, F64 fY)=0;
	virtual IFXRESULT IFXAPI  MoveTo(F64 fX, F64 fY)=0;
	virtual IFXRESULT IFXAPI  CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy, U32 uNumberOfCurveSteps)=0;
	virtual IFXRESULT IFXAPI  EndPath()=0;
	virtual IFXRESULT IFXAPI  GetBoundingBox(F64 pMin[3], F64 pMax[3])=0;
	virtual IFXRESULT IFXAPI  GetCharIndex(U32 uPickedMeshIndex, I32 *pCharIndex)=0;

	// additional glyph string generator functions
	virtual IFXRESULT IFXAPI  StartGlyphString()=0;
	virtual IFXRESULT IFXAPI  StartGlyph()=0;
	virtual IFXRESULT IFXAPI  EndGlyph(F64 fWidth, F64 fHeight)=0;
	virtual IFXRESULT IFXAPI  EndGlyphString()=0;

	virtual IFXRESULT IFXAPI  GetGlyphCommandList(IFXSimpleList** ppGlyphList)=0;
	virtual IFXRESULT IFXAPI  SetGlyphCommandList(IFXSimpleList* pGlyphList)=0;
	virtual IFXRESULT IFXAPI  ReplayGlyphCommandList(U32 uCurveSteps)=0;

	// mesh concatenator. Can be used independently from Glyph
	virtual IFXRESULT IFXAPI CollapseGlyph( IFXMesh** ppMeshList, IFXMesh** ppOutMesh, U32 uMeshCount )=0;
};

#endif
