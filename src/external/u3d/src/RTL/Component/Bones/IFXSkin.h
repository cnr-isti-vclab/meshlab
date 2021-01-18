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
	@file IFXSkin.h
*/

#ifndef IFXSKIN_H
#define IFXSKIN_H

#include "IFXVoidStar.h"
#include "IFXPackWeights.h"
#include "IFXBoneCacheArray.h"
#include "IFXVertexWeights.h"
#include "IFXMeshVertexMap.h"

class IFXMeshInterface;
class IFXCharacter;
class IFXMeshVertexMap;
class IFXMeshVertexArray;
class IFXCoreNode;

class IFXLongList: public IFXList<I32>
{
public:
	IFXLongList( void ) 
		{ SetAutoDestruct( TRUE ); };
};

class IFXLongArray: public IFXArray<I32> 
{
};

class IFXLongListArray: public IFXArray<IFXLongList> 
{
};

/**
	weights map into the mesh by index

	switching meshes without reseting weights is probably a bad idea

	input mesh holds original data with potential changing face structure
	output mesh holds deformed vertices and altered normals
	these may be the same mesh

	@note both meshes must be of the same implementation
*/
class IFXSkin
{
public:
	IFXSkin( void )
	{
		m_character = NULL;
		m_vertexordered = TRUE;
		m_inmesh = NULL;
		m_outmesh = NULL;
		m_invsqrt = NULL;
		CreateInvSqrtTable(  );
		m_bUseVectorUnit = FALSE;
	};

	virtual ~IFXSkin( void )
	{
		if( m_invsqrt )
			delete[] m_invsqrt;
	};

	BOOL GetVertexOrdered( void )   
		{ return m_vertexordered; };

	void SetVertexOrdered( BOOL set ) 
		{ m_vertexordered = set; };

	IFXMeshInterface *GetInputMesh( void ) 
		{ return m_inmesh; };

	const IFXMeshInterface *GetInputMeshConst( void ) const 
		{ return m_inmesh; };

	void SetInputMesh( IFXMeshInterface *set ) 
		{ m_inmesh = set; };

	IFXMeshInterface *GetOutputMesh( void ) 
		{ return m_outmesh;};

	const IFXMeshInterface *GetOutputMeshConst( void ) const 
		{ return m_outmesh;};

	void SetOutputMesh( IFXMeshInterface *set ) 
		{ m_outmesh=set; };

	void RegenerateWeights( BOOL use_joints, BOOL readonly );
	void FilterWeights( void );
	void SmoothWeights( I32 iterations, F32 threshhold, F32 weldmax, F32 modelsize );
	void RelinkWeights( void );
	void CalculateJointCrossSections( BOOL updatecache, BOOL computetips );
	void DeformMesh( BOOL renormalize, BOOL clod );
	void CalcBounds( IFXVector3 *min, IFXVector3 *max );
	void SetCharacter( IFXCharacter* set ) 
		{ m_character = set; };

	void ComputeVertexOffsets( void );
	void ComputePackVertexOffset( U32 meshid, IFXPackVertex *packvertex );
	void RemoveRogueWeights( void );
	IFXVertexWeights& GetVertexWeights( void ) 
		{ return m_vertexweights; };

	IFXArray<IFXMeshVertexMap> &GetReplicants( void ) 
		{ return m_replicants; };

	IFXArray<IFXMeshVertexArray> &GetMasterVertices( void ) 
		{ return m_mastervertices; };

	BOOL GetMasterMeshVertex( U32 meshid, U32 vertex, U32 *mmeshid, U32 *mvertex );
	void ClearWeights( void )
	{
		m_vertexweights.Clear(  );
		m_replicants.Clear(  );
	};

	void CalculateInfluences( F32 limit );
	void PackVertexWeights( void );
	void UnpackVertexWeights( void );

	/// Returns reference to m_packweightarray
	/// @param pInRequesterBy (optional) - pointer to callee, see comment 
	/// for #m_pPackVertexWeightsCreatorID 
	IFXArray<IFXPackWeights> &GetPackVertexWeights(
		void * const pInRequestedBy = NULL) 
	{
		m_pPackVertexWeightsCreatorID = pInRequestedBy;
		return m_packweightarray;
	};

	void* GetPackVertexWeightsCreatorID() 
	{
		return m_pPackVertexWeightsCreatorID;
	};

	IFXString Out( BOOL verbose = FALSE ) const;

private:
	BOOL CalcLocalOffset( I32 boneid, const IFXVector3 &world, IFXVector3 *offset );
	void SortVertexWeightsForCache( void );

	void ComputeDeformedVertices( BOOL clod );
	void ComputeDeformedVerticesPacked( BOOL clod );
	void ComputeDeformedVerticesPackedSSE( BOOL clod );

	void FindNeighbors( IFXMeshInterface *mesh, IFXArray<IFXMeshVertexMap> &neighbors );
	void MapWeights( IFXMeshInterface *mesh, IFXArray<IFXLongListArray> &map );

	void FloodFill( const IFXArray<IFXMeshVertexMap> &neighbors, 
					const IFXArray<IFXLongListArray> &map, 
					BOOL scanfirst, I32 weightid, I32 newboneid );

	void CreateInvSqrtTable( void );
	void NormalizeOutputNormals( void );

	void PrepareBoneCacheArray( void );

	static BOOL CalculateJointsForBone( IFXCoreNode &node, 
		IFXTransform &transform, IFXVariant state );

	static  void CalculateJointForBone( IFXCoreNode &node, 
		IFXTransform &transform, BOOL tip, BOOL computetips );

	IFXArray<IFXPackWeights> m_packweightarray;
	IFXVertexWeights m_vertexweights;
	IFXBoneCacheArray m_bonecachearray;

	/// replicated or colocated verts
	IFXArray<IFXMeshVertexMap> m_replicants;

	/// chosen master replicant/colocatee
	IFXArray<IFXMeshVertexArray> m_mastervertices;

	IFXCharacter *m_character;
	IFXMeshInterface *m_inmesh, *m_outmesh;
	F32 *m_invsqrt;
	BOOL m_vertexordered;
	BOOL m_bUseVectorUnit;

	/// Contains pointer to BoneWeightModifier that initialized m_packweightarray
	/// @note used in CIFXBoneWeightsModifier:: methods to allow to associate 
	/// m_packweightarray with his creator
	void *m_pPackVertexWeightsCreatorID;
};

#endif
