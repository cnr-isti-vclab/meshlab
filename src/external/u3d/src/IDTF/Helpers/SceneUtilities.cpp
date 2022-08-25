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
@file SceneUtilitites.cpp

This module defines ...
*/


//***************************************************************************
//    Defines
//***************************************************************************


//***************************************************************************
//    Includes
//***************************************************************************

#include "IFXCheckX.h"
#include "IFXNameMap.h"

#include "SceneUtilities.h"
#include "DebugInfo.h"

using namespace U3D_IDTF;

//***************************************************************************
//    Constants
//***************************************************************************


//***************************************************************************
//    Enumerations
//***************************************************************************


//***************************************************************************
//    Classes, structures and types
//***************************************************************************


//***************************************************************************
//    Global data
//***************************************************************************


//***************************************************************************
//    Local data
//***************************************************************************


//***************************************************************************
//    Local function prototypes
//***************************************************************************


//***************************************************************************
//    Public methods
//***************************************************************************

//------------------------------------------------------------------
/**
Class ructor

@param     None

@return    Nothing
**/
//------------------------------------------------------------------

SceneUtilities::SceneUtilities() :
IFXDEFINEMEMBER(m_pCoreServicesMain),
IFXDEFINEMEMBER(m_pCoreServices),
IFXDEFINEMEMBER(m_pSceneGraph),
IFXDEFINEMEMBER(m_pShaderPalette),
IFXDEFINEMEMBER(m_pGeneratorPalette),
IFXDEFINEMEMBER(m_pNodePalette)
{
	m_bInit = false;
}

//------------------------------------------------------------------
/**
Class destructor

@param     None

@return    Nothing
**/
//------------------------------------------------------------------
SceneUtilities::~SceneUtilities()
{
	m_bInit = false;
}

//****************************************************************
// Scenegraph initialization
//****************************************************************
IFXRESULT SceneUtilities::InitializeScene( U32 profile, F64 scaleFactor )
{
	IFXRESULT result = IFX_OK;

	if( m_bInit )
	{
		result = IFX_E_ALREADY_INITIALIZED;
		IFXASSERT(0);
	}

	// Initialize the core services
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( CID_IFXCoreServices, IID_IFXCoreServices,
			(void**)&m_pCoreServicesMain );
	}

	if( IFXSUCCESS( result ) )
	{
		result = m_pCoreServicesMain->Initialize( profile, scaleFactor );
		if(IFXSUCCESS(result)) {
			IFXRELEASE(m_pCoreServices);
			result = m_pCoreServicesMain->GetWeakInterface(&m_pCoreServices);
		}
	}

	// Get hold of the scene graph
	if( m_pCoreServices && IFXSUCCESS( result ) )
		result = m_pCoreServices->GetSceneGraph( IID_IFXSceneGraph,
		(void**)&m_pSceneGraph );

	// Gain access to the palettes
	if( m_pSceneGraph )
	{
		if( IFXSUCCESS( result ) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::NODE,
				&m_pNodePalette );
		}
		if( IFXSUCCESS( result ) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::GENERATOR,
				&m_pGeneratorPalette );
		}
		if( IFXSUCCESS( result ) )
		{
			result = m_pSceneGraph->GetPalette( IFXSceneGraph::SHADER,
				&m_pShaderPalette );
		}
	}
	// Flag us as initialized
	if( IFXSUCCESS( result ) )
	{
		m_bInit = true;
	}

	return result;
}

IFXRESULT SceneUtilities::GetSceneMetaData( IFXMetaDataX** ppMetaData )
{
	IFXRESULT result = IFX_OK;
	IFXMetaDataX* pMetaData = NULL;

	if( !m_bInit || !ppMetaData || !m_pCoreServices )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = m_pCoreServices->QueryInterface(
		IID_IFXMetaDataX, (void**)&pMetaData );
	if( IFXSUCCESS( result ) )
		*ppMetaData = pMetaData; //releasing interface is left to the caller

	return result;
}


//****************************************************************
// Debugging
//****************************************************************
//------------------------------------------------------------------
/**
Dump the current state of the U3D engine to a human-readable
data file.    Extremely useful for debugging or understanding how
data are organized in the engine.

@param        const char* pFile        The name of the data file.

@return    IFXRESULT                 Return status of this method.
**/
//------------------------------------------------------------------
IFXRESULT SceneUtilities::WriteDebugInfo( const char* pFile )
{
	IFXRESULT     result = IFX_OK;
	DebugInfo    debugInfo;

	if( !m_bInit || !pFile )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ))
		result = debugInfo.Init( pFile );

	if( IFXSUCCESS( result ))
	{
		IFXDECLARELOCAL( IFXPalette, pViewPalette );
		IFXDECLARELOCAL( IFXPalette, pLightPalette );
		IFXDECLARELOCAL( IFXPalette, pSimTaskPalette );
		IFXDECLARELOCAL( IFXPalette, pTexturePalette );
		IFXDECLARELOCAL( IFXPalette, pMaterialPalette );
		IFXDECLARELOCAL( IFXPalette, pMixerPalette );
		IFXDECLARELOCAL( IFXPalette, pMotionPalette );

		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::VIEW, &pViewPalette ));
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::LIGHT, &pLightPalette ));
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::SIM_TASK, &pSimTaskPalette ));
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette ));
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pMaterialPalette ));
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette ));
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pMotionPalette ));

		// Dump debug information
		debugInfo.WriteNodePalette( m_pNodePalette );
		debugInfo.WriteLightPalette( pLightPalette );
		debugInfo.WriteViewPalette( pViewPalette );
		debugInfo.WriteModelPalette( m_pGeneratorPalette );
		debugInfo.WriteShaderPalette( m_pShaderPalette );
		debugInfo.WriteMaterialPalette( pMaterialPalette );
		debugInfo.WriteTexturePalette( pTexturePalette );
		debugInfo.WriteSimulationTaskPalette( pSimTaskPalette );
		debugInfo.WriteMixerPalette( pMixerPalette, pMotionPalette );
		debugInfo.WriteMotionPalette( pMotionPalette );
	}

	return result;
}


//****************************************************************
// Disk I/O
//****************************************************************
//------------------------------------------------------------------
/**
Saves the state of the U3D engine to a U3D file.

@param        const char* pFileName     The name of the U3D file to write
@param     IFXExportOptions in_ExportOptions    Indicates which scenegraph elements
should be written to the U3D file.

@return    IFXRESULT         Return status of this method.
**/
//------------------------------------------------------------------
IFXRESULT SceneUtilities::WriteSceneToFile( const IFXCHAR* pFileName,
										   IFXExportOptions in_ExportOptions)
{
	IFXRESULT         result = IFX_OK;
	IFXWriteManager*    pIFXWriteManager = NULL;
	IFXWriteBuffer*     pIFXWriteBuffer = NULL;
	IFXStdio*         pIFXStdio = NULL;

	if( !m_bInit || !pFileName)
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Initialize the write manager
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( CID_IFXWriteManager, IID_IFXWriteManager,
			(void**)&pIFXWriteManager );
	}

	if( pIFXWriteManager)
	{
		if( IFXSUCCESS( result ) )
			result = pIFXWriteManager->Initialize( m_pCoreServices );

		// Create the write buffer
		if( IFXSUCCESS( result ) )
		{
			result = IFXCreateComponent( CID_IFXStdioWriteBuffer,
				IID_IFXWriteBuffer,
				(void**)&pIFXWriteBuffer );
		}

		if( pIFXWriteBuffer && IFXSUCCESS( result ) )
		{
			result = pIFXWriteBuffer->QueryInterface( IID_IFXStdio,
				(void**)&pIFXStdio );
		}

		// Open the output file
		if( pIFXStdio && IFXSUCCESS( result ) )
			result = pIFXStdio->Open( (IFXCHAR*)pFileName );

		// Mark everything in the database for write
		if( m_pSceneGraph && IFXSUCCESS( result ) )
			result = m_pSceneGraph->Mark();

		// And write the results
		if( pIFXWriteManager && IFXSUCCESS( result ) )
			result = pIFXWriteManager->Write( pIFXWriteBuffer, in_ExportOptions );

		// Now close the output file.
		if( pIFXStdio && IFXSUCCESS( result ) )
			result = pIFXStdio->Close();

		IFXRELEASE( pIFXStdio );
		IFXRELEASE( pIFXWriteBuffer );
	}

	// And release the rest of the components
	IFXRELEASE( pIFXWriteManager );

	return result;
}

