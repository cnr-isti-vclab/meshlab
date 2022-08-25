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
//
//	CIFXGlyph3DGenerator.h
//
//	DESCRIPTION
//		Header file for the 3D glyph generator classes.
//
//	NOTES
//      None.
//
//***************************************************************************
#ifndef __CIFXGlyph3DGenerator_CLASS_H__
#define __CIFXGlyph3DGenerator_CLASS_H__

#include "IFXGlyph3DGenerator.h"
#include "IFXSimpleList.h"
#include "IFXContourGenerator.h"

const U32 GLYPH_GENERATOR_MAX_CHARS = 64;

class CIFXGlyph3DGenerator : public IFXGlyph3DGenerator
{

public:
	// IFXUnknown methods
	U32 IFXAPI  AddRef (void);
	U32 IFXAPI  Release (void);
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// Factory function.
	friend IFXRESULT IFXAPI_CALLTYPE CIFXGlyph3DGenerator_Factory( IFXREFIID interfaceId, void** ppInterface );

	// IFXTextGenerator methods
	IFXRESULT  IFXAPI 	 Initialize(SIFXGlyphGeneratorParams* pParams);
	IFXRESULT  IFXAPI 	 BuildMesh(SIFXGlyphMeshParams* pInitInfo);

	// IFXContourGenerator methods
	IFXRESULT  IFXAPI 	 StartPath();
	IFXRESULT  IFXAPI 	 LineTo(F64 fX, F64 fY);
	IFXRESULT  IFXAPI 	 MoveTo(F64 fX, F64 fY);
	IFXRESULT  IFXAPI 	 CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy, U32 m_uNumberOfCurveSteps);
	IFXRESULT  IFXAPI 	 EndPath();
	IFXRESULT  IFXAPI 	 GetBoundingBox(F64 pMin[2], F64 pMax[2]);
	IFXRESULT  IFXAPI 	 GetCharIndex(U32 uPickedMeshIndex, I32 *pCharIndex);


	// additional glyph string generator functions
	IFXRESULT  IFXAPI 	 StartGlyphString();
	IFXRESULT  IFXAPI 	 StartGlyph();
	IFXRESULT  IFXAPI 	 EndGlyph(F64 fWidth, F64 fHeight);
	IFXRESULT  IFXAPI 	 EndGlyphString();

	// functions needed to clone this object
	IFXRESULT  IFXAPI 	 GetGlyphCommandList(IFXSimpleList** ppGlyphList);
	IFXRESULT  IFXAPI 	 SetGlyphCommandList(IFXSimpleList* pGlyphList);
	IFXRESULT  IFXAPI 	 ReplayGlyphCommandList(U32 uCurveSteps);

	IFXRESULT IFXAPI CollapseGlyph
	(
		IFXMesh** ppMeshList,
		IFXMesh** ppOutMesh,
		U32 uMeshCount
	);

private:
	CIFXGlyph3DGenerator();
	virtual ~CIFXGlyph3DGenerator();
	IFXRESULT GetTotalMeshCount(SIFXGlyphMeshParams* pParams, U32* pCount);

	IFXRESULT CallTessellator
	(
		SIFXGlyphMeshParams* pInitInfo,
		IFXContourTessellator* pContourTessellator,
		IFXMeshGroup* pFinalMeshGroup,
		U32* pFinalMeshGroupIndex
	);

	IFXRESULT BuildGlyph
	(
		U32 uGlyphIndex,
		SIFXGlyphMeshParams* pInitInfo,
		IFXMeshGroup* pFinalMeshGroup,
		U32* pFinalMeshGroupIndex
	);

	IFXRESULT BuildBoundingBox
	(
		SIFXGlyphMeshParams* pInitInfo,
		IFXSimpleList* pGlyphList,
		IFXMeshGroup** ppMeshGroup
	);

	IFXRESULT CopyMeshGroupToFinalMesh
	(
		IFXMeshGroup* pMeshGroup,
		IFXMeshGroup* pFinalMeshGroup,
		U32 *pFinalMeshGroupIndex,
		U32 uGlyphIndex
	);

	IFXRESULT ApplyTextureCoordinates
	(
		IFXMeshGroup* pMeshGroup
	);

	IFXRESULT ApplyTextureCoordinates
	(
		IFXMesh* pMesh,
		IFXVector3 vBoxMin,
		IFXVector3 vBoxMax
	);

	IFXRESULT CalcBoundingBox
	(
		IFXMesh* pMesh,
		IFXVector3& vBoxMin,
		IFXVector3& vBoxMax
	);

	IFXRESULT CollapseFinalMeshGroup
	(
		IFXMeshGroup* pInMeshGroup,
		IFXMeshGroup** ppOutMeshGroup,
		U32 uGlyphCount
	);


	IFXSimpleList*				m_pGlyphString;
	IFXSimpleList*				m_pGlyphCommands;
//	IFXSimpleList*				m_pGlyphData;
	IFXContourGenerator*		m_pContourGenerator;
	SIFXGlyphGeneratorParams*	m_pGlyphGenParams;
	I32*						m_pMeshToGlyphIndexMap;
	U32							m_uRefCount;
	F64							m_dWidthAccumulator;
	F64							m_dHeightAccumulator;

	F64							xMin, xMax, yMin, yMax;
};

#endif // #define __CIFXGlyph3DGenerator_CLASS_H__
