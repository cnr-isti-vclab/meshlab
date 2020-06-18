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
	@file	CIFXAuthorCLODResource.h
*/

#ifndef CIFXAUTHORCLODRESOURCE_H
#define CIFXAUTHORCLODRESOURCE_H

#include "IFXAuthorCLODResource.h"
#include "CIFXModifier.h"
#include "IFXBoundSphereDataElement.h"
#include "IFXMeshCompiler.h"
#include "IFXCLODManagerInterface.h"
#include "IFXNeighborResControllerIntfc.h"
#include "IFXAutoRelease.h"

class CIFXAuthorCLODResource : public CIFXModifier,
                       virtual public  IFXAuthorCLODResource
{
public:
	// IFXUnknown
	U32 IFXAPI        AddRef ();
	U32 IFXAPI        Release ();
	IFXRESULT IFXAPI  QueryInterface (IFXREFIID riid, void **ppv);

	// IFXMarker
	IFXRESULT   IFXAPI 	SetSceneGraph( IFXSceneGraph* pInSceneGraph );

	// IFXMarkerX
	void IFXAPI  GetEncoderX (IFXEncoderX*& rpEncoderX) ;

	// IFXModifier
	IFXRESULT IFXAPI  GetOutputs ( 
							IFXGUID**& rpOutOutputs,
							U32&       rOutNumberOfOutputs,
							U32*&	   rpOutOutputDepAttrs );

	IFXRESULT IFXAPI  GetDependencies (    
							IFXGUID*   pInOutputDID,
							IFXGUID**& rppOutInputDependencies,
							U32&       rOutNumberInputDependencies,
							IFXGUID**& rppOutOutputDependencies,
							U32&       rOutNumberOfOutputDependencies,
							U32*&	   rpOutOutputDepAttrs );

	IFXRESULT IFXAPI  GenerateOutput ( 
							U32    inOutputDataElementIndex,
							void*& rpOutData, BOOL& rNeedRelease );

	IFXRESULT IFXAPI  SetDataPacket ( 
							IFXModifierDataPacket* pInInputDataPacket,
							IFXModifierDataPacket* pInDataPacket );

	IFXRESULT IFXAPI  Notify ( IFXModifierMessage eInMessage, void* pMessageContext );

	IFXCLODManager* IFXAPI GetCLODController();
	IFXNeighborResControllerInterface* IFXAPI GetNeighborResController();

	F32      IFXAPI		GetCLODLevel() 
	{
		return m_fCLODLevel;
	}

	IFXRESULT IFXAPI 	SetCLODLevel( F32 inCLODRatio );

	virtual IFXRESULT IFXAPI  GetCLODAuto(BOOL* pbOutCLODAuto)
	{ 
		*pbOutCLODAuto = m_bCLODAuto; 
		return IFX_OK;
	}

	virtual IFXRESULT IFXAPI  SetCLODAuto(BOOL bInCLODAuto) 
	{ 
		m_bCLODAuto = bInCLODAuto; 
		return IFX_OK;
	}

	virtual IFXRESULT IFXAPI  GetCLODBias(F32* pbOutCLODBias)
	{ 
		*pbOutCLODBias = m_fCLODBias; 
		return IFX_OK;
	}

	virtual IFXRESULT IFXAPI  SetCLODBias(F32 bInCLODBias) 
	{ 
		m_fCLODBias = bInCLODBias; return IFX_OK;
	}

	const IFXVector4& IFXAPI GetBoundingSphere() 
	{ 
		return m_pBoundSphereDataElement->Bound(); 
	}

	IFXRESULT IFXAPI 	SetBoundingSphere(const IFXVector4& vInBoundingSphere)
	{ 
		m_pBoundSphereDataElement->Bound() = vInBoundingSphere; 
		return IFX_OK; 
	}

	const IFXMatrix4x4& IFXAPI GetTransform() 
	{ 
		return m_transform[0]; 
	}

	IFXRESULT IFXAPI 	SetTransform(const IFXMatrix4x4& tInTransform);
	IFXRESULT IFXAPI 	InvalidateTransform();

	IFXRESULT IFXAPI 	GetMeshGroup(IFXMeshGroup**);
	IFXRESULT IFXAPI 	GetUpdatesGroup(IFXUpdatesGroup**);
	IFXRESULT IFXAPI 	GetNeighborMesh(IFXNeighborMesh**);

	virtual IFXRESULT IFXAPI  BuildDataBlockQueue();
	virtual void IFXAPI  GetDataBlockQueueX(IFXDataBlockQueueX*& rpDataBlockQueueX);

	// IFXAuthorCLODResource
	IFXRESULT IFXAPI 	GetAuthorMesh(IFXAuthorCLODMesh*& rpAuthorCLODMesh) ;
	IFXRESULT IFXAPI 	SetAuthorMesh(IFXAuthorCLODMesh* pAuthorCLODMesh) ;
	IFXRESULT IFXAPI 	SetAuthorMeshFinal(IFXAuthorCLODMesh* pAuthorCLODMesh) ;

	IFXRESULT IFXAPI 	GetCreaseAngle(F32& rCreaseAngle) ;
	IFXRESULT IFXAPI 	SetCreaseAngle(F32 creaseAngle) ;

	void IFXAPI 	GetNormalCreaseParameter(F32& rNormalCreaseParameter);
	void IFXAPI 	SetNormalCreaseParameter(F32 normalCreaseParameter);
	void IFXAPI 	GetNormalUpdateParameter(F32& rNormalUpdateParameter);
	void IFXAPI 	SetNormalUpdateParameter(F32 normalUpdateParameter);
	void IFXAPI 	GetNormalTolerance(F32& rNormalTolerance);
	void IFXAPI 	SetNormalTolerance(F32 normalTolerance);

	// Bones support
	IFXSkeleton* IFXAPI GetBones( void ) 
	{ 
		return m_pBones; 
	}

	IFXRESULT IFXAPI 	SetBones( IFXSkeleton* pBonesGen ) 
	{ 
		if( pBonesGen )
		{
			m_pBones = pBonesGen; 
			m_pBones->AddRef();
		}

		return IFX_OK;
	}

	IFXRESULT IFXAPI 	Transfer() ;

	IFXRESULT IFXAPI 	GetAuthorMeshMap(IFXMeshMap** ppAuthorMeshMap);
	IFXRESULT IFXAPI 	SetAuthorMeshMap(IFXMeshMap* pAuthorMeshMap);

	IFXRESULT IFXAPI 	GetRenderMeshMap(IFXMeshMap** ppRenderMeshMap);
	IFXRESULT IFXAPI 	SetRenderMeshMap(IFXMeshMap* pRenderMeshMap);

	void IFXAPI 	GetExcludeNormals(BOOL &bExcludeNormals);
	void IFXAPI 	SetExcludeNormals(BOOL bExcludeNormals);

private:
	IFXRESULT BuildMeshGroup();
	IFXRESULT BuildNeighborMesh();
	IFXRESULT BuildCLODController();
	IFXRESULT BuildNeighborResController();
	void ClearMeshGroup();
	void ClearCLODManager();
	void ClearNeighborResController();

private:
	CIFXAuthorCLODResource();
	virtual ~CIFXAuthorCLODResource();
	friend IFXRESULT IFXAPI_CALLTYPE CIFXAuthorCLODResource_Factory( 
										IFXREFIID interfaceId, void** ppInterface );

	// IFXUnknown
	U32 m_refCount;

	// IFXAuthorCLODResource
	U32 m_transformDataElementIndex;
	U32 m_meshGroupDataElementIndex;
	U32 m_CLODControllerDataElementIndex;
	U32 m_neighborResControllerDataElementIndex;
	U32 m_neighborMeshDataElementIndex;
	U32 m_boundSphereDataElementIndex;
	U32 m_bonesManagerDataElementIndex;

	IFXAuthorCLODMesh* m_pAuthorMesh;
	F32 m_fCreaseAngle;
	F32 m_normalCreaseParameter;
	F32 m_normalUpdateParameter;
	F32 m_normalTolerance;

	IFXMeshMap*	m_pAuthorMeshMap;
	IFXMeshMap*	m_pRenderMeshMap;

	IFXMeshCompiler* m_pMeshCompiler;
	IFXMeshSize*     m_pMeshSizes;
	U32				 m_NumMeshSizes;

	BOOL m_bMeshGroupDirty;
	F32  m_fCLODLevel;
	BOOL m_bCLODAuto;
	F32  m_fCLODBias;

	BOOL m_bExcludeNormals;

	IFXMeshGroup*    m_pMeshGroup;
	IFXNeighborMesh* m_pNeighborMesh;

	IFXArray<IFXMatrix4x4> m_transform;
	IFXBoundSphereDataElement* m_pBoundSphereDataElement;


	IFXUpdatesGroup* m_pUpdatesGroup;
	IFXCLODManager*  m_pCLODController;

	BOOL m_bBuiltNeighborMesh;
	IFXNeighborResControllerInterface* m_pNeighborResController;

	IFXDECLAREMEMBER(IFXDataBlockQueueX, m_pDataBlockQueueX);

	// Bones support
	IFXSkeleton* m_pBones;
};

#endif