/**
Loads a U3D file

@param	const IFXCHAR* pFileName     The name of the U3D file to load

@return IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::LoadU3DFile( const IFXCHAR* pFileName )
{
	IFXRESULT         result = IFX_OK;
	IFXLoadManager        *pIFXLoadManager = NULL;
	IFXReadBuffer     *pIFXReadBuffer = NULL;
	IFXStdio*         pIFXStdio = NULL;

	if( !m_bInit || !pFileName )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Initialize the load manager
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( CID_IFXLoadManager,
			IID_IFXLoadManager,
			(void**)&pIFXLoadManager );
	}

	if( pIFXLoadManager)
	{
		if( IFXSUCCESS( result ) )
			result = pIFXLoadManager->Initialize( m_pCoreServices );

		// Create the read buffer
		if( IFXSUCCESS( result ) )
		{
			result = IFXCreateComponent( CID_IFXStdioReadBuffer,
				IID_IFXReadBuffer,
				(void**)&pIFXReadBuffer );
		}
		if( pIFXReadBuffer && IFXSUCCESS( result ) )
		{
			result = pIFXReadBuffer->QueryInterface( IID_IFXStdio,
				(void**)&pIFXStdio );
		}

		// Open the input file
		if( pIFXStdio && IFXSUCCESS( result ) )
			result = pIFXStdio->Open( (IFXCHAR*)pFileName );

		// And load its contents
		if( pIFXLoadManager && IFXSUCCESS( result ) )
			result = pIFXLoadManager->Load( pIFXReadBuffer, FALSE, TRUE );

		// Now close the input file.
		if( pIFXStdio && IFXSUCCESS( result ) )
			result = pIFXStdio->Close();
	}

	// And release the rest of the components
	IFXRELEASE( pIFXStdio );
	IFXRELEASE( pIFXLoadManager );
	IFXRELEASE( pIFXReadBuffer );

	return result;
}

/**
Creates File Reference.

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateFileReference( 
								const IFXString& rName,
								IFXFileReference** ppFileReference )
{
	IFXRESULT result = IFX_OK;
	IFXFileReference* pFileReference = NULL;
	IFXDECLARELOCAL( IFXPalette, pPalette );
	U32 index = 0;

	if( !m_bInit && ppFileReference )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent( CID_IFXFileReference, IID_IFXFileReference, (void**)&pFileReference );

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMarker, pMarker );

		result = pFileReference->QueryInterface(IID_IFXMarker, (void**)&pMarker );

		if( IFXSUCCESS( result ) )
			result = pMarker->SetSceneGraph( m_pSceneGraph );
	}

	if( IFXSUCCESS( result ) )
		result = m_pCoreServices->GetFileReferencePalette( &pPalette );

	if( IFXSUCCESS( result ) )
		result = pPalette->Add( rName.Raw(), &index );

	if( IFXSUCCESS( result ) )
		result = pPalette->SetResourcePtr( index, pFileReference );

	if( IFXSUCCESS( result ) )
		*ppFileReference = pFileReference; // releasing the object is left to user

	return result;
}

//****************************************************************
// Node utilities:    finding, creation
//****************************************************************

/**
Create a node somewhere in the scenegraph

@param const IFXString& rNodeName  Node name to create
@param IFXNode** ppNode Created node

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateNodePlaceholder( 
								const IFXString& rNodeName,
								U32* pNodeId )
{
	IFXRESULT result = IFX_OK;
	U32       nodeId = 0;

	if( !m_bInit )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXPalette, pPalette );

		result = m_pSceneGraph->GetPalette( 
						IFXSceneGraph::NODE, &pPalette );

		// Add an node's palette entry.
		if( IFXSUCCESS( result ) )
		{
			if( rNodeName == L"<NULL>" || rNodeName == L"" )
			{
				// If the name is null, look for the world node (palette ID 0).
				nodeId = 0;
			}
			else
			{
				result = pPalette->Add( rNodeName.Raw(), &nodeId );

				// if node's placeholder exists it is okay
				if( result == IFX_W_ALREADY_EXISTS )
					result = IFX_OK;
			}
		}
	}

	if( IFXSUCCESS( result ) && NULL != pNodeId )
		*pNodeId = nodeId;

	return result;
}

/**
Create a node somewhere in the scenegraph

@param const IFXString& rNodeName  Node name to create
@param IFXNode** ppNode Created node

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateNode( 
								const IFXString& rNodeName,
								const IFXCID& rComponentId,
								IFXNode** ppNode,
								U32* pNodeId )
{
	IFXRESULT result = IFX_OK;
	U32       nodeId = 0;
	IFXNode*  pNode = NULL;
	IFXDECLARELOCAL( IFXPalette, pPalette );

	if( !m_bInit && ppNode )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = FindNode( rNodeName, &pNode );

	if( result == IFX_E_CANNOT_FIND ||
		result == IFX_E_PALETTE_NULL_RESOURCE_POINTER )
	{
		// Create a component with an IFXNode interface
		result = IFXCreateComponent( 
							rComponentId, IID_IFXNode, (void**)&pNode );

		// Initialize the node
		if( IFXSUCCESS( result ) )
			result = pNode->SetSceneGraph( m_pSceneGraph );

		if( IFXSUCCESS( result ) )
			result = m_pSceneGraph->GetPalette( 
							IFXSceneGraph::NODE, &pPalette );

		// Add an entry for the group node into the node palette.
		if( IFXSUCCESS( result ) )
		{
			result = pPalette->Add( rNodeName.Raw(), &nodeId );

			// if node's placeholder exists it is okay
			if( result == IFX_W_ALREADY_EXISTS )
				result = IFX_OK;
		}

		// Point the node palette entry to the component (created above).
		if( IFXSUCCESS( result ) )
			result = pPalette->SetResourcePtr( nodeId, pNode );
	}

	if( IFXSUCCESS( result ) )
	{
		*ppNode = pNode; // releasing the object is left to user
		if( NULL != pNodeId )
			*pNodeId = nodeId;
	}
	else
		IFXRELEASE( pNode );

	return result;
}

/**
Creates a view (camera) node that is able to see the entire
scene graph

@param     const IFXString& rNodeName  Node name to create
@param   const ParentList& rParentList Parent data list on the node to create
@param     IFXView** ppView    A pointer to the new view node.

@return    IFXRESULT         Return status of this method.

@note This method create a view object. Releasing this object is left to
the user.
**/
IFXRESULT SceneUtilities::CreateViewNode(
								const IFXString& rNodeName,
								const IFXString& rResourceName,
								IFXView** ppView )
{
	IFXRESULT result = IFX_OK;
	U32 resourceId = 0;
	IFXView* pView = NULL;
	IFXDECLARELOCAL( IFXNode, pNode );
	IFXDECLARELOCAL( IFXPalette, pPalette );

	if( !m_bInit || !ppView )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	IFXTRACE_GENERIC( L"Camera - %ls:\n", rNodeName.Raw() );
	result = CreateNode( rNodeName, CID_IFXView, &pNode );

	// Get the component's IFXView interface
	if( pNode && IFXSUCCESS( result ) )
		result = pNode->QueryInterface( IID_IFXView, (void**)&pView );

	// Find view resource and point it to the view node
	if( IFXSUCCESS( result ) )
		result = m_pSceneGraph->GetPalette( IFXSceneGraph::VIEW, &pPalette );

	if( IFXSUCCESS( result ) )
		result = pPalette->Find( &rResourceName, &resourceId );

	if( IFX_E_CANNOT_FIND == result )
		result = pPalette->Add( rResourceName, &resourceId );

	// add empty entry to the palette
	if( IFXSUCCESS( result ) )
		result = pView->SetViewResourceID( resourceId );

	if( IFXSUCCESS( result ) )
		*ppView = pView; // Releasing the object is left to the user
	else
		IFXRELEASE( pView );

	return result;
}

