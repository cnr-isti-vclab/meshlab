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
  @file ModifierParser.cpp

      This module defines ...
*/


//***************************************************************************
//  Includes
//***************************************************************************

#include "IFXCheckX.h"
#include "FileScanner.h"
#include "Tokens.h"
#include "ModifierParser.h"
#include "ShadingModifier.h"
#include "IFXShadingModifier.h"
#include "AnimationModifier.h"
#include "BoneWeightModifier.h"
#include "CLODModifier.h"
#include "SubdivisionModifier.h"
#include "GlyphModifier.h"

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

ModifierParser::ModifierParser( FileScanner* pScanner, Modifier* pModifier )
: BlockParser( pScanner ), m_pScanner( pScanner ), m_pModifier( pModifier )
{
	IFXCHECKX_RESULT( NULL != pModifier, IFX_E_INVALID_POINTER );
	IFXCHECKX_RESULT( NULL != pScanner, IFX_E_INVALID_POINTER );
}

ModifierParser::~ModifierParser()
{
	m_pScanner = NULL;
	m_pModifier = NULL;
}

IFXRESULT ModifierParser::Parse()
{
	IFXRESULT result = IFX_OK;
	IFXString type = m_pModifier->GetType();

	result = ParseCommonModifierData();

	if( IFXSUCCESS( result ) )
		result = BlockBegin( IDTF_PARAMETERS );

	if( IFXSUCCESS( result ) )
	{
		if( type == IDTF_SHADING_MODIFIER )
			result = ParseShadingModifier();
		else if( type == IDTF_ANIMATION_MODIFIER )
			result = ParseAnimationModifier();
		else if( type == IDTF_BONE_WEIGHT_MODIFIER )
			result = ParseBoneWeightModifier();
		else if( type == IDTF_CLOD_MODIFIER )
			result = ParseCLODModifier();
		else if( type == IDTF_SUBDIVISION_MODIFIER )
			result = ParseSubdivisionModifier();
		else if( type == IDTF_GLYPH_MODIFIER )
			result = ParseGlyphModifier();
		else
			result = IFX_E_UNKNOWN_MODIFIER_TYPE;
	}

	if( IFXSUCCESS( result ) )
		result = BlockEnd();

	if( IFXSUCCESS( result ) )
		result = ParseMetaData( m_pModifier );

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

//***************************************************************************
//  Protected methods
//***************************************************************************

IFXRESULT ModifierParser::ParseCommonModifierData()
{
	IFXRESULT result = IFX_OK;
	IFXString name;
	IFXString chainType;

	result = m_pScanner->ScanStringToken( IDTF_MODIFIER_NAME, &name );

	if( IFXSUCCESS( result ) )
	{
		result = m_pScanner->ScanStringToken( IDTF_MODIFIER_CHAIN_TYPE, &chainType );

		if( IFX_E_TOKEN_NOT_FOUND == result )
		{
			// assume node modifier chain type
			chainType = IDTF_NODE;
			result = IFX_OK;
		}
	}

	if( IFXSUCCESS( result ) )
	{
		/**
			@todo Should be removed, keep it here for backward compatibility
		*/
		I32 index = -1;

		result = m_pScanner->ScanIntegerToken( IDTF_MODIFIER_CHAIN_INDEX, &index );

		if( IFX_E_TOKEN_NOT_FOUND == result )
			result = IFX_OK;
	}

	if( IFXSUCCESS( result ) )
	{
		m_pModifier->SetName( name );
		m_pModifier->SetChainType( chainType );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierParser::ParseAnimationModifier()
{
	IFXRESULT result = IFX_OK;
	IFXString playing;
	IFXString rootLock;
	IFXString autoBlend;
	IFXString singleTrack;
	F32 timeScale;
	F32 blendTime;
	I32 motionCount = -1;
	AnimationModifier* pAnimationModifier = 
		static_cast< AnimationModifier* >( m_pModifier );

	result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_ANIMATION_PLAYING, 
								&playing );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_ROOT_BONE_LOCKED, 
								&rootLock );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_SINGLE_TRACK, 
								&singleTrack );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_AUTO_BLEND, 
								&autoBlend );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_TIME_SCALE, &timeScale );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_BLEND_TIME, &blendTime );

	if( IFXSUCCESS( result ) )
	{
		if( playing == "TRUE" )
			pAnimationModifier->m_playing = TRUE;
		else 
			pAnimationModifier->m_playing = FALSE;

		if( rootLock == "TRUE" )
			pAnimationModifier->m_rootLock = TRUE;
		else 
			pAnimationModifier->m_rootLock = FALSE;

		if( singleTrack == "TRUE" )
			pAnimationModifier->m_singleTrack = TRUE;
		else 
			pAnimationModifier->m_singleTrack = FALSE;

		if( autoBlend == "TRUE" )
			pAnimationModifier->m_autoBlend = TRUE;
		else 
			pAnimationModifier->m_autoBlend = FALSE;

		pAnimationModifier->SetTimeScale( timeScale );
		pAnimationModifier->SetBlendTime( blendTime );
	}

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_MOTION_COUNT, 
											   &motionCount );
	
	if( IFXSUCCESS( result ) && motionCount > 0 )
	{
		result = BlockBegin( IDTF_MOTION_INFO_LIST );

		I32 i;
		for( i = 0; i < motionCount && IFXSUCCESS( result ); ++i )
		{
			MotionInfo motionInfo;
			I32 motionInfoNumber;
			IFXString loop;
			IFXString sync;

			result = BlockBegin( IDTF_MOTION_INFO, &motionInfoNumber );

			if( IFXSUCCESS( result ) && motionInfoNumber == i )
			{
				result = m_pScanner->ScanStringToken( IDTF_MOTION_NAME, 
														&motionInfo.m_name );

				if( IFXSUCCESS( result ) )
					result = m_pScanner->ScanStringToken( 
												IDTF_ATTRIBUTE_LOOP, 
												&loop );

				if( IFXSUCCESS( result ) )
					result = m_pScanner->ScanStringToken( 
												IDTF_ATTRIBUTE_SYNC, 
												&sync );

				if( IFXSUCCESS( result ) )
					result = m_pScanner->ScanFloatToken( 
												IDTF_TIME_OFFSET, 
												&motionInfo.m_timeOffset );

				if( IFXSUCCESS( result ) )
					result = m_pScanner->ScanFloatToken( IDTF_TIME_SCALE, 
														&motionInfo.m_timeScale );

				if( IFXSUCCESS( result ) )
					result = BlockEnd();
			}

			if( IFXSUCCESS( result ) )
			{
				if( loop == "TRUE" )
					motionInfo.m_loop = TRUE;
				else 
					motionInfo.m_loop = FALSE;

				if( sync == "TRUE" )
					motionInfo.m_sync = TRUE;
				else 
					motionInfo.m_sync = FALSE;

				pAnimationModifier->AddMotionInfo( motionInfo );
			}
		}

		if( IFXSUCCESS( result ) )
			result = BlockEnd();
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierParser::ParseBoneWeightModifier()
{
	IFXRESULT result = IFX_OK;
	IFXString attributes;
	F32 inverseQuant;
	I32 positionCount = -1;
	BoneWeightModifier* pBoneWeightModifier = 
		static_cast< BoneWeightModifier* >( m_pModifier );

	result = m_pScanner->ScanStringToken( IDTF_ATTRIBUTES, &attributes );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_INVERSE_QUANT, &inverseQuant );

	if( IFXSUCCESS( result ) )
		pBoneWeightModifier->SetInverseQuant( inverseQuant );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_POSITION_COUNT, 
											   &positionCount );
	if( IFXSUCCESS( result ) )
		result = BlockBegin( IDTF_POSITION_BONE_WEIGHT_LIST );

	I32 i;
	for( i = 0; i < positionCount && IFXSUCCESS( result ); ++i )
	{
		I32 positionNumber = -1;
		BoneWeightList boneWeightList;

		result = BlockBegin( IDTF_BONE_WEIGHT_LIST, &positionNumber );

		if( IFXSUCCESS( result ) && positionNumber == i )
		{
			I32 boneWeightCount = -1;

			result = m_pScanner->ScanIntegerToken( IDTF_BONE_WEIGHT_COUNT, 
													  &boneWeightCount );

			if( IFXSUCCESS( result ) && boneWeightCount > 0 )
			{
				result = BlockBegin( IDTF_BONE_INDEX_LIST );

				// parse bone indeces
				for( I32 j = 0; j < boneWeightCount && IFXSUCCESS( result ); ++j )
				{
					I32 boneIndex;

					result = m_pScanner->ScanInteger( &boneIndex );

					if( IFXSUCCESS( result ) )
						boneWeightList.AddBoneIndex( boneIndex );
				}
				
				if( IFXSUCCESS( result ) )
					result = BlockEnd();

				if( 0 != boneWeightCount-1 && IFXSUCCESS( result ) )
				{
					result = BlockBegin( IDTF_BONE_WEIGHT_LIST );

					// parse bone weights
					for( I32 k = 0; k < boneWeightCount-1 && IFXSUCCESS( result ); ++k )
					{
						I32 boneWeight;

						result = m_pScanner->ScanInteger( &boneWeight );

						if( IFXSUCCESS( result ) )
							boneWeightList.AddBoneWeight( boneWeight );
					}

					if( IFXSUCCESS( result ) )
						result = BlockEnd();
				}
			}

			if( IFXSUCCESS( result ) )
				result = BlockEnd();
		}

		if( IFXSUCCESS( result ) )
			pBoneWeightModifier->AddBoneWeightList( boneWeightList );
	}

	if( IFXSUCCESS( result ) )
		result = BlockEnd();

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierParser::ParseShaderList( ShaderList* pShaderList )
{
	IFXRESULT result = IFX_OK;

	I32 shaderCount = 0;

	if( NULL == pShaderList )
		result = IFX_E_INVALID_POINTER;

	if( IFXSUCCESS( result ) )
		result = ParseStarter(); // 4 shader list
	
	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_SHADER_COUNT, &shaderCount );

	if( IFXSUCCESS( result ) )
			result = m_pScanner->ScanToken( IDTF_SHADER_NAME_LIST );

	if( IFXSUCCESS( result ) )
	{
		I32 shaderNumber = -1;
		IFXString shaderName;

		result = ParseStarter(); // 5 shader name list

		for( I32 j = 0; j < shaderCount && IFXSUCCESS( result ); ++j )
		{
			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanIntegerToken( IDTF_SHADER, &shaderNumber );

			if( IFXSUCCESS( result ) && shaderNumber == j )
				result = m_pScanner->ScanStringToken( IDTF_SHADER_NAME, &shaderName );

			if( IFXSUCCESS( result ) )
				pShaderList->AddShaderName( shaderName );
		}
		if( IFXSUCCESS( result ) )
			result = ParseTerminator(); // 5 shader name list
	}
	if( IFXSUCCESS( result ) )
		result = ParseTerminator();// 4 shader list

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierParser::ParseShadingModifier()
{
	IFXRESULT result = IFX_OK;
	I32 shaderListCount = 0;
	U32 attr = 0;
	IFXString attrToken;
	ShadingModifier* pShadingModifier = 
		static_cast< ShadingModifier* >( m_pModifier );

	result = m_pScanner->ScanStringToken( IDTF_ATTRIBUTE_GLYPH, &attrToken );// optional
	if( IFXSUCCESS( result ) &&  attrToken == "TRUE" )
		attr |= IFXShadingModifier::GLYPH;

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK;

	result = m_pScanner->ScanStringToken( IDTF_ATTRIBUTE_MESH, &attrToken );// optional
	if( IFXSUCCESS( result ) &&  attrToken == "TRUE" )
		attr |= IFXShadingModifier::MESH;

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK;

	result = m_pScanner->ScanStringToken( IDTF_ATTRIBUTE_LINE, &attrToken );// optional
	if( IFXSUCCESS( result ) &&  attrToken == "TRUE" )
		attr |= IFXShadingModifier::LINE;

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK;

	if( !attr ) //set default value
		attr = IFXShadingModifier::MESH | IFXShadingModifier::LINE |
			   IFXShadingModifier::POINT | IFXShadingModifier::GLYPH;

	result = m_pScanner->ScanStringToken( IDTF_ATTRIBUTE_POINT, &attrToken );// optional
	if( IFXSUCCESS( result ) &&  attrToken == "TRUE" )
		attr |= IFXShadingModifier::POINT;

	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = IFX_OK;

	pShadingModifier->SetAttributes( attr );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_SHADER_LIST_COUNT, &shaderListCount );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanToken( IDTF_SHADER_LIST_LIST );

	// it's allowed to use SHADING_GROUP intead of SHADER_LIST_LIST 
	// for consistency with spec
	if( IFX_E_TOKEN_NOT_FOUND == result )
		result = m_pScanner->ScanToken( IDTF_SHADING_GROUP );

	if( IFXSUCCESS( result ) )
		result = ParseStarter(); // 3 shader list list

	I32 i;
	for( i = 0; i < shaderListCount && IFXSUCCESS( result ); ++i )
	{
		ShaderList shaderList;

		if( IFXSUCCESS( result ) )
		{
			I32 shaderListNumber = -1;

			if( IFXSUCCESS( result ) )
				result = m_pScanner->ScanIntegerToken( IDTF_SHADER_LIST, 
													   &shaderListNumber );

			if( IFXSUCCESS( result ) && shaderListNumber == i )
				result = ParseShaderList( &shaderList );
		}

		if( IFXSUCCESS( result ) )
			pShadingModifier->AddShaderList( shaderList );
	}

	if( IFXSUCCESS( result ) )
		result = ParseTerminator();// 3 shader list list

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierParser::ParseCLODModifier()
{
	IFXRESULT result = IFX_OK;
	CLODModifier* pCLODModifier = 
		static_cast< CLODModifier* >( m_pModifier );
	IFXString autoLODControl;
	F32 lodBias, clodLevel;

	result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_AUTO_LOD_CONTROL, 
								&autoLODControl );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_LOD_BIAS, &lodBias );

	// it is ok that lodBias is not defined if auto LOD control disabled
	if( result == IFX_E_TOKEN_NOT_FOUND && autoLODControl == NULL )
		result = IFX_OK;

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_CLOD_LEVEL, &clodLevel );

	// it is ok that CLOD level is not defined if auto LOD control enabled
	if( result == IFX_E_TOKEN_NOT_FOUND && autoLODControl == IFX_TRUE )
		result = IFX_OK;

	if( IFXSUCCESS( result ) )
	{
		pCLODModifier->SetAutoLODControl( autoLODControl );
		pCLODModifier->SetLODBias( lodBias );
		pCLODModifier->SetCLODLevel( clodLevel );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierParser::ParseSubdivisionModifier()
{
	IFXRESULT result = IFX_OK;
	SubdivisionModifier* pSubdivModifier = 
		static_cast< SubdivisionModifier* >( m_pModifier );
	IFXString enabled, adaptive;
	F32 tension, error = 0;
	I32 depth;

	result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_ENABLED, 
								&enabled );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_ADAPTIVE, 
								&adaptive );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( IDTF_DEPTH, &depth );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_TENSION, &tension );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanFloatToken( IDTF_ERROR, &error );

	// it's ok that error is not defined if adaptive is used
	if( result == IFX_E_TOKEN_NOT_FOUND && adaptive == IFX_TRUE )
		result = IFX_OK;

	if( IFXSUCCESS( result ) )
	{
		pSubdivModifier->SetEnabled( enabled );
		pSubdivModifier->SetAdaptive( adaptive );
		pSubdivModifier->SetDepth( depth );
		pSubdivModifier->SetTension( tension );
		pSubdivModifier->SetError( error );
	}

	IFXASSERT( IFXSUCCESS( result ) ); 
	return result;
}

