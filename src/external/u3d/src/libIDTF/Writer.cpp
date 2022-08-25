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

//------------- INCLUDES ----------------------------------------------------

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <wchar.h>

#include "Writer.h"
#include "Tokens.h"
#include "DefaultSettings.h"
#include "IFXOSUtilities.h"

//------------- PUBLIC METHODS ----------------------------------------------
#define toutf(in)					\
U32 len##in;						\
IFXOSGetUtf8StrSize(in, &len##in);	\
U8*  u8##in = new U8[len##in+1];	\
IFXOSConvertWideCharStrToUtf8( in, u8##in, len##in+1 );

#define freetmp(in)					\
delete[] u8##in;

// contructor
IFXWriter::IFXWriter():
m_fileName(NULL),
m_pFileHandle(0),
m_indent(0),
m_plain(false),
m_precision(DEFAULT_PRECISION),
m_fixed(true),
m_exportDefaults(false)
{
}

// constructor
IFXWriter::IFXWriter( const char* fileName ):
m_pFileHandle(0),
m_indent(0),
m_plain(false),
m_precision(DEFAULT_PRECISION),
m_fixed(true),
m_exportDefaults(false)
{
	m_fileName = new char[strlen(fileName)+1];
	strcpy(m_fileName, fileName);
}

// destructor
IFXWriter::~IFXWriter()
{
	if ( m_fileName != 0 )
		delete [] m_fileName;
}

// open output file	
bool IFXWriter::open()
{
	m_pFileHandle = fopen( m_fileName, "w" );
	if( m_pFileHandle == 0 )
		return false;

	return true;
}

// close output file
bool IFXWriter::close()
{
	if( m_pFileHandle != 0 )
	{
		fprintf( m_pFileHandle, "\n" );
		fclose( m_pFileHandle );
		m_pFileHandle = NULL;
	}
	return true;
}

// begin block
void IFXWriter::begin( const IFXCHAR* in )
{
	indent();
	if( wcscmp( in, L"" ) != 0 )
		{ toutf(in); fprintf( m_pFileHandle, "%s ", u8in ); freetmp(in); }
	fprintf( m_pFileHandle, "{" );
	incIndent();
}

void IFXWriter::begin( const IFXCHAR* in, const int par )
{
	indent();
	if( wcscmp( in, L"" ) != 0 )
		{ toutf(in); fprintf( m_pFileHandle, "%s ", u8in ); freetmp(in); }
	fprintf( m_pFileHandle, "%d ", par );
	fprintf( m_pFileHandle, "{" );
	incIndent();
}

void IFXWriter::begin( const IFXCHAR* in, const char* par, bool quote )
{
	indent();
	if( wcscmp( in, L"" ) != 0 )
		{ toutf(in); fprintf( m_pFileHandle, "%s ", u8in ); freetmp(in); }
	if( strcmp( par, "" ) != 0 )
	{
		if( quote )
			fprintf( m_pFileHandle, "\"%s\" ", par );
		else
			fprintf( m_pFileHandle, "%s ", par );
	}
	fprintf( m_pFileHandle, "{" );
	incIndent();
}

void IFXWriter::begin( const IFXCHAR* in, const IFXCHAR* par, bool quote )
{
	indent();
	if( wcscmp( in, L"" ) != 0 )
		{ toutf(in); fprintf( m_pFileHandle, "%s ", u8in ); freetmp(in); }
	if( wcscmp( par, L"" ) != 0 )
	{
		if( quote )
			{ toutf(par); fprintf( m_pFileHandle, "\"%s\" ", u8par ); freetmp(par); }
		else
			{ toutf(par); fprintf( m_pFileHandle, "%s ", u8par ); freetmp(par); }
	}
	fprintf( m_pFileHandle, "{" );
	incIndent();
}

void IFXWriter::begin( const IFXCHAR* in, const IFXString& par, bool quote )
{
	begin( in, par.Raw(), quote );
}

// end block
void IFXWriter::end()
{
	decIndent();
	indent();
	fprintf( m_pFileHandle, "}" );
}

// output string param
void IFXWriter::output( const IFXCHAR* id, const char* param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	{ toutf(id); fprintf( m_pFileHandle, "%s ", u8id ); freetmp(id); }
	fprintf( m_pFileHandle, " \"%s\"", param );
}

// output string param
void IFXWriter::output( const IFXCHAR* id, const IFXCHAR* param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	{ toutf(id); toutf(param); fprintf( m_pFileHandle, "%s \"%s\"", u8id, u8param ); freetmp(param); freetmp(id); }
}

// output string param
void IFXWriter::output( const IFXCHAR* id, const IFXString& param, bool isIndent )
{
	output( id, param.Raw(), isIndent );
}

// output float param
void IFXWriter::output( const IFXCHAR* id, const float param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	if( wcscmp( id, L"" ) != 0 )
		{ toutf(id); fprintf( m_pFileHandle, "%s ", u8id ); freetmp(id); }
	output( param );
}

// output int param
void IFXWriter::output( const IFXCHAR* id, const int param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	{ toutf(id); fprintf( m_pFileHandle, "%s ", u8id ); freetmp(id); }
	output( param );
}

// output unsigned int param
void IFXWriter::output( const IFXCHAR* id, const unsigned int param, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	{ toutf(id); fprintf( m_pFileHandle, "%s ", u8id ); freetmp(id); }
	output( param );
}

// output color param
void IFXWriter::output( const IFXCHAR* id, const Color& p )
{
	output( id, true );
	output( p.GetColor().R() );
	output( p.GetColor().G() );
	output( p.GetColor().B() );
}

// output point3 param
void IFXWriter::output( const IFXCHAR* id, const Point& p, bool isIndent )
{
	if( !m_plain && isIndent == true ) indent();
	if( wcscmp( id, L"" ) != 0 )
		{ toutf(id); fprintf( m_pFileHandle, "%s ", u8id ); freetmp(id); }
	output( p.GetPoint().XC() );
	output( p.GetPoint().YC() );
	output( p.GetPoint().ZC() );
}

// output int3 param
void IFXWriter::output( const IFXCHAR* id, const U32* p )
{
	if( !m_plain )
		indent();
	if( wcscmp( id, L"" ) != 0 )
		{ toutf(id); fprintf( m_pFileHandle, "%s ", u8id ); freetmp(id); }
	output( p[0] );
	output( p[1] );
	output( p[2] );
}

// Print out a transformation matrix.
void IFXWriter::output( const IFXCHAR* id, const IFXMatrix4x4& m )
{
	const F32 *raw = m.RawConst();

	begin( id );

	// Dump the whole Matrix
	indent();
	output(*raw++);
	output(*raw++);
	output(*raw++);
	outputeol(*raw++);

	indent();
	output(*raw++);
	output(*raw++);
	output(*raw++);
	outputeol(*raw++);

	indent();
	output(*raw++);
	output(*raw++);
	output(*raw++);
	outputeol(*raw++);

	indent();
	output(*raw++);
	output(*raw++);
	output(*raw++);
	outputeol(*raw++);

	end();
}

void IFXWriter::output( const IFXCHAR* id, const Quat& q )
{
	output( id, q.GetQuat().X() );
	output( L"", q.GetQuat().Y(), false );
	output( L"", q.GetQuat().Z(), false );
	output( L"", q.GetQuat().H(), false );

}

void IFXWriter::newline()
{
	fprintf( m_pFileHandle, "\n" );
}

void IFXWriter::exportFileHeader()
{
	const I32	IDTF_MIN_VERSION = 100;
	const IFXCHAR IDTF_FORMAT_NAME[] = L"IDTF";

	output( IDTF_FORMAT, IDTF_FORMAT_NAME, false );
	output( IDTF_VERSION, IDTF_MIN_VERSION );
	newline();
}

void IFXWriter::exportSceneData( const SceneData* pSceneData )
{
	if ( pSceneData->GetMetaDataCount() > 0 )
	{
		begin( IDTF_SCENE );
		exportMeta( pSceneData );
		end();
		newline();
	}
}

void IFXWriter::exportFileReference( const FileReference* pFileReference )
{
	if ( pFileReference->GetScopeName().Length() == 0 )
		return;
	begin( IDTF_FILE_REFERENCE );
	output( IDTF_SCOPE_NAME, pFileReference->GetScopeName() );
	exportUrlList( pFileReference->GetUrlList() );
	const U32 FilterCount = pFileReference->GetFilterCount();
	output( IDTF_FILTER_COUNT, FilterCount );
	if ( FilterCount > 0 )
	{
		begin( IDTF_FILTER_LIST );
		for( U32 FilterIndex = 0; FilterIndex < FilterCount; FilterIndex++)
		{
			const Filter& rFilter = pFileReference->GetFilter( FilterIndex );
			begin( IDTF_FILTER, FilterIndex );
			const IFXString& FilterType = rFilter.GetType();
			output( IDTF_FILTER_TYPE, FilterType );
			if ( FilterType == IDTF_FILTER_TYPE_NAME )
				output( IDTF_FILTER_OBJECT_NAME, rFilter.GetObjectName() );
			else if ( FilterType == IDTF_FILTER_TYPE_TYPE )
			{
				indent();
				{ const IFXCHAR *ptmp = IDTF_FILTER_OBJECT_TYPE; toutf(ptmp); fprintf( m_pFileHandle, "%s %X", u8ptmp, rFilter.GetObjectType() ); freetmp(ptmp); }
			}
			end();
		}
		end();
	}
	output( IDTF_NAME_COLLISION_POLICY, pFileReference->GetCollisionPolicy() );
	output( IDTF_WORLD_ALIAS_NAME, pFileReference->GetWorldAlias() );
	end();
	newline();
}

void IFXWriter::exportNodes( const NodeList* pNodeList )
{
	const U32 NodeCount = pNodeList->GetNodeCount();
	for( U32 index = 0; index < NodeCount; index++ )
		exportNode( pNodeList->GetNode(index) );
}

void IFXWriter::exportNode( const Node* pNode )
{
	begin( IDTF_NODE,  pNode->GetType(), true );
	// write out node name
	output( IDTF_NODE_NAME, pNode->GetName() );
	const ParentList& rParents =  pNode->GetParentList(); 
	const U32 ParentCount = rParents.GetParentCount();
	begin( IDTF_PARENT_LIST );
	output( IDTF_PARENT_COUNT, ParentCount );
	for ( U32 index = 0; index < ParentCount; index++ )
	{
		const ParentData& rParent = rParents.GetParentData( index );
		begin( IDTF_PARENT, index );
		// write out parent info
		output( IDTF_PARENT_NAME, rParent.GetParentName() );
		// write out node tm
		output( IDTF_PARENT_TM, rParent.GetParentTM() );
		end();
	}
	end();
	
	if ( pNode->GetType() != IDTF_GROUP )
		output( IDTF_RESOURCE_NAME, pNode->GetResourceName() );

	if ( pNode->GetType() == IDTF_VIEW )
	{
		const ViewNode* pViewNode = static_cast< const ViewNode* >( pNode );
		const ViewNodeData& rViewData = pViewNode->GetViewData();

		begin( IDTF_VIEW_DATA );

		if ( rViewData.GetUnitType() != IDTF_VIEW_UNIT_PIXEL || m_exportDefaults )
			output( IDTF_VIEW_ATTRIBUTE_SCREEN_UNIT, rViewData.GetUnitType() );
		
		output( IDTF_VIEW_TYPE, rViewData.GetType() );
		
		F32 NearClip, FarClip;
		rViewData.GetClipping( &NearClip, &FarClip );
		if ( NearClip != VIEW_NEAR_CLIP || FarClip != VIEW_FAR_CLIP || m_exportDefaults )
		{
			setFixed( false );
			output( IDTF_VIEW_NEAR_CLIP, NearClip );
			output( IDTF_VIEW_FAR_CLIP, FarClip );
			setFixed( true );
		}

		output( IDTF_VIEW_PROJECTION, rViewData.GetProjection() );

		setFixed( false );
		F32 Width, Height, HorPos, VertPos;
		rViewData.GetViewPort( &Width, &Height, &HorPos, &VertPos );
		if ( Width != VIEW_PORT_WIDTH || m_exportDefaults )
			output( IDTF_VIEW_PORT_WIDTH, Width );
		if ( Height != VIEW_PORT_HEIGHT || m_exportDefaults )
			output( IDTF_VIEW_PORT_HEIGHT, Height );
		if ( HorPos != VIEW_PORT_H_POSITION || m_exportDefaults )
			output( IDTF_VIEW_PORT_H_POSITION, HorPos );
		if ( VertPos != VIEW_PORT_V_POSITION || m_exportDefaults )
			output( IDTF_VIEW_PORT_V_POSITION, VertPos );
		
		if ( rViewData.GetBackdropCount() != 0 )
		{
			const U32& rBackdropCount = rViewData.GetBackdropCount();
			output( IDTF_BACKDROP_COUNT, rBackdropCount );
			begin( IDTF_BACKDROP_LIST );
			for ( U32 index = 0; index < rBackdropCount; index++ )
			{
				const ViewTexture& rBackdrop = rViewData.GetBackdrop( index );
				begin( IDTF_BACKDROP, index );
				output( IDTF_TEXTURE_NAME, rBackdrop.m_name, true );
				output( IDTF_TEXTURE_BLEND, rBackdrop.m_blend );
				output( IDTF_ROTATION, rBackdrop.m_rotation );
				output( IDTF_LOCATION_X, rBackdrop.m_locationX );
				output( IDTF_LOCATION_Y, rBackdrop.m_locationY );
				output( IDTF_REG_POINT_X, rBackdrop.m_regPointX );
				output( IDTF_REG_POINT_Y, rBackdrop.m_regPointY );
				output( IDTF_SCALE_X, rBackdrop.m_scaleX );
				output( IDTF_SCALE_Y, rBackdrop.m_scaleY );
				end();
			}
			end();
		}

		if ( rViewData.GetOverlayCount() != 0 )
		{
			const U32& rOverlayCount = rViewData.GetOverlayCount();
			output( IDTF_OVERLAY_COUNT, rOverlayCount );
			begin( IDTF_OVERLAY_LIST );
			for ( U32 index = 0; index < rOverlayCount; index++ )
			{
				const ViewTexture& rOverlay = rViewData.GetOverlay( index );
				begin( IDTF_OVERLAY, index );
				output( IDTF_TEXTURE_NAME, rOverlay.m_name, true );
				output( IDTF_TEXTURE_BLEND, rOverlay.m_blend );
				output( IDTF_ROTATION, rOverlay.m_rotation );
				output( IDTF_LOCATION_X, rOverlay.m_locationX );
				output( IDTF_LOCATION_Y, rOverlay.m_locationY );
				output( IDTF_REG_POINT_X, rOverlay.m_regPointX );
				output( IDTF_REG_POINT_Y, rOverlay.m_regPointY );
				output( IDTF_SCALE_X, rOverlay.m_scaleX );
				output( IDTF_SCALE_Y, rOverlay.m_scaleY );
				end();
			}
			end();
		}
		setFixed( true );
		end();
	}

	if ( pNode->GetType() == IDTF_MODEL )
	{
		const ModelNode* pModelNode = static_cast< const ModelNode* >( pNode );
		if ( pModelNode->GetVisibility() != L"FRONT" || m_exportDefaults )
			output( IDTF_MODEL_VISIBILITY, pModelNode->GetVisibility() );
	}
	
	exportMeta( pNode );

	end();
	newline();
}

void IFXWriter::exportMeta( const MetaDataList* pMetaDataList )
{
	const U32& rMetaDataCount = pMetaDataList->GetMetaDataCount();
	if ( rMetaDataCount == 0 )
		return;
	
	U32 MetaDataCount = 0;
	for ( U32 index = 0; index < rMetaDataCount; index++ )
		if( pMetaDataList->GetMetaData(index).GetAttribute() == IDTF_STRING_META_DATA )
			MetaDataCount++;
	if ( MetaDataCount == 0 )
		return;
		
	begin( IDTF_META_DATA );
	output( IDTF_META_DATA_COUNT, MetaDataCount );
	for ( U32 index = 0; index < rMetaDataCount; index++ )
	{
		const MetaData& rMetaData = pMetaDataList->GetMetaData( index );
		const IFXString& rAttribute = rMetaData.GetAttribute();
		const IFXString& rKey = rMetaData.GetKey();
		if( rAttribute == IDTF_STRING_META_DATA )
		{
			begin( IDTF_META_DATA, index);
			output( IDTF_META_DATA_ATTRIBUTE, rAttribute, true );
			output( IDTF_META_DATA_KEY, rKey );
			output( IDTF_META_DATA_VALUE, rMetaData.GetStringValue() );
			end();
		}
	}
	end();
}

void IFXWriter::exportViewResource( const ViewResource& rViewResource )
{
	output( IDTF_RESOURCE_NAME, rViewResource.GetName() );
	const U32 RootNodeCount = rViewResource.GetRootNodeCount();
	output( IDTF_VIEW_PASS_COUNT, RootNodeCount );
	if ( RootNodeCount > 0 )
	{
		begin( IDTF_VIEW_ROOT_NODE_LIST );
		for ( U32 RootNodeIndex = 0; RootNodeIndex < RootNodeCount; RootNodeIndex++ )
		{
			begin( IDTF_ROOT_NODE, RootNodeIndex );
			const IFXString& rRootNode = rViewResource.GetRootNode( RootNodeIndex );
			if ( rRootNode == L"" )
				output( IDTF_ROOT_NODE_NAME, L"<NULL>" );
			else
				output( IDTF_ROOT_NODE_NAME, rRootNode );
			end();
		}
		end();
	}
	exportMeta( &rViewResource );
}

void IFXWriter::exportLightResource( const LightResource& rLightResource )
{
	output( IDTF_RESOURCE_NAME, rLightResource.GetName() );
	output( IDTF_LIGHT_TYPE, rLightResource.m_type );
	output( IDTF_LIGHT_COLOR, rLightResource.m_color );
	output( IDTF_LIGHT_ATTENUATION, rLightResource.m_attenuation );
	if ( rLightResource.m_type == IDTF_SPOT_LIGHT )
		output( IDTF_LIGHT_SPOT_ANGLE, rLightResource.m_spotAngle );
	output( IDTF_LIGHT_INTENSITY, rLightResource.m_intensity );
	exportMeta( &rLightResource );	
}

void IFXWriter::exportModelResource( const ModelResource* pModelResource )
{
	const ModelDescription& ModelDescription = pModelResource->m_modelDescription;
	output( IDTF_RESOURCE_NAME, pModelResource->GetName() );
	output( IDTF_MODEL_TYPE, pModelResource->m_type );
	begin( pModelResource->m_type );
	if ( pModelResource->m_type == IDTF_MESH )
	{
		const MeshResource* pMeshResource = static_cast< const MeshResource* >( pModelResource );
		const I32 faceCount = pMeshResource->faceCount;
		output( IDTF_FACE_COUNT, faceCount );
		output( IDTF_MODEL_POSITION_COUNT, ModelDescription.positionCount );
		if ( ModelDescription.basePositionCount > 0 )
			output( IDTF_MODEL_BASE_POSITION_COUNT, ModelDescription.basePositionCount );
		output( IDTF_MODEL_NORMAL_COUNT, ModelDescription.normalCount );
		output( IDTF_MODEL_DIFFUSE_COLOR_COUNT, ModelDescription.diffuseColorCount );
		output( IDTF_MODEL_SPECULAR_COLOR_COUNT, ModelDescription.specularColorCount );
		output( IDTF_MODEL_TEXTURE_COORD_COUNT, ModelDescription.textureCoordCount );
		output( IDTF_MODEL_BONE_COUNT, ModelDescription.boneCount );
		exportShadingDescription( pModelResource );
		
		if ( faceCount > 0 )
			exportInt3List( IDTF_MESH_FACE_POSITION_LIST, faceCount, pMeshResource->m_facePositions );
		if ( ModelDescription.normalCount > 0 && faceCount > 0 )
			exportInt3List( IDTF_MESH_FACE_NORMAL_LIST, faceCount, pMeshResource->m_faceNormals );
		if ( faceCount > 0 )
			exportIntList( IDTF_MESH_FACE_SHADING_LIST, faceCount, pMeshResource->m_faceShaders );
		if ( ModelDescription.textureCoordCount > 0 && faceCount > 0 )
		{
			begin( IDTF_MESH_FACE_TEXTURE_COORD_LIST );
			for ( I32 index = 0; index < faceCount; index++ )
			{
				begin( IDTF_MESH_FACE, index );
				const I32 rShadingIndex = pMeshResource->m_faceShaders.GetElementConst( index );
				const ShadingDescription& rShading = pModelResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
				const U32 layers = rShading.GetTextureLayerCount();
				
				const FaceTexCoords& faceTexCoords = pMeshResource->m_faceTextureCoords.GetElementConst( index );
				
				// texture layer loop
				// do not define texture coordinates for face if it has 0 texture layers
				for( U32 TextureLayerIndex = 0; TextureLayerIndex < layers ; ++TextureLayerIndex )
				{
					output( IDTF_TEXTURE_LAYER, TextureLayerIndex );
					// Get the texture coordinates for specified texture layer for this face
					const Int3& texCoord = faceTexCoords.m_texCoords.GetElementConst( TextureLayerIndex );
					output( IDTF_MESH_FACE_TEX_COORD, texCoord.GetA(), false );
					output( texCoord.GetB() );
					output( texCoord.GetC() );
				}
				
				end();
			}
			end();
		}
		if ( ModelDescription.diffuseColorCount > 0 && faceCount > 0 )
			exportInt3List( IDTF_MESH_FACE_DIFFUSE_COLOR_LIST, faceCount, pMeshResource->m_faceDiffuseColors );
		if ( ModelDescription.specularColorCount > 0 && faceCount > 0 )
			exportInt3List( IDTF_MESH_FACE_SPECULAR_COLOR_LIST, faceCount, pMeshResource->m_faceSpecularColors );
		if ( ModelDescription.positionCount > 0 )
			exportPointList( IDTF_MODEL_POSITION_LIST, ModelDescription.positionCount, pModelResource->m_positions );
		if ( ModelDescription.normalCount > 0 )
			exportPointList( IDTF_MODEL_NORMAL_LIST, ModelDescription.normalCount, pModelResource->m_normals );
		if ( ModelDescription.diffuseColorCount > 0 )
			exportColorList( IDTF_MODEL_DIFFUSE_COLOR_LIST, ModelDescription.diffuseColorCount, pModelResource->m_diffuseColors );
		if ( ModelDescription.specularColorCount > 0 )
			exportColorList( IDTF_MODEL_SPECULAR_COLOR_LIST, ModelDescription.specularColorCount, pModelResource->m_specularColors );
		if ( ModelDescription.textureCoordCount > 0 )
		{
			begin( IDTF_MODEL_TEXTURE_COORD_LIST );
			for ( I32 TextureCoordIndex = 0; TextureCoordIndex < ModelDescription.textureCoordCount; TextureCoordIndex++ )
			{
				const IFXVector4& item = pModelResource->m_textureCoords.GetElementConst( TextureCoordIndex );
				indent();
				output( item.X() );
				output( item.Y() );
				output( item.Z() );
				output( item.H() );
			}
			end();
		}
		if ( ModelDescription.boneCount > 0 )
		{
			begin( IDTF_MODEL_SKELETON );
			setPrecision( 1 );
			for ( I32 BoneIndex = 0; BoneIndex < ModelDescription.boneCount; BoneIndex++ )
			{
				begin( IDTF_BONE, BoneIndex );
				const BoneInfo& rBoneInfo = pModelResource->m_modelSkeleton.GetBoneInfo( BoneIndex );
				output( IDTF_BONE_NAME, rBoneInfo.name );
				output( IDTF_PARENT_BONE_NAME, rBoneInfo.parentName );
				output( IDTF_BONE_LENGTH, rBoneInfo.length );
				output( IDTF_BONE_DISPLACEMENT, rBoneInfo.displacement );
				output( IDTF_BONE_ORIENTATION, rBoneInfo.orientation );
				end();
			}
			restorePrecision();
			end();
		}
		if ( ModelDescription.basePositionCount > 0 )
			exportIntList( IDTF_MODEL_BASE_POSITION_LIST, ModelDescription.basePositionCount, pModelResource->m_basePositions );
	}
	if ( pModelResource->m_type == IDTF_LINE_SET )
	{
		const LineSetResource* pLineSetResource = static_cast< const LineSetResource* >( pModelResource );
		const I32 lineCount = pLineSetResource->lineCount;
		output( IDTF_LINE_COUNT, lineCount );
		output( IDTF_MODEL_POSITION_COUNT, ModelDescription.positionCount );
		output( IDTF_MODEL_NORMAL_COUNT, ModelDescription.normalCount );
		output( IDTF_MODEL_DIFFUSE_COLOR_COUNT, ModelDescription.diffuseColorCount );
		output( IDTF_MODEL_SPECULAR_COLOR_COUNT, ModelDescription.specularColorCount );
		output( IDTF_MODEL_TEXTURE_COORD_COUNT, ModelDescription.textureCoordCount );
		exportShadingDescription( pModelResource );
		
		if ( lineCount > 0 )
			exportInt2List( IDTF_LINE_POSITION_LIST, lineCount, pLineSetResource->m_linePositions );
		if ( ModelDescription.normalCount > 0 && lineCount > 0 )
			exportInt2List( IDTF_LINE_NORMAL_LIST, lineCount, pLineSetResource->m_lineNormals );
		if ( lineCount > 0 )
			exportIntList( IDTF_LINE_SHADING_LIST, lineCount, pLineSetResource->m_lineShaders );
		if ( ModelDescription.textureCoordCount > 0 && lineCount > 0 )
		{
			begin( IDTF_LINE_TEXTURE_COORD_LIST );
			for ( I32 index = 0; index < lineCount; index++ )
			{
				begin( IDTF_LINE, index );
				const I32 rShadingIndex = pLineSetResource->m_lineShaders.GetElementConst( index );
				const ShadingDescription& rShading = pModelResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
				const U32 layers = rShading.GetTextureLayerCount();
				
				const LineTexCoords& lineTexCoords = pLineSetResource->m_lineTextureCoords.GetElementConst( index );
				
				// texture layer loop
				// do not define texture coordinates for face if it has 0 texture layers
				for( U32 TextureLayerIndex = 0; TextureLayerIndex < layers ; ++TextureLayerIndex )
				{
					output( IDTF_TEXTURE_LAYER, TextureLayerIndex );
					// Get the texture coordinates for specified texture layer for this face
					const Int2& texCoord = lineTexCoords.m_texCoords.GetElementConst( TextureLayerIndex );
					output( IDTF_LINE_TEX_COORD, texCoord.GetA(), false );
					output( texCoord.GetB() );
				}
				
				end();
			}
			end();
		}
		if ( ModelDescription.diffuseColorCount > 0 && lineCount > 0 )
			exportInt2List( IDTF_LINE_DIFFUSE_COLOR_LIST, lineCount, pLineSetResource->m_lineDiffuseColors );
		if ( ModelDescription.specularColorCount > 0 && lineCount > 0 )
			exportInt2List( IDTF_LINE_SPECULAR_COLOR_LIST, lineCount, pLineSetResource->m_lineSpecularColors );
		if ( ModelDescription.positionCount > 0 )
			exportPointList( IDTF_MODEL_POSITION_LIST, ModelDescription.positionCount, pModelResource->m_positions );
		if ( ModelDescription.normalCount > 0 )
			exportPointList( IDTF_MODEL_NORMAL_LIST, ModelDescription.normalCount, pModelResource->m_normals );
		if ( ModelDescription.diffuseColorCount > 0 )
			exportColorList( IDTF_MODEL_DIFFUSE_COLOR_LIST, ModelDescription.diffuseColorCount, pModelResource->m_diffuseColors );
		if ( ModelDescription.specularColorCount > 0 )
			exportColorList( IDTF_MODEL_SPECULAR_COLOR_LIST, ModelDescription.specularColorCount, pModelResource->m_specularColors );
		if ( ModelDescription.textureCoordCount > 0 )
		{
			begin( IDTF_MODEL_TEXTURE_COORD_LIST );
			for ( I32 TextureCoordIndex = 0; TextureCoordIndex < ModelDescription.textureCoordCount; TextureCoordIndex++ )
			{
				const IFXVector4& item = pModelResource->m_textureCoords.GetElementConst( TextureCoordIndex );
				indent();
				output( item.X() );
				output( item.Y() );
				output( item.Z() );
				output( item.H() );
			}
			end();
		}
	}
	if ( pModelResource->m_type == IDTF_POINT_SET )
	{
		const PointSetResource* pPointSetResource = static_cast< const PointSetResource* >( pModelResource );
		const I32 pointCount = pPointSetResource->pointCount;
		output( IDTF_POINT_COUNT, pointCount );
		output( IDTF_MODEL_POSITION_COUNT, ModelDescription.positionCount );
		output( IDTF_MODEL_NORMAL_COUNT, ModelDescription.normalCount );
		output( IDTF_MODEL_DIFFUSE_COLOR_COUNT, ModelDescription.diffuseColorCount );
		output( IDTF_MODEL_SPECULAR_COLOR_COUNT, ModelDescription.specularColorCount );
		output( IDTF_MODEL_TEXTURE_COORD_COUNT, ModelDescription.textureCoordCount );
		exportShadingDescription( pModelResource );
		
		if ( pointCount > 0 )
			exportIntList( IDTF_POINT_POSITION_LIST, pointCount, pPointSetResource->m_pointPositions );
		if ( ModelDescription.normalCount > 0 && pointCount > 0 )
			exportIntList( IDTF_POINT_NORMAL_LIST, pointCount, pPointSetResource->m_pointNormals );
		if ( pointCount > 0 )
			exportIntList( IDTF_POINT_SHADING_LIST, pointCount, pPointSetResource->m_pointShaders );
		if ( ModelDescription.textureCoordCount > 0 && pointCount > 0 )
		{
			begin( IDTF_POINT_TEXTURE_COORD_LIST );
			for ( I32 index = 0; index < pointCount; index++ )
			{
				begin( IDTF_POINT, index );
				const I32 rShadingIndex = pPointSetResource->m_pointShaders.GetElementConst( index );
				const ShadingDescription& rShading = pModelResource->m_shadingDescriptions.GetShadingDescription( rShadingIndex );
				const U32 layers = rShading.GetTextureLayerCount();
				
				const PointTexCoords& pointTexCoords = pPointSetResource->m_pointTextureCoords.GetElementConst( index );
				
				// texture layer loop
				// do not define texture coordinates for face if it has 0 texture layers
				for( U32 TextureLayerIndex = 0; TextureLayerIndex < layers ; ++TextureLayerIndex )
				{
					output( IDTF_TEXTURE_LAYER, TextureLayerIndex );
					// Get the texture coordinates for specified texture layer for this face
					const I32& texCoord = pointTexCoords.m_texCoords.GetElementConst( TextureLayerIndex );
					output( IDTF_POINT_TEX_COORD, texCoord, false );
				}
				
				end();
			}
			end();
		}
		if ( ModelDescription.diffuseColorCount > 0 && pointCount > 0 )
			exportIntList( IDTF_POINT_DIFFUSE_COLOR_LIST, pointCount, pPointSetResource->m_pointDiffuseColors );
		if ( ModelDescription.specularColorCount > 0 && pointCount > 0 )
			exportIntList( IDTF_POINT_SPECULAR_COLOR_LIST, pointCount, pPointSetResource->m_pointSpecularColors );
		if ( ModelDescription.positionCount > 0 )
			exportPointList( IDTF_MODEL_POSITION_LIST, ModelDescription.positionCount, pModelResource->m_positions );
		if ( ModelDescription.normalCount > 0 )
			exportPointList( IDTF_MODEL_NORMAL_LIST, ModelDescription.normalCount, pModelResource->m_normals );
		if ( ModelDescription.diffuseColorCount > 0 )
			exportColorList( IDTF_MODEL_DIFFUSE_COLOR_LIST, ModelDescription.diffuseColorCount, pModelResource->m_diffuseColors );
		if ( ModelDescription.specularColorCount > 0 )
			exportColorList( IDTF_MODEL_SPECULAR_COLOR_LIST, ModelDescription.specularColorCount, pModelResource->m_specularColors );
		if ( ModelDescription.textureCoordCount > 0 )
		{
			begin( IDTF_MODEL_TEXTURE_COORD_LIST );
			for ( I32 TextureCoordIndex = 0; TextureCoordIndex < ModelDescription.textureCoordCount; TextureCoordIndex++ )
			{
				const IFXVector4& item = pModelResource->m_textureCoords.GetElementConst( TextureCoordIndex );
				indent();
				output( item.X() );
				output( item.Y() );
				output( item.Z() );
				output( item.H() );
			}
			end();
		}
	}
	end();
	exportMeta( pModelResource );	
}

void IFXWriter::exportShaderResource( const Shader& rShaderResource )
{
	output( IDTF_RESOURCE_NAME, rShaderResource.GetName() );
	if ( rShaderResource.m_lightingEnabled != L"TRUE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_LIGHTING_ENABLED, rShaderResource.m_lightingEnabled );
	if ( rShaderResource.m_alphaTestEnabled != L"FALSE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_ALPHA_TEST_ENABLED, rShaderResource.m_alphaTestEnabled );
	if ( rShaderResource.m_useVertexColor != L"FALSE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_USE_VERTEX_COLOR, rShaderResource.m_useVertexColor );
	if ( rShaderResource.m_alphaTestReference != DEFAULT_ALPHA_TEST_REFERENCE || m_exportDefaults )
		output( IDTF_SHADER_ALPHA_TEST_REFERENCE, rShaderResource.m_alphaTestReference );
	if ( rShaderResource.m_alphaTestFunction != DEFAULT_COLOR_ALPHA_TEST_FUNCTION || m_exportDefaults )
		output( IDTF_SHADER_ALPHA_TEST_FUNCTION, rShaderResource.m_alphaTestFunction );
	if ( rShaderResource.m_colorBlendFunction != DEFAULT_COLOR_BLEND_FUNCTION || m_exportDefaults )
		output( IDTF_SHADER_COLOR_BLEND_FUNCTION, rShaderResource.m_colorBlendFunction );
	output( IDTF_SHADER_MATERIAL_NAME, rShaderResource.m_materialName );
	const U32 TextureLayerCount = rShaderResource.GetTextureLayerCount();
	output( IDTF_SHADER_ACTIVE_TEXTURE_COUNT, TextureLayerCount );
	if ( TextureLayerCount > 0 )
	{
		begin( IDTF_SHADER_TEXTURE_LAYER_LIST );
		for( U32 TextureLayerIndex = 0; TextureLayerIndex < TextureLayerCount; TextureLayerIndex++ )
		{
			const TextureLayer& rTextureLayer = rShaderResource.GetTextureLayer( TextureLayerIndex );
			begin( IDTF_TEXTURE_LAYER, rTextureLayer.m_channel);
			if ( rTextureLayer.m_intensity != DEFAULT_INTENSITY || m_exportDefaults )
				output( IDTF_TEXTURE_LAYER_INTENSITY, rTextureLayer.m_intensity );
			if ( rTextureLayer.m_blendFunction != DEFAULT_BLEND_FUNCTION || m_exportDefaults )
				output( IDTF_TEXTURE_LAYER_BLEND_FUNCTION, rTextureLayer.m_blendFunction );
			if ( rTextureLayer.m_blendSource != DEFAULT_BLEND_SOURCE || m_exportDefaults )
				output( IDTF_TEXTURE_LAYER_BLEND_SOURCE, rTextureLayer.m_blendSource );
			if ( rTextureLayer.m_blendConstant != DEFAULT_BLEND_CONSTANT || m_exportDefaults )
				output( IDTF_TEXTURE_LAYER_BLEND_CONSTANT, rTextureLayer.m_blendConstant );
			if ( rTextureLayer.m_mode != DEFAULT_TEXTURE_MODE || m_exportDefaults )
				output( IDTF_TEXTURE_LAYER_MODE, rTextureLayer.m_mode );
			if ( rTextureLayer.m_alphaEnabled != L"FALSE" || m_exportDefaults )
				output( IDTF_TEXTURE_LAYER_ALPHA_ENABLED, rTextureLayer.m_alphaEnabled );
			if ( rTextureLayer.m_alphaEnabled != L"UV" || m_exportDefaults )
				output( IDTF_TEXTURE_LAYER_REPEAT, rTextureLayer.m_repeat );
			output( IDTF_TEXTURE_NAME, rTextureLayer.m_textureName );
			end();
		}
		end();
	}
	exportMeta( &rShaderResource );	
}

void IFXWriter::exportMaterialResource( const Material& rMaterialResource )
{
	output( IDTF_RESOURCE_NAME, rMaterialResource.GetName() );
	if ( rMaterialResource.m_ambientEnabled != L"TRUE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_AMBIENT_ENABLED, rMaterialResource.m_ambientEnabled );
	if ( rMaterialResource.m_diffuseEnabled != L"TRUE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_DIFFUSE_ENABLED, rMaterialResource.m_diffuseEnabled );
	if ( rMaterialResource.m_specularEnabled != L"TRUE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_SPECULAR_ENABLED, rMaterialResource.m_specularEnabled );
	if ( rMaterialResource.m_emissiveEnabled != L"TRUE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_EMISSIVE_ENABLED, rMaterialResource.m_emissiveEnabled );
	if ( rMaterialResource.m_reflectivityEnabled != L"TRUE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_REFLECTIVITY_ENABLED, rMaterialResource.m_reflectivityEnabled );
	if ( rMaterialResource.m_opacityEnabled != L"TRUE" || m_exportDefaults )
		output( IDTF_ATTRIBUTE_OPACITY_ENABLED, rMaterialResource.m_opacityEnabled );
	output( IDTF_MATERIAL_AMBIENT, rMaterialResource.m_ambient );
	output( IDTF_MATERIAL_DIFFUSE, rMaterialResource.m_diffuse );
	output( IDTF_MATERIAL_SPECULAR, rMaterialResource.m_specular );
	output( IDTF_MATERIAL_EMISSIVE, rMaterialResource.m_emissive );
	output( IDTF_MATERIAL_REFLECTIVITY, rMaterialResource.m_reflectivity );
	output( IDTF_MATERIAL_OPACITY, rMaterialResource.m_opacity );
	exportMeta( &rMaterialResource );	
}

void IFXWriter::exportTextureResource( const Texture& rTextureResource )
{
	output( IDTF_RESOURCE_NAME, rTextureResource.GetName() );
	if ( rTextureResource.GetHeight() > 0 )
		output( IDTF_TEXTURE_HEIGHT, rTextureResource.GetHeight() );
	if ( rTextureResource.GetWidth() > 0 )
		output( IDTF_TEXTURE_WIDTH, rTextureResource.GetWidth() );
	if ( rTextureResource.GetImageType() != L"RGB" || m_exportDefaults )
		output( IDTF_TEXTURE_IMAGE_TYPE, rTextureResource.GetImageType() );
	if ( rTextureResource.GetImageFormatCount() > 0 )
	{
		const U32 ImageFormatCount = rTextureResource.GetImageFormatCount();
		if ( ! ( ImageFormatCount == 1 && rTextureResource.IsExternal() == false &&
				rTextureResource.GetImageFormat(0).m_compressionType == IDTF_IMAGE_COMPRESSION_TYPE_JPEG24 &&
				rTextureResource.GetImageFormat(0).m_alpha == IDTF_FALSE &&
				rTextureResource.GetImageFormat(0).m_red == IDTF_TRUE &&
				rTextureResource.GetImageFormat(0).m_green == IDTF_TRUE &&
				rTextureResource.GetImageFormat(0).m_blue == IDTF_TRUE &&
				rTextureResource.GetImageFormat(0).m_luminance == IDTF_FALSE )
			|| m_exportDefaults	)
		{
			output( IDTF_TEXTURE_IMAGE_COUNT, ImageFormatCount );
			begin( IDTF_IMAGE_FORMAT_LIST );
			for ( U32 ImageFormatIndex = 0; ImageFormatIndex < ImageFormatCount; ImageFormatIndex++ )
			{
				const ImageFormat& rImageFormat = rTextureResource.GetImageFormat( ImageFormatIndex );
				begin( IDTF_IMAGE_FORMAT, ImageFormatIndex );
				//						if ( rImageFormat.m_compressionType != L"JPEG24" || m_exportDefaults )
				output( IDTF_COMPRESSION_TYPE, rImageFormat.m_compressionType );
				if ( rImageFormat.m_alpha != L"FALSE" || m_exportDefaults )
					output( IDTF_ALPHA_CHANNEL, rImageFormat.m_alpha );
				if ( rImageFormat.m_blue != L"FALSE" || m_exportDefaults )
					output( IDTF_BLUE_CHANNEL, rImageFormat.m_blue );
				if ( rImageFormat.m_green != L"FALSE" || m_exportDefaults )
					output( IDTF_GREEN_CHANNEL, rImageFormat.m_green );
				if ( rImageFormat.m_red != L"FALSE" || m_exportDefaults )
					output( IDTF_RED_CHANNEL, rImageFormat.m_red );
				if ( rImageFormat.m_luminance != L"FALSE" || m_exportDefaults )
					output( IDTF_LUMINANCE, rImageFormat.m_luminance );
				if ( rImageFormat.m_urlList.GetUrlCount() > 0 )
					exportUrlList( rImageFormat.m_urlList );
				end();
			}
			end();
		}
	}
	if ( rTextureResource.IsExternal() == false )
		output( IDTF_TEXTURE_PATH, rTextureResource.GetPath() );
	if ( rTextureResource.m_textureImage.IsSet() )
	{
		IFXString outTGAFile( rTextureResource.GetPath() );
//		outTGAFile.Concatenate( L".tga" );
		rTextureResource.m_textureImage.Write( outTGAFile.Raw() );		
	}
	
	exportMeta( &rTextureResource );
}

void IFXWriter::exportMotionResource( const MotionResource& rMotionResource )
{
	output( IDTF_RESOURCE_NAME, rMotionResource.GetName() );
	const U32 MotionTrackCount = rMotionResource.GetMotionTrackCount();
	output( IDTF_MOTION_TRACK_COUNT, MotionTrackCount );
	begin( IDTF_MOTION_TRACK_LIST );
	for ( U32 MotionTrackIndex = 0; MotionTrackIndex < MotionTrackCount; MotionTrackIndex++ )
	{
		const MotionTrack& rMotionTrack = rMotionResource.GetMotionTrack( MotionTrackIndex );
		begin( IDTF_MOTION_TRACK, MotionTrackIndex );
		output( IDTF_MOTION_TRACK_NAME, rMotionTrack.m_name );
		const U32 KeyFrameCount = rMotionTrack.GetKeyFrameCount();
		output( IDTF_MOTION_TRACK_SAMPLE_COUNT, KeyFrameCount );
		begin( IDTF_KEY_FRAME_LIST );
		for ( U32 KeyFrameIndex = 0; KeyFrameIndex < KeyFrameCount ; KeyFrameIndex++ )
		{
			const KeyFrame& rKeyFrame = rMotionTrack.GetKeyFrame( KeyFrameIndex );
			setPrecision( 1 );
			begin( IDTF_KEY_FRAME, KeyFrameIndex );
			output( IDTF_KEY_FRAME_TIME, rKeyFrame.m_time );
			output( IDTF_KEY_FRAME_DISPLACEMENT, rKeyFrame.m_displacement );
			output( IDTF_KEY_FRAME_ROTATION, rKeyFrame.m_rotation );
			output( IDTF_KEY_FRAME_SCALE, rKeyFrame.m_scale );
			end();
			restorePrecision();
		}
		end();
		end();
	}
	end();
	exportMeta( &rMotionResource );
}

void IFXWriter::exportResources( const SceneResources* pSceneResources)
{
	
	if ( pSceneResources->GetViewResourceList().GetResourceCount() > 0)
	{
		const ViewResourceList& rViewResourceList = pSceneResources->GetViewResourceList();
		const U32 ViewResourceCount = rViewResourceList.GetResourceCount();
		begin( IDTF_RESOURCE_LIST, IDTF_VIEW, true);
		output( IDTF_RESOURCE_COUNT, ViewResourceCount);
		for ( U32 ViewResourceIndex = 0; ViewResourceIndex < ViewResourceCount; ViewResourceIndex++ )
		{
			begin( IDTF_RESOURCE, ViewResourceIndex );
			exportViewResource( rViewResourceList.GetResource( ViewResourceIndex ) );
			end();
		}
		end();
		newline();
	}
	
	if ( pSceneResources->GetLightResourceList().GetResourceCount() > 0)
	{
		const LightResourceList& rLightResourceList = pSceneResources->GetLightResourceList();
		const U32 LightResourceCount = rLightResourceList.GetResourceCount();
		begin( IDTF_RESOURCE_LIST, IDTF_LIGHT, true);
		output( IDTF_RESOURCE_COUNT, LightResourceCount);
		for ( U32 LightResourceIndex = 0; LightResourceIndex < LightResourceCount; LightResourceIndex++ )
		{
			begin( IDTF_RESOURCE, LightResourceIndex );
			exportLightResource( rLightResourceList.GetResource( LightResourceIndex ) );
			end();
		}
		end();
		newline();
	}

	if ( pSceneResources->GetModelResourceList().GetResourceCount() > 0)
	{
		const ModelResourceList& rModelResourceList = pSceneResources->GetModelResourceList();
		const U32 ModelResourceCount = rModelResourceList.GetResourceCount();
		begin( IDTF_RESOURCE_LIST, IDTF_MODEL, true);
		output( IDTF_RESOURCE_COUNT, ModelResourceCount);
		for ( U32 ModelResourceIndex = 0; ModelResourceIndex < ModelResourceCount; ModelResourceIndex++ )
		{
			begin( IDTF_RESOURCE, ModelResourceIndex );
			exportModelResource( rModelResourceList.GetResource( ModelResourceIndex ) );
			end();
		}
		end();
		newline();
	}
	
	if ( pSceneResources->GetShaderResourceList().GetResourceCount() > 0)
	{
		const ShaderResourceList& rShaderResourceList = pSceneResources->GetShaderResourceList();
		const U32 ShaderResourceCount = rShaderResourceList.GetResourceCount();
		begin( IDTF_RESOURCE_LIST, IDTF_SHADER, true);
		output( IDTF_RESOURCE_COUNT, ShaderResourceCount);
		for ( U32 ShaderResourceIndex = 0; ShaderResourceIndex < ShaderResourceCount; ShaderResourceIndex++ )
		{
			begin( IDTF_RESOURCE, ShaderResourceIndex );
			exportShaderResource( rShaderResourceList.GetResource( ShaderResourceIndex ) );
			end();
		}
		end();
		newline();
	}
	
	if ( pSceneResources->GetMaterialResourceList().GetResourceCount() > 0)
	{
		const MaterialResourceList& rMaterialResourceList = pSceneResources->GetMaterialResourceList();
		const U32 MaterialResourceCount = rMaterialResourceList.GetResourceCount();
		begin( IDTF_RESOURCE_LIST, IDTF_MATERIAL, true);
		output( IDTF_RESOURCE_COUNT, MaterialResourceCount );
		for ( U32 MaterialResourceIndex = 0; MaterialResourceIndex < MaterialResourceCount; MaterialResourceIndex++ )
		{
			begin( IDTF_RESOURCE, MaterialResourceIndex );
			exportMaterialResource( rMaterialResourceList.GetResource( MaterialResourceIndex ) );
			end();
		}
		end();
		newline();
	}
	
	if ( pSceneResources->GetTextureResourceList().GetResourceCount() > 0)
	{
		const TextureResourceList& rTextureResourceList = pSceneResources->GetTextureResourceList();
		const U32 TextureResourceCount = rTextureResourceList.GetResourceCount();
		begin( IDTF_RESOURCE_LIST, IDTF_TEXTURE, true);
		output( IDTF_RESOURCE_COUNT, TextureResourceCount);
		for ( U32 TextureResourceIndex = 0; TextureResourceIndex < TextureResourceCount; TextureResourceIndex++ )
		{
			begin( IDTF_RESOURCE, TextureResourceIndex );
			exportTextureResource( rTextureResourceList.GetResource( TextureResourceIndex ) );
			end();
		}
		end();
		newline();
	}
	
	if ( pSceneResources->GetMotionResourceList().GetResourceCount() > 0)
	{
		const MotionResourceList& rMotionResourceList = pSceneResources->GetMotionResourceList();
		const U32 MotionResourceCount = rMotionResourceList.GetResourceCount();
		begin( IDTF_RESOURCE_LIST, IDTF_MOTION, true);
		output( IDTF_RESOURCE_COUNT, MotionResourceCount);
		for ( U32 MotionResourceIndex = 0; MotionResourceIndex < MotionResourceCount; MotionResourceIndex++ )
		{
			begin( IDTF_RESOURCE, MotionResourceIndex );
			exportMotionResource( rMotionResourceList.GetResource( MotionResourceIndex ) );
			end();
		}
		end();
		newline();
	}
}

void IFXWriter::exportShadingDescription( const ModelResource* pModelResource )
{
	const ModelDescription& ModelDescription = pModelResource->m_modelDescription;
	output( IDTF_MODEL_SHADING_COUNT, ModelDescription.shadingCount );
	if ( ModelDescription.shadingCount > 0 )
	{
		begin( IDTF_MODEL_SHADING_DESCRIPTION_LIST );
		for ( I32 index = 0; index < ModelDescription.shadingCount; index++ )
		{
			begin( IDTF_SHADING_DESCRIPTION, index );
			const ShadingDescription& rShadingDescription = pModelResource->m_shadingDescriptions.GetShadingDescription( index );
			output( IDTF_TEXTURE_LAYER_COUNT, rShadingDescription.m_textureLayerCount );
			if ( rShadingDescription.m_textureLayerCount > 0 )
			{
				begin( IDTF_TEXTURE_COORD_DIMENSION_LIST );
				for ( U32 jndex = 0; jndex < rShadingDescription.m_textureLayerCount; jndex++ )
				{
					output( IDTF_TEXTURE_LAYER, jndex );
					output( IDTF_TEXTURE_LAYER_DIMENSION, rShadingDescription.GetTextureCoordDimention(jndex), false );
				}
				end();
			}
			output( IDTF_SHADER_ID, rShadingDescription.m_shaderId );
			end();
		}
		end();
	}
}

void IFXWriter::exportModifiers( const ModifierList* pModifierList )
{
	const U32 ModifierCount = pModifierList->GetModifierCount();
	for ( U32 ModifierIndex = 0; ModifierIndex < ModifierCount; ModifierIndex++ )
		exportModifier( pModifierList->GetModifier( ModifierIndex ) );
}

void IFXWriter::exportModifier( const Modifier* pModifier )
{
	const IFXString& rModifierType = pModifier->GetType(); 
	begin( IDTF_MODIFIER, pModifier->GetType(), true );
	output( IDTF_MODIFIER_NAME, pModifier->GetName() );
	if ( pModifier->GetChainType() != IDTF_NODE || m_exportDefaults )
		output( IDTF_MODIFIER_CHAIN_TYPE, pModifier->GetChainType() );
	begin( IDTF_PARAMETERS );
	if ( rModifierType == IDTF_SHADING_MODIFIER )
		exportShadingModifier( static_cast< const ShadingModifier* >(pModifier) );
	else if( rModifierType == IDTF_ANIMATION_MODIFIER )
		exportAnimationModifier( static_cast< const AnimationModifier* >(pModifier) );
	else if( rModifierType == IDTF_BONE_WEIGHT_MODIFIER )
		exportBoneWeightModifier( static_cast< const BoneWeightModifier* >(pModifier) );
	else if( rModifierType == IDTF_CLOD_MODIFIER )
		exportCLODModifier( static_cast< const CLODModifier* >(pModifier) );
	else if( rModifierType == IDTF_SUBDIVISION_MODIFIER )
		exportSubdivisionModifier( static_cast< const SubdivisionModifier* >(pModifier) );
	else if( rModifierType == IDTF_GLYPH_MODIFIER )
		exportGlyphModifier( static_cast< const GlyphModifier* >(pModifier) );
	end();
	exportMeta( pModifier );
	end();
	newline();
}

void IFXWriter::exportShadingModifier( const ShadingModifier* pShadingModifier )
{
	const U32 Attributes = pShadingModifier->GetAttributes();                                                                                                                                  
	const U32 ATTRMESH = 1;
	const U32 ATTRLINE = 1 << 1;
	const U32 ATTRPOINT = 1 << 2;
	const U32 ATTRGLYPH = 1 << 3;
	if ( Attributes != ( ATTRMESH | ATTRLINE | ATTRPOINT | ATTRGLYPH ) || m_exportDefaults )
	{
		if ( Attributes & ATTRGLYPH || m_exportDefaults )
			output( IDTF_ATTRIBUTE_GLYPH, IDTF_TRUE );
		if ( Attributes & ATTRMESH || m_exportDefaults )
			output( IDTF_ATTRIBUTE_MESH, IDTF_TRUE );
		if ( Attributes & ATTRLINE || m_exportDefaults )
			output( IDTF_ATTRIBUTE_LINE, IDTF_TRUE );
		if ( Attributes & ATTRPOINT || m_exportDefaults )
			output( IDTF_ATTRIBUTE_POINT, IDTF_TRUE );
	}
	const U32 ShaderListCount = pShadingModifier->GetShaderListCount();
	output( IDTF_SHADER_LIST_COUNT, ShaderListCount );
	begin( IDTF_SHADER_LIST_LIST );
	for ( U32 ShaderListIndex = 0; ShaderListIndex < ShaderListCount; ShaderListIndex++ )
	{
		const ShaderList& rShaderList = pShadingModifier->GetShaderList( ShaderListIndex );
		begin( IDTF_SHADER_LIST, ShaderListIndex );
		const U32 ShaderCount = rShaderList.GetShaderCount();
		output( IDTF_SHADER_COUNT, ShaderCount );
		begin( IDTF_SHADER_NAME_LIST );
		for( U32 ShaderIndex = 0; ShaderIndex < ShaderCount; ShaderIndex++ )
		{
			output( IDTF_SHADER, ShaderIndex );
			output( IDTF_SHADER_NAME, rShaderList.GetShaderName( ShaderIndex ), false );
		}
		end();
		end();
	}
	
	end();
}

void IFXWriter::exportAnimationModifier( const AnimationModifier* pAnimationModifier )
{
	output( IDTF_ATTRIBUTE_ANIMATION_PLAYING, pAnimationModifier->m_playing ? IDTF_TRUE : IDTF_FALSE );
	output( IDTF_ATTRIBUTE_ROOT_BONE_LOCKED, pAnimationModifier->m_rootLock ? IDTF_TRUE : IDTF_FALSE );
	output( IDTF_ATTRIBUTE_SINGLE_TRACK, pAnimationModifier->m_singleTrack ? IDTF_TRUE : IDTF_FALSE );
	output( IDTF_ATTRIBUTE_AUTO_BLEND, pAnimationModifier->m_autoBlend ? IDTF_TRUE : IDTF_FALSE );
	setPrecision( 1 );
	output( IDTF_TIME_SCALE, pAnimationModifier->GetTimeScale() );
	output( IDTF_BLEND_TIME, pAnimationModifier->GetBlendTime() );
	const U32 MotionInfoCount = pAnimationModifier->GetMotionInfoCount();
	if ( MotionInfoCount > 0 )
	{
		output( IDTF_MOTION_COUNT, MotionInfoCount );
		begin( IDTF_MOTION_INFO_LIST );
		for( U32 MotionInfoIndex = 0; MotionInfoIndex < MotionInfoCount; MotionInfoIndex++ )
		{
			const MotionInfo& rMotionInfo = pAnimationModifier->GetMotionInfo( MotionInfoIndex );
			begin( IDTF_MOTION_INFO, MotionInfoIndex );
			output( IDTF_MOTION_NAME, rMotionInfo.m_name );
			output( IDTF_ATTRIBUTE_LOOP, rMotionInfo.m_loop ? IDTF_TRUE : IDTF_FALSE );
			output( IDTF_ATTRIBUTE_SYNC, rMotionInfo.m_sync ? IDTF_TRUE : IDTF_FALSE );
			output( IDTF_TIME_OFFSET, rMotionInfo.m_timeOffset );
			output( IDTF_TIME_SCALE, rMotionInfo.m_timeScale );
			end();
		}
		end();
	}
	restorePrecision();
}

void IFXWriter::exportBoneWeightModifier( const BoneWeightModifier* pBoneWeightModifier )
{
// likely one more workaround for a bug
	if ( pBoneWeightModifier->GetAttributes().Length() == 0 )
		output( IDTF_ATTRIBUTES, IDTF_MESH );
	else
		output( IDTF_ATTRIBUTES, pBoneWeightModifier->GetAttributes() );
	setPrecision( 1 );
	output( IDTF_INVERSE_QUANT, pBoneWeightModifier->GetInverseQuant() );
	restorePrecision();
	const U32 BoneWeightListCount = pBoneWeightModifier->GetBoneWeightListCount();
	output( IDTF_POSITION_COUNT, BoneWeightListCount );
	begin( IDTF_POSITION_BONE_WEIGHT_LIST );
	for( U32 BoneWeightListIndex = 0; BoneWeightListIndex < BoneWeightListCount; BoneWeightListIndex++ )
	{
		const BoneWeightList& rBoneWeightList = pBoneWeightModifier->GetBoneWeightList( BoneWeightListIndex );
		begin( IDTF_BONE_WEIGHT_LIST, BoneWeightListIndex );
		const U32 BoneIndexCount = rBoneWeightList.GetBoneIndexCount();
		output( IDTF_BONE_WEIGHT_COUNT, BoneIndexCount );
		if ( BoneIndexCount > 0 )
		{
			begin( IDTF_BONE_INDEX_LIST );
			for( U32 BoneIndexIndex = 0; BoneIndexIndex < BoneIndexCount; BoneIndexIndex++)
			{
				output( L"", rBoneWeightList.GetBoneIndex( BoneIndexIndex ) );
			}
			end();
//			untested part
			if ( BoneIndexCount != 1 )
			{
				begin( IDTF_BONE_WEIGHT_LIST );
				for( U32 BoneWeightIndex = 0; BoneWeightIndex < BoneIndexCount-1; BoneWeightIndex++ )
				{
					output( L"", rBoneWeightList.GetBoneWeight( BoneWeightIndex ) );
				}
				end();
			}
		}
		end();
	}
	end();
}

void IFXWriter::exportCLODModifier( const CLODModifier* pCLODModifier )
{
	output( IDTF_ATTRIBUTE_AUTO_LOD_CONTROL, pCLODModifier->GetAutoLODControl() );
	setPrecision( 2 );
	output( IDTF_LOD_BIAS, pCLODModifier->GetLODBias() );
	setPrecision( 1 );
	output( IDTF_CLOD_LEVEL, pCLODModifier->GetCLODLevel() );
	restorePrecision();
}

void IFXWriter::exportSubdivisionModifier( const SubdivisionModifier* pSubdivisionModifier )
{
	output( IDTF_ATTRIBUTE_ENABLED, pSubdivisionModifier->GetEnabled() );
	output( IDTF_ATTRIBUTE_ADAPTIVE, pSubdivisionModifier->GetAdaptive() );
	output( IDTF_DEPTH, pSubdivisionModifier->GetDepth() );
	setPrecision( 1 );
	output( IDTF_TENSION, pSubdivisionModifier->GetTension() );
	output( IDTF_ERROR, pSubdivisionModifier->GetError() );
	restorePrecision();
}

void IFXWriter::exportGlyphModifier( const GlyphModifier* pGlyphModifier )
{
	setPrecision( 1 );
	output( IDTF_ATTRIBUTE_BILLBOARD, pGlyphModifier->GetBillboard() );
//	bug ?
	if (  pGlyphModifier->GetSingleShader().Length() == 0 )
		output( IDTF_ATTRIBUTE_SINGLESHADER, IDTF_FALSE );
	else
		output( IDTF_ATTRIBUTE_SINGLESHADER, pGlyphModifier->GetSingleShader() );
	const U32 CommandCount = pGlyphModifier->GetCommandCount();
	output( IDTF_GLYPH_COMMAND_COUNT, CommandCount );
	if ( CommandCount > 0 )
	{
		begin( IDTF_GLYPH_COMMAND_LIST );
		for( U32 CommandIndex = 0; CommandIndex < CommandCount; CommandIndex++ )
		{
			const GlyphCommand* pGlyphCommand = pGlyphModifier->GetCommand( CommandIndex );
			begin( IDTF_GLYPH_COMMAND, CommandIndex );
			const IFXString& rType = pGlyphCommand->GetType();
			output( IDTF_GLYPH_COMMAND_TYPE, rType );
			if ( rType == IDTF_END_GLYPH )
			{
				const EndGlyph* pEndGlyph = static_cast< const EndGlyph* >(pGlyphCommand);
				output( IDTF_END_GLYPH_OFFSET_X, pEndGlyph->m_offset_x );
				output( IDTF_END_GLYPH_OFFSET_Y, pEndGlyph->m_offset_y );
			}
			else if ( rType == IDTF_MOVE_TO )
			{
				const MoveTo* pMoveTo = static_cast< const MoveTo* >(pGlyphCommand);
				output( IDTF_MOVE_TO_X, pMoveTo->m_moveto_x );
				output( IDTF_MOVE_TO_Y, pMoveTo->m_moveto_y );
			}
			else if ( rType == IDTF_LINE_TO )
			{
				const LineTo* pLineTo = static_cast< const LineTo* >(pGlyphCommand);
				output( IDTF_LINE_TO_X, pLineTo->m_lineto_x );
				output( IDTF_LINE_TO_Y, pLineTo->m_lineto_y );
			}
			else if ( rType == IDTF_CURVE_TO )
			{
				const CurveTo* pCurveTo = static_cast< const CurveTo* >(pGlyphCommand);
				output( IDTF_CONTROL1_X, pCurveTo->m_control1_x );
				output( IDTF_CONTROL1_Y, pCurveTo->m_control1_y );
				output( IDTF_CONTROL2_X, pCurveTo->m_control2_x );
				output( IDTF_CONTROL2_Y, pCurveTo->m_control2_y );
				output( IDTF_ENDPOINT_X, pCurveTo->m_endpoint_x );
				output( IDTF_ENDPOINT_Y, pCurveTo->m_endpoint_y );
			}
			end();
		}
		end();
	}
	output( IDTF_GLYPH_TM, pGlyphModifier->GetTM() );
	restorePrecision();
}

void IFXWriter::exportInt3List( const IFXCHAR* pToken, const I32 count, const IFXArray< Int3 >& rList)
{
	begin( pToken );
	for ( I32 index = 0; index < count; index++ )
	{
		const Int3& data = rList.GetElementConst( index );
		indent();
		output( data.GetA() );
		output( data.GetB() );
		output( data.GetC() );
	}
	end();
}

void IFXWriter::exportInt2List( const IFXCHAR* pToken, const I32 count, const IFXArray< Int2 >& rList)
{
	begin( pToken );
	for ( I32 index = 0; index < count; index++ )
	{
		const Int2& data = rList.GetElementConst( index );
		indent();
		output( data.GetA() );
		output( data.GetB() );
	}
	end();
}

void IFXWriter::exportIntList( const IFXCHAR* pToken, const I32 count, const IFXArray< I32 >& rList)
{
	begin( pToken );
	for ( I32 index = 0; index < count; index++ )
	{
		const I32& data = rList.GetElementConst( index );
		indent();
		output( data );
	}
	end();
}

void IFXWriter::exportPointList( const IFXCHAR* pToken, const I32 count, const IFXArray< Point >& rList)
{
	begin( pToken );
	for ( I32 index = 0; index < count; index++ )
	{
		const Point& data = rList.GetElementConst( index );
		output( L"", data );
	}
	end();
}

void IFXWriter::exportColorList( const IFXCHAR* pToken, const I32 count, const IFXArray< Color >& rList)
{
	begin( pToken );
	for ( I32 index = 0; index < count; index++ )
	{
		const Color& data = rList.GetElementConst( index );
		output( L"", data );
	}
	end();
}

void IFXWriter::exportUrlList( const UrlList& rList)
{
	const U32 UrlCount = rList.GetUrlCount();
	output( IDTF_URL_COUNT, UrlCount );
	begin( IDTF_URL_LIST );
	for ( U32 UrlIndex = 0; UrlIndex < UrlCount; UrlIndex++ )
	{
		output( IDTF_URL, UrlIndex );
		output( L"", rList.GetUrl( UrlIndex ), false );
	}
	end();
}

void IFXWriter::exportUrlList( const IFXArray<IFXString>& rList)
{
	const U32 UrlCount = rList.GetNumberElements();
	output( IDTF_URL_COUNT, UrlCount );
	begin( IDTF_URL_LIST );
	for ( U32 UrlIndex = 0; UrlIndex < UrlCount; UrlIndex++ )
	{
		output( IDTF_URL, UrlIndex );
		output( L"", rList.GetElementConst( UrlIndex ), false );
	}
	end();
}
//------------- PROTECTED METHODS -------------------------------------------

// output point3
void IFXWriter::output( const Point* p )
{
	output( p->GetPoint().XC() );
	output( p->GetPoint().YC() );
	output( p->GetPoint().ZC() );
}

// output color
void IFXWriter::output( const Color* p )
{
	output( p->GetColor().R() );
	output( p->GetColor().G() );
	output( p->GetColor().B() );
}

// output Int
void IFXWriter::output( const int in ) const
{
	fprintf( m_pFileHandle, "%d ", in );
}

// output unsigned Int
void IFXWriter::output( const unsigned int in ) const
{
	fprintf( m_pFileHandle, "%u ", in );
}

// output Float
void IFXWriter::output( const float in ) const
{
	char format[MAX_TOKEN_LEN];
	char prec[MAX_TOKEN_LEN];

	sprintf( prec, "%d", m_precision );
	if ( m_fixed != true )
	{
		strcpy( format, "%g " );
	}
	else
	{
		strcpy( format, "%." );
		strcat( format, prec );
		strcat( format, "f " );
	}
	fprintf( m_pFileHandle, format, in );
}
void IFXWriter::outputeol( const float in ) const
{
	char format[MAX_TOKEN_LEN];
	char prec[MAX_TOKEN_LEN];
  
	sprintf( prec, "%d", m_precision );
	if ( m_fixed != true )
	{
		strcpy( format, "%g " );
	}
	else
	{
		strcpy( format, "%." );
		strcat( format, prec );
		strcat( format, "f" );
	}
	fprintf( m_pFileHandle, format, in );
}

// output String
void IFXWriter::output( const IFXCHAR* in, bool isIndent )
{
	if( isIndent == true ) indent();
	{ toutf(in); fprintf( m_pFileHandle, "%s ", u8in ); freetmp(in); }
}

// output Indent
void IFXWriter::indent() const
{
	fprintf( m_pFileHandle, "\n" );
	for( int i = 0; i < m_indent; ++i )
	{
		fprintf( m_pFileHandle, "\t" );
	}
}

//------------- PRIVATE METHODS ---------------------------------------------
