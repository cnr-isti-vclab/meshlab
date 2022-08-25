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
	@file	CIFXAuthorLineSet.h

			Declaration of the CIFXAuthorLineSet
*/

#ifndef __CIFXAUTHORLINESET_H__
#define __CIFXAUTHORLINESET_H__

#include "IFXAuthorLineSet.h"
#include "IFXAuthorLineSetAccess.h"
#include "IFXAutoPtr.h"
#include "IFXEnums.h"

class CIFXAuthorLineSet : virtual public IFXAuthorLineSet, 
						  virtual public IFXAuthorLineSetAccess
{
public:
		// IFXUnknown methods
		U32 IFXAPI  AddRef ();
		U32 IFXAPI  Release ();
		IFXRESULT IFXAPI  QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

		//
		// IFXAuthorLineSet methods
		IFXRESULT  IFXAPI 	 Allocate(const IFXAuthorLineSetDesc*);
		IFXRESULT  IFXAPI 	 Reallocate(const IFXAuthorLineSetDesc*);
		IFXRESULT  IFXAPI 	 Deallocate();

		IFXRESULT  IFXAPI 	 Copy(IFXREFIID, void**);

		IFXAuthorLineSetDesc* IFXAPI GetLineSetDesc();
		IFXRESULT  IFXAPI 	 SetLineSetDesc(const IFXAuthorLineSetDesc*);
		IFXAuthorLineSetDesc* IFXAPI GetMaxLineSetDesc();
		IFXRESULT  IFXAPI 	 GetNumAllocatedTexLineLayers(U32* pLayers) ;
		
		IFXRESULT  IFXAPI 	 GetPositionLines(IFXU32Line** );
		IFXRESULT  IFXAPI 	 GetNormalLines(IFXU32Line** );
		IFXRESULT  IFXAPI 	 GetDiffuseLines(IFXU32Line** );
		IFXRESULT  IFXAPI 	 GetSpecularLines(IFXU32Line** );
		IFXRESULT  IFXAPI 	 GetTexCoordsLines (IFXU32Line** );
		IFXRESULT  IFXAPI 	 GetMaterialsLines (U32** ); 

		IFXRESULT  IFXAPI 	 GetPositionLine(U32 ,	IFXU32Line* );
		IFXRESULT  IFXAPI 	 SetPositionLine(U32 ,	const IFXU32Line* );

		IFXRESULT  IFXAPI 	 GetNormalLine(U32 ,	IFXU32Line* );
		IFXRESULT  IFXAPI 	 SetNormalLine(U32 ,	const IFXU32Line* );

		IFXRESULT  IFXAPI 	 GetDiffuseLine(U32 ,	IFXU32Line* );
		IFXRESULT  IFXAPI 	 SetDiffuseLine(U32 ,	const IFXU32Line* );

		IFXRESULT  IFXAPI 	 GetSpecularLine(U32 ,	IFXU32Line* )	;
		IFXRESULT  IFXAPI 	 SetSpecularLine(U32 ,	const IFXU32Line* );

		IFXRESULT  IFXAPI 	 GetTexLines(U32, IFXU32Line**); 
		IFXRESULT  IFXAPI 	 GetTexLine(U32 layer, U32 index, IFXU32Line* pTextureLine);
		IFXRESULT  IFXAPI 	 SetTexLine(U32 layer, U32 index, const IFXU32Line* pLine);

		IFXRESULT  IFXAPI 	 GetLineMaterials(U32**);        


		IFXRESULT  IFXAPI 	 GetPositions (IFXVector3** );
		IFXRESULT  IFXAPI 	 GetNormals(IFXVector3** );
		IFXRESULT  IFXAPI 	 GetDiffuseColors(IFXVector4** );
		IFXRESULT  IFXAPI 	 GetSpecularColors(IFXVector4** )	;
		IFXRESULT  IFXAPI 	 GetTexCoords (IFXVector4** );
		IFXRESULT  IFXAPI 	 GetMaterials (IFXAuthorMaterial** ); 
		
		IFXRESULT  IFXAPI 	 GetPosition(U32 index,IFXVector3* pVector3);		
		IFXRESULT  IFXAPI 	 SetPosition(U32 index,const IFXVector3* pVector3);
		
		IFXRESULT  IFXAPI 	 GetNormal(U32 index, IFXVector3* pVector3);
		IFXRESULT  IFXAPI 	 SetNormal(U32 index, const IFXVector3* pVector3);

		IFXRESULT  IFXAPI 	 GetDiffuseColor(U32 index, IFXVector4* pColor);
		IFXRESULT  IFXAPI 	 SetDiffuseColor(U32 index,IFXVector4* pColor);

		IFXRESULT  IFXAPI 	 GetSpecularColor(U32 index, IFXVector4* pColor);
		IFXRESULT  IFXAPI 	 SetSpecularColor(U32 index, IFXVector4* pColor);
		
		IFXRESULT  IFXAPI 	 GetTexCoord(U32 index, IFXVector4* pVector4);
		IFXRESULT  IFXAPI 	 SetTexCoord(U32 index, const IFXVector4* pVector4);

		IFXRESULT  IFXAPI 	 GetMaterial(U32 index, IFXAuthorMaterial*);
		IFXRESULT  IFXAPI 	 SetMaterial(U32 index, const IFXAuthorMaterial*);
		

		IFXRESULT  IFXAPI 	 GetLineMaterial(U32 index, U32* pLineMaterial);
		IFXRESULT  IFXAPI 	 SetLineMaterial(U32 index, U32 lineMaterialID);

		IFXVector4 IFXAPI CalcBoundSphere();

		//
		//IFXAuthorLinesetAccess
		//
		IFXRESULT  IFXAPI 	 SetPositionArray(IFXVector3*	pVector3, U32 index);	
 
		IFXRESULT  IFXAPI 	 SetNormalArray(IFXVector3* pVector3, U32 index);
		
		IFXRESULT  IFXAPI 	 SetDiffuseColorArray(IFXVector4* pColor, U32 index);
		
		IFXRESULT  IFXAPI 	 SetSpecularColorArray(IFXVector4* pColor, U32 index);
		
 		IFXRESULT  IFXAPI 	 SetTexCoordArray(IFXVector4* pVector3, U32 index);

private:
	CIFXAuthorLineSet();
	virtual ~CIFXAuthorLineSet();

	friend IFXRESULT IFXAPI_CALLTYPE CIFXAuthorLineSet_Factory( 
												IFXREFIID interfaceId,  
												void**   ppInterface );
	
	// Reference count for IFXUnknown
	U32	m_uRefCount;		
	
	IFXAuthorLineSetDesc    m_CurLineSetDesc;		
	IFXAuthorLineSetDesc    m_MaxLineSetDesc;		
	
	IFXAutoPtr< IFXU32Line > m_pPositionLines;  ///< The indices of Positions
	IFXAutoPtr< IFXU32Line > m_pNormalLines;	///< The indices of Normals
	IFXAutoPtr< IFXU32Line > m_pDiffuseLines;   ///< The indices of Diffuse colors
	IFXAutoPtr< IFXU32Line > m_pSpecularLines;  ///< The indices of Spec. colors
	IFXAutoPtr< IFXU32Line > m_pTexCoordLines[IFX_MAX_TEXUNITS]; ///< Indeces of Tex. coords
	IFXAutoPtr< U32 >		 m_pMaterialsLines; ///< Id of materials

	IFXAutoPtr< IFXVector3 >		m_pPositions;	
	IFXAutoPtr< IFXVector3 >		m_pNormals;
	IFXAutoPtr< IFXVector4 >		m_pDiffuseColors;
	IFXAutoPtr< IFXVector4 >		m_pSpecularColors;
	IFXAutoPtr< IFXVector4 >		m_pTexCoords;
	IFXAutoPtr< IFXAuthorMaterial >	m_pMaterials;
};

#endif