IFXRESULT ModifierParser::ParseGlyphModifier()
{
	IFXRESULT result = IFX_OK;
	GlyphModifier* pGlyphModifier = 
		static_cast< GlyphModifier* >( m_pModifier );
	IFXString billboard;
	IFXString singleShader;
	IFXMatrix4x4 tm;
	I32 count = 0;

	result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_BILLBOARD, 
								&billboard );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanStringToken( 
								IDTF_ATTRIBUTE_SINGLESHADER, 
								&singleShader );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanIntegerToken( 
								IDTF_GLYPH_COMMAND_COUNT, 
								&count );

	if( IFXSUCCESS( result ) && count > 0 )
	{
		I32 i, number;
		IFXString type;

		result = BlockBegin( IDTF_GLYPH_COMMAND_LIST );

		for( i = 0; i < count && IFXSUCCESS( result ); ++i )
		{
			if( IFXSUCCESS( result ) )
				result = BlockBegin( IDTF_GLYPH_COMMAND, &number );

			if( IFXSUCCESS( result ) && number == i )
				result = m_pScanner->ScanStringToken( 
										IDTF_GLYPH_COMMAND_TYPE, &type );

			if( IFXSUCCESS( result ) )
			{
				if( type == IDTF_END_GLYPH )
				{
					EndGlyph glyph;
					F32 offset_x = 0, offset_y = 0;

					result = m_pScanner->ScanFloatToken( 
											IDTF_END_GLYPH_OFFSET_X, 
											&offset_x );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
											IDTF_END_GLYPH_OFFSET_Y, 
											&offset_y );

					if( IFXSUCCESS( result ) )
					{
						glyph.SetType( IDTF_END_GLYPH );
						glyph.m_offset_x = offset_x;
						glyph.m_offset_y = offset_y;

						result = pGlyphModifier->AddCommand( &glyph );
					}
				}
				else if( type == IDTF_MOVE_TO )
				{
					MoveTo glyph;
					F32 x = 0, y = 0;

					result = m_pScanner->ScanFloatToken( 
										IDTF_MOVE_TO_X, &x );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
										IDTF_MOVE_TO_Y, &y );

					if( IFXSUCCESS( result ) )
					{
						glyph.SetType( IDTF_MOVE_TO );
						glyph.m_moveto_x = x;
						glyph.m_moveto_y = y;

						result = pGlyphModifier->AddCommand( &glyph );
					}
				}
				else if( type == IDTF_LINE_TO )
				{
					LineTo glyph;
					F32 x = 0, y = 0;

					result = m_pScanner->ScanFloatToken( 
										IDTF_LINE_TO_X, &x );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
										IDTF_LINE_TO_Y, &y );

					if( IFXSUCCESS( result ) )
					{
						glyph.SetType( IDTF_LINE_TO );
						glyph.m_lineto_x = x;
						glyph.m_lineto_y = y;

						result = pGlyphModifier->AddCommand( &glyph );
					}
				}
				else if( type == IDTF_CURVE_TO )
				{
					CurveTo glyph;
					F32 x1 = 0, y1 = 0, x2 = 0, y2 = 0, end_x = 0, end_y = 0;

					result = m_pScanner->ScanFloatToken( 
										IDTF_CONTROL1_X, &x1 );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
										IDTF_CONTROL1_Y, &y1 );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
										IDTF_CONTROL2_X, &x2 );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
										IDTF_CONTROL2_Y, &y2 );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
										IDTF_ENDPOINT_X, &end_x );

					if( IFXSUCCESS( result ) )
						result = m_pScanner->ScanFloatToken( 
										IDTF_ENDPOINT_Y, &end_y );

					if( IFXSUCCESS( result ) )
					{
						glyph.SetType( IDTF_CURVE_TO );
						glyph.m_control1_x = x1;
						glyph.m_control1_y = y1;
						glyph.m_control2_x = x2;
						glyph.m_control2_y = y2;
						glyph.m_endpoint_x = end_x;
						glyph.m_endpoint_y = end_y;

						result = pGlyphModifier->AddCommand( &glyph );
					}
				}
				else
				{
					GlyphCommand glyph;

					glyph.SetType( type );

					result = pGlyphModifier->AddCommand( &glyph );
				}
			}

			if( IFXSUCCESS( result ) )
				result = BlockEnd();
		}

		if( IFXSUCCESS( result ) )
			result = BlockEnd();
	}

	if( IFXSUCCESS( result ) )
		result = BlockBegin( IDTF_GLYPH_TM );

	if( IFXSUCCESS( result ) )
		result = m_pScanner->ScanTM( &tm );

	if( IFXSUCCESS( result ) )
		result = BlockEnd();

	if( IFXSUCCESS( result ) )
	{
		pGlyphModifier->SetBillboard( billboard );
		pGlyphModifier->SetSingleShader( singleShader );
		pGlyphModifier->SetTM( tm );
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