IFXRESULT SceneUtilities::CreateViewResource(
								const IFXString& rName,
								IFXViewResource** ppViewResource )
{
	IFXRESULT result = IFX_OK;
	IFXViewResource* pResource = NULL;

	if( !m_bInit || !ppViewResource )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		U32 resourceId = 0;
		IFXDECLARELOCAL( IFXPalette, pPalette );

		if( IFXSUCCESS( result ) )
			result = m_pSceneGraph->GetPalette(
									IFXSceneGraph::VIEW, &pPalette );

		if( IFXSUCCESS( result ) )
			result = pPalette->Find( &rName, &resourceId );

		if( IFX_E_CANNOT_FIND == result )
		{
			// resource was not created
			result = IFXCreateComponent( CID_IFXViewResource, 
								IID_IFXViewResource, (void**)&pResource );

			if( IFXSUCCESS( result ) )
				result = pResource->SetSceneGraph( m_pSceneGraph );

			if( IFXSUCCESS( result ) )
			{
				// palette entry does not exist
				result = pPalette->Add( rName.Raw(), &resourceId );
			}

			if( IFXSUCCESS( result ) )
				result = pPalette->SetResourcePtr( 
								resourceId, pResource ); // make addref
		}
		else
		{
			result = pPalette->GetResourcePtr(
									resourceId,
									IID_IFXViewResource,
									(void**)&pResource );
		}
	}

	if( IFXSUCCESS( result ) )
		*ppViewResource = pResource; // Releasing the object is left to the user

	return result;
}


/**
Create a group node somewhere in the scenegraph

@param     const IFXString& rNodeName  Node name to create

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateGroupNode( const IFXString& rNodeName,
										  IFXNode** ppNode )
{
	IFXRESULT result = IFX_OK;
	IFXNode*  pNode = NULL;

	if( !m_bInit || !ppNode )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	result = CreateNode( rNodeName, CID_IFXGroup, &pNode );

	if( IFXSUCCESS( result ) )
		*ppNode = pNode;

	return result;
}

//------------------------------------------------------------------
/**
Create a light node and hook it into the scene graph

@param   const IFXString& rNodeName  Node name to create
@param   const ParentList& rParentList Parent data list on the node to create
@param   IFXLight** ppLight    A pointer to the light resource, *not* the
								light node that uses this resource.

@return    IFXRESULT         Return status of this method.
**/
//------------------------------------------------------------------
IFXRESULT SceneUtilities::CreateLightNode( 
									const IFXString& rNodeName,
									const IFXString& rResourceName,
									IFXLight** ppLight )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXNode, pNode );
	IFXDECLARELOCAL( IFXPalette, pPalette );
	IFXLight* pLight = NULL;
	U32 resourceId = 0;

	if( !m_bInit || !ppLight )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	IFXTRACE_GENERIC( L"Light - %ls:\n", rNodeName.Raw() );
	result = CreateNode( rNodeName, CID_IFXLight, &pNode );

	// Also get the component's IFXLight interface
	if( IFXSUCCESS( result ) )
		result = pNode->QueryInterface( IID_IFXLight, (void**)&pLight );

	if( IFXSUCCESS( result ) )
		result = m_pSceneGraph->GetPalette( 
							IFXSceneGraph::LIGHT, &pPalette );

	if( IFXSUCCESS( result ) )
		result = pPalette->Find( &rResourceName, &resourceId );

	if( IFX_E_CANNOT_FIND == result )
		result = pPalette->Add( rResourceName, &resourceId );

	if( IFXSUCCESS( result ) )
		result = pLight->SetLightResourceID( resourceId );

	if( IFXSUCCESS( result ) )
		*ppLight = pLight;
	else
		IFXRELEASE( pLight );

	return result;
}

/**
@note Does not create resource if it's already exist.
*/
IFXRESULT SceneUtilities::CreateLightResource(
								const IFXString& rName,
								IFXLightResource** ppLightResource )
{
	IFXRESULT result = IFX_OK;
	IFXLightResource* pResource = NULL;

	if( !m_bInit || !ppLightResource )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		U32 resourceId = 0;
		IFXDECLARELOCAL( IFXPalette, pPalette );

		if( IFXSUCCESS( result ) )
			result = m_pSceneGraph->GetPalette(
									IFXSceneGraph::LIGHT, &pPalette );

		if( IFXSUCCESS( result ) )
			result = pPalette->Find( &rName, &resourceId );

		if( IFX_E_CANNOT_FIND == result )
		{
			// resource was not created

			result = IFXCreateComponent( CID_IFXLightResource, 
								IID_IFXLightResource, (void**)&pResource );

			if( IFXSUCCESS( result ) )
				result = pResource->SetSceneGraph( m_pSceneGraph );

			if( IFXSUCCESS( result ) )
			{
				// Add palette entry
				result = pPalette->Add( rName.Raw(), &resourceId );
			}

			if( IFXSUCCESS( result ) )
				result = pPalette->SetResourcePtr( 
								resourceId, pResource ); // make addref
		}
		else
		{
			result = pPalette->GetResourcePtr(
									resourceId,
									IID_IFXLightResource,
									(void**)&pResource );
		}
	}

	if( IFXSUCCESS( result ) )
		*ppLightResource = pResource; // Releasing the object is left to the user

	return result;
}


