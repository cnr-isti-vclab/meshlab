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
	@file	CIFXGlyph2DModifier.h

			Header file for the 2D glyph modifier class.
*/

#ifndef IFXGLYPH2DMODIFIER_H
#define IFXGLYPH2DMODIFIER_H

#include "IFXGlyph2DModifier.h"
#include "IFXGlyph3DGenerator.h"
#include "IFXMarkerX.h"
#include "CIFXModifier.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXGenerator.h"

class CIFXGlyph2DModifier : private CIFXModifier,
					virtual public  IFXGlyph2DModifier,
					virtual public  IFXGenerator
{
	CIFXGlyph2DModifier();
	virtual  ~CIFXGlyph2DModifier();
	friend
		IFXRESULT IFXAPI_CALLTYPE CIFXGlyph2DModifier_Factory(IFXREFIID iid, void** ppv);

public:
	// IFXUnknown
	U32 IFXAPI            AddRef ();
	U32 IFXAPI            Release ();
	IFXRESULT IFXAPI      QueryInterface (IFXREFIID riid, void **ppv);

	// IFXMarker
	IFXRESULT  IFXAPI    SetSceneGraph( IFXSceneGraph* pInSceneGraph );


	// IFXModifier
	IFXRESULT IFXAPI  GetOutputs ( IFXGUID**& rpOutOutputs,
		U32&       rOutNumberOfOutputs,
		U32*&    rpOutOutputDepAttrs );
	IFXRESULT IFXAPI  GetDependencies (    IFXGUID*   pInOutputDID,
		IFXGUID**& rppOutInputDependencies,
		U32&       rOutNumberInputDependencies,
		IFXGUID**& rppOutOutputDependencies,
		U32&       rOutNumberOfOutputDependencies,
		U32*&    rpOutOutputDepAttrs );

	IFXRESULT IFXAPI  GenerateOutput ( U32    inOutputDataElementIndex,
		void*& rpOutData, BOOL& rNeedRelease );

	IFXRESULT IFXAPI  SetDataPacket ( IFXModifierDataPacket* pInInputDataPacket,
		IFXModifierDataPacket* pInDataPacket );
	IFXRESULT IFXAPI  Notify ( IFXModifierMessage eInMessage,
		void*               pMessageContext );

	// IFXMarkerX
	void IFXAPI     GetEncoderX (IFXEncoderX*& rpEncoderX);

	// IFXGlyph2DModifier interface...
	IFXRESULT  IFXAPI 	 Initialize( F64 width, F64 spacing, F64 height );
	IFXRESULT  IFXAPI 	 SetAttributes(U32 glyphAttributes);
	IFXRESULT  IFXAPI 	 GetAttributes(U32* pGlyphAttributes);
	// exposure of contour generator fuctions
	IFXRESULT  IFXAPI 	 StartPath();
	IFXRESULT  IFXAPI 	 LineTo(F64 fX, F64 fY);
	IFXRESULT  IFXAPI 	 MoveTo(F64 fX, F64 fY);
	IFXRESULT  IFXAPI 	 CurveTo(F64 fCx1, F64 fCy1, F64 fCx2, F64 fCy2, F64 fAx, F64 fAy);
	IFXRESULT  IFXAPI 	 EndPath();
	IFXRESULT  IFXAPI 	 GetBoundingBox(F64 pMin[3], F64 pMax[3]);
	IFXRESULT  IFXAPI 	 GetCharIndex(U32 uPickedMeshIndex, I32 *pCharIndex);

	// additional glyph string generator functions
	IFXRESULT  IFXAPI 	 StartGlyphString();
	IFXRESULT  IFXAPI 	 StartGlyph();
	IFXRESULT  IFXAPI 	 EndGlyph(F64 fWidth, F64 fHeight);
	IFXRESULT  IFXAPI 	 EndGlyphString();

	// get/set glyph command string
	IFXRESULT  IFXAPI 	 GetGlyphCommandList(IFXSimpleList** ppGlyphList);
	IFXRESULT  IFXAPI 	 SetGlyphCommandList(IFXSimpleList* pGlyphList);

	const IFXVector4& IFXAPI GetBoundingSphere() { return m_pBoundSphereDataElement->Bound(); }
	IFXRESULT  IFXAPI 	 SetBoundingSphere(const IFXVector4& vInBoundingSphere);

	const IFXMatrix4x4& IFXAPI GetTransform() { return m_drawOffset; }
	IFXRESULT  IFXAPI 	 SetTransform(const IFXMatrix4x4& offset);
	IFXRESULT  IFXAPI 	 InvalidateTransform();

	IFXMatrix4x4 CalcBillboardOffset();
	IFXMatrix4x4* IFXAPI GetViewTransform( void ) { return &m_viewTransform; }
	IFXRESULT  IFXAPI 	 SetViewTransform( IFXMatrix4x4 transform );

private:
	// IFXUnknown
	U32 m_uRefCount;

	// IFXModifier
	static const IFXGUID* m_scpOutputDIDs[];
	static const IFXGUID* m_scpNeighborMeshDeps[];
	static const U32 m_scpMeshGroupUnchangedAttrs[];

	U32 m_uMeshGroupDataElementIndex;
	U32 m_uNeighborMeshDataElementIndex;
	U32 m_uBoundSphereDataElementIndex;

	// IFXGlyph2DModifier attributes...

	IFXGlyph3DGenerator*  m_pGlyphGenerator;
	BOOL          m_bBuildingContours;

	SIFXGlyphMeshParams m_sParameters;
	U32 m_attributes;

	IFXBoundSphereDataElement* m_pBoundSphereDataElement;

	IFXRESULT InvalidateMeshGroup();
	IFXMatrix4x4 m_viewTransform;
	IFXMatrix4x4 m_drawOffset;

	U32 m_glyphMeshStart;
	U32 m_meshGroupChangeCount;
	F32 m_modelBoundingDistance;
	U32 m_rebuild;
};

#endif
