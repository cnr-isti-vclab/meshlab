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

/// @file	IFXVertexAttributes.h

#ifndef IFX_VERTEXATTRIBUTES_H
#define IFX_VERTEXATTRIBUTES_H

#define IFX_MESH_MAX_NUM_TEXCOORDS IFX_MAX_TEXUNITS

struct IFXVertexAttribData
{
	/** How many sets of texture coordinates are in this mesh (up to 8)?*/
	U32 m_uNumTexCoordLayers:4;
	
	/** Of the above, how many of these are interleaved 
	    into the position/normal/color data?*/
	U32 m_uNumHWTexCoordLayers:4;
	
	/** How many bytes are there per texture coordinate layer (1-4)?*/
	U32 m_uTexCoordSize0:2;
	U32 m_uTexCoordSize1:2;
	U32 m_uTexCoordSize2:2;
	U32 m_uTexCoordSize3:2;
	U32 m_uTexCoordSize4:2;
	U32 m_uTexCoordSize5:2;
	U32 m_uTexCoordSize6:2;
	U32 m_uTexCoordSize7:2;
	
	/** Does this mesh have position info?*/
	U32 m_bHasPositions:1;
	
	/** Does this mesh have normal info?*/
	U32 m_bHasNormals:1;
	
	/** Does this mesh have diffuse colors?*/
	U32 m_bHasDiffuseColors:1;
	
	/** Are the diffuse colors BGR (or RGB)?*/
	U32 m_bDiffuseIsBGR:1;
	
	/** Does this mesh have specular colors?*/
	U32 m_bHasSpecularColors:1;
	
	/** Are the specular colors BGR (or RGB)?*/
	U32 m_bSpecularIsBGR:1;
	
	/** Uses 32 bit indices (if not, indices are 16 bit).*/
	U32 m_b32BitIndices:1;
};

union IFXVertexAttributes
{
public:
	IFXVertexAttributes()
	{
		m_uAllData = 0;
		
		m_uData.m_uNumTexCoordLayers = 1;
		m_uData.m_uNumHWTexCoordLayers = 8;
		m_uData.m_uTexCoordSize0 = 2;
		m_uData.m_uTexCoordSize1 = 2;
		m_uData.m_uTexCoordSize2 = 2;
		m_uData.m_uTexCoordSize3 = 2;
		m_uData.m_uTexCoordSize4 = 2;
		m_uData.m_uTexCoordSize5 = 2;
		m_uData.m_uTexCoordSize6 = 2;
		m_uData.m_uTexCoordSize7 = 2;
		
		m_uData.m_bHasPositions = 1;
		m_uData.m_bHasNormals = 1;
		m_uData.m_bHasDiffuseColors = 0;
		m_uData.m_bHasSpecularColors = 0;
		m_uData.m_bDiffuseIsBGR = 1;
		m_uData.m_bSpecularIsBGR = 1;
		m_uData.m_b32BitIndices = 1;
	}
	
	IFXVertexAttribData m_uData;
	
	U32 m_uAllData;

	U32 GetTexCoordSize(U32 uLayer=0) const
	{
		U32 uRet = 2;
		switch(uLayer)
		{
		case 0:
			uRet = m_uData.m_uTexCoordSize0;
			break;
		case 1:
			uRet = m_uData.m_uTexCoordSize1;
			break;
		case 2:
			uRet = m_uData.m_uTexCoordSize2;
			break;
		case 3:
			uRet = m_uData.m_uTexCoordSize3;
			break;
		case 4:
			uRet = m_uData.m_uTexCoordSize4;
			break;
		case 5:
			uRet = m_uData.m_uTexCoordSize5;
			break;
		case 6:
			uRet = m_uData.m_uTexCoordSize6;
			break;
		case 7:
			uRet = m_uData.m_uTexCoordSize7;
			break;
		default:
			return 0;
		}
		if(uRet == 0)
		{
			uRet = 4;
		}

		return uRet;
	}
};

#ifdef _DEBUG
inline BOOL ValidTCAccess(U32 uTCSize, IFXVertexAttributes va, U32 uTexLayer)
{
	if(uTexLayer > va.m_uData.m_uNumTexCoordLayers)
		return FALSE;
	if(uTCSize > va.GetTexCoordSize(uTexLayer))
		return FALSE;

	return TRUE;
}
#endif

#endif