//------------------------------------------------------------------
/**
Create a model node and attach it to the scene graph

@param   const IFXString& rNodeName  Node name to create

@return    IFXRESULT         Return status of this method.
**/
//------------------------------------------------------------------
IFXRESULT SceneUtilities::CreateModelNode( 
									const IFXString& rNodeName,
									const IFXString& rResourceName,
									IFXModel** ppModel )
{
	IFXRESULT result = IFX_OK;
	U32 resourceId = 0;
	IFXDECLARELOCAL( IFXNode, pNode );
	IFXDECLARELOCAL( IFXPalette, pPalette );
	IFXModel* pModel = NULL;

	if( !m_bInit || !ppModel )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	IFXTRACE_GENERIC( L"Model - %ls:\n", rNodeName.Raw() );
	result = CreateNode( rNodeName, CID_IFXModel, &pNode );

	// And also its IFXModel interface
	if( IFXSUCCESS( result ) )
		result = pNode->QueryInterface( IID_IFXModel, (void**)&pModel );

	if( IFXSUCCESS( result ) )
	{
		result = m_pSceneGraph->GetPalette(
									IFXSceneGraph::GENERATOR, &pPalette );

		if( IFXSUCCESS( result ) )
			result = pPalette->Find( &rResourceName, &resourceId );

		if( IFX_E_CANNOT_FIND == result )
			result = pPalette->Add( rResourceName, &resourceId );

		if( IFXSUCCESS( result ) )
			result = pModel->SetResourceIndex( resourceId );
	}

	if( IFXSUCCESS( result ) )
		*ppModel = pModel; // Releasing the object is left to the user
	else
		IFXRELEASE( pModel );

	return result;
}

//------------------------------------------------------------------
/**
	Return the IFXNode associated with a given node name.

	@param	rName	The case sensitive name of the node to look for.
	@param	ppNode	The returned IFX Node from the node palette.

	@return	Return status of this method.
**/
//------------------------------------------------------------------
IFXRESULT SceneUtilities::FindNode( 
								   const IFXString& rName, 
								   IFXNode** ppNode, 
								   U32* pNodeId )
{
	IFXRESULT   result = IFX_OK;
	U32         nodeId = 0;
	IFXDECLARELOCAL( IFXUnknown, pUnknown );
	IFXDECLARELOCAL( IFXPalette, pPalette );
	IFXNode*    pNode = NULL;

	if( !m_bInit || !ppNode )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = m_pSceneGraph->GetPalette(
									IFXSceneGraph::NODE, &pPalette );

	if( IFXSUCCESS( result ) )
	{
		if( rName == L"<NULL>" || rName == L"" )
		{
			// If the name is null, look for the world node (palette ID 0).
			nodeId = 0;
		}
		else
		{
			// Else look for this name in the node palette.
			result = pPalette->Find( rName.Raw(), &nodeId );
		}
	}

	// Get the component pointed to by this palette entry
	if( IFXSUCCESS( result ) )
		result = pPalette->GetResourcePtr( nodeId, &pUnknown );

	// Get the IFXNode interface of the component
	if( IFXSUCCESS( result ) )
		result = pUnknown->QueryInterface( IID_IFXNode, (void**) &pNode );

	if( IFXSUCCESS( result ) )
	{
		*ppNode = pNode; // releasing ppNode is left to the caller
		if( NULL != pNodeId )
			*pNodeId = nodeId;
	}

	return result;
}



//****************************************************************
// Resource creation
//****************************************************************

/**
Create a node somewhere in the scenegraph

@param const IFXString& rName  Resource name to create

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateResourcePlaceholder( 
								const IFXString& rName,
								IFXSceneGraph::EIFXPalette palette,
								U32* pId )
{
	IFXRESULT result = IFX_OK;
	U32       id = 0;

	if( !m_bInit )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXPalette, pPalette );

		result = m_pSceneGraph->GetPalette( palette, &pPalette );

		// Add an resource's palette entry.
		if( IFXSUCCESS( result ) )
		{
			if( rName == L"" )
			{
				id = 0;
			}
			else
			{
				result = pPalette->Add( (IFXString*)&rName, &id );

				// if node's placeholder exists it is okay
				if( result == IFX_W_ALREADY_EXISTS )
					result = IFX_OK;
			}
		}
	}

	if( IFXSUCCESS( result ) && NULL != pId )
		*pId = id;

	return result;
}


/**
Create a author CLOD resource

@param	const IFXString& rName  The name of the generator in
								the palette
@param  IFXAuthorMesh * pMesh   The mesh data to add to the generator
@param	const IFXAuthorCLODCompressionParams& rParams
								The quality factors with which to save
								the mesh data (range = 0 to 1000).    
								Larger values correspond to higher quality
								with less compression. Smaller values result 
								in higher compression but a mesh 
								representation of poorer quality.
@param  U32 modelPriority       The streaming priority of the data 
								for this mesh.
@param  IFXAuthorCLODResource** ppModel     The new resource itself.

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateMeshResource(
								const IFXString& rName,
								IFXAuthorMesh* pMesh,
								IFXAuthorGeomCompilerParams& rParams,
								IFXAuthorCLODResource** ppModel )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXPalette, pPalette );
	IFXAuthorCLODResource* pModel = NULL;

	if( !m_bInit || !pMesh || !ppModel )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		result = m_pSceneGraph->GetPalette(
									IFXSceneGraph::GENERATOR, &pPalette );
	}

	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXGenerator, pGenerator );
		U32 resourceId = 0;

		result = pPalette->Find( &rName, &resourceId );

		if( IFX_E_CANNOT_FIND == result )
		{
			IFXAuthorGeomCompiler* pCompiler = NULL;

			// Create the IFXAuthorGeomCompiler, which we will use to convert the
			// IFXAuthorMeshGroup into a part of the IFXAuthorCLODResource
			result = IFXCreateComponent( 
								CID_IFXAuthorGeomCompiler,
								IID_IFXAuthorGeomCompiler,
								(void**) &pCompiler);

			// Initialize the mesh compiler
			if( IFXSUCCESS( result ) )
				result = pCompiler->SetSceneGraph( m_pSceneGraph );

			if( IFXSUCCESS( result ) )
			{
				// Compile the mesh.
				/* NOTE: 
					The IFXAuthorCLODParams allows you to change the compile 
					process, set up a status callback, determine if the 
					compile was canceled by the user, and other useful things
				*/
				result = pCompiler->Compile( 
										(IFXString&)rName, pMesh,
										&pModel, FALSE, &rParams );
			}

			if( IFXSUCCESS( result ) )
				result = pPalette->Add( rName.Raw(), &resourceId );

			// And point the palette entry to the actual resource
			if( IFXSUCCESS( result ) )
				result = pPalette->SetResourcePtr( 
								resourceId, pModel ); // make addref

			IFXRELEASE(pCompiler);
		}
		else
		{
			result = pPalette->GetResourcePtr(
									resourceId,
									IID_IFXGenerator,
									(void**)&pGenerator );

			if( IFXSUCCESS( result ) )
				result = pGenerator->QueryInterface( 
										IID_IFXAuthorCLODResource,
										(void**)&pModel );
		}
	}

	if( IFXSUCCESS( result ) )
		*ppModel = pModel; // releasing pGenerator is left to the caller

	return result;
}


/**
Create a generator object from an IFXAuthorPointSet and add it to the
generator palette

@param     const IFXString& rName      The name of the generator in the
palette
@param     IFXAuthorPointSet* pPointSet  The point set data to add to the generator
@param     U32 modelPriority           The streaming priority of the data for
this point set.
@param     IFXAuthorPointSetResource** ppModel     The new generator itself.

@return    IFXRESULT         Return status of this method.
**/
IFXRESULT SceneUtilities::CreatePointSetResource(
								const IFXString& rName,
								IFXAuthorPointSet* pPointSet,
								U32 modelPriority,
								IFXAuthorPointSetResource** ppModel )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXGenerator, pGenerator );
	IFXAuthorPointSetResource* pPointSetGenerator = NULL;

	if( !m_bInit || !pPointSet || !ppModel )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = CreateModelResource( rName, CID_IFXAuthorPointSetResource,
									modelPriority, &pGenerator );

	if( pGenerator && IFXSUCCESS( result ) )
		result = pGenerator->QueryInterface( IID_IFXAuthorPointSetResource,
										(void**)&pPointSetGenerator );

	if( pPointSetGenerator && IFXSUCCESS( result ) )
		result = pPointSetGenerator->SetAuthorPointSet( pPointSet );

	if( IFXSUCCESS( result ) )
		*ppModel = pPointSetGenerator;

	return result;
}


