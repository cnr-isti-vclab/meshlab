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
@file ResourceConverter.cpp

This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ResourceConverter.h"
#include "MetaDataConverter.h"
#include "SceneResources.h"
#include "SceneUtilities.h"
#include "ConverterOptions.h"

#include "MeshConverter.h"
#include "LineSetConverter.h"
#include "PointSetConverter.h"
#include "TextureConverter.h"

#include "IFXMotionResource.h"
#include "IFXViewResource.h"
#include "IFXLightResource.h"
#include "IFXShaderLitTexture.h"

#include "IFXMotionResource.h"
#include "IFXString.h"
#include "IFXAuthor.h"
#include "IFXCheckX.h"

#include "Tokens.h"

#ifndef LIBIDTF
extern FILE *stdmsg;
#endif

using namespace U3D_IDTF;

//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************


//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************


//***************************************************************************
//  Local data
//***************************************************************************


//***************************************************************************
//  Local function prototypes
//***************************************************************************


//***************************************************************************
//  Public methods
//***************************************************************************

ResourceConverter::ResourceConverter( 
									 SceneResources* pSceneResources,
									 SceneUtilities* pSceneUtils,
									 ConverterOptions* pOptions )
: m_pResources( pSceneResources ), 
  m_pSceneUtils( pSceneUtils ),
  m_pOptions( pOptions )
{
	IFXCHECKX_RESULT( NULL != pSceneResources, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pSceneUtils, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pOptions, IFX_E_INVALID_POINTER );
}

ResourceConverter::~ResourceConverter()
{
	m_pResources = NULL;
	m_pSceneUtils = NULL;
	m_pOptions = NULL;
}

