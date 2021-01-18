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
/*
@file  CIFXSceneGraph.cpp                                                    */

#include "IFXSceneGraphPCH.h"
#include "CIFXSceneGraph.h"
#include "IFXCoreServices.h"
#include "CIFXNode.h"
#include "CIFXLight.h"
#include "CIFXModel.h"
#include "IFXTextureObject.h"
#include "IFXMaterialResource.h"
#include "IFXMotionResource.h"
#include "IFXShaderLitTexture.h"
#include "IFXMixerConstruct.h"
#include "IFXRenderingCIDs.h"
#include "IFXScheduler.h"
#include "IFXAuthorCLODResource.h"
#include "IFXAuthorCLODMesh.h"

//#define USE_REF_COUNT_CHECK // Testing remote change conflicts...
#ifdef USE_REF_COUNT_CHECK

class CRefCounter
{
public:
	// IFXNameMap methods
	virtual U32	AddRef( void );
	virtual U32 Release( void );

public:
	CRefCounter();
	virtual ~CRefCounter();

private:
	U32				m_uRefCount;	
};


static CRefCounter RefCounter;
CRefCounter::CRefCounter( )
{
	m_uRefCount = 0;
}


CRefCounter::~CRefCounter( )
{
//	Make sure we left in a clean state
	IFXASSERT( 0 == m_uRefCount );
} // CRefCounter::~CRefCounter


U32 CRefCounter::AddRef()
{
	
	return ++m_uRefCount;
}


U32 CRefCounter::Release()
{
	
	return --m_uRefCount;
}

#endif


CIFXSceneGraph::CIFXSceneGraph()
{

#ifdef USE_REF_COUNT_CHECK

	RefCounter.AddRef();

#endif

	// IFXUnknown attributes...
	m_uRefCount = 0;

	// IFXMarker attributes...
	m_uRunningMarker = 1;
	m_uMark = 0;
	m_uPriority = 256;
	m_uUserData = 0;

	// IFXSceneGraph attributes...
	m_bInitialized = FALSE;
	m_uDebugFlags = 0;

	m_pCS = NULL;
	U32 uPaletteIndex=0;
	for (uPaletteIndex=0; uPaletteIndex < NUMBER_OF_PALETTES; uPaletteIndex++)
		m_pPalettes[uPaletteIndex] = NULL;

	m_bAnimationEnabledState = TRUE;

	m_pClockSubject = NULL;
}


CIFXSceneGraph::~CIFXSceneGraph()
{

#ifdef USE_REF_COUNT_CHECK

	RefCounter.Release();

#endif

	// Release the palettes
	U32 uPaletteIndex = 0;
	for (; uPaletteIndex < NUMBER_OF_PALETTES; uPaletteIndex++)
		if (m_pPalettes[uPaletteIndex])
			m_pPalettes[uPaletteIndex]->Release();

	IFXRELEASE( m_pCS );
	IFXRELEASE( m_pClockSubject );
}

IFXRESULT CIFXSceneGraph::GetSimClockSubject(IFXSubject** ppOutClockSubject)
{
	if (m_pClockSubject)
	{
		m_pClockSubject->AddRef(); 
	}
	*ppOutClockSubject = m_pClockSubject;

	return IFX_OK;
}

#if 0
IFXRESULT CIFXSceneGraph::SetSimClockSubject(IFXSubject* pInClockSubject)
{
	IFXRELEASE( m_pClockSubject );
	m_pClockSubject = pInClockSubject;

	if (m_pClockSubject)
	{
		m_pClockSubject->AddRef();
	}

	return IFX_OK;
}
#endif

IFXRESULT CIFXSceneGraph::GetCoreServices(IFXCoreServices **ppCS)
{
	IFXRESULT rc = IFX_OK;
	if (!ppCS)
		rc = IFX_E_INVALID_POINTER;
	else if (!m_pCS)
		rc = IFX_E_NOT_INITIALIZED;
	else
	{
		*ppCS = m_pCS; // copy CoreServices ptr
		m_pCS->AddRef();
	}

	IFXRETURN(rc);
}


