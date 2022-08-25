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
	@file	CIFXAuthorPointSet.h
*/

#ifndef CIFXAuthorPointSet_H
#define CIFXAuthorPointSet_H

#include "IFXAuthorPointSet.h"
#include "IFXAuthorPointSetAccess.h"
#include "IFXAutoPtr.h"

class CIFXAuthorPointSet :	virtual public IFXAuthorPointSet, 
							virtual public IFXAuthorPointSetAccess
{
public:

	// IFXUnknown methods
	U32 IFXAPI  AddRef ();
	U32 IFXAPI  Release ();
	IFXRESULT IFXAPI  QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// IFXAuthorPointSet methods
	IFXRESULT  IFXAPI 	 Allocate(const IFXAuthorPointSetDesc*);
	IFXRESULT  IFXAPI 	 Reallocate(const IFXAuthorPointSetDesc*);
	IFXRESULT  IFXAPI 	 Deallocate();

	IFXRESULT  IFXAPI 	 Copy(IFXREFIID, void**);

	IFXAuthorPointSetDesc* IFXAPI GetPointSetDesc();
	IFXRESULT  IFXAPI 	 SetPointSetDesc(const IFXAuthorPointSetDesc*);
	IFXAuthorPointSetDesc* IFXAPI GetMaxPointSetDesc();
	IFXRESULT  IFXAPI 	 GetNumAllocatedTexPointLayers(U32* pLayers) ;
	
	IFXRESULT  IFXAPI 	 GetPositionPoints(U32** ppPositionPoints);
	IFXRESULT  IFXAPI 	 GetNormalPoints(U32** ppNormalPoints);
	IFXRESULT  IFXAPI 	 GetDiffusePoints(U32** ppDiffusePoints);
	IFXRESULT  IFXAPI 	 GetSpecularPoints(U32** ppSpecularPoints);
	IFXRESULT  IFXAPI 	 GetTexCoordsPoints (U32** ppTexCoordsPoints);
	IFXRESULT  IFXAPI 	 GetMaterialsPoints (U32** ppMaterialPoints); 

	IFXRESULT  IFXAPI 	 GetPositionPoint(U32,	U32* pPositionPoint);
	IFXRESULT  IFXAPI 	 SetPositionPoint(U32,	const U32* pPositionPoint);

	IFXRESULT  IFXAPI 	 GetNormalPoint(U32,	U32* );
	IFXRESULT  IFXAPI 	 SetNormalPoint(U32,	const U32* );

	IFXRESULT  IFXAPI 	 GetDiffusePoint(U32,	U32* );
	IFXRESULT  IFXAPI 	 SetDiffusePoint(U32,	const U32* );

	IFXRESULT  IFXAPI 	 GetSpecularPoint(U32,	U32* )	;
	IFXRESULT  IFXAPI 	 SetSpecularPoint(U32,	const U32* );

	IFXRESULT  IFXAPI 	 GetTexPoints(U32, U32**); 
	IFXRESULT  IFXAPI 	 GetTexPoint(U32, U32, U32* );
	IFXRESULT  IFXAPI 	 SetTexPoint(U32, U32, const U32*);

	IFXRESULT  IFXAPI 	 GetPointMaterials(U32**);        

	IFXRESULT  IFXAPI 	 GetPositions (IFXVector3** );
	IFXRESULT  IFXAPI 	 GetNormals(IFXVector3** );
	IFXRESULT  IFXAPI 	 GetDiffuseColors(IFXVector4** );
	IFXRESULT  IFXAPI 	 GetSpecularColors(IFXVector4** )	;
	IFXRESULT  IFXAPI 	 GetTexCoords (IFXVector4** );
	IFXRESULT  IFXAPI 	 GetMaterials (IFXAuthorMaterial** ); 
	
	IFXRESULT  IFXAPI 	 GetPosition(U32 idx,IFXVector3* pVector3);		
	IFXRESULT  IFXAPI 	 SetPosition(U32 idx,const IFXVector3* pVector3);
	
	IFXRESULT  IFXAPI 	 GetNormal(U32 idx, IFXVector3* pVector3);
	IFXRESULT  IFXAPI 	 SetNormal(U32 idx, const IFXVector3* pVector3);

	IFXRESULT  IFXAPI 	 GetDiffuseColor(U32 idx, IFXVector4* pColor);
	IFXRESULT  IFXAPI 	 SetDiffuseColor(U32 idx, IFXVector4* pColor);

	IFXRESULT  IFXAPI 	 GetSpecularColor(U32 idx, IFXVector4* pColor);
	IFXRESULT  IFXAPI 	 SetSpecularColor(U32 idx, IFXVector4* pColor);
	
	IFXRESULT  IFXAPI 	 GetTexCoord(U32 idx, IFXVector4* pVector4);
	IFXRESULT  IFXAPI 	 SetTexCoord(U32 idx, const IFXVector4* pVector4);

	IFXRESULT  IFXAPI 	 GetMaterial(U32 idx, IFXAuthorMaterial*);
	IFXRESULT  IFXAPI 	 SetMaterial(U32 idx, const IFXAuthorMaterial*);
	

	IFXRESULT  IFXAPI 	 GetPointMaterial(U32 idx, U32* pPointMaterial);
	IFXRESULT  IFXAPI 	 SetPointMaterial(U32 idx, U32 pointMaterialID);

	IFXVector4 IFXAPI	CalcBoundSphere();

	//
	//IFXAuthorPointSetAccess
	//
	IFXRESULT  IFXAPI 	 SetPositionArray(IFXVector3*	pVector3, U32 idx);
	IFXRESULT  IFXAPI 	 SetNormalArray(IFXVector3* pVector3, U32 idx);
	IFXRESULT  IFXAPI 	 SetDiffuseColorArray(IFXVector4* pColor, U32 idx);
	IFXRESULT  IFXAPI 	 SetSpecularColorArray(IFXVector4* pColor, U32 idx);
 	IFXRESULT  IFXAPI 	 SetTexCoordArray(IFXVector4* pVector3, U32 idx);

private:
	CIFXAuthorPointSet();
	virtual ~CIFXAuthorPointSet();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXAuthorPointSet_Factory( 
										IFXREFIID interfaceId,  
										void**    ppInterface );
	
	// Reference count for IFXUnknown
	U32	m_uRefCount;		
	
	IFXAuthorPointSetDesc    m_CurPointSetDesc;		
	IFXAuthorPointSetDesc    m_MaxPointSetDesc;		
	
	IFXAutoPtr< U32 >	m_pPositionPoints;  ///< The indices of Positions
	IFXAutoPtr< U32 >	m_pNormalPoints;	///< The indices of Normals
	IFXAutoPtr< U32 >	m_pDiffusePoints;   ///< The indices of Diffuse colors
	IFXAutoPtr< U32 >	m_pSpecularPoints;  ///< The indices of Spec. colors
	IFXAutoPtr< U32 >	m_pTexCoordPoints[IFX_MAX_TEXUNITS]; ///< Indeces of Tex. coords
	IFXAutoPtr< U32 >	m_pMaterialsPoints; ///< Id of materials

	IFXAutoPtr< IFXVector3 >		m_pPositions;	
	IFXAutoPtr< IFXVector3 >		m_pNormals;
	IFXAutoPtr< IFXVector4 >		m_pDiffuseColors;
	IFXAutoPtr< IFXVector4 >		m_pSpecularColors;
	IFXAutoPtr< IFXVector4 >		m_pTexCoords;
	IFXAutoPtr< IFXAuthorMaterial >	m_pMaterials;
};

#endif