/**
Create a generator object from an IFXAuthorLineSet and add it to the
generator palette

@param     const IFXString& rName      The name of the generator in the
palette
@param     IFXAuthorLineSet* pLineSet  The line set data to add to the generator
@param     U32 modelPriority           The streaming priority of the data for
this line set.
@param     IFXAuthorLineSetResource** ppModel     The new generator itself.

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateLineSetResource(
								const IFXString& rName,
								IFXAuthorLineSet* pLineSet,
								U32 modelPriority,
								IFXAuthorLineSetResource** ppModel )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXGenerator, pGenerator );
	IFXAuthorLineSetResource* pLineSetGenerator = NULL;

	if( !m_bInit || !pLineSet || !ppModel )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = CreateModelResource( rName, CID_IFXAuthorLineSetResource,
								modelPriority, &pGenerator );

	if( pGenerator && IFXSUCCESS( result ) )
		result = pGenerator->QueryInterface( IID_IFXAuthorLineSetResource,
										(void**)&pLineSetGenerator );

	if( pLineSetGenerator && IFXSUCCESS( result ) )
		result = pLineSetGenerator->SetAuthorLineSet( pLineSet );

	if( IFXSUCCESS( result ) )
		*ppModel = pLineSetGenerator;

	return result;
}


//------------------------------------------------------------------
/**
Get the IFXAuthorCLODResource associated with a named model node.

@param     const IFXString& pModelName     The name of the node for which we want
the generator.
@param     IFXAuthorCLODResource **ppAuthorCLODResource The returned generator.

@return    IFXRESULT         Return status of this method.
**/
//------------------------------------------------------------------
IFXRESULT SceneUtilities::FindResourceByModelName(
								const IFXString& rModelName,
								IFXAuthorCLODResource** ppAuthorCLODResource )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXModel, pModel );
	U32         generatorID = 0;
	IFXAuthorCLODResource*  pAuthorCLODResource = NULL;

	if( !m_bInit || !ppAuthorCLODResource )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get the generator palette
	if( IFXSUCCESS( result ) )
	{
		if( !m_pGeneratorPalette )
			result = IFX_E_NOT_INITIALIZED;
	}

	if( IFXSUCCESS( result ) )
		result = FindModel( rModelName , &pModel );

	if( pModel && IFXSUCCESS( result ) )
		generatorID = pModel->GetResourceIndex();

	if( m_pGeneratorPalette && IFXSUCCESS( result ) )
	{
		result = m_pGeneratorPalette->GetResourcePtr(
			generatorID,
			IID_IFXAuthorCLODResource,
			(void **)&pAuthorCLODResource );
	}

	if( pAuthorCLODResource && IFXSUCCESS( result ) )
	{
		// Freeing pACLODResource is left to the caller
		*ppAuthorCLODResource = pAuthorCLODResource;
	}

	return result;
}


IFXRESULT SceneUtilities::FindPaletteEntry(
								const IFXString& rName,
								IFXSceneGraph::EIFXPalette palette,
								IFXUnknown** ppPaletteEntry,
								U32* pPaletteId )
{
	IFXRESULT result = IFX_OK;
	U32 paletteId = 0;
	IFXDECLARELOCAL( IFXPalette, pPalette );
	IFXUnknown* pPaletteEntry = NULL;

	if( !m_bInit || !ppPaletteEntry || !pPaletteId )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get the palette
	if( m_pSceneGraph && IFXSUCCESS( result ) )
		result = m_pSceneGraph->GetPalette(
									palette, &pPalette );

	if( IFXSUCCESS( result ) )
	{
		if( rName == L"<NULL>" || rName == L"" )
		{
			// If the name is null, look for the 0 entry (palette ID 0).
			paletteId = 0;
		}
		else
		{
			// Else look for this name in the palette.
			result = pPalette->Find( rName.Raw(), &paletteId );
		}
	}

	if( IFXSUCCESS( result ) )
		result = pPalette->GetResourcePtr(
									paletteId,
									IID_IFXUnknown,
									(void**)&pPaletteEntry );

	if( IFXSUCCESS( result ) )
	{
		// Releasing ppResource is left to the caller
		*ppPaletteEntry = pPaletteEntry;
		*pPaletteId = paletteId;
	}

	return result;
}

IFXRESULT SceneUtilities::FindTexture(
									  const IFXString& rName,
									  IFXTextureObject** ppResource,
									  U32* pTextureId )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXUnknown, pUnknown );
	IFXTextureObject* pResource = NULL;
	U32 textureId = 0;

	if( !m_bInit || !ppResource )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = FindPaletteEntry(
		rName,
		IFXSceneGraph::TEXTURE,
		&pUnknown,
		&textureId );

	if( IFXSUCCESS( result ) )
		result = pUnknown->QueryInterface( IID_IFXTextureObject, (void**)&pResource );

	if( IFXSUCCESS( result ) )
	{
		// Releasing ppResource is left to the caller
		*ppResource = pResource;
		*pTextureId = textureId;
	}

	return result;
}

IFXRESULT SceneUtilities::AddTexturePaletteEntry(
									  const IFXString& rName,
									  U32* pTextureId )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXPalette, pPalette );
	U32 textureId = 0;

	if( !m_bInit )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( m_pSceneGraph && IFXSUCCESS( result ) )
		result = m_pSceneGraph->GetPalette(
									IFXSceneGraph::TEXTURE, &pPalette );

	if( IFXSUCCESS( result ) )
		result = pPalette->Find( &rName, &textureId );

	if( IFX_E_CANNOT_FIND == result ) 
	{
		// Add the texture name to the palette if not found
		result = pPalette->Add( (IFXString*)&rName, &textureId );
	}

	if( IFXSUCCESS( result ) )
		*pTextureId = textureId;

	return result;
}

IFXRESULT SceneUtilities::FindMaterial(
									   const IFXString& rName,
									   IFXMaterialResource** ppResource,
									   U32* pMaterialId )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXUnknown, pUnknown );
	IFXMaterialResource* pResource = NULL;
	U32 materialId = 0;

	if( !m_bInit || !ppResource )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = FindPaletteEntry(
						rName,
						IFXSceneGraph::MATERIAL,
						&pUnknown,
						&materialId );

	if( IFXSUCCESS( result ) )
		result = pUnknown->QueryInterface( IID_IFXMaterialResource, (void**)&pResource );

	if( IFXSUCCESS( result ) )
	{
		// Releasing ppResource is left to the caller
		*ppResource = pResource;
		*pMaterialId = materialId;
	}

	return result;
}

