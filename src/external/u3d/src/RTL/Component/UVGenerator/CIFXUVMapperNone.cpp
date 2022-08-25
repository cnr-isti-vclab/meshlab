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

#include "CIFXUVMapperNone.h"
#include "IFXSceneGraph.h"
#include "IFXPalette.h"
#include "IFXLightSet.h"
#include "IFXLight.h"

CIFXUVMapperNone::CIFXUVMapperNone(BOOL bNeedTexCoords) 
: m_bNeedTexCoords(bNeedTexCoords)
{
	m_uRefCount=0;
}

CIFXUVMapperNone::~CIFXUVMapperNone() 
{
}

//---------------------------------------------------------------------------
//	CIFXUVMapperNone::AddRef
//
//	This method increments the reference count for an interface on a
//	component.  It should be called for every new copy of a pointer to an
//	interface on a given component.  It returns a U32 that contains a value
//	from 1 to 2^32 - 1 that defines the new reference count.  The return
//	value should only be used for debugging purposes.
//---------------------------------------------------------------------------
U32 CIFXUVMapperNone::AddRef() 
{
	return ++m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXUVMapperNone::Release
//
//	This method decrements the reference count for the calling interface on a
//	component.  It returns a U32 that contains a value from 1 to 2^32 - 1
//	that defines the new reference count.  The return value should only be
//	used for debugging purposes.  If the reference count on a component falls
//	to zero, the component is destroyed.
//---------------------------------------------------------------------------
U32 CIFXUVMapperNone::Release()
{
	if ( !( --m_uRefCount ) )
	{
		delete this;

		// This second return point is used so that the deleted object's
		// reference count isn't referenced after the memory is released.
		return 0;
	}

	return m_uRefCount;
}

//---------------------------------------------------------------------------
//	CIFXUVMapperNone::QueryInterface
//
//	This method provides access to the various interfaces supported by a
//	component.  Upon success, it increments the component's reference count,
//	hands back a pointer to the specified interface and returns IFX_OK.
//	Otherwise, it returns either IFX_E_INVALID_POINTER or IFX_E_UNSUPPORTED.
//
//	A number of rules must be adhered to by all implementations of
//	QueryInterface.  For a list of such rules, refer to the Microsoft COM
//	description of the IUnknown::QueryInterface method.
//---------------------------------------------------------------------------
IFXRESULT CIFXUVMapperNone::QueryInterface( IFXREFIID interfaceId, void** ppInterface )
{
	IFXRESULT	result	= IFX_OK;

	if ( ppInterface )
	{
		if ( interfaceId == IID_IFXUVMapper || interfaceId == IID_IFXUnknown )
			*ppInterface = ( IFXUVMapper* ) this;
		else
		{
			*ppInterface = NULL;

			result = IFX_E_UNSUPPORTED;
		}

		if ( IFXSUCCESS( result ) )
			( ( IFXUnknown* ) *ppInterface )->AddRef();
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

// Factory method
IFXRESULT IFXAPI_CALLTYPE CIFXUVMapperNone_Factory( IFXREFIID interfaceId, void** ppInterface ) 
{
	IFXRESULT	result;

	if ( ppInterface )
	{
		// It doesn't exist, so try to create it.  Note:  The component
		// class sets up gs_pSingleton upon construction and NULLs it
		// upon destruction.
		CIFXUVMapperNone	*pComponent	= new CIFXUVMapperNone;

		if ( pComponent )
		{
			// Perform a temporary AddRef for our usage of the component.
			pComponent->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pComponent->QueryInterface( interfaceId, ppInterface );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pComponent->Release();
		}
		else
			result = IFX_E_OUT_OF_MEMORY;
	}
	else
		result = IFX_E_INVALID_POINTER;

	return result;
}

// IFXUVMapper methods
IFXRESULT CIFXUVMapperNone::Apply(IFXMesh& rMesh, 
								  IFXUVMapParameters* pMapParams, 						
								  IFXMatrix4x4* pModelMatrix,					  
								  IFXMatrix4x4* pViewMatrix, 
								  const IFXLightSet* pLightSet)
{
	IFXRESULT rc=IFX_OK;
	
	if(pMapParams==NULL || pViewMatrix==NULL || pLightSet==NULL) 
	{
		rc=IFX_E_INVALID_POINTER;
	}
	
	if(IFXSUCCESS(rc)) 
	{
		BOOL bRebuildMesh=IFX_FALSE;
		IFXVertexAttributes iAttributes;

		// does this mesh support texture layers?
		// and if so, does it support the specified texture layer index
		iAttributes=rMesh.GetAttributes();
		U32 nPreviousTextureLayers = iAttributes.m_uData.m_uNumTexCoordLayers;

		if(m_bNeedTexCoords == FALSE)
		{
			if(nPreviousTextureLayers == 0)
			{
				bRebuildMesh = IFX_TRUE;
				iAttributes.m_uData.m_uNumTexCoordLayers = 1;
			}
		}
		else if(iAttributes.m_uData.m_uNumTexCoordLayers < (pMapParams->uTextureLayer+1)) 
		{
			bRebuildMesh=IFX_TRUE;
			iAttributes.m_uData.m_uNumTexCoordLayers=(pMapParams->uTextureLayer+1);
		}

		if(bRebuildMesh==IFX_TRUE) 
		{

			if(IFXSUCCESS(rc)) 
			{
				// re-allocate the mesh with new texture layer count.  Old data
				// should not be destroyed, just new tex-coords should be added to 
				// the existing mesh
				U32 numVerts = rMesh.GetNumVertices();
				U32 numFaces = rMesh.GetNumFaces();
				U32 maxNumVerts = rMesh.GetMaxNumVertices();
				U32 maxNumFaces = rMesh.GetMaxNumFaces();

				rc = rMesh.Allocate(iAttributes, 
					maxNumVerts, 
					maxNumFaces);

				if( maxNumVerts > numVerts )
					rMesh.SetNumVertices( numVerts );
				if( maxNumFaces > numFaces )
					rMesh.SetNumFaces( numFaces );

				// Initialize the new UV layers to the current values of the base layer.
				if(IFXSUCCESS(rc)) 
				{
					IFXVector2Iter UVSourceIter;
					rMesh.GetTexCoordIter(UVSourceIter, 0);

					if(0 == nPreviousTextureLayers)
					{
						// Need to initialize TextureCoordLayer to 0
						IFXVector2* pvTC = 0;
						U32 i;
						for( i = 0 ; i < rMesh.GetMaxNumVertices(); i++)
						{
							pvTC = UVSourceIter.Next();
							pvTC->U() = 0;
							pvTC->V() = 0;
						}
						UVSourceIter.PointAt(0);
					}

					IFXVector2Iter UVDestIter[IFX_MAX_TEXUNITS];
					U32 layer;
					for( layer = nPreviousTextureLayers; 
						 layer < iAttributes.m_uData.m_uNumTexCoordLayers;
						 layer++ )
					{
						rMesh.GetTexCoordIter((UVDestIter[layer]), layer);
					}

					IFXVector2* pvTexCoord;
					U32	uNumVertex = rMesh.GetMaxNumVertices();
					U32	uIndex;
					for(uIndex=0; uIndex<uNumVertex; uIndex++)
					{
						pvTexCoord = UVSourceIter.Next();
						for( layer = nPreviousTextureLayers; 
							 layer < iAttributes.m_uData.m_uNumTexCoordLayers;
							 layer++ )
							*UVDestIter[layer].Next() = *pvTexCoord;
					}
				}
			}
		} 
	}

	// we now have a garanteed good mesh in rMesh or an error code
	if(IFXSUCCESS(rc)) 
	{
		if(NeedToMap(rMesh, pMapParams))
		{
			rc=Map(rMesh, pMapParams, pModelMatrix, pViewMatrix, pLightSet);
		}
	}

	return rc;
}

IFXRESULT CIFXUVMapperNone::Map(IFXMesh& rMesh, 
								IFXUVMapParameters* pMapParams,
								IFXMatrix4x4* pModelMatrix,
								IFXMatrix4x4* pViewMatrix, 
								const IFXLightSet* pLightSet ) 
{
	return IFX_OK;
}

BOOL CIFXUVMapperNone::NeedToMap(IFXMesh& rMesh, IFXUVMapParameters* pMapParams)
{
	BOOL bRet = FALSE;

	IFXMeshAttributes eRenderTCs = rMesh.GetRenderTexCoordsInUse();
	if(eRenderTCs[IFX_MESH_RENDER_TC0 + pMapParams->uTextureLayer])
	{
		// Don't use any generated texture coordinates, just the originals
		eRenderTCs.reset(IFX_MESH_RENDER_TC0 + pMapParams->uTextureLayer);
		rMesh.SetRenderTexCoordsInUse(eRenderTCs);
	}

	return bRet;
}

IFXLight* CIFXUVMapperNone::GetClosestLight(const IFXLightSet* pLightSet)
{
	IFXLight* pLight = 0;
	I32 closestIndex = -1;
	U32 lightInstance;

	U32 iLightIndex;
	for( iLightIndex = 0; iLightIndex < pLightSet->GetNumLights(); iLightIndex++)
	{
		IFXLight *curLight = NULL;
		pLightSet->GetLight(iLightIndex, curLight, lightInstance);
		U32 lightType = (U32)-1;
		if( curLight ) 
		{
			IFXLightResource *pLR = curLight->GetLightResource();
			if( NULL != pLR )
				lightType = pLR->GetType();
			IFXRELEASE( pLR );
		}
		IFXRELEASE(curLight);

		if(lightType != IFXLightResource::AMBIENT)
		{
			closestIndex = iLightIndex;
			break;
		}
	}

	if(closestIndex >= 0)
	{
		pLightSet->GetLight(closestIndex, pLight, lightInstance);
	}

	return pLight;
}
