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
    @file	CIFXAuthorMesh.h

			Implementation of CIFXAuthorMesh.
*/

#ifndef CIFXAuthorMesh_H
#define CIFXAuthorMesh_H

#include "IFXAuthorMesh.h"
#include "IFXAuthorCLODMesh.h"
#include "IFXAuthorCLODAccess.h"
#include "IFXEnums.h"
#include "IFXAutoPtr.h"

class CIFXAuthorMesh : public IFXAuthorCLODAccess
{
public:
	// Member functions:  IFXUnknown.
	U32 IFXAPI 			AddRef ();
	U32 IFXAPI 			Release ();
	IFXRESULT IFXAPI 	QueryInterface ( IFXREFIID interfaceId, void** ppInterface );

	// IFXAuthorMesh
	virtual IFXRESULT IFXAPI  Allocate(const IFXAuthorMeshDesc*);
	virtual IFXRESULT IFXAPI  Reallocate(const IFXAuthorMeshDesc*);
	virtual IFXRESULT IFXAPI  Deallocate();

	virtual IFXRESULT IFXAPI  Copy(IFXREFIID, void**);
	virtual IFXRESULT IFXAPI  GenerateNormals(IFXAuthorMeshNormalGen);

	virtual const IFXAuthorMeshDesc* IFXAPI GetMeshDesc();
	virtual IFXRESULT IFXAPI  SetMeshDesc(const IFXAuthorMeshDesc*);
	virtual const IFXAuthorMeshDesc* IFXAPI GetMaxMeshDesc();

	virtual IFXRESULT IFXAPI  GetNumAllocatedTexFaceLayers(U32*);

	virtual IFXRESULT IFXAPI  Lock();
	virtual IFXRESULT IFXAPI  GetMaterials(IFXAuthorMaterial**);
	virtual IFXRESULT IFXAPI  GetPositionFaces(IFXAuthorFace**);
	virtual IFXRESULT IFXAPI  GetNormalFaces(IFXAuthorFace**);
	virtual IFXRESULT IFXAPI  GetDiffuseFaces(IFXAuthorFace**);
	virtual IFXRESULT IFXAPI  GetSpecularFaces(IFXAuthorFace**);
	virtual IFXRESULT IFXAPI  GetTexFaces(U32, IFXAuthorFace**);
	virtual IFXRESULT IFXAPI  GetFaceMaterials(U32**);
	
	virtual IFXRESULT IFXAPI  GetBaseVertices(U32**);

	virtual IFXRESULT IFXAPI  GetPositions(IFXVector3**);
	virtual IFXRESULT IFXAPI  GetNormals(IFXVector3**);
	virtual IFXRESULT IFXAPI  GetDiffuseColors(IFXVector4**);
	virtual IFXRESULT IFXAPI  GetSpecularColors(IFXVector4**);
	virtual IFXRESULT IFXAPI  GetTexCoords(IFXVector4**);
	virtual IFXRESULT IFXAPI  Unlock();

	virtual IFXRESULT IFXAPI  GetMaterial(U32, IFXAuthorMaterial*);
	virtual IFXRESULT IFXAPI  SetMaterial(U32, const IFXAuthorMaterial*);		