IFXRESULT CIFXSceneGraph::Initialize(IFXCoreServices *pCS)
{
	IFXRESULT result = IFX_OK;

	// store a copy of the core services ptr
	if(NULL != pCS) 
	{
		IFXRELEASE(m_pCS);
		m_pCS = pCS;
		m_pCS->AddRef();
	}

	IFXScheduler *pScheduler = NULL;
	m_pCS->GetScheduler( IID_IFXScheduler, (void**)&pScheduler);

	if (pScheduler != NULL) {
		IFXSimulationManager *pSimMgr = NULL;
		pScheduler->GetSimulationManager( &pSimMgr );
		IFXASSERT( pSimMgr );
		IFXRELEASE( pScheduler );

		IFXClock *pClock = NULL;
		pSimMgr->GetClock( &pClock );
		IFXASSERT( pClock );
		IFXRELEASE( pSimMgr );

		IFXRELEASE( m_pClockSubject );
		pClock->QueryInterface( IID_IFXSubject, (void**)&m_pClockSubject );
		IFXASSERT( m_pClockSubject );
		IFXRELEASE( pClock );
	}

	U32 uPaletteIndex = 0;
	for (; uPaletteIndex < NUMBER_OF_PALETTES; uPaletteIndex++)
	{
		IFXRELEASE(m_pPalettes[uPaletteIndex]);
		result = IFXCreateComponent( CID_IFXPalette, 
		                             IID_IFXPalette, 
		                             (void**)&(m_pPalettes[uPaletteIndex]) );
		if (IFXFAILURE(result)) 
		{
			break;
		}

		result = m_pPalettes[uPaletteIndex]->Initialize(10);
		if (IFXFAILURE(result))	
		{
			break;
		}
	}

	// Create the default palette entries at palette ID = 0
	IFXUnknown* pUnknown = NULL;
	IFXModifier* pMod2 = NULL;
	IFXString defaultName;

	defaultName.Assign( PALETTE_DEFAULT_MATERIAL );
	m_pPalettes[MATERIAL]->SetDefault( &defaultName );
	IFXCreateComponent( CID_IFXMaterialResource, 
						IID_IFXUnknown, 
						(void**)&pUnknown);
	IFXMaterialResource* pMaterialResource = NULL;
	pUnknown->QueryInterface( IID_IFXMaterialResource, 
		(void**)&pMaterialResource );
	pMaterialResource->SetSceneGraph( (IFXSceneGraph*)this );
	IFXRELEASE( pMaterialResource );
	m_pPalettes[MATERIAL]->SetDefaultResourcePtr(pUnknown);
	IFXRELEASE( pUnknown );

	defaultName.Assign( PALETTE_DEFAULT_MODEL );
	m_pPalettes[GENERATOR]->SetDefault( &defaultName );
	IFXCreateComponent( CID_IFXAuthorCLODResource, 
						IID_IFXModifier, 
						(void**)&pMod2);
	IFXAuthorCLODResource* pAuthorCLODResource = NULL;
	pMod2->QueryInterface( IID_IFXAuthorCLODResource, 
		(void**)&pAuthorCLODResource );
	pAuthorCLODResource->SetSceneGraph( (IFXSceneGraph*)this );
	IFXAuthorCLODMesh* pMesh = NULL;
	IFXCreateComponent( CID_IFXAuthorMesh, 
						IID_IFXAuthorCLODMesh, 
						(void**)&pMesh);
	pAuthorCLODResource->SetAuthorMesh( pMesh );
	IFXRELEASE( pMesh );
	IFXRELEASE(pAuthorCLODResource);

	// add the plane to the Palette
	m_pPalettes[GENERATOR]->SetDefaultResourcePtr(pMod2);
	IFXRELEASE( pMod2 );

	defaultName.Assign( PALETTE_DEFAULT_SHADER );
	m_pPalettes[SHADER]->SetDefault( &defaultName );
	IFXCreateComponent( CID_IFXShaderLitTexture, 
						IID_IFXUnknown, 
						(void**)&pUnknown );
	{
		IFXShaderLitTexture* pShaderLitTexture = NULL;
		pUnknown->QueryInterface( IID_IFXShaderLitTexture, 
								  (void**)&pShaderLitTexture );
		pShaderLitTexture->SetSceneGraph( (IFXSceneGraph*)this );
		IFXRELEASE( pShaderLitTexture );
	}
	m_pPalettes[SHADER]->SetDefaultResourcePtr(pUnknown);
	IFXRELEASE( pUnknown );

	//* MotionResource pallete
	defaultName.Assign( PALETTE_DEFAULT_IFX_MOTION );
	m_pPalettes[MOTION]->SetDefault( &defaultName );
	IFXCreateComponent( CID_IFXMotionResource, 
						IID_IFXUnknown, 
						(void**)&pUnknown);
	{
		IFXMotionResource* pMotionResource = NULL;
		pUnknown->QueryInterface( IID_IFXMotionResource, 
								  (void**)&pMotionResource );
		pMotionResource->SetSceneGraph((IFXSceneGraph*)this);
		pMotionResource->GetMotionRef()->SetName(L"DefaultMotion");
		IFXRELEASE(pMotionResource);
	}
	m_pPalettes[MOTION]->SetDefaultResourcePtr(pUnknown);
	IFXRELEASE( pUnknown );

	//* MotionConstruct pallete
	defaultName.Assign( PALETTE_DEFAULT_MOTION );
	m_pPalettes[MIXER]->SetDefault( &defaultName );
	IFXCreateComponent( CID_IFXMixerConstruct,
						IID_IFXUnknown,
						(void**)&pUnknown);
	{
		IFXMixerConstruct* pMixer = NULL;
		pUnknown->QueryInterface( IID_IFXMixerConstruct, 
								  (void**)&pMixer );
		pMixer->SetSceneGraph((IFXSceneGraph*)this);
		IFXRELEASE(pMixer);
	}
	m_pPalettes[MIXER]->SetDefaultResourcePtr(pUnknown);
	IFXRELEASE(pUnknown);

	// Create default texture
	defaultName.Assign( PALETTE_DEFAULT_TEXTURE );
	m_pPalettes[TEXTURE]->SetDefault( &defaultName ); 

	IFXTextureObject* pTextureObject = NULL;

	
	IFXCreateComponent( CID_IFXTextureObject, 
						IID_IFXTextureObject, 
						(void**)&pTextureObject );

	IFXASSERT(pTextureObject);
	pTextureObject->SetSceneGraph((IFXSceneGraph*)this);
	pTextureObject->SetDefaultTexture();
	pTextureObject->QueryInterface( IID_IFXUnknown, (void**)&pUnknown );
	m_pPalettes[TEXTURE]->SetDefaultResourcePtr(pUnknown);
	IFXRELEASE(pUnknown);
	IFXRELEASE(pTextureObject);

	// Create the world group
	defaultName.Assign( PALETTE_DEFAULT_GROUP );
	m_pPalettes[NODE]->SetDefault( &defaultName );
	IFXCreateComponent(CID_IFXGroup, IID_IFXUnknown, (void**)&pUnknown);
	pUnknown->QueryInterface(IID_IFXModifier, (void**)&pMod2);
	pMod2->SetSceneGraph( (IFXSceneGraph*)this );
	m_pPalettes[NODE]->SetDefaultResourcePtr(pUnknown);
	IFXRELEASE( pMod2 );
	IFXRELEASE( pUnknown );

	// set default light resource
	defaultName.Assign( PALETTE_DEFAULT_LIGHT );
	m_pPalettes[LIGHT]->SetDefault( &defaultName );
	IFXCreateComponent( CID_IFXLightResource, 
            IID_IFXUnknown, 
            (void**)&pUnknown);
    {
        IFXLightResource* pLightResource = NULL;
		pUnknown->QueryInterface( IID_IFXLightResource, 
                (void**)&pLightResource );
		pLightResource->SetSceneGraph((IFXSceneGraph*)this);
		IFXRELEASE(pLightResource);
	}

	m_pPalettes[LIGHT]->SetDefaultResourcePtr(pUnknown);
	IFXRELEASE( pUnknown );

	// set default view resource
	defaultName.Assign( PALETTE_DEFAULT_CAMERA );
	m_pPalettes[VIEW]->SetDefault( &defaultName );
	IFXCreateComponent( CID_IFXViewResource, 
            IID_IFXUnknown, 
            (void**)&pUnknown);
	{
		IFXDECLARELOCAL(IFXViewResource,pViewResource);

		pUnknown->QueryInterface( IID_IFXViewResource, 
                (void**)&pViewResource );
		pViewResource->SetSceneGraph((IFXSceneGraph*)this);
		pViewResource->SetNumRenderPasses( 1 );
		pViewResource->SetRootNode( 0, 0 );
	}

	m_pPalettes[VIEW]->SetDefaultResourcePtr(pUnknown);
	IFXRELEASE( pUnknown );
	
	if ( IFXSUCCESS( result ) )
		m_bInitialized = TRUE;

	return result;
} // CIFXSceneGraph::Initialize(IFXCoreServices *pCS)