IFXRESULT ResourceConverter::Convert()
{
	IFXRESULT result = IFX_OK;

	try
	{
#ifndef LIBIDTF
		fprintf(stdmsg,"Converting\n");
#else
		IFXTRACE_GENERIC( L"[Converter] Converting\n");
#endif
		IFXCHECKX( ConvertViewResources() );
		IFXCHECKX( ConvertLightResources() );
		IFXCHECKX( ConvertMotionResources() );
		IFXCHECKX( ConvertTextureResources() );
		IFXCHECKX( ConvertMaterialResources() );
		IFXCHECKX( ConvertShaderResources() );
		IFXCHECKX( ConvertModelResources() );
	}
	catch( IFXException& rException )
	{
		result = rException.GetIFXResult();
	}
	catch( ... )
	{
		IFXTRACE_GENERIC( L"[Converter] Received an unknown exception\n" );
		result = IFX_E_UNDEFINED;
	}

	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************


//***************************************************************************
//  Private methods
//***************************************************************************


IFXRESULT ResourceConverter::ConvertMotionResources()
{
	IFXRESULT result = IFX_OK;
	const MotionResourceList& rResourceList = 
		m_pResources->GetMotionResourceList();
	const U32 resourceCount = rResourceList.GetResourceCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != resourceCount )
#ifndef LIBIDTF
		fprintf(stdmsg,"Motion Resources (%d)\t\t", resourceCount);
#else
	{
		dbgMessage.ToString(resourceCount,10);
		dbgMessage = IFXString(L"[Converter] Motion Resources (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t\t";
	}
#endif

	U32 i;
	for( i = 0; i < resourceCount && IFXSUCCESS( result ); ++i )
	{
		U32 motionId = 0;

		const MotionResource& rIDTFResource = rResourceList.GetResource( i );
		IFXDECLARELOCAL( IFXMotionResource, pMotionResource );

		result = m_pSceneUtils->CreateMotionResource(
			rIDTFResource.GetName(),
			m_pOptions->animQuality, 0,
			&motionId,
			&pMotionResource );

		U32 j;
		for( j = 0;
			j < rIDTFResource.GetMotionTrackCount() &&
			IFXSUCCESS( result ) &&
			NULL != pMotionResource;
		++j )
		{
			U32 trackId = 0;
			const MotionTrack& rIDTFTrack = rIDTFResource.GetMotionTrack( j );

			if( IFXSUCCESS( result ) )
				result = pMotionResource->AddTrack( 
									(IFXString*)&rIDTFTrack.m_name, &trackId );

			// Load up the motion with the keyframe data
			if( IFXSUCCESS( result ) )
				result = ConvertKeyFrames( 
									trackId, pMotionResource, rIDTFTrack );
		}

		// convert motion resource meta-data
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		if( pMotionResource && IFXSUCCESS( result ) )
			result = pMotionResource->QueryInterface( 
										IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( &rIDTFResource, pMetaData );
			metaDataConverter.Convert();
		}

#ifndef LIBIDTF
		fprintf(stdmsg,"|");
#else
		dbgMessage+= L"|";
#endif
	}

	if( 0 != resourceCount )
	{
#ifndef LIBIDTF
		if( IFXSUCCESS( result ) )
			fprintf(stdmsg,"\tDone\n");
		else
			fprintf(stdmsg,"\tFailed\n");
#else
		if( IFXSUCCESS( result ) )
			dbgMessage+= L"\tDone\n";
		else
			dbgMessage+= L"\tFailed\n";
		IFXTRACE_GENERIC( dbgMessage.Raw() );
#endif
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT ResourceConverter::ConvertTextureResources()
{
	IFXRESULT result = IFX_OK;

	const TextureResourceList& rResourceList = 
		m_pResources->GetTextureResourceList();
	const U32 resourceCount = rResourceList.GetResourceCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != resourceCount )
#ifndef LIBIDTF
		fprintf(stdmsg,"Texture Resources (%d)\t\t", resourceCount);
#else
	{
		dbgMessage.ToString(resourceCount,10);
		dbgMessage = IFXString(L"[Converter] Texture Resources (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t\t";
	}
#endif

	U32 i;
	for( i = 0; i < resourceCount && IFXSUCCESS( result ); ++i )
	{
		TextureConverter converter( 
								&rResourceList.GetResource( i ), 
								m_pSceneUtils );

		converter.SetQuality( 
						m_pOptions->textureQuality,
						m_pOptions->textureLimit );

		result = converter.Convert();
#ifndef LIBIDTF
		fprintf(stdmsg,"|");
#else
		dbgMessage+= L"|";
#endif
	}

	if( 0 != resourceCount )
	{
#ifndef LIBIDTF
		if( IFXSUCCESS( result ) )
			fprintf(stdmsg,"\tDone\n");
		else
			fprintf(stdmsg,"\tFailed\n");
#else
		if( IFXSUCCESS( result ) )
			dbgMessage+= L"\tDone\n";
		else
			dbgMessage+= L"\tFailed\n";
		IFXTRACE_GENERIC( dbgMessage.Raw() );
#endif
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT ResourceConverter::ConvertMaterialResources()
{
	IFXRESULT result = IFX_OK;

	const MaterialResourceList& rResourceList = 
		m_pResources->GetMaterialResourceList();
	const U32 resourceCount = rResourceList.GetResourceCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != resourceCount )
#ifndef LIBIDTF
		fprintf(stdmsg,"Material Resources (%d)\t\t", resourceCount);
#else
	{
		dbgMessage.ToString(resourceCount,10);
		dbgMessage = IFXString(L"[Converter] Material Resources (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t";
	}
#endif

	U32 i;
	for( i = 0; i < resourceCount && IFXSUCCESS( result ); ++i )
	{
		result = ConvertMaterial( rResourceList.GetResource( i ) );

#ifndef LIBIDTF
		fprintf(stdmsg,"|");
#else
		dbgMessage+= L"|";
#endif
	}

	if( 0 != resourceCount )
	{
#ifndef LIBIDTF
		if( IFXSUCCESS( result ) )
			fprintf(stdmsg,"\tDone\n");
		else
			fprintf(stdmsg,"\tFailed\n");
#else
		if( IFXSUCCESS( result ) )
			dbgMessage+= L"\tDone\n";
		else
			dbgMessage+= L"\tFailed\n";
		IFXTRACE_GENERIC( dbgMessage.Raw() );
#endif
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT ResourceConverter::ConvertShaderResources()
{
	IFXRESULT result = IFX_OK;
	const ShaderResourceList& rResourceList = 
		m_pResources->GetShaderResourceList();
	const U32 resourceCount = rResourceList.GetResourceCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != resourceCount )
#ifndef LIBIDTF
		fprintf(stdmsg,"Shader Resources (%d)\t\t", resourceCount);
#else
	{
		dbgMessage.ToString(resourceCount,10);
		dbgMessage = IFXString(L"[Converter] Shader Resources (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t";
	}
#endif

	U32 i;
	for( i = 0; i < resourceCount && IFXSUCCESS( result ); ++i )
	{
		result = ConvertShader( rResourceList.GetResource( i ) );

#ifndef LIBIDTF
		fprintf(stdmsg,"|");
#else
		dbgMessage+= L"|";
#endif
	}

	if( 0 != resourceCount )
	{
#ifndef LIBIDTF
		if( IFXSUCCESS( result ) )
			fprintf(stdmsg,"\tDone\n");
		else
			fprintf(stdmsg,"\tFailed\n");
#else
		if( IFXSUCCESS( result ) )
			dbgMessage+= L"\tDone\n";
		else
			dbgMessage+= L"\tFailed\n";
		IFXTRACE_GENERIC( dbgMessage.Raw() );
#endif
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT ResourceConverter::ConvertModelResources()
{
	IFXRESULT result = IFX_OK;
	const ModelResourceList& rResourceList = 
		m_pResources->GetModelResourceList();
	const U32 resourceCount = rResourceList.GetResourceCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != resourceCount )
#ifndef LIBIDTF
		fprintf(stdmsg,"Model Resources (%d)\t\t", resourceCount);
#else
	{
		dbgMessage.ToString(resourceCount,10);
		dbgMessage = IFXString(L"[Converter] Model Resources (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t";
	}
#endif

	U32 i;
	for( i = 0; i < resourceCount && IFXSUCCESS( result ); ++i )
	{
		const ModelResource* pIDTFResource = rResourceList.GetResource( i );

		if( pIDTFResource->m_type == IDTF_MESH )
		{
			const MeshResource* pMeshResource = 
				static_cast<const MeshResource*>(pIDTFResource);
			MeshConverter converter( pMeshResource, m_pSceneUtils );

			converter.SetDefaultQuality( m_pOptions->geoQuality );
			converter.SetPositionQuality( m_pOptions->positionQuality );
			converter.SetTexCoordQuality( m_pOptions->texCoordQuality );
			converter.SetNormalQuality( m_pOptions->normalQuality );
			converter.SetDiffuseColorQuality( m_pOptions->diffuseQuality );
			converter.SetSpecularColorQuality( m_pOptions->specularQuality );
			converter.SetZeroAreaFacesRemoval( m_pOptions->removeZeroAreaFaces );
			converter.SetZeroAreaFaceTolerance( m_pOptions->zeroAreaFaceTolerance );
			converter.SetNormalsExclusion( m_pOptions->excludeNormals );

			result = converter.Convert();
		}
		else if( pIDTFResource->m_type == IDTF_LINE_SET )
		{
			const LineSetResource* pLineSetResource = 
				static_cast<const LineSetResource*>(pIDTFResource);
			LineSetConverter converter( pLineSetResource, m_pSceneUtils );
			result = converter.Convert();
		}
		else if( pIDTFResource->m_type == IDTF_POINT_SET )
		{
			const PointSetResource* pPointSetResource = 
				static_cast<const PointSetResource*>(pIDTFResource);
			PointSetConverter converter( pPointSetResource, m_pSceneUtils );
			result = converter.Convert();
		}
		else
			result = IFX_E_UNDEFINED;

#ifndef LIBIDTF
		fprintf(stdmsg,"|");
#else
		dbgMessage+= L"|";
#endif
	}

	if( 0 != resourceCount )
	{
#ifndef LIBIDTF
		if( IFXSUCCESS( result ) )
			fprintf(stdmsg,"\tDone\n");
		else
			fprintf(stdmsg,"\tFailed\n");
#else
		if( IFXSUCCESS( result ) )
			dbgMessage+= L"\tDone\n";
		else
			dbgMessage+= L"\tFailed\n";
		IFXTRACE_GENERIC( dbgMessage.Raw() );
#endif
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT ResourceConverter::ConvertLightResources()
{
	IFXRESULT result = IFX_OK;
	const LightResourceList& rResourceList = 
		m_pResources->GetLightResourceList();
	const U32 resourceCount = rResourceList.GetResourceCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != resourceCount )
#ifndef LIBIDTF
		fprintf(stdmsg,"Light Resources (%d)\t\t", resourceCount);
#else
	{
		dbgMessage.ToString(resourceCount,10);
		dbgMessage = IFXString(L"[Converter] Light Resources (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t";
	}
#endif

	U32 i;
	for( i = 0; i < resourceCount && IFXSUCCESS( result ); ++i )
	{
		const LightResource& rIDTFResource = rResourceList.GetResource( i );
		IFXDECLARELOCAL( IFXLightResource, pIFXResource );

		result = m_pSceneUtils->CreateLightResource(
			rIDTFResource.GetName(),
			&pIFXResource );

		// Convert Light Resource parameters

		if( rIDTFResource.m_type == IDTF_DIRECTIONAL_LIGHT )
		{
			// Directional light
			pIFXResource->SetType( IFXLightResource::DIRECTIONAL );
		}
		else if( rIDTFResource.m_type == IDTF_POINT_LIGHT )
		{
			// Point (omnidirectional) light
			pIFXResource->SetType( IFXLightResource::POINT );
		}
		else if( rIDTFResource.m_type == IDTF_SPOT_LIGHT )
		{
			// Spotlight
			pIFXResource->SetType( IFXLightResource::SPOT );
		}
		else if( rIDTFResource.m_type == IDTF_AMBIENT_LIGHT )
		{
			// Ambient light
			pIFXResource->SetType( IFXLightResource::AMBIENT );
		}
		else
		{
			// It's really bad if we don't find a light that we recognize...
			IFXASSERT(0);
		}

		// convert the light color
		if( IFXSUCCESS(result) )
		{
			const IFXVector4& color = rIDTFResource.m_color.GetColor();
			pIFXResource->SetColor( color );
		}

		// -- Read the light attenuation vector, which specifies how the light
		//    intensity changes as a function of distance.  The vector elements
		//    are constant, linear, and quadratic. The lighting function used
		//    is 1 / (c + d*l + d*d*q), which is the attenuation model used by
		//    OpenGL and DirectX.  Note that this probably will not map to the
		//    more advanced lighting model found in most 3D authoring tools - you
		//    will need to translate it as best you can.
		if( IFXSUCCESS(result) )
		{
			const IFXVector3& point = rIDTFResource.m_attenuation.GetPoint();
			result = pIFXResource->SetAttenuation( (F32*)point.RawConst() );
		}

		// convert the Spot angle, if we have a spot light
		if( IFXSUCCESS(result) && rIDTFResource.m_type == IDTF_SPOT_LIGHT )
		{
			pIFXResource->SetSpotAngle( rIDTFResource.m_spotAngle );
		}

		// convert light resource meta-data
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		if( IFXSUCCESS( result ) )
			result = pIFXResource->QueryInterface( 
									IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( &rIDTFResource, pMetaData );
			metaDataConverter.Convert();
		}

#ifndef LIBIDTF
		fprintf(stdmsg,"|");
#else
		dbgMessage+= L"|";
#endif
	}

	if( 0 != resourceCount )
	{
#ifndef LIBIDTF
		if( IFXSUCCESS( result ) )
			fprintf(stdmsg,"\tDone\n");
		else
			fprintf(stdmsg,"\tFailed\n");
#else
		if( IFXSUCCESS( result ) )
			dbgMessage+= L"\tDone\n";
		else
			dbgMessage+= L"\tFailed\n";
		IFXTRACE_GENERIC( dbgMessage.Raw() );
#endif
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT ResourceConverter::ConvertViewResources()
{
	IFXRESULT result = IFX_OK;
	const ViewResourceList& rResourceList = 
			m_pResources->GetViewResourceList();

	const U32 resourceCount = rResourceList.GetResourceCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != resourceCount )
#ifndef LIBIDTF
		fprintf(stdmsg,"View Resources (%d)\t\t", resourceCount);
#else
	{
		dbgMessage.ToString(resourceCount,10);
		dbgMessage = IFXString(L"[Converter] View Resources (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t";
	}
#endif

	U32 i;
	for( i = 0; i < resourceCount && IFXSUCCESS( result ); ++i )
	{
		const ViewResource& rIDTFResource = rResourceList.GetResource( i );
		IFXDECLARELOCAL( IFXViewResource, pIFXResource );

		result = m_pSceneUtils->CreateViewResource(
									rIDTFResource.GetName(),
									&pIFXResource );

		// Point the view to this resource. This isn't necessarily the view node's
		// parent. Rather, it indicates what subsection of the scenegraph should
		// be visualized by this camera/view. You could point a custom camera's
		// "RootNode" at any node in the scenegraph. The camera will only see those
		// nodes at or below the node in the parent/child hierarchy.
		if( IFXSUCCESS( result ) )
		{
			const U32& rPassCount = rIDTFResource.GetRootNodeCount();
			result = pIFXResource->SetNumRenderPasses( rPassCount );

			U32 j;
			for( j = 0; j < rPassCount && IFXSUCCESS( result ); j++ ) 
			{
				IFXDECLARELOCAL( IFXNode, pRootNode );
				const IFXString& rRootNodeName = rIDTFResource.GetRootNode( j );
				U32 nodeIndex = 0;

				result = pIFXResource->SetCurrentRenderPass( j );

				if( IFXSUCCESS( result ) )
					result = m_pSceneUtils->FindNode( 
									rRootNodeName, &pRootNode, &nodeIndex );

				if( result == IFX_E_CANNOT_FIND )
				{
					// root node isn't in the palette, so create the node
					// placeholder. pointer to the node's resource 
					// will be set later during node conversion
					result = m_pSceneUtils->CreateNodePlaceholder( 
												rRootNodeName, 
												&nodeIndex );
				}

				if( IFXSUCCESS( result ) )
					result = pIFXResource->SetRootNode( nodeIndex, 0 );
			}
		}

		// Convert view resource meta-data
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		if( IFXSUCCESS( result ) )
			result = pIFXResource->QueryInterface( 
										IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( &rIDTFResource, pMetaData );
			metaDataConverter.Convert();
		}

#ifndef LIBIDTF
		fprintf(stdmsg,"|");
#else
		dbgMessage+= L"|";
#endif
	}

	if( 0 != resourceCount )
	{
#ifndef LIBIDTF
		if( IFXSUCCESS( result ) )
			fprintf(stdmsg,"\tDone\n");
		else
			fprintf(stdmsg,"\tFailed\n");
#else
		if( IFXSUCCESS( result ) )
			dbgMessage+= L"\tDone\n";
		else
			dbgMessage+= L"\tFailed\n";
		IFXTRACE_GENERIC( dbgMessage.Raw() );
#endif
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT ResourceConverter::ConvertKeyFrames(
	U32 trackId,
	IFXMotionResource* pMotion,
	const MotionTrack& rIDTFTrack )
{
	IFXRESULT result = IFX_OK;
	U32 count = 0;
	U32 i = 0;
	IFXKeyFrame* pKeys = NULL;

	if( NULL == pMotion )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	if( IFXSUCCESS( result ) )
		count = rIDTFTrack.GetKeyFrameCount();

	// Allocate an array for the keyframe date
	if(  IFXSUCCESS( result ) )
	{
		pKeys = new IFXKeyFrame[count];
		if( NULL == pKeys )
		{
			result = IFX_E_OUT_OF_MEMORY;
		}
	}

	if( pKeys && IFXSUCCESS( result ) )
	{
		// Load all the keyframes
		for( i = 0; i < count && IFXSUCCESS( result ); ++i )
		{
			const KeyFrame& rIDTFKeyFrame = rIDTFTrack.GetKeyFrame( i );

			// Get the time for the current key
			pKeys[i].Time() = rIDTFKeyFrame.m_time;

			// And the position of the bone at the current time
			pKeys[i].Location() = rIDTFKeyFrame.m_displacement.GetPoint();

			// The rotation of the bone at the current time
			pKeys[i].Rotation() = rIDTFKeyFrame.m_rotation.GetQuat();

			// The scale of the bone at the current time.
			pKeys[i].Scale() = rIDTFKeyFrame.m_scale.GetPoint();
		}
	}

	// Now that we have loaded the keyframe data, insert it into the IFXMotionResource
	if( pMotion && IFXSUCCESS( result ) )
		result = pMotion->InsertKeyFrames( trackId, count, pKeys );

	if( NULL != pKeys )
		IFXDELETE_ARRAY( pKeys );

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}


IFXRESULT ResourceConverter::ConvertMaterial(
	const Material& rIDTFMaterial )
{
	IFXRESULT result = IFX_OK;
	U32 attributes = 0;
	IFXDECLARELOCAL( IFXMaterialResource, pMaterial );

	// Create an IFXShaderLitTexture for this shader.
	if( m_pSceneUtils && IFXSUCCESS( result ) )
	{
		result = m_pSceneUtils->CreateMaterial(
			rIDTFMaterial.GetName(),
			&pMaterial );
	}

	// set material attributes
	if( IFXSUCCESS( result ) )
	{
		if( rIDTFMaterial.m_ambientEnabled == L"TRUE" )
			attributes |= IFXMaterialResource::AMBIENT;

		if( rIDTFMaterial.m_diffuseEnabled == L"TRUE" )
			attributes |= IFXMaterialResource::DIFFUSE;

		if( rIDTFMaterial.m_specularEnabled == L"TRUE" )
			attributes |= IFXMaterialResource::SPECULAR;

		if( rIDTFMaterial.m_emissiveEnabled == L"TRUE" )
			attributes |= IFXMaterialResource::EMISSIVE;

		if( rIDTFMaterial.m_opacityEnabled == L"TRUE" )
			attributes |= IFXMaterialResource::OPACITY;

		if( rIDTFMaterial.m_reflectivityEnabled == L"TRUE" )
			attributes |= IFXMaterialResource::REFLECTIVITY;

		pMaterial->SetAttributes( attributes );
	}

	// set material properties
	if( IFXSUCCESS( result ) )
	{
		// Store the ambient color in the material hanging off the shader.
		if( IFXSUCCESS( result ) )
			result = pMaterial->SetAmbient( rIDTFMaterial.m_ambient.GetColor() );

		// Store the diffuse color in the material hanging off the shader.
		if( IFXSUCCESS( result ) )
			result = pMaterial->SetDiffuse( rIDTFMaterial.m_diffuse.GetColor() );

		// Store the specular color in the material hanging off the shader.
		if( IFXSUCCESS( result ) )
			result = pMaterial->SetSpecular( rIDTFMaterial.m_specular.GetColor() );

		// Store self illumination value (emissive) in the material
		if( IFXSUCCESS( result ) )
			result = pMaterial->SetEmission( rIDTFMaterial.m_emissive.GetColor() );

		// Store the opacity value in the material hanging off the shader
		if( IFXSUCCESS( result ) )
			result = pMaterial->SetOpacity( rIDTFMaterial.m_opacity );

		// Store it in the material hanging off the shader
		if( IFXSUCCESS( result ) )
			result = pMaterial->SetReflectivity( rIDTFMaterial.m_reflectivity );
	}

	// convert material meta-data
	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		result = pMaterial->QueryInterface( IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( &rIDTFMaterial, pMaterial );
			metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}

IFXRESULT ResourceConverter::ConvertShader(
	const Shader& rIDTFShader )
{
	IFXRESULT result = IFX_OK;
	const IFXString& shaderName = rIDTFShader.GetName();
	IFXDECLARELOCAL( IFXShaderLitTexture, pShader );

	// Create an IFXShaderLitTexture for this shader.
	if( m_pSceneUtils && IFXSUCCESS( result ) )
	{
		result = m_pSceneUtils->CreateShader(
			shaderName,
			rIDTFShader.m_materialName,
			&pShader );
	}

	// set lit texture shader parameters
	if( IFXSUCCESS( result ) )
	{
		// set lighting enabled
		if( rIDTFShader.m_lightingEnabled == L"TRUE" )
			result = pShader->SetLightingEnabled( TRUE );
		else
			result = pShader->SetLightingEnabled( FALSE );

		if( IFXSUCCESS( result ) )
		{
			// set alpha test enabled
			if( rIDTFShader.m_alphaTestEnabled == L"TRUE" )
				result = pShader->SetAlphaTestEnabled( TRUE );
			else
				result = pShader->SetAlphaTestEnabled( FALSE );
		}

		IFXRenderMaterial& rRenderMaterial = pShader->GetRenderMaterial();

		if( IFXSUCCESS( result ) )
		{
			// set use vertex color
			if( rIDTFShader.m_useVertexColor == L"TRUE" )
				result = rRenderMaterial.SetUseVertexColors( TRUE );
			else
				result = rRenderMaterial.SetUseVertexColors( FALSE );
		}

		IFXRenderBlend& rRenderBlend = pShader->GetRenderBlend();

		if( IFXSUCCESS( result ) )
		{
			// set alpha test function
			if( rIDTFShader.m_alphaTestFunction == DEFAULT_COLOR_ALPHA_TEST_FUNCTION )
				result = rRenderBlend.SetTestFunc( IFX_ALWAYS );
			else if( rIDTFShader.m_alphaTestFunction == L"NEVER" )
				result = rRenderBlend.SetTestFunc( IFX_NEVER );
			else if( rIDTFShader.m_alphaTestFunction == L"LESS" )
				result = rRenderBlend.SetTestFunc( IFX_LESS );
			else if( rIDTFShader.m_alphaTestFunction == L"GREATER" )
				result = rRenderBlend.SetTestFunc( IFX_GREATER );
			else if( rIDTFShader.m_alphaTestFunction == L"EQUAL" )
				result = rRenderBlend.SetTestFunc( IFX_EQUAL );
			else if( rIDTFShader.m_alphaTestFunction == L"NOT_EQUAL" )
				result = rRenderBlend.SetTestFunc( IFX_NOT_EQUAL );
			else if( rIDTFShader.m_alphaTestFunction == L"LEQUAL" )
				result = rRenderBlend.SetTestFunc( IFX_LEQUAL );
			else if( rIDTFShader.m_alphaTestFunction == L"GEQUAL" )
				result = rRenderBlend.SetTestFunc( IFX_GEQUAL );
			else
				result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS( result ) )
		{
			//set color blend function
			if( rIDTFShader.m_colorBlendFunction == DEFAULT_COLOR_BLEND_FUNCTION )
				result = rRenderBlend.SetBlendFunc( IFX_FB_ALPHA_BLEND );
			else if( rIDTFShader.m_colorBlendFunction == L"ADD" )
				result = rRenderBlend.SetBlendFunc( IFX_FB_ADD );
			else if( rIDTFShader.m_colorBlendFunction == L"MULTIPLY" )
				result = rRenderBlend.SetBlendFunc( IFX_FB_MULT );
			else if( rIDTFShader.m_colorBlendFunction == L"INV_ALPHA_BLEND" )
				result = rRenderBlend.SetBlendFunc( IFX_FB_INV_ALPHA_BLEND );
			else
				result = IFX_E_UNSUPPORTED;
		}

		if( IFXSUCCESS( result ) )
		{
			// set alpha test reference
			result = rRenderBlend.SetReference( rIDTFShader.m_alphaTestReference );
		}
	}

	if( pShader )
	{
		// Now convert all active texture layer used by the shader
		U32 i;
		for( i = 0; i < rIDTFShader.GetTextureLayerCount()
			&& IFXSUCCESS( result );
			++i )
		{
			result = ConvertTextureLayer( rIDTFShader.GetTextureLayer(i), pShader );
		}
	}

	// convert shader resource meta-data
	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		result = pShader->QueryInterface( IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( &rIDTFShader, pShader );
			metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}


IFXRESULT ResourceConverter::ConvertTextureLayer(
	const TextureLayer& rIDTFTextureLayer,
	IFXShaderLitTexture* pShader )
{
	IFXRESULT result = IFX_OK;
	U32 channel = rIDTFTextureLayer.m_channel;
	IFXShaderLitTexture::TextureMode textureMode = IFXShaderLitTexture::TM_NONE;
	IFXShaderLitTexture::BlendFunction blendFunction = IFXShaderLitTexture::BLEND;
	IFXShaderLitTexture::BlendSource blendSource = IFXShaderLitTexture::CONSTANT;
	enum RepeatMode                                                                                                               
	{                                                                                                                        
		REPEAT_NONE = 0,
		REPEAT_U    = 1,
		REPEAT_V    = 2,
		REPEAT_UV   = 3
	} repeatMode;                                                                                                                       

	if( !pShader )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT(  IFXSUCCESS( result ) );
	}

	if( rIDTFTextureLayer.m_mode == DEFAULT_TEXTURE_MODE )
		textureMode = IFXShaderLitTexture::TM_NONE;
	else if( rIDTFTextureLayer.m_mode == L"TM_PLANAR" )
		textureMode = IFXShaderLitTexture::TM_PLANAR;
	else if( rIDTFTextureLayer.m_mode == L"TM_CYLINDRICAL" )
		textureMode = IFXShaderLitTexture::TM_CYLINDRICAL;
	else if( rIDTFTextureLayer.m_mode == L"TM_SPHERICAL" )
		textureMode = IFXShaderLitTexture::TM_SPHERICAL;
	else if( rIDTFTextureLayer.m_mode == L"TM_REFLECTION" )
		textureMode = IFXShaderLitTexture::TM_REFLECTION;
	else
		result = IFX_E_UNSUPPORTED;

	IFXASSERT( IFXSUCCESS( result ) );

	if( rIDTFTextureLayer.m_blendFunction == DEFAULT_BLEND_FUNCTION )
		blendFunction = IFXShaderLitTexture::MULTIPLY;
	else if( rIDTFTextureLayer.m_blendFunction ==  L"ADD" )
		blendFunction = IFXShaderLitTexture::ADD;
	else if( rIDTFTextureLayer.m_blendFunction == L"REPLACE" )
		blendFunction = IFXShaderLitTexture::REPLACE;
	else if( rIDTFTextureLayer.m_blendFunction == L"BLEND" )
		blendFunction = IFXShaderLitTexture::BLEND;
	else
		result = IFX_E_UNSUPPORTED;

	IFXASSERT( IFXSUCCESS( result ) );

	if( rIDTFTextureLayer.m_blendSource == DEFAULT_BLEND_SOURCE )
		blendSource = IFXShaderLitTexture::CONSTANT;
	else if( rIDTFTextureLayer.m_blendSource == L"ALPHA" )
		blendSource = IFXShaderLitTexture::ALPHA;
	else
		result = IFX_E_UNSUPPORTED;

	IFXASSERT( IFXSUCCESS( result ) );

	if( rIDTFTextureLayer.m_repeat == L"UV" || rIDTFTextureLayer.m_repeat == L"VU" )
		repeatMode = REPEAT_UV;
	else if( rIDTFTextureLayer.m_repeat == L"U" )
		repeatMode = REPEAT_U;
	else if( rIDTFTextureLayer.m_repeat == L"V" )
		repeatMode = REPEAT_V;
	else if( rIDTFTextureLayer.m_repeat == L"NONE" )
		repeatMode = REPEAT_NONE;
	else
		result = IFX_E_UNSUPPORTED;

	IFXASSERT( IFXSUCCESS( result ) );

	// set texture mode
	if( IFXSUCCESS( result ) )
		result = pShader->SetTextureMode(
		channel, textureMode );

	// set blend function
	if( IFXSUCCESS( result ))
		result = pShader->SetBlendFunction(
		channel, blendFunction );

	// set blend source
	if( IFXSUCCESS( result ))
		result = pShader->SetBlendSource(
		channel, blendSource );

	// set blend constant
	if( IFXSUCCESS( result ))
		result = pShader->SetBlendConstant( channel, rIDTFTextureLayer.m_blendConstant );

	// set texture intensity
	if( IFXSUCCESS( result ))
		result = pShader->SetTextureIntensity( channel, rIDTFTextureLayer.m_intensity );

	// set texture repeat
	if( IFXSUCCESS( result ))
		result = pShader->SetTextureRepeat( channel, repeatMode );

	// enable this texture channel
	if( IFXSUCCESS( result ) )
		result = pShader->SetChannels(
		pShader->GetChannels() | (1<<channel) );

	// enable alpha for this texture channel
	if( rIDTFTextureLayer.m_alphaEnabled == L"TRUE" && IFXSUCCESS( result ) )
		result = pShader->SetAlphaTextureChannels(
		pShader->GetAlphaTextureChannels() | (1<<channel) );

	// Assign the texture to the desired texture layer.
	if( m_pSceneUtils && IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXTextureObject, pTextureObject );
		U32 texId = 0;

		result = m_pSceneUtils->FindTexture(
			rIDTFTextureLayer.m_textureName,
			&pTextureObject,
			&texId );

		if( IFXSUCCESS( result ) )
			result = pShader->SetTextureID( channel, texId );
	}

	IFXASSERT( IFXSUCCESS( result ) );
	return result;
}


//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