IFXRESULT SceneUtilities::FindShader(
									 const IFXString& rName,
									 IFXShaderLitTexture** ppResource,
									 U32* pShaderId )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXUnknown, pUnknown );
	IFXShaderLitTexture* pResource = NULL;
	U32 shaderId = 0;

	if( !m_bInit || !ppResource )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
		result = FindPaletteEntry(
					rName,
					IFXSceneGraph::SHADER,
					&pUnknown,
					&shaderId );

	if( IFXSUCCESS( result ) )
		result = pUnknown->QueryInterface( IID_IFXShaderLitTexture, (void**)&pResource );

	if( IFXSUCCESS( result ) )
	{
		// Releasing ppResource is left to the caller
		*ppResource = pResource;
		*pShaderId = shaderId;
	}

	return result;
}


IFXRESULT SceneUtilities::FindMotionResource(
	const IFXString& rName,
	IFXMotionResource** ppResource )
{
	IFXRESULT result = IFX_OK;
	U32 resourceId = 0;
	IFXDECLARELOCAL( IFXPalette, pPalette );
	IFXMotionResource* pResource = NULL;

	if( !m_bInit || !ppResource )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get the generator palette
	if( IFXSUCCESS( result ) )
		IFXCHECKX( m_pSceneGraph->GetPalette(
		IFXSceneGraph::MOTION, &pPalette ));

	if( IFXSUCCESS( result ) )
		result = pPalette->Find( &rName, &resourceId );

	if( IFXSUCCESS( result ) )
		result = pPalette->GetResourcePtr(
		resourceId,
		IID_IFXViewResource,
		(void**)&pResource );

	if( pResource && IFXSUCCESS( result ) )
	{
		// Releasing ppResource is left to the caller
		*ppResource = pResource;
	}

	return result;
}


//------------------------------------------------------------------
/**
Get the IFXModel node given only its name

@param  const IFXString& rModelName The name of the node for which we want
the model mode.
@param  IFXModel** ppModel      The returned model node.

@return IFXRESULT         Return status of this method.
**/
//------------------------------------------------------------------
IFXRESULT SceneUtilities::FindModel( const IFXString& rModelName,
									IFXModel** ppModel)
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXNode, pNode );
	IFXModel* pModel = NULL;
	U32       modelID = 0;

	if( !m_bInit || !ppModel )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get our model node
	if( m_pNodePalette && IFXSUCCESS( result ) )
		result = m_pNodePalette->Find( rModelName, &modelID );

	if( m_pNodePalette && IFXSUCCESS( result ) )
		result = m_pNodePalette->GetResourcePtr(modelID, IID_IFXNode, (void **)&pNode);

	// Get the model's resource
	if( pNode && IFXSUCCESS( result ) )
		result = pNode->QueryInterface( IID_IFXModel, (void**)&pModel );

	if( ppModel && pModel && IFXSUCCESS( result ) )
		*ppModel = pModel; // Releasing pModel is left to the caller

	return result;
}

IFXRESULT SceneUtilities::FindModelResource(
								const IFXString& rResourceName,
								IFXGenerator** ppModelResource )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXPalette, pPalette );
	IFXGenerator* pModelResource = NULL;
	U32 resourceID = 0;

	if( !m_bInit || !ppModelResource )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get our model resource
	if( IFXSUCCESS( result ) )
		result = m_pSceneGraph->GetPalette(
									IFXSceneGraph::GENERATOR, &pPalette );

	if( IFXSUCCESS( result ) )
		result = pPalette->Find( &rResourceName, &resourceID );

	if( IFXSUCCESS( result ) )
		result = pPalette->GetResourcePtr( resourceID, IID_IFXGenerator, (void**)&pModelResource );

	if( IFXSUCCESS( result ) )
		*ppModelResource = pModelResource; // Releasing ppModelResource is left to the caller

	return result;
}



/**
Create a primitive and add it to the generator palette

@param     const IFXString& rName             The name of the primitive in
the model palette
@param     IFXREFCID componentId         The component ID (CID) of the primitive to create
@param     U32 primitivePriority         The streaming priority of the data
associated with this primitive.
@param     U32* pModelID                 The model palette ID of the new primitive
@param     IFXModifier** ppPrimitive     A pointer to the new primitive

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateModelResource( const IFXString& rName,
										  IFXREFCID componentId,
										  U32 priority,
										  IFXGenerator** ppGenerator )
{
	IFXRESULT result = IFX_OK;
	IFXGenerator* pGenerator    = NULL;

	if( !m_bInit || !ppGenerator )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		U32 resourceId = 0;
		IFXDECLARELOCAL( IFXPalette, pPalette );

		if( IFXSUCCESS( result ) )
			result = m_pSceneGraph->GetPalette(
									IFXSceneGraph::GENERATOR, &pPalette );

		if( IFXSUCCESS( result ) )
			result = pPalette->Find( &rName, &resourceId );

		if( IFX_E_CANNOT_FIND == result )
		{
			// resource was not created
			if( CID_IFXAuthorCLODResource == componentId )
			{
				result = IFXCreateComponent( CID_IFXAuthorCLODResource,
											IID_IFXGenerator,
											(void**)&pGenerator);
			}
			else if( CID_IFXAuthorLineSetResource == componentId )
			{
				result = IFXCreateComponent( CID_IFXAuthorLineSetResource,
											IID_IFXGenerator,
											(void**)&pGenerator);
			}
			else if( CID_IFXAuthorPointSetResource == componentId )
			{
				result = IFXCreateComponent( CID_IFXAuthorPointSetResource,
											IID_IFXGenerator,
											(void**)&pGenerator);
			}
			else
				result = IFX_E_UNDEFINED;

			// Initialize the generator through the marker interface:
			if( IFXSUCCESS( result ) )
				result = pGenerator->SetSceneGraph( m_pSceneGraph );

			if( IFXSUCCESS( result ))
				pGenerator->SetPriority( priority );

			if( IFXSUCCESS( result ) )
			{
				// palette entry does not exist
				result = pPalette->Add( rName.Raw(), &resourceId );
			}

			if( IFXSUCCESS( result ) )
				result = pPalette->SetResourcePtr( 
								resourceId, pGenerator ); // make addref
		}
		else
		{
			result = pPalette->GetResourcePtr(
									resourceId,
									IID_IFXGenerator,
									(void**)&pGenerator );
		}
	}

	// Freeing pGenerator is left to the caller
	if( pGenerator && IFXSUCCESS( result ) )
		*ppGenerator = pGenerator;

	return result;
}

//****************************************************************
// Surface properties
//****************************************************************

/**
	Create a IFXShaderLitTexture and set IFXMaterialResource that is uses.

@param	const IFXString& pShaderName  
		The name of the shader resource to create.

@param  const IFXString& pMaterialName
		The name of the material.

@param  IFXShaderLitTexture** ppShader    
		The new shader resource.

@return IFXRESULT         Return status of this method.
@retval IFX_OK            Shader successfully created.
@retval IFX_W_ALREADY_EXISTS  A shader of this name already exists in the
								palette.
*/
IFXRESULT SceneUtilities::CreateShader(
									   const IFXString& rShaderName,
									   const IFXString& rMaterialName,
									   IFXShaderLitTexture** ppShader )
{
	IFXRESULT result = IFX_OK;

	U32 shaderId = 0;
	U32 materialId = 0;
	IFXDECLARELOCAL( IFXPalette, pPalette );
	IFXShaderLitTexture* pShader = NULL;

	if( !m_bInit || !ppShader )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Create the IFXShaderLitTexture resource
	if( IFXSUCCESS( result ) )
		result = IFXCreateComponent(
		CID_IFXShaderLitTexture,
		IID_IFXShaderLitTexture,
		(void**)&pShader );

	// Initialize the new shader
	if( IFXSUCCESS( result ) )
		result = pShader->SetSceneGraph( m_pSceneGraph );

	if( m_pSceneGraph && IFXSUCCESS( result ) )
		result = m_pSceneGraph->GetPalette(
		IFXSceneGraph::SHADER, &pPalette );

	// Make an entry for the shader in the shader palette
	if( IFXSUCCESS( result ) )
	{
		result = pPalette->Add( (IFXString*)&rShaderName, &shaderId );

		if( IFX_W_ALREADY_EXISTS == result )
			return result;
	}

	// Point the palette entry to the shader resource
	if( IFXSUCCESS( result ) )
		result = pPalette->SetResourcePtr( shaderId, pShader );

	// Create an IFXMaterialResource for this shader.
	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMaterialResource, pMaterial );
		result = FindMaterial( rMaterialName, &pMaterial, &materialId );
	}

	// Point the shader to the material palette entry corresponding to the
	// new material
	if( IFXSUCCESS( result ) )
		result = pShader->SetMaterialID( materialId );

	// Enable the material channel on the shader so that the material properties
	// can be rendered.
	if( IFXSUCCESS( result ) )
		result = pShader->SetFlags( IFXShaderLitTexture::MATERIAL );

	if( IFXSUCCESS( result ) )
	{
		*ppShader = pShader; // releasing pShader is left to the caller
	}

	return result;
}

