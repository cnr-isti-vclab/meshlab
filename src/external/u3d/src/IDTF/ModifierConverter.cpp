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
  @file ModifierConverter.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "ModifierConverter.h"
#include "MetaDataConverter.h"
#include "SceneUtilities.h"
#include "Tokens.h"

#include "ShadingModifier.h"
#include "AnimationModifier.h"
#include "BoneWeightModifier.h"
#include "CLODModifier.h"
#include "SubdivisionModifier.h"
#include "GlyphModifier.h"

#include "IFXBoneWeightsModifier.h"
#include "IFXVertexMap.h"
#include "IFXString.h"

#include "ModifierList.h"

#include "IFXCheckX.h"

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

ModifierConverter::ModifierConverter( ModifierList* pModifierList, 
									  SceneUtilities* pSceneUtils )
: m_pModifierList( pModifierList ), m_pSceneUtils( pSceneUtils )
{
	IFXCHECKX_RESULT( NULL != pModifierList, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pSceneUtils, IFX_E_INVALID_POINTER );
}

ModifierConverter::~ModifierConverter()
{
}

IFXRESULT ModifierConverter::Convert()
{
	IFXRESULT result = IFX_OK;

	const U32 count = m_pModifierList->GetModifierCount();
#ifdef LIBIDTF
	IFXString dbgMessage;
#endif

	if( 0 != count )
#ifndef LIBIDTF
		fprintf(stdmsg,"Modifiers (%d)\t\t\t", count);
#else
	{
		dbgMessage.ToString(count,10);
		dbgMessage = IFXString(L"[Converter] Modifiers (") + dbgMessage;
		dbgMessage = dbgMessage + L")\t\t";
	}
#endif

	U32 index;
	for( index = 0; index < count && IFXSUCCESS( result ); ++index )
	{
		const Modifier* pModifier = m_pModifierList->GetModifier( index );

		if( NULL != pModifier )
		{
			const IFXString& type = pModifier->GetType();

			if( type == IDTF_SHADING_MODIFIER )
			{
				result = ConvertShadingModifier( 
							static_cast<const ShadingModifier*>( pModifier ) );
#ifndef LIBIDTF
				fprintf(stdmsg,"|");
#else
				dbgMessage+= L"|";
#endif
			}
			else if( type == IDTF_BONE_WEIGHT_MODIFIER )
			{
				result = ConvertBoneWeightModifier( 
							static_cast<const BoneWeightModifier*>( pModifier ) );
#ifndef LIBIDTF
				fprintf(stdmsg,"|");
#else
				dbgMessage+= L"|";
#endif
			}
			else if( type == IDTF_CLOD_MODIFIER )
			{
				result = ConvertCLODModifier( 
							static_cast<const CLODModifier*>( pModifier ) );
#ifndef LIBIDTF
				fprintf(stdmsg,"|");
#else
				dbgMessage+= L"|";
#endif
			}
			else if( type == IDTF_SUBDIVISION_MODIFIER )
			{
				result = ConvertSubdivisionModifier( 
							static_cast<const SubdivisionModifier*>( pModifier ) );
#ifndef LIBIDTF
				fprintf(stdmsg,"|");
#else
				dbgMessage+= L"|";
#endif
			}
			else if( type == IDTF_GLYPH_MODIFIER )
			{
				result = ConvertGlyphModifier( 
							static_cast<const GlyphModifier*>( pModifier ) );
#ifndef LIBIDTF
				fprintf(stdmsg,"|");
#else
				dbgMessage+= L"|";
#endif
			}
			else if( type == IDTF_ANIMATION_MODIFIER )
			{
				// do nothing here
			}
			else
				result = IFX_E_UNSUPPORTED;
		}
		else
			result = IFX_E_INVALID_POINTER;
	}

	// we should convert animation modifiers after bone weight modifiers
	// because they can use bone weights data from bone weight modifiers
	for( index = 0; index < count && IFXSUCCESS( result ); ++index )
	{
		const Modifier* pModifier = m_pModifierList->GetModifier( index );

		if( NULL != pModifier )
		{
			const IFXString& type = pModifier->GetType();

			if( type == IDTF_ANIMATION_MODIFIER )
			{
				result = ConvertAnimationModifier( 
							static_cast<const AnimationModifier*>( pModifier ) );
#ifndef LIBIDTF
				fprintf(stdmsg,"|");
#else
				dbgMessage+= L"|";
#endif
			}
		}
		else
			result = IFX_E_INVALID_POINTER;
	}

	if( 0 != count )
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

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT ModifierConverter::ConvertShadingModifier( 
						const ShadingModifier* pIDTFModifier )
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL( IFXShadingModifier, pModifier );

	const IFXString& rModName = pIDTFModifier->GetName();
	const IFXString& rModChainType = pIDTFModifier->GetChainType();

	result = m_pSceneUtils->AddShadingModifier( 
								rModName,
								rModChainType,
								&pModifier );

	if( IFXSUCCESS( result ) )
		result = pModifier->SetAttributes( pIDTFModifier->GetAttributes() );
	U32 i;
	for( i = 0; 
		 i < pIDTFModifier->GetShaderListCount() && IFXSUCCESS( result ); 
		 ++i )
	{
		IFXDECLARELOCAL( IFXShaderList, pShaderList );

		//create shader set
		result = IFXCreateComponent( 
					CID_IFXShaderList, IID_IFXShaderList, (void**)&pShaderList );

		if( IFXSUCCESS( result ))
		{
			const ShaderList& shaderList = pIDTFModifier->GetShaderList( i );
			U32 shaderCount = shaderList.GetShaderCount();
			result = pShaderList->Allocate( shaderCount );

			if( IFXSUCCESS( result ) )
			{
				U32 id = 0;
				U32 j;
				for( j = 0; j < shaderCount && IFXSUCCESS( result ); ++j )
				{
					IFXDECLARELOCAL( IFXShaderLitTexture, pShader );
					const IFXString& shaderName = shaderList.GetShaderName( j );

					result = m_pSceneUtils->FindShader( 
												shaderName, &pShader, &id );

					if( IFXSUCCESS( result ) )
						result = pShaderList->SetShader( j, id );
				}
				if( IFXSUCCESS( result ) )
					result = pModifier->SetElementShaderList( i, pShaderList );
			}
		}
	}

	// convert shading modifier meta-data
	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		if( pModifier && IFXSUCCESS( result ) )
			result = pModifier->QueryInterface( 
									IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( pIDTFModifier, pModifier );
			metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT  ModifierConverter::ConvertAnimationModifier( 
							const AnimationModifier* pIDTFModifier )
{
	IFXRESULT result = IFX_OK;
	IFXDECLARELOCAL( IFXAnimationModifier, pModifier );
	const IFXString& rModName = pIDTFModifier->GetName();
	const IFXString& rModChainType = pIDTFModifier->GetChainType();

	result = m_pSceneUtils->AddAnimationModifier( 
								rModName,
								rModChainType,
								pIDTFModifier->m_singleTrack,
								&pModifier );

	// Now add the modifier to this node.
	if( IFXSUCCESS( result ) )
	{
		// Set animation modifier parameters
		pModifier->Playing() = pIDTFModifier->m_playing;
		pModifier->RootLock() = pIDTFModifier->m_rootLock;
		pModifier->AutoBlend() = pIDTFModifier->m_autoBlend;

		// Now assign motions for this node to the modifier
		U32 i;
		for( i = 0; 
			 i < pIDTFModifier->GetMotionInfoCount() 
				 && IFXSUCCESS( result );
			 ++i )
		{
			const MotionInfo& rMotionInfo = 
				pIDTFModifier->GetMotionInfo( i );

			const IFXString& rMotionName = rMotionInfo.m_name;

			result = m_pSceneUtils->AssignAnimationToModifier( 
											pModifier,
											rMotionName,
											rMotionInfo.m_timeOffset,
											rMotionInfo.m_timeScale,
											rMotionInfo.m_loop,
											rMotionInfo.m_sync );
		}

		// convert animation modifier meta-data
		if( IFXSUCCESS( result ) )
		{
			IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

			result = pModifier->QueryInterface( 
									IID_IFXMetaDataX, (void**)&pMetaData );

			if( IFXSUCCESS( result ) )
			{
				MetaDataConverter metaDataConverter( 
										pIDTFModifier, pModifier );
				metaDataConverter.Convert();
			}
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierConverter::ConvertBoneWeightModifier( 
							const BoneWeightModifier* pIDTFModifier )
{
	IFXRESULT result = IFX_OK;
	const IFXString& rModChainType = pIDTFModifier->GetChainType();
	const IFXString& rModName = pIDTFModifier->GetName();
	IFXDECLARELOCAL( IFXBoneWeightsModifier, pModifier );

	if( NULL == pIDTFModifier )
	{
		result = IFX_E_INVALID_POINTER;
		IFXASSERT( IFXSUCCESS( result ) );
	}

	if( IFXSUCCESS( result ) )
		result = m_pSceneUtils->AddBoneWeightModifier(
										rModName, 
										rModChainType,
										&pModifier );

	if( pModifier && IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXAuthorCLODResource, pAuthorCLODResource );
		IFXDECLARELOCAL( IFXMeshMap, pAuthorToCompressMap );
		IFXVertexMap* pVertexMap = NULL;

		F32 inverseQuant = pIDTFModifier->GetInverseQuant();
		const U32& positionCount = 
			pIDTFModifier->GetBoneWeightListCount();

		if( rModChainType == IDTF_NODE )
		{
			result = m_pSceneUtils->FindResourceByModelName( 
										rModName, &pAuthorCLODResource );
		}

		else if( rModChainType == IDTF_MODEL )
		{
			IFXDECLARELOCAL( IFXGenerator, pGenerator );

			result = m_pSceneUtils->FindModelResource( 
										rModName, &pGenerator );

			if( IFXSUCCESS( result ) )
				result = pGenerator->QueryInterface( 
										IID_IFXAuthorCLODResource, 
										(void**)&pAuthorCLODResource);
		}
		else
			result = IFX_E_UNSUPPORTED;

		if( IFXSUCCESS( result ) )
			result = pAuthorCLODResource->GetAuthorMeshMap( 
												&pAuthorToCompressMap );

		// Get the vertex map
		if( pAuthorToCompressMap && IFXSUCCESS( result ) )
		{
			pVertexMap = pAuthorToCompressMap->GetPositionMap();

			if( !pVertexMap )
				result = IFX_E_NOT_INITIALIZED;
		}

#ifdef _DEBUG
		// sanity check
		if( IFXSUCCESS( result ) )
		{
			IFXDECLARELOCAL( IFXAuthorCLODMesh, pAuthorMesh );
			result = pAuthorCLODResource->GetAuthorMesh( pAuthorMesh );

			if( IFXSUCCESS( result ) )
			{
				IFXAuthorMeshDesc* pAuthorMeshDesc = 
					(IFXAuthorMeshDesc *)pAuthorMesh->GetMaxMeshDesc();
				U32 numAuthorVertices = pAuthorMeshDesc->NumPositions;

				// Sanity check for scrubbing
				// If this map was built properly,
				// the number of map entries should equal the
				// number of vertices in the original mesh.
				IFXASSERT( 
					pVertexMap->GetNumMapEntries() == numAuthorVertices );

				// Sanity check for bone weight modifier parser
				IFXASSERT( numAuthorVertices == positionCount )
			}
		}
#endif

		if( IFXSUCCESS( result ) )
			result = pModifier->SetTotalBoneWeightCountForAuthorMesh(
												positionCount );

		U32 meshIndex = 0;
		U32 vertexIndex = 0;
		U32 i;
		for( i = 0; i < positionCount && IFXSUCCESS( result ); ++i )
		{
			const BoneWeightList& rIDTFBoneWeightList = 
				pIDTFModifier->GetBoneWeightList( i );
			const U32& boneWeightCount = 
				rIDTFBoneWeightList.GetBoneIndexCount();

			if( boneWeightCount > 0 )
			{
				U32* pBoneIDs = new U32[boneWeightCount];
				F32* pBoneWeights = new F32[boneWeightCount];
				
				// There should be only one or zero vertex copy since this
				// is not a render mesh
				IFXASSERT( pVertexMap->GetNumVertexCopies(i) <= 1 );

				if( NULL != pBoneIDs && NULL != pBoneWeights )
				{
					F32 boneWeightSum = 0.0f;
					U32 j;
					for( j = 0; j < boneWeightCount-1; ++j )
					{
						pBoneIDs[j] = rIDTFBoneWeightList.GetBoneIndex( j );
						pBoneWeights[j] = 
							inverseQuant * rIDTFBoneWeightList.GetBoneWeight( j );
						boneWeightSum += pBoneWeights[j];
					}

					pBoneIDs[boneWeightCount-1] = 
						rIDTFBoneWeightList.GetBoneIndex( boneWeightCount-1 );

					// sanity check
					IFXASSERT( boneWeightSum <= 1.0f );
					pBoneWeights[boneWeightCount-1] = 1.0f - boneWeightSum;

					// We should map to the scrubbed position indices
					result = pVertexMap->GetVertexCopy(
									i, 0, &meshIndex, &vertexIndex );

					// All mesh indices should be 0 since this
					// is not a render mesh
					IFXASSERT( meshIndex == 0 );

					// Now load bone weights into the bone weight modifier
					if( IFXSUCCESS( result ) )
					{
						result = pModifier->SetBoneWeightsForAuthorMesh(
							vertexIndex, boneWeightCount, pBoneIDs, pBoneWeights );
					}

					delete [] pBoneIDs;
					delete [] pBoneWeights;
				}
			}
		}
	}

	// convert bone weight modifier meta-data
	if( pModifier && IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		result = pModifier->QueryInterface( 
									IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( pIDTFModifier, pModifier );
			metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierConverter::ConvertCLODModifier( const CLODModifier* pIDTFModifier )
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL( IFXCLODModifier, pModifier );

	const IFXString& rModName = pIDTFModifier->GetName();
	const IFXString& rModChainType = pIDTFModifier->GetChainType();

	result = m_pSceneUtils->AddCLODModifier( 
								rModName,
								rModChainType,
								&pModifier );

	if( IFXSUCCESS( result ) )
	{
		const IFXString& rAutoLODControl = 
			pIDTFModifier->GetAutoLODControl();

		if( rAutoLODControl == IDTF_TRUE )
		{
			result = pModifier->SetCLODScreenSpaceControllerState( TRUE );

			if( IFXSUCCESS( result ) )
				result = pModifier->SetLODBias( pIDTFModifier->GetLODBias() );
		}
		else if( rAutoLODControl == IDTF_FALSE )
		{
			result = pModifier->SetCLODScreenSpaceControllerState( FALSE );

			if( IFXSUCCESS( result ) )
				result = pModifier->SetCLODLevel( pIDTFModifier->GetCLODLevel() );
		}
		else
			result = IFX_E_UNSUPPORTED;
	}


	// convert CLOD modifier meta-data
	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		if( pModifier && IFXSUCCESS( result ) )
			result = pModifier->QueryInterface( 
										IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( 
								pIDTFModifier, pModifier );
			metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierConverter::ConvertSubdivisionModifier( 
	const SubdivisionModifier* pIDTFModifier )
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL( IFXSubdivModifier, pModifier );

	const IFXString& rModName = pIDTFModifier->GetName();
	const IFXString& rModChainType = pIDTFModifier->GetChainType();

	result = m_pSceneUtils->AddSubdivisionModifier( 
								rModName,
								rModChainType,
								&pModifier );

	if( IFXSUCCESS( result ) )
	{
		const IFXString& rEnabled = pIDTFModifier->GetEnabled();
		if( rEnabled == IDTF_TRUE )
			result = pModifier->SetEnable( TRUE );
		else if( rEnabled == IDTF_FALSE )
			result = pModifier->SetEnable( FALSE );
		else
			result = IFX_E_UNSUPPORTED;
	}

	if( IFXSUCCESS( result ) )
	{
		const IFXString& rAdaptive = pIDTFModifier->GetAdaptive();
		if( rAdaptive == IDTF_TRUE )
		{
			result = pModifier->SetAdaptive( TRUE );
		
			if( IFXSUCCESS( result ) )
				result = pModifier->SetError( 
											pIDTFModifier->GetError() );
		}
		else if( rAdaptive == IDTF_FALSE )
			result = pModifier->SetAdaptive( FALSE );
		else
			result = IFX_E_UNSUPPORTED;
	}

	if( IFXSUCCESS( result ) )
		result = pModifier->SetDepth( pIDTFModifier->GetDepth() );

	if( IFXSUCCESS( result ) )
		result = pModifier->SetTension( pIDTFModifier->GetTension() );

	// convert subdivision modifier meta-data
	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		if( pModifier && IFXSUCCESS( result ) )
			result = pModifier->QueryInterface( 
										IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( 
								pIDTFModifier, pModifier );
			metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierConverter::ConvertGlyphModifier( const GlyphModifier* pIDTFModifier )
{
	IFXRESULT result = IFX_OK;

	IFXDECLARELOCAL( IFXGlyph2DModifier, pModifier );
	IFXDECLARELOCAL( IFXGlyphCommandList, pCommandList);
//	IFXGlyphCommandList* pCommandList = NULL;

	const IFXString& rModName = pIDTFModifier->GetName();
	const IFXString& rModChainType = pIDTFModifier->GetChainType();

	// convert command list

	result = IFXCreateComponent( 
					CID_IFXGlyphCommandList, 
					IID_IFXGlyphCommandList, (void**)&pCommandList );

	U32 i;
	for( i = 0; 
			i < pIDTFModifier->GetCommandCount() && IFXSUCCESS( result ); 
			++i )
	{
		const GlyphCommand* pCommand = pIDTFModifier->GetCommand( i );

		if( NULL != pCommand )
		{
			const IFXString& rType = pCommand->GetType();

			if( rType == IDTF_LINE_TO )
			{
				const LineTo* pLineTo = 
					static_cast<const LineTo*>(pCommand);
				result = pCommandList->AddLineToBlock( 
											pLineTo->m_lineto_x, 
											pLineTo->m_lineto_y );
			}
			else if( rType == IDTF_MOVE_TO )
			{
				const MoveTo* pMoveTo = 
					static_cast<const MoveTo*>(pCommand);
				result = pCommandList->AddMoveToBlock(
											pMoveTo->m_moveto_x, 
											pMoveTo->m_moveto_y );
			}
			else if( rType == IDTF_CURVE_TO )
			{
				const CurveTo* pCurveTo = 
					static_cast<const CurveTo*>(pCommand);
				result = pCommandList->AddCurveToBlock( 
											pCurveTo->m_control1_x, 
											pCurveTo->m_control1_y,
											pCurveTo->m_control2_x, 
											pCurveTo->m_control2_y,
											pCurveTo->m_endpoint_x, 
											pCurveTo->m_endpoint_y,
											30 );
			}
			else if( rType == IDTF_START_GLYPH_STRING )
			{
				result = pCommandList->AddTagBlock( 
					IFXGlyph2DCommands::IGG_TYPE_STARTGLYPHSTRING );
			}
			else if( rType == IDTF_END_GLYPH_STRING )
			{
				result = pCommandList->AddTagBlock( 
					IFXGlyph2DCommands::IGG_TYPE_ENDGLYPHSTRING );
			}
			else if( rType == IDTF_START_PATH )
			{
				result = pCommandList->AddTagBlock( 
					IFXGlyph2DCommands::IGG_TYPE_STARTPATH );
			}
			else if( rType == IDTF_END_PATH )
			{
				result = pCommandList->AddTagBlock( 
					IFXGlyph2DCommands::IGG_TYPE_ENDPATH );
			}
			else if( rType == IDTF_START_GLYPH )
			{
				result = pCommandList->AddTagBlock( 
					IFXGlyph2DCommands::IGG_TYPE_STARTGLYPH );
			}
			else if( rType == IDTF_END_GLYPH )
			{
				const EndGlyph* pEndGlyph = 
					static_cast<const EndGlyph*>(pCommand);

				result = pCommandList->AddTagBlock( 
									IFXGlyph2DCommands::IGG_TYPE_ENDGLYPH, 
									pEndGlyph->m_offset_x, 
									pEndGlyph->m_offset_y );
			}
			else
				result = IFX_E_UNSUPPORTED;
		}
		else
			result = IFX_E_INVALID_POINTER;
	}

	// add glyph modifier
	if( IFXSUCCESS( result ) )
		result = m_pSceneUtils->AddGlyphModifier( 
								rModName,
								rModChainType,
								1, 1, 1, pCommandList,
								&pModifier );

	// convert attribute
	if( IFXSUCCESS( result ) )
	{
		const IFXString& rBillboard = pIDTFModifier->GetBillboard();
		const IFXString& rSingleShader = pIDTFModifier->GetSingleShader();

		U32 attr = 0;

		if( rBillboard == IDTF_TRUE )
			attr |= IFXGlyph2DModifier::BILLBOARD;
		if( rSingleShader == IDTF_TRUE )
			attr |= IFXGlyph2DModifier::SINGLE_SHADER;
		pModifier->SetAttributes( attr );
	}

	// convert transform
	if( IFXSUCCESS( result ) )
	{
		result = pModifier->SetTransform( pIDTFModifier->GetTM() );
	}

	// convert glyph modifier meta-data
	if( IFXSUCCESS( result ) )
	{
		IFXDECLARELOCAL( IFXMetaDataX, pMetaData );

		if( pModifier && IFXSUCCESS( result ) )
			result = pModifier->QueryInterface( 
									IID_IFXMetaDataX, (void**)&pMetaData );

		if( IFXSUCCESS( result ) )
		{
			MetaDataConverter metaDataConverter( 
								pIDTFModifier, pModifier );
			metaDataConverter.Convert();
		}
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}


//***************************************************************************
//  Private methods
//***************************************************************************

//***************************************************************************
//  Global functions
//***************************************************************************


//***************************************************************************
//  Local functions
//***************************************************************************