IFXRESULT IFXAPI_CALLTYPE CIFXSceneGraph_Factory(IFXREFIID riid, void **ppv)
{
	IFXRESULT result;

	if ( ppv )
	{
		// Create the CIFXClassName component.
		CIFXSceneGraph *pSceneGraph = new CIFXSceneGraph();

		if ( pSceneGraph )
		{
			// Perform a temporary AddRef for our usage of the component.
			pSceneGraph->AddRef();

			// Attempt to obtain a pointer to the requested interface.
			result = pSceneGraph->QueryInterface( riid, ppv );

			// Perform a Release since our usage of the component is now
			// complete.  Note:  If the QI fails, this will cause the
			// component to be destroyed.
			pSceneGraph->Release();
		}
		else result = IFX_E_OUT_OF_MEMORY;
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}

// IFXUnknown
U32 CIFXSceneGraph::AddRef()
{
	

	return ++m_uRefCount;
}

U32 CIFXSceneGraph::Release()
{
	
	if( m_uRefCount == 1 )
	{
		PreDestruct();
		delete this ;
		return 0 ;
	}
	else
		return --m_uRefCount;
}

IFXRESULT CIFXSceneGraph::QueryInterface(IFXREFIID riid, void **ppv)
{
	IFXRESULT result = IFX_OK;

	if ( ppv )
	{
		if ( riid == IID_IFXUnknown    )
			*ppv = (IFXUnknown*)this;
		else if ( riid == IID_IFXSceneGraph )
			*ppv = (IFXSceneGraph*)this;
		else if ( riid == IID_IFXMarker )
			*ppv = (IFXMarker*)this;
		else if ( riid == IID_IFXSubject )
			*ppv = (IFXSubject*)this;
		else if ( riid == IID_IFXMetaDataX ) 
			*ppv = ( IFXMetaDataX* ) this;
		else
		{
			*ppv = NULL;
			result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS(result) )
			AddRef();
	}
	else result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXSceneGraph::Mark(void)
{
	IFXRESULT result = IFX_E_UNDEFINED;

	// Mark local
	m_uMark = m_uRunningMarker;

	// Mark all the components in the scenegraph database...
	U32 uPaletteIndex = 0;
	for (; uPaletteIndex < NUMBER_OF_PALETTES; uPaletteIndex++)
	{
		if ( m_pPalettes[uPaletteIndex] )
		{
			U32 uID = 0;
			IFXMarker* pMarker = NULL;

			result = m_pPalettes[uPaletteIndex]->First(&uID);

			while ( IFXSUCCESS(result) )
			{
				result = m_pPalettes[uPaletteIndex]->GetResourcePtr(
														uID,
														IID_IFXMarker,
														(void**)&pMarker );

				if (IFXSUCCESS(result))
					result = pMarker->Mark();

				// it's ok if there's no resource.
				else if (    (IFX_E_PALETTE_NULL_RESOURCE_POINTER == result)
					      || (IFX_E_INVALID_RANGE == result) )
					result = IFX_OK ;

				IFXRELEASE( pMarker );

				if (IFXSUCCESS(result))
					result = m_pPalettes[uPaletteIndex]->Next(&uID);
			}
			if (result == IFX_E_PALETTE_INVALID_ENTRY) 
				result = IFX_OK;
		}
		else result = IFX_E_NOT_INITIALIZED;
	}

	return result;
}


IFXRESULT CIFXSceneGraph::Marked(BOOL* pbOutMarked)
{
	IFXRESULT result = IFX_OK;

	if (pbOutMarked)
		*pbOutMarked = (m_uMark == m_uRunningMarker);
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


void CIFXSceneGraph::SetPriority( U32 uInPriority,
								  BOOL bRecursive,		// optional. default == IFX_TRUE.
								  BOOL bPromotionOnly )	// optional. default == IFX_TRUE.
{
	// take the priority if it's a higher priority and we're only promoting:
	if (bPromotionOnly == IFX_TRUE)
	{
		if (uInPriority < m_uPriority)
			m_uPriority = uInPriority ;
	}
	else
		m_uPriority = uInPriority ;

	// Call SetPriority() on all children recursively:
	if (bRecursive == IFX_TRUE)
	{
		// set all the components in the scenegraph database:
		IFXRESULT rc = IFX_OK ;
		U32 uPaletteIndex = 0;

		// go through all the palettes:
		for (uPaletteIndex = 0; uPaletteIndex < NUMBER_OF_PALETTES; uPaletteIndex++)
		{
			if ( m_pPalettes[uPaletteIndex] != NULL )
			{
				U32 uID					= 0 ;
				//IFXUnknown* pUnknown	= NULL ;
				IFXMarker* pMarker		= NULL ;

				// get the first entry:
				rc = m_pPalettes[uPaletteIndex]->First(&uID);

				while ( IFXSUCCESS(rc) )
				{
					// get the IFXUnknown handle:
					rc = m_pPalettes[uPaletteIndex]->GetResourcePtr( uID,
																	 IID_IFXMarker,
																	 (void**)&pMarker );

					// propagate the call:
					if (IFXSUCCESS(rc))
						pMarker->SetPriority( uInPriority, 
											  bRecursive,
											  bPromotionOnly);

					// it's ok if there's no resource.
					else if (    (IFX_E_PALETTE_NULL_RESOURCE_POINTER == rc)
							  || (IFX_E_INVALID_RANGE == rc) )
						rc = IFX_OK ;


					IFXRELEASE( pMarker );

					// get the next entry in the palette:
					if (IFXSUCCESS(rc))
						rc = m_pPalettes[uPaletteIndex]->Next(&uID);
				} // end while (stepping through palette entries)

				if (rc == IFX_E_PALETTE_INVALID_ENTRY) 
					rc = IFX_OK;
			} // if ( m_pPalettes[uPaletteIndex] != NULL )

			else rc = IFX_E_NOT_INITIALIZED;
		} // end for (uPaletteIndex = 0; uPaletteIndex < NUMBER_OF_PALETTES; uPaletteIndex++)

	} // end if (bRecursive == IFX_TRUE)

	return ;
}


IFXRESULT CIFXSceneGraph::GetDebugFlags( U32* puOutDebugFlags )
{
	IFXRESULT result = IFX_OK;

	if (puOutDebugFlags)
		*puOutDebugFlags = m_uDebugFlags;
	else 
		result = IFX_E_INVALID_POINTER;

	return result;
}


IFXRESULT CIFXSceneGraph::GetPalette( EIFXPalette  eInPalette, 
                                      IFXPalette** ppOutPalette )
{
	IFXRESULT result = IFX_E_UNDEFINED;

	if (m_bInitialized)
	{
		if (eInPalette < NUMBER_OF_PALETTES)
		{
			if (m_pPalettes[eInPalette])
			{
				*ppOutPalette = m_pPalettes[eInPalette];
				m_pPalettes[eInPalette]->AddRef();
				result = IFX_OK;
			}
			else result = IFX_E_INVALID_POINTER;
		}
		else result = IFX_E_INVALID_HANDLE;
	}
	else result = IFX_E_NOT_INITIALIZED;

	return result;
}


U32 CIFXSceneGraph::CurrentMark(void)
{
	return m_uRunningMarker;
}


IFXRESULT CIFXSceneGraph::UnmarkAll(void)
{
	IFXRESULT result = IFX_E_UNDEFINED;

	// Increment running marker.
	// Note: A component is marked if it's m_uMark is equal to 
	//       m_uRunningMarker.
	m_uRunningMarker++; 

	if (m_uRunningMarker == 0)
	{ // We have exceeded the size of the datatype.

		// Explicitly set all marks in the database to zero.
		// Note: Since the m_uRunningMarker is currently zero, 
		//       we can just call Mark().
		result = Mark();

		// Increment the marker to unmark everything.
		m_uRunningMarker++;
	}
	else result = IFX_OK;

	return result;
}


IFXRESULT CIFXSceneGraph::SetDebugFlags(U32 uInDebugFlags)
{
	m_uDebugFlags = uInDebugFlags;
	return IFX_OK;
}



IFXRESULT CIFXSceneGraph::GetAnimationEnabledState( BOOL* pState )
//	This method is used to get the current animation enabled state.  If the
//	animation enabled state is TRUE, animation is globally enabled.  If
//	FALSE, animation is globally disabled and NO animation should playback.
//	Upon success, the specified BOOL is initialized with the animation
//	enabled state and IFX_OK is returned.  Otherwise, the following values
//	are returned:  IFX_E_INVALID_POINTER or IFX_E_NOT_INITIALIZED.
{
	IFXRESULT	result	= IFX_OK;

	if ( m_pCS )
	{
		if ( pState )
			*pState = m_bAnimationEnabledState;
		else
			result = IFX_E_INVALID_POINTER;
	}
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}


IFXRESULT CIFXSceneGraph::SetAnimationEnabledState( BOOL state )
//	This method is used to set the animation enabled state.  If the animation
//	enabled state is TRUE, animation is globally enabled.  If FALSE,
//	animation is globally disabled and NO animation should playback.  Upon
//	success, IFX_OK is returned.  Otherwise, IFX_E_NOT_INITIALIZED is
//	returned.
{
	IFXRESULT	result	= IFX_OK;

	if ( m_pCS )
		m_bAnimationEnabledState = state;
	else
		result = IFX_E_NOT_INITIALIZED;

	return result;
}