//-------------------------------------------------------------------------------------------------
/**
Return the material used by a given IFXShaderLitTexture

@param     IFXShaderLitTexture *pShaderLitTexture
@param     IFXMaterial ** ppMaterial

@return    IFXRESULT                 Return status of this method.
**/
//-------------------------------------------------------------------------------------------------
IFXRESULT SceneUtilities::GetMaterialFromShader( IFXShaderLitTexture* pShaderLitTexture,
												IFXMaterialResource** ppMaterial)
{
	IFXRESULT	result = IFX_OK;
	U32			uMaterialID = 0;
	IFXUnknown*	pUnknown = NULL;

	if( !m_bInit || !pShaderLitTexture || !ppMaterial )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get the ID of the material used by the shader.
	if( pShaderLitTexture && IFXSUCCESS( result ) )
	{
		uMaterialID = pShaderLitTexture->GetMaterialID();
	}

	IFXDECLARELOCAL( IFXPalette, pMaterialPalette );
	IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pMaterialPalette ));

	// Get the indexed material from the material palette
	if( IFXSUCCESS( result ) )
	{
		result = pMaterialPalette->GetResourcePtr(uMaterialID, &pUnknown);
	}

	// Return this with the IFXMaterialResource interface
	if( pUnknown && IFXSUCCESS( result ) )
	{
		// Freeing this pointer is up to the user!
		result = pUnknown->QueryInterface(IID_IFXMaterialResource, (void**) ppMaterial);
	}

	IFXRELEASE(pUnknown);

	return result;
}

//-------------------------------------------------------------------------------------------------
/**
Return the texture used by a given IFXShaderLitTexture

@param     IFXShaderLitTexture *pShaderLitTexture
@param     IFXTextureObject** ppTexture

@return    IFXRESULT Return status of this method.
**/
//-------------------------------------------------------------------------------------------------
IFXRESULT SceneUtilities::GetTextureFromShader( 
								IFXShaderLitTexture* pShaderLitTexture,
								IFXTextureObject** ppTexture,
								U32 layer )
{
	IFXRESULT result = IFX_OK;
	U32       textureId = 0;
	IFXDECLARELOCAL( IFXUnknown, pUnknown );
	IFXDECLARELOCAL( IFXPalette, pTexturePalette );

	if( !m_bInit || !pShaderLitTexture || !ppTexture )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Get the ID of the material used by the shader.
	if( pShaderLitTexture && IFXSUCCESS( result ) )
	{
		result = pShaderLitTexture->GetTextureID( layer, &textureId );
	}


	// Get the indexed material from the material palette
	if( IFXSUCCESS( result ) )
	{
		IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette ));
		result = pTexturePalette->GetResourcePtr( textureId, &pUnknown );
	}

	// Return this with the IFXTextureObject interface
	if( pUnknown && IFXSUCCESS( result ) )
	{
		// Freeing this pointer is up to the user!
		result = pUnknown->QueryInterface( IID_IFXTextureObject, (void**) ppTexture );
	}

	return result;
}

/**
Create a material resource holding basic surface properties

@param     const IFXString& rName The name of the new material
@param     IFXMaterialResource** ppMaterial     The new material resource itself.

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateMaterial(
	const IFXString& rName,
	IFXMaterialResource** ppMaterial )
{
	IFXRESULT result = IFX_OK;
	U32 materialId = 0;
	IFXDECLARELOCAL( IFXUnknown, pUnknown );

	if( !m_bInit || !ppMaterial )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Create the IFXMaterialResource object with an IFXUnknown interface
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( CID_IFXMaterialResource, IID_IFXUnknown,
			(void**)&pUnknown);
	}

	// Ask for the IFXMaterialResource resource on our object
	if( pUnknown && IFXSUCCESS( result ) )
	{
		result = pUnknown->QueryInterface( IID_IFXMaterialResource, (void**)ppMaterial );
	}

	// Initialize the material
	if( ppMaterial && IFXSUCCESS( result ) )
	{
		result = (*ppMaterial)->SetSceneGraph( m_pSceneGraph );
	}

	IFXDECLARELOCAL( IFXPalette, pMaterialPalette );
	IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::MATERIAL, &pMaterialPalette ));

	// Create an entry in the material resource palette for this material
	if( IFXSUCCESS( result ) )
	{
		result = pMaterialPalette->Add( (IFXString*)&rName, &materialId );
	}

	// Point the palette entry to the material resource.
	if( IFXSUCCESS( result ) )
	{
		result = pMaterialPalette->SetResourcePtr( materialId, pUnknown );
	}

	return result;
}

/**
Create a texture object

@param  const IFXString& name    The name of the new texture
@param	STextureSourceInfo& sourceImageInfo Image info
@param  U32 priority    The streaming priority of the data
						associated with this texture
@param  U32 sizeLimit         The maximum allowed size of texture
@param  U8* data             The raw bitmap to place in the texture object

@note      The raw bitmap must contain non-aligned rows of RGB or RGBA data.
The pixel rows may not be represented as alternating rows of R, G, B,
and A, and the bitmap data passed to this routine must be fully
decompressed.

@return    IFXRESULT         Return status of this method.
*/
IFXRESULT SceneUtilities::CreateTexture(
								const IFXString& rName,
								const U32 priority,
								IFXTextureObject** ppTextureObject )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXPalette, pTexturePalette );
	IFXTextureObject* pTextureObject = NULL;
	U32 textureId = 0;

	if( !m_bInit || !ppTextureObject )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( CID_IFXTextureObject, IID_IFXTextureObject,
			(void**)&pTextureObject );
	}

	// Initialize the texture object
	if( IFXSUCCESS( result ) )
		result = pTextureObject->SetSceneGraph( m_pSceneGraph );

	if( IFXSUCCESS( result ) )
		pTextureObject->SetPriority( priority );

	// Set the default pixel format to use when rendering the Texture Object
	if( IFXSUCCESS( result ) )
		result = pTextureObject->SetRenderFormat( IFX_RGBA_8888 );

	if( m_pSceneGraph && IFXSUCCESS( result ) )
		result = m_pSceneGraph->GetPalette( IFXSceneGraph::TEXTURE, &pTexturePalette );

	if( IFXSUCCESS( result ) )
		result = AddTexturePaletteEntry( &rName, &textureId );

	// Point the texture palette entry to the texture object we created above.
	if( IFXSUCCESS( result ) )
		result = pTexturePalette->SetResourcePtr( textureId, pTextureObject );

	if( IFXSUCCESS( result ) )
		*ppTextureObject = pTextureObject; // releasing the object is left to the caller
	else
		IFXRELEASE( pTextureObject );

	return result;
}