	virtual IFXRESULT IFXAPI  GetPositionFace(U32, IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetPositionFace(U32, const IFXAuthorFace*);

	virtual IFXRESULT IFXAPI  GetNormalFace(U32, IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetNormalFace(U32, const IFXAuthorFace*);

	virtual IFXRESULT IFXAPI  GetDiffuseFace(U32, IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetDiffuseFace(U32, const IFXAuthorFace*);

	virtual IFXRESULT IFXAPI  GetSpecularFace(U32, IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetSpecularFace(U32, const IFXAuthorFace*);

	virtual IFXRESULT IFXAPI  GetTexFace(U32, U32, IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetTexFace(U32, U32, const IFXAuthorFace*);

	virtual IFXRESULT IFXAPI  GetFaceMaterial(U32,  U32*);	
	virtual IFXRESULT IFXAPI  SetFaceMaterial(U32, U32);

	virtual IFXRESULT IFXAPI  GetBaseVertex(U32,  U32*);	
	virtual IFXRESULT IFXAPI  SetBaseVertex(U32, U32);

	virtual IFXRESULT IFXAPI  GetPosition(U32, IFXVector3*);
	virtual IFXRESULT IFXAPI  SetPosition(U32, const IFXVector3*);

	virtual IFXRESULT IFXAPI  GetNormal(U32, IFXVector3*);
	virtual IFXRESULT IFXAPI  SetNormal(U32, const IFXVector3*);

	virtual IFXRESULT IFXAPI  GetDiffuseColor(U32, IFXVector4*);
	virtual IFXRESULT IFXAPI  SetDiffuseColor(U32, IFXVector4*);

	virtual IFXRESULT IFXAPI  GetSpecularColor(U32, IFXVector4*);
	virtual IFXRESULT IFXAPI  SetSpecularColor(U32, IFXVector4*);

	virtual IFXRESULT IFXAPI  GetTexCoord(U32, IFXVector4*);
	virtual IFXRESULT IFXAPI  SetTexCoord(U32, const IFXVector4*);
	
	// Trim off unwanted low resolutions (defaults to zero), 
	virtual U32		  IFXAPI  SetMinResolution(U32);
	virtual U32       IFXAPI  GetMinResolution();

	// Trim off unwanted high resolution vertices (defaults to NumPositions). 
	virtual U32		  IFXAPI  SetFinalMaxResolution(U32);
	virtual U32		  IFXAPI  GetFinalMaxResolution();

	virtual IFXVector4 IFXAPI CalcBoundSphere();

	// IFXAuthorCLODMesh
	// used by decompressor to hand over array of vertex updates
	virtual IFXRESULT IFXAPI  SetUpdates(IFXAuthorVertexUpdate*);
	virtual IFXRESULT IFXAPI  GetUpdates(IFXAuthorVertexUpdate**);

	virtual IFXRESULT IFXAPI  GetVertexUpdate(U32, const IFXAuthorVertexUpdate*&);
	virtual IFXRESULT IFXAPI  SetVertexUpdate(U32,const IFXAuthorVertexUpdate*);

	virtual U32		  IFXAPI  SetMaxResolution(U32);
	// max resolution given current state of streaming.
	virtual U32		  IFXAPI  GetMaxResolution();
	virtual U32		  IFXAPI  SetResolution(U32);   
	virtual U32		  IFXAPI  GetResolution();

	// IFXAuthorCLODAccess
	virtual IFXRESULT IFXAPI  SetPositionArray(IFXVector3*, U32);
	virtual IFXRESULT IFXAPI  SetNormalArray(IFXVector3*, U32);
	virtual IFXRESULT IFXAPI  SetDiffuseColorArray(IFXVector4*, U32);
	virtual IFXRESULT IFXAPI  SetSpecularColorArray(IFXVector4*, U32);
	virtual IFXRESULT IFXAPI  SetTexCoordArray(IFXVector4*, U32);

	virtual IFXRESULT IFXAPI  SetPositionFaces(IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetNormalFaces(IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetDiffuseFaces(IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetSpecularFaces(IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetTexFaces(U32, IFXAuthorFace*);
	virtual IFXRESULT IFXAPI  SetFaceMaterials(U32*);
	virtual IFXRESULT IFXAPI  SetMaxNumFaces(U32);

	friend IFXRESULT IFXAPI_CALLTYPE CIFXAuthorMesh_Factory( 
											IFXREFIID interfaceId,
											void**    ppInterface );
private:
	CIFXAuthorMesh();
	virtual ~CIFXAuthorMesh();

	IFXRESULT GenFlatNormals(
					IFXVector3* pOutFlatNormals, 
					IFXAuthorFace* pOutNormalFaces);
	IFXRESULT GenSmoothNormals();
	IFXRESULT GenSmoothMatNormals();
	
	U32	m_refCount;	// Number of outstanding references to the object.
	IFXAuthorMeshDesc m_CurMeshDesc;
	IFXAuthorMeshDesc m_MaxMeshDesc;
	U32 m_normalAlloc; ///< number of allocated normals

	U32 m_MeshFlags; ///< The Mesh state
	IFXAutoPtr< IFXAuthorFace > m_pPositionFaces; ///< The indices of Positions
	IFXAutoPtr< IFXAuthorFace > m_pNormalFaces;	  ///< The indices of Normals
	IFXAutoPtr< IFXAuthorFace > m_pDiffuseFaces;  ///< The indices of Diffuse color
	IFXAutoPtr< IFXAuthorFace > m_pSpecularFaces; ///< The indices of Spec. color
	IFXAutoPtr< IFXAuthorFace > m_pTexCoordFaces[IFX_MAX_TEXUNITS];
	IFXAutoPtr< U32 >			m_pFaceMaterials; ///< Id of materials.
	IFXAutoPtr< U32 >			m_pBaseVertices;

	IFXAutoPtr< IFXVector3 >	m_pPositions;	
	IFXAutoPtr< IFXVector3 >	m_pNormals;
	IFXAutoPtr< IFXVector4 >	m_pDiffuseColors;
	IFXAutoPtr< IFXVector4 >	m_pSpecularColors;
	IFXAutoPtr< IFXVector4 >	m_pTexCoords;
	IFXAutoPtr< IFXAuthorVertexUpdate >  m_pUpdates; 

	IFXAutoPtr< IFXAuthorMaterial >	m_pMaterials;

	U32 m_MinResolution;
	U32 m_FinalMaxResolution;
	U32 m_MaxResolution;
};

#endif