//------------------------------------------------------------------
/**
Turn wireframe rendering of a shader on or off

@param     const char* pShaderName        The name of the shader to set into wireframe mode
@param     BOOL onOff                        Turn wireframe on or off.

@return    IFXRESULT         Return status of this method.
**/
//------------------------------------------------------------------
IFXRESULT SceneUtilities::SetRenderWire(
										const IFXString& rShaderName,
										BOOL onOff )
{
	IFXRESULT         result = IFX_OK;
	IFXPalette            *pShaderPalette = NULL;
	IFXShaderLitTexture *pMyShaderLT = NULL;
	U32                 myShaderID = 0;
	U32                 uChannels = 0;

	if( !m_bInit )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	if( IFXSUCCESS( result ) )
	{
		pShaderPalette = m_pShaderPalette;

		if( !pShaderPalette)
			result = IFX_E_NOT_INITIALIZED;
	}

	if( pShaderPalette && IFXSUCCESS( result ) )
	{
		result = pShaderPalette->Find( &rShaderName, &myShaderID);
	}

	if( pShaderPalette && IFXSUCCESS( result ) )
	{
		result = pShaderPalette->GetResourcePtr(
			myShaderID,
			IID_IFXShaderLitTexture,
			(void **)&pMyShaderLT );
	}

	if( pMyShaderLT && IFXSUCCESS( result ) )
	{
		uChannels = pMyShaderLT->GetFlags();

		if( onOff )
		{
			uChannels |= IFXShaderLitTexture::WIRE;
		}
		else
		{
			uChannels &= (0xffffffff^IFXShaderLitTexture::WIRE);
		}

		result = pMyShaderLT->SetFlags(uChannels);
	}

	// Free everything up.
	IFXRELEASE(pMyShaderLT);

	return result;
}

//****************************************************************
// Animation
//****************************************************************

//------------------------------------------------------------------
/**
Adds a motion resource to the motion resource palette and
returns the new motion resource.

@param     IFXString* rMotionName    name of the Motion resource to add.
@param     U32 motionQuality     The quality factor with which to save
the animation represented by this motion.    Larger values
correspond to higher quality with less compression.    Smaller
values result in higher compression but a mesh representation
of poorer quality.
@param     U32 motionPriority        The streaming priority for the data
making up this motion.
@param     U32* out_MotionID         ID of the motion resource created and added.
@param     IFXMotionResource** pMotionResource     The motion resource returned.

@return    IFXRESULT                 Return status of this method.
@note        pMotionResource must be released by the caller.
**/
//------------------------------------------------------------------

IFXRESULT SceneUtilities::CreateMotionResource(
	const IFXString& rMotionName,
	const U32 motionQuality,
	const U32 motionPriority,
	U32* pMotionID,
	IFXMotionResource** ppMotionResource )
{
	IFXRESULT result = IFX_OK;
	IFXMotionResource* pMotionResource = NULL;

	if( !m_bInit || !ppMotionResource || !pMotionID )
	{
		result = IFX_E_NOT_INITIALIZED;
		IFXASSERT(0);
	}

	// Create the IFXMotionResource component with an IFXMotionResource
	// interface
	if( IFXSUCCESS( result ) )
	{
		result = IFXCreateComponent( CID_IFXMotionResource, IID_IFXMotionResource,
			(void**)&pMotionResource );
	}

	// Initialize the component.
	if( pMotionResource && IFXSUCCESS( result ) )
		result = pMotionResource->SetSceneGraph( m_pSceneGraph );

	if( pMotionResource && IFXSUCCESS( result ) )
		pMotionResource->SetQualityFactorX( motionQuality );

	if( pMotionResource && IFXSUCCESS( result ) )
		pMotionResource->SetPriority( motionPriority );

	IFXDECLARELOCAL( IFXPalette, pMotionPalette );
	IFXCHECKX( m_pSceneGraph->GetPalette( IFXSceneGraph::MOTION, &pMotionPalette ));

	// Create an entry for the IFXMotionResource in the motion resource palette.
	if( IFXSUCCESS( result ) )
		result = pMotionPalette->Add( rMotionName, pMotionID );

	// Point the palette entry to the IFXMotionResource we created
	if( pMotionID && IFXSUCCESS( result ) )
		result = pMotionPalette->SetResourcePtr( *pMotionID, (IFXUnknown*)pMotionResource );

	// Now, add a fully-mapped mixer to the mixer palette for this motion
	if( m_pSceneGraph && IFXSUCCESS( result ) )
	{
		U32 mixerID = 0;
		IFXDECLARELOCAL( IFXPalette, pMixerPalette );

		// Get the mixer palette
		result = m_pSceneGraph->GetPalette( IFXSceneGraph::MIXER, &pMixerPalette );

		if( pMixerPalette && IFXSUCCESS( result ))
		{
			result = pMixerPalette->Find( rMotionName, &mixerID );

			// if not found, add it to the palette
			if( IFXFAILURE( result ) )
				result = pMixerPalette->Add( rMotionName, &mixerID );
		}

		// Now create the mixer resource (ruct) for this palette entry
		if( pMixerPalette && IFXSUCCESS( result ))
		{
			IFXDECLARELOCAL( IFXMixerConstruct, pMixerConstruct );
			result = pMixerPalette->GetResourcePtr(
				mixerID,
				IID_IFXMixerConstruct,
				(void**)&pMixerConstruct );

			// if there was no resource pointer, then create one
			if( IFXFAILURE( result ) )
			{
				result = IFXCreateComponent(
					CID_IFXMixerConstruct,
					IID_IFXMixerConstruct,
					(void**)&pMixerConstruct );

				if( pMixerConstruct && IFXSUCCESS( result ) )
				{
					result = pMixerConstruct->SetSceneGraph( m_pSceneGraph );

					// Link the mixer ruct to the motion resource
					if( IFXSUCCESS( result ))
						pMixerConstruct->SetMotionResource( pMotionResource );
				}

				// set the resource pointer in the palette (mixer to mixer construct).
				if( pMixerConstruct && IFXSUCCESS( result ) )
				{
					IFXDECLARELOCAL( IFXUnknown, pUnknown );
					result = pMixerConstruct->QueryInterface( IID_IFXUnknown, (void**)&pUnknown );

					if( IFXSUCCESS( result ) )
						result = pMixerPalette->SetResourcePtr( mixerID, pUnknown );
				}
			}
		}
	}

	// Pass back the fully hooked-up motion resource.
	if( pMotionResource && IFXSUCCESS( result ) )
		*ppMotionResource = pMotionResource; // pMotionResource is released by the caller

	return result;
}


//***************************************************************************
//    Protected methods
//***************************************************************************


//***************************************************************************
//    Private methods
//***************************************************************************


//***************************************************************************
//    Global functions
//***************************************************************************


//***************************************************************************
//    Local functions
//***************************************************************************
