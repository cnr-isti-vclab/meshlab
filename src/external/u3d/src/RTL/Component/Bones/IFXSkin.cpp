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
	@file IFXSkin.cpp
*/

#include "IFXSkin.h"
#include "IFXLong3.h"
#include "IFXMeshInterface.h"
#include "IFXMeshVertexMap.h"
#include "IFXCharacter.h"
#include "IFXBoneNodeList.h"

#define IFXSKIN_STRICT              FALSE

#define IFXSKIN_NEIGHBOR_DEBUG      FALSE
#define IFXSKIN_SMOOTH_DEBUG        FALSE
#define IFXSKIN_REGENERATE_DEBUG    FALSE
#define IFXSKIN_ROGUEPATCH_DEBUG    FALSE
#define IFXSKIN_RELINK_DEBUG        FALSE
#define IFXSKIN_RELINK_BY_WEIGHTS   FALSE   ///< else by position

#define IFXSKIN_FILTER_DEBUG        FALSE
#define IFXSKIN_FILTER_MIN          0.001f

#define IFXJOINT_USEMOREWEIGHTS     TRUE    ///< include close relatives
#define IFXFIXED_JOINTS             FALSE   ///< counteracts joint generation
#define IFXFIXED_JOINTSIZE          2.0f
#define IFXELLIPTICAL_JOINTS        ( !IFXFIXED_JOINTS )

#define IFXJOINT_MAXRAD             5.0f    ///< joint max x, y over bonelength
#define IFXJOINT_MAXASPECT          5.0f    ///< joint max x/y or y/x
#define IFXJOINT_MAXDISPLACE        0.3f    ///< joint max cx/rx or cy/ry
#define IFXJOINT_MAXCHILDASPECT     1.0f    ///< joint max child base/parent tip
#define IFXJOINT_MAXTIPCHANGE       0.1f    ///< joint max ( tip-base )/length
#define IFXJOINT_MINTIPCHANGE       -0.5f   ///< joint min ( tip-base )/length

#define IFXDEFORM_VERTEXORIENTED    TRUE
#define IFXDEFORM_GLOBALORIENTED    TRUE
#define IFXSKIN_FASTNORMALIZE       TRUE
#define IFXSKIN_USE_SQRT_TABLE      TRUE
#define IFXSKIN_SQRT_TABLESIZE      200
#define IFXSKIN_SQRT_SCALESIZE      190

IFXString IFXSkin::Out( BOOL verbose ) const
{
	IFXString result( "IFXSkin::Out() not implemented" ); 
	return result; 
}


void IFXSkin::CreateInvSqrtTable( void )
{
	if( m_invsqrt )
		return; 

	m_invsqrt = new F32[IFXSKIN_SQRT_TABLESIZE]; 
	I32 m; 
	for( m = 1; m <IFXSKIN_SQRT_TABLESIZE; m++ )
		m_invsqrt[m] = 1.0f / ( F32 )sqrt( m / ( F32 )( IFXSKIN_SQRT_SCALESIZE - 1 ) ); 

	m_invsqrt[0] = 0.0f; 
}



/**
	@note should check bones, too
*/
void IFXSkin::CalcBounds( IFXVector3* min, IFXVector3* max )
{
	if( !m_inmesh )
	{
		min->Reset(); 
		max->Reset(); 
		return; 
	}

	I32 meshid, meshes = m_inmesh->GetNumberMeshes(); 

	if( !meshes )
	{
		min->Set( 100.0f, 100.0f, 100.0f ); 
		max->Set( -100.0f, -100.0f, -100.0f ); 
		return; 
	}

	min->Set( 1e9, 1e9, 1e9 ); 
	max->Set( -1e9, -1e9, -1e9 ); 
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_inmesh->ChooseMeshIndex( meshid ); 

		I32 m, vertex, vertices = m_inmesh->GetMaxNumberVertices(); 
		for( vertex = 0; vertex < vertices; vertex++ )
		{
			const F32 *verts = m_inmesh->GetVertexConst( vertex ); 
			for( m = 0; m < 3; m++ )
			{
				if( ( *min )[m] > verts[m] )
					( *min )[m] = verts[m]; 
				if( ( *max )[m] < verts[m] )
					( *max )[m] = verts[m]; 
			}
		}
	}
}


/******************************************************************************
	find all verts sharing a face or opposing the same edge

	NOTE only manifolds are considered
******************************************************************************/
void IFXSkin::FindNeighbors( IFXMeshInterface *mesh, 
										IFXArray <IFXMeshVertexMap > &neighbors )
{
	IFXArray <IFXMeshVertexMap > &m_replicants = GetReplicants(); 
	IFXArray <IFXLong3List > edges;     // verts that oppose an edge
	IFXMeshVertex mv1, mv2; 
	I32 vertex2, vertex3; 

	neighbors.Clear(); 

	I32 meshid, meshes = m_inmesh->GetNumberMeshes(); 
	neighbors.ResizeToAtLeast( meshes ); 

	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_inmesh->ChooseMeshIndex( meshid ); 
		edges.Clear(); 

		//* face mates
		mv1.GetMeshIndex() = meshid; 
		mv2.GetMeshIndex() = meshid; 
		I32 vertex, vertices = m_inmesh->GetMaxNumberVertices(); 

		neighbors[meshid].ResizeToAtLeast( vertices ); 

		for( vertex = 0; vertex < vertices; vertex++ )
			neighbors[meshid][vertex].SetAutoDestruct( true ); 

		IFXListContext context; 
		I32 m, face, faces = mesh->GetMaxNumberFaces(); 
		for( face = 0; face < faces; face++ )
		{
			const U32 *verts = mesh->GetFaceVerticesConst( face ); 
			for( m = 0; m < 3; m++ )
			{
				vertex = verts[m]; 
				vertex2 = verts[( m+1 )%3]; 
				vertex3 = verts[( m+2 )%3]; 

				mv1.GetVertexIndex() = vertex; 
				mv2.GetVertexIndex() = vertex2; 

				// mirror vertices on each other
				if( !neighbors[meshid][vertex].SearchForContent( context, &mv2 ) )
					**( neighbors[meshid][vertex]
											.Append( new IFXMeshVertex ) ) = mv2; 
				if( !neighbors[meshid][vertex2].SearchForContent( context, &mv1 ) )
					**( neighbors[meshid][vertex2]
											.Append( new IFXMeshVertex ) ) = mv1; 

				//* edge - make sure vertex2 is the larger index
				if( vertex2 < vertex )
				{
					I32 tmp = vertex2; 
					vertex2 = vertex; 
					vertex = tmp; 
				}

				edges.ResizeToAtLeast( vertex+1 ); 

				// find any existing edge
				BOOL found = false; 
				IFXLong3 *long3; 
				IFXListContext edgecontext; 
				edges[vertex].ToHead( edgecontext ); 
				while( ( long3 = edges[vertex].PostIncrement( edgecontext ) )!= NULL )
				{
					if( ( *long3 )[0] == vertex2 )
					{
						found = true; 
						I32 vertex4 = ( *long3 )[1]; 

#if IFXSKIN_NEIGHBOR_DEBUG
						// tag as negative to enhance printout
						mv1.GetVertexIndex() = -vertex3; 
						mv2.GetVertexIndex() = -vertex4; 
#else
						mv1.GetVertexIndex() = vertex3; 
						mv2.GetVertexIndex() = vertex4; 
#endif

						// mirror across edge
						if( !neighbors[meshid][vertex3]
												.SearchForContent( context, &mv2 ) )
							**( neighbors[meshid][vertex3]
												.Append( new IFXMeshVertex ) ) = mv2; 
						if( !neighbors[meshid][vertex4]
												.SearchForContent( context, &mv1 ) )
							**( neighbors[meshid][vertex4]
												.Append( new IFXMeshVertex ) ) = mv1; 
						break; 
					}
				}
				if( !found )
					( **edges[vertex].Append( new IFXLong3 ) )
													.Set( vertex2, vertex3, 0 ); 
			}
		}

		// NOTE verts nearby are potential replicants, 
		// but don't get used if they are already associated by face structure

		//* replicants
		for( vertex = 0; vertex < vertices; vertex++ )
		{
			IFXMeshVertexList &replist = m_replicants[meshid][vertex]; 
			IFXMeshVertex *mv; 
			IFXListContext context, context2; 

			replist.ToHead( context ); 
			while( ( mv = replist.PostIncrement( context ) )!= NULL )
			{
				if( !neighbors[meshid][vertex]
										.SearchForContent( context2, mv ) )
					**( neighbors[meshid][vertex].Append( new IFXMeshVertex ) ) = *mv; 
			}
		}
	}

#if IFXSKIN_NEIGHBOR_DEBUG
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_inmesh->ChooseMeshIndex( meshid ); 
		I32 vertex, vertices = m_inmesh->GetMaxNumberVertices(); 

		IFXListContext context; 
		IFXMeshVertex *mv; 
		for( vertex = 0; vertex < vertices; vertex++ )
		{
			IFXTRACE_GENERIC( L"%d:%d", meshid, vertex ); 
			neighbors[meshid][vertex].ToHead( context ); 
			while( ( mv = neighbors[meshid][vertex].PostIncrement( context ) )!= NULL )
			{
				IFXTRACE_GENERIC( L" %d:%d", mv->GetMeshIndex(), mv->GetVertexIndex() ); 

				if( mv->GetVertexIndex() < 0 )
					mv->GetVertexIndex() *= -1; 
			}
			IFXTRACE_GENERIC( L"\n" ); 
		}
	}
#endif
}


/******************************************************************************
	find all weights for each vertex
******************************************************************************/
void IFXSkin::MapWeights( IFXMeshInterface *mesh, IFXArray <IFXLongListArray > &map )
{
	map.Clear(); 

	I32 meshid, meshes = m_inmesh->GetNumberMeshes(); 
	map.ResizeToAtLeast( meshes ); 
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_inmesh->ChooseMeshIndex( meshid ); 
		I32 vertex, vertices = m_inmesh->GetMaxNumberVertices(); 

		map[meshid].ResizeToAtLeast( vertices ); 
		for( vertex = 0; vertex < vertices; vertex++ )
			map[meshid][vertex].SetAutoDestruct( true ); 

		I32 m, length = m_vertexweights.GetNumberElements(); 
		//I32 faces = m_inmesh->GetMaxNumberFaces(); 

		// create mapping
		for( m = 0; m < length; m++ )
		{
			IFXVertexWeight &rVertexWeight = m_vertexweights[m]; 

			if( ( U32 )rVertexWeight.GetMeshIndex() == mesh->GetMeshIndex() )
			{
				vertex = rVertexWeight.GetVertexIndex(); 

				if( vertex < vertices )
					**( map[meshid][vertex].Append( new I32 ) ) = m; 
			}
		}
	}
}


/******************************************************************************
	merges split weights ( same vertex, same bone, multiple entries )
	eliminates zero weights
	eliminates weights with meshid or vertid out of bounds
******************************************************************************/
void IFXSkin::FilterWeights( void )
{
	// map each mesh/vert to n vert weights
	IFXArray <IFXLongListArray > map; 

	MapWeights( m_inmesh, map ); 

	I32 meshid, meshes = m_inmesh->GetNumberMeshes(); 

	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_inmesh->ChooseMeshIndex( meshid ); 
		I32 vertex, vertices = m_inmesh->GetMaxNumberVertices(); 

		// merge splits
		for( vertex = 0; vertex < vertices; vertex++ )
		{
			I32 *mapping1, *mapping2; 
			IFXListContext context1, context2; 

			map[meshid][vertex].ToHead( context1 ); 
			while( ( mapping1 = map[meshid][vertex].PostIncrement( context1 ) )!= NULL )
			{
				I32 bone1 = m_vertexweights[*mapping1].GetBoneIndex(); 

				context2 = context1; 

				while( ( mapping2 = map[meshid][vertex].PostIncrement( context2 ) )
																	!= NULL )
				{
					F32 weight1 = m_vertexweights[*mapping1]
														.GetBoneWeight(); 
					I32 bone2 = m_vertexweights[*mapping2].GetBoneIndex(); 

#if IFXSKIN_FILTER_DEBUG
					IFXTRACE_GENERIC( L"vs %d/%d %d, %d\n", meshid, vertex, bone1, bone2 ); 
#endif

					if( bone1 == bone2 )
					{
						F32 weight2 = m_vertexweights[*mapping2]
														.GetBoneWeight(); 

						m_vertexweights[*mapping1]
											.SetBoneWeight( weight1+weight2 ); 
						m_vertexweights[*mapping2].SetBoneWeight( 0.0f ); 

#if IFXSKIN_FILTER_DEBUG
						IFXTRACE_GENERIC( L"merge %d, %d %d/", *mapping1, *mapping2, meshid ); 
						IFXTRACE_GENERIC( L"%d ", m_vertexweights[*mapping1]
														.GetVertexIndex() ); 
						IFXTRACE_GENERIC( L"%d", m_vertexweights[*mapping2]
														.GetVertexIndex() ); 
						IFXTRACE_GENERIC( L" %.6G %.6G\n", weight1, weight2 ); 
#endif
					}
				}
			}
		}
	}

	I32 m, length = m_vertexweights.GetNumberElements(); 
	// remove near zero weights and any bonenodes out-of-bounds
	for( m = 0; m < length; m++ )
	{
		BOOL remove = false; 
		meshid = m_vertexweights[m].GetMeshIndex(); 
		m_inmesh->ChooseMeshIndex( meshid ); 
		I32 vertices = m_inmesh->GetMaxNumberVertices(); 

		if( meshid < 0 || meshid >= meshes )
			remove = true; 
		else
		{
			I32 vertex = m_vertexweights[m].GetVertexIndex(); 

			remove = ( vertex < 0 || vertex >= vertices ); 
		}

		if( remove || m_vertexweights[m].GetBoneWeight() <IFXSKIN_FILTER_MIN ||
				m_vertexweights[m].GetBoneIndex() < 0 ||
				m_vertexweights[m].GetBoneIndex() >= 
											m_character->GetBoneTableSize() )
		{
#if IFXSKIN_FILTER_DEBUG
	IFXTRACE_GENERIC( L"remove %d %d:%d ", m, meshid, m_vertexweights[m].GetVertexIndex() ); 
	IFXTRACE_GENERIC( L"of %d ", vertices ); 
	IFXTRACE_GENERIC( L"bone %d/%d weight %.6G\n", m_vertexweights[m].GetBoneIndex(), 
			m_character->GetBoneTableSize(), m_vertexweights[m].GetBoneWeight() ); 
#endif

			// deep copy
			m_vertexweights[m].CopyFrom( m_vertexweights[length-1] ); 
			length--; 
			m--; 
		}
	}
	m_vertexweights.ResizeToExactly( length ); 

}


/******************************************************************************
	clear existing weights
	set single weighting to closest bone

	first pass: use_joints = false
	second pass: use_joints = true

	if readonly, use existing vertex weights and just create proximity data
******************************************************************************/
void IFXSkin::RegenerateWeights( BOOL use_joints, BOOL readonly )
{
	const IFXVector3 zero( 0.0f, 0.0f, 0.0f ); 

	// map each mesh/vert to n vert weights
	IFXArray <IFXLongListArray > map; 
	IFXListContext context; 
	if( readonly )
		MapWeights( m_inmesh, map ); 
	else
		GetVertexWeights().Clear(); 

	I32 boneid, bones = m_character->GetBoneTableSize(); 
	IFXVector3 result; 

	for( boneid = 0; boneid < bones; boneid++ )
	{
		IFXBoneNode *bonenode = m_character->LookupBoneIndex( boneid ); 
		if( !bonenode )
			continue; 
		bonenode->BestVertexScore() = -1.0f; 
	}

	I32 meshid, meshes = m_inmesh->GetNumberMeshes(); 
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_inmesh->ChooseMeshIndex( meshid ); 
		I32 vertex, vertices = m_inmesh->GetMaxNumberVertices(); 

		for( vertex = 0; vertex < vertices; vertex++ )
		{
			I32 picked = -1; 
			F32 dist = 1.0f, pickdist = -1.0f; 
			boneid = 0; 

			I32 *mapping = NULL; 
			IFXLongList *maplist = NULL; 
			if( readonly )
			{
				maplist = &map[meshid][vertex]; 
				maplist->ToHead( context ); 
			}

			BOOL done = false; 
			while( !done )
			{
				if( readonly )
				{
					mapping = maplist->PostIncrement( context ); 

					if( !mapping )
						break; 

					boneid = m_vertexweights[*mapping].GetBoneIndex(); 
				}

				IFXBoneNode *bonenode = m_character->LookupBoneIndex( boneid ); 
				if( bonenode && ( !use_joints || bonenode->IsInfluential() ) )
				{
					F32 bonelength = bonenode->GetLength(); 
					I32 thisbone = boneid; 
					//BOOL has_children = ( bonenode->Children()
					//                              .GetNumberElements() > 0 ); 

					F32 basejointsize; 
					F32 tipjointsize; 

					I32 pass; 
					for( pass = 0; pass < 2; pass++ )
					{
						if( !pass )
						{
							//* check bone
							bonenode->StoredTransform()
									.ReverseTransformVector( 
									m_inmesh->GetVertexConst( vertex ), result ); 

							IFXCylinder &bounds = bonenode
													->CylindricalBounds(); 
							IFXVector3 &basescale = bounds.GetScale( 0 ); 

							basejointsize = 0.5f*( basescale[1]+basescale[2] ); 

							// use real tip data
							IFXVector3 &tipscale = bounds.GetScale( 1 ); 
							tipjointsize = 0.5f*( tipscale[1]+tipscale[2] ); 
						}
						else
						{
							//* check implant ( and give credit to parent )
							if( !( bonenode->Displacement() == zero ) &&
											bonenode->Parent()->IsBone() )
							{
								IFXBoneNode *parent = 
										( IFXBoneNode * )bonenode->Parent(); 

								IFXTransform implant = parent
														->StoredTransform(); 
								implant.Translate( parent->GetLength(), 
																	0.0f, 0.0f ); 
								implant.Rotate( 
									bonenode->ImplantReferenceRotation() ); 
								implant.ReverseTransformVector( 
									m_inmesh->GetVertexConst( vertex ), result ); 

								bonelength = bonenode->ImplantLength(); 
								thisbone = parent->GetBoneIndex(); 

								// implant tip uses this bone's base
								IFXCylinder &bounds = 
											bonenode->CylindricalBounds(); 
								IFXVector3 &jointscale = bounds.GetScale( 0 ); 
								tipjointsize = 0.5f*( jointscale[1]+jointscale[2] ); 

								// implant base uses parent's tip
								IFXCylinder &parentbounds = 
												parent->CylindricalBounds(); 
								IFXVector3 &parentscale = parentbounds
																.GetScale( 1 ); 
								basejointsize = 0.5f*
											( parentscale[1]+parentscale[2] ); 

								/// @todo: reduce influence when an implant
								basejointsize *= 0.9f; 
								tipjointsize *= 0.9f; 
							}
							else
								continue; 
						}

						F32 jointsize; 
						if( result[0] > 0.0f )
						{
							if( result[0] > bonelength )
							{
								// after tip
								result[0] -= bonelength; 
								jointsize = 1.0f; 

								jointsize -= result[0]/bonelength; 
								if( jointsize < 0.1f )
									jointsize = 0.1f; 
								jointsize *= tipjointsize; 
							}
							else
							{
								// adjacent to bone
								if( bonelength > 0.0f )
									jointsize = basejointsize+
												( tipjointsize-basejointsize )
													*result[0]/bonelength; 
								else
									jointsize = basejointsize; 

								result[0] = 0.0f; 
							}
						}
						else
						{
							// before base
							jointsize = ( 1.0f+result[0]/bonelength ); 
							if( jointsize < 0.1f )
								jointsize = 0.1f; 
							jointsize *= basejointsize; 
						}

						// reduce effect of maverick cross-sections
						const F32 maxaspect = 2.0f; 
						if( jointsize > bonelength*maxaspect )
							jointsize = bonelength*maxaspect; 

						if( !use_joints )
							jointsize = 1.0f; 

						if( jointsize <= 0.0f )
							continue; 

						result.Scale( 1.0f/jointsize ); 

						dist = result.CalcMagnitude(); 
						if( picked < 0 || dist < pickdist )
						{
							picked = thisbone; 
							pickdist = dist; 
						}

#if IFXSKIN_REGENERATE_DEBUG
						if( meshid == 1 && vertex == 447 )
						{
							IFXTRACE_GENERIC( L"vert %d:%d bone %d len %.6G %.6G, %.6G = %.6G", 
								meshid, vertex, boneid, bonelength, 
								basejointsize, tipjointsize, jointsize ); 
							IFXTRACE_GENERIC( L" to %ls dist %.6G picked = %d\n", 
								result.Out().Raw(), dist, picked ); 
						}
#endif
					}
				}

				if( readonly )
					m_vertexweights[*mapping].Offset()[0] = dist;       // ok?
				else
					done = ( ++boneid >= bones ); 
			}

#if IFXSKIN_STRICT
			IFXASSERT( picked >= 0 ); 
#endif
			if( picked >= 0 )
			{
				// remember which vertex scored best for each bone
				IFXBoneNode *bonenode = m_character->LookupBoneIndex( picked ); 
				IFXASSERT( bonenode ); 
				F32 &bestscore = bonenode->BestVertexScore(); 
				if( bestscore < 0.0f || pickdist < bestscore )
				{
					bestscore = pickdist; 
					bonenode->BestVertex().SetMeshVertex( meshid, vertex ); 
				}

				if( !readonly )
				{
					IFXVertexWeight &weight = GetVertexWeights()
														.CreateNewElement(); 
					weight.SetBoneIndex( picked ); 
					weight.SetMeshIndex( meshid ); 
					weight.SetVertexIndex( vertex ); 
					weight.SetBoneWeight( 1.0f ); 
					weight.Offset()[0] = pickdist;  // temporarily save score
				}
			}
		}
	}
}


/******************************************************************************
	expects offset[0] to contain proximity score
	uses offset[1] as a validity flag

	finds and replaces disjoint patches of weights
******************************************************************************/
void IFXSkin::RemoveRogueWeights( void )
{
	IFXTRACE_GENERIC( L"RemoveRogueWeights()\n" ); 

	// map each mesh/vert to n mesh/vert's
	IFXArray <IFXMeshVertexMap > neighbors; 
	// map each mesh/vert to n vert weights
	IFXArray <IFXLongListArray > map; 
	IFXListContext longcontext; 
	I32 *mapping; 

	FindNeighbors( m_inmesh, neighbors ); 
	MapWeights( m_inmesh, map ); 

	// reset validity flag
	I32 m, length = m_vertexweights.GetNumberElements(); 
	for( m = 0; m < length; m++ )
	{
		IFXVertexWeight &rVertexWeight = m_vertexweights[m]; 
		rVertexWeight.Offset()[1] = 0.0f;        // clear validity flag
	}

#if IFXSKIN_ROGUEPATCH_DEBUG
	IFXTRACE_GENERIC( L"Rogue: fill valid patches <  <  <  <  <  <  <  <  <  <  <  <  <  < \n" ); 
#endif
	// propagate valid patches from best vertex of each bone
	I32 boneid, bones = m_character->GetBoneTableSize(); 
	for( boneid = 0; boneid < bones; boneid++ )
	{
		IFXBoneNode *bonenode = m_character->LookupBoneIndex( boneid ); 
		if( !bonenode )
			continue; 
		if( bonenode->BestVertexScore() >= 0.0f )
		{
			U32 meshid = 0xffffffff, vertid = 0xffffffff; 
			bonenode->BestVertex().GetMeshVertex( &meshid, &vertid ); 

			if( meshid!= 0xffffffff && vertid!= 0xffffffff )
			{
				map[meshid][vertid].ToHead( longcontext ); 
				while( ( mapping = map[meshid][vertid]
										.PostIncrement( longcontext ) ) != NULL )
				{
					if( mapping && m_vertexweights[*mapping].GetBoneIndex()
																	 == boneid )
					{
						FloodFill( neighbors, map, false, *mapping, boneid ); 
						break; 
					}
				}
			}
		}
	}

#if IFXSKIN_ROGUEPATCH_DEBUG
	IFXTRACE_GENERIC( L"Rogue: fill invalid patches <  <  <  <  <  <  <  <  <  <  <  <  <  < \n" ); 
#endif
	//* fill in invalid patches with valid data
	for( m = 0; m < length; m++ )
	{
		IFXVertexWeight &rVertexWeight = m_vertexweights[m]; 
		I32 meshid = rVertexWeight.GetMeshIndex(); 
		I32 vertid = rVertexWeight.GetVertexIndex(); 

		if( rVertexWeight.Offset()[1] == 0.0f )
		{
			// search neighbors
			IFXListContext context; 
			IFXMeshVertex *neighborvertex; 
			neighbors[meshid][vertid].ToHead( context ); 
			while( ( neighborvertex = 
					neighbors[meshid][vertid].PostIncrement( context ) ) != NULL )
			{
				I32 neighmesh = neighborvertex->GetMeshIndex(); 
				I32 neighvert = neighborvertex->GetVertexIndex(); 

				map[neighmesh][neighvert].ToHead( longcontext ); 
				while( ( mapping = map[neighmesh][neighvert]
										.PostIncrement( longcontext ) ) != NULL )
				{
					// if valid neighbor
					if( m_vertexweights[*mapping].Offset()[1] > 0.0f )
					{
						FloodFill( neighbors, map, false, m, 
									m_vertexweights[*mapping].GetBoneIndex() ); 
						break; 
					}
				}

				// if flooded
				if( mapping )
					break; 
			}
		}
	}

#if IFXSKIN_ROGUEPATCH_DEBUG
	IFXTRACE_GENERIC( L"Rogue: assign remaining orphans <  <  <  <  <  <  <  <  <  <  <  <  <  < \n" ); 
#endif
}


class IFXFloodLevel
{
	public:
		I32             weightindex; 
		IFXListContext  context; 
}; 
/******************************************************************************
	validity stored temporarily as non-zero weight
******************************************************************************/
void IFXSkin::FloodFill( const IFXArray <IFXMeshVertexMap > &neighbors, 
						const IFXArray <IFXLongListArray > &map, 
						BOOL scanfirst, I32 weightid, I32 newboneid )
{
	IFXArray <IFXFloodLevel > floodstack; 
	IFXListContext longcontext; 
	I32 *mapping; 

	const I32 oldboneid = m_vertexweights[weightid].GetBoneIndex(); 
	U32 meshid = m_vertexweights[weightid].GetMeshIndex(); 
	U32 vertid = m_vertexweights[weightid].GetVertexIndex(); 
	I32 boneid = newboneid; 

#if IFXSKIN_ROGUEPATCH_DEBUG
	IFXTRACE_GENERIC( L"FloodFill() vert %d:%d to boneid %d\n", meshid, vertid, boneid ); 
#endif

	F32 bestscore = 0.0f; 
	I32 pass; 
	for( pass = scanfirst; pass >= 0; pass-- )
	{
#if IFXSKIN_ROGUEPATCH_DEBUG
		IFXTRACE_GENERIC( L" PASS %d\n", pass ); 
#endif
		if( pass )
			boneid = -1; 
		else if( boneid < 0 )
			break; 

		floodstack.CreateNewElement().weightindex = weightid; 

		IFXASSERT( meshid < neighbors.GetNumberElements() ); 
		IFXASSERT( vertid < neighbors[meshid].GetNumberElements() ); 
		neighbors[meshid][vertid].ToHead( floodstack[0].context ); 

		while( floodstack.GetNumberElements() > 0 )
		{
			I32 level = floodstack.GetNumberElements()-1; 
			IFXVertexWeight &rVertexWeight = 
								m_vertexweights[floodstack[level].weightindex]; 

			if( pass )
			{
				if( boneid < 0 || bestscore > rVertexWeight.Offset()[0] )
				{
					boneid = rVertexWeight.GetBoneIndex(); 
					bestscore = rVertexWeight.Offset()[0]; 
				}
				rVertexWeight.SetBoneIndex( -1 ); 
			}
			else
				rVertexWeight.SetBoneIndex( boneid ); 

			rVertexWeight.Offset()[1] = 1.0f; 

			meshid = rVertexWeight.GetMeshIndex(); 
			vertid = rVertexWeight.GetVertexIndex(); 
#if IFXSKIN_ROGUEPATCH_DEBUG
			IFXTRACE_GENERIC( L" on %d:%d\n", meshid, vertid ); 
#endif

			// search neighbors
			IFXMeshVertex *neighborvertex; 
			IFXASSERT( meshid < neighbors.GetNumberElements() ); 
			IFXASSERT( vertid < neighbors[meshid].GetNumberElements() ); 
			if( ( neighborvertex = neighbors[meshid][vertid]
							.PostIncrement( floodstack[level].context ) ) != NULL )
			{
				I32 neighmesh = neighborvertex->GetMeshIndex(); 
				I32 neighvert = neighborvertex->GetVertexIndex(); 
#if IFXSKIN_ROGUEPATCH_DEBUG
				IFXTRACE_GENERIC( L" neighbor %d:%d\n", neighmesh, neighvert ); 
#endif

				IFXASSERT( ( U32 )neighmesh < map.GetNumberElements() ); 
				IFXASSERT( ( U32 )neighvert < map[neighmesh].GetNumberElements() ); 

				// find weight with matching boneid, if any
				map[neighmesh][neighvert].ToHead( longcontext ); 
				while( ( mapping = map[neighmesh][neighvert]
										.PostIncrement( longcontext ) ) != NULL )
				{
					I32 neighboneid = m_vertexweights[*mapping]
														.GetBoneIndex(); 

					// if invalid neighbor of oldboneid
					if( ( scanfirst || neighboneid == oldboneid ) &&
							( m_vertexweights[*mapping].Offset()[1] == 0.0f ||
							( !pass && neighboneid < 0 ) ) )
					{
#if IFXSKIN_ROGUEPATCH_DEBUG
						IFXTRACE_GENERIC( L" push\n" ); 
#endif
						level++; 
						floodstack.ResizeToExactly( level+1 ); 
						floodstack[level].weightindex = *mapping; 

						IFXASSERT( ( U32 )neighmesh < neighbors.GetNumberElements() ); 
						IFXASSERT( ( U32 )neighvert < neighbors[neighmesh]
													.GetNumberElements() ); 
						neighbors[neighmesh][neighvert].ToHead( 
													floodstack[level].context ); 
						break; 
					}
				}
			}
			else
			{
#if IFXSKIN_ROGUEPATCH_DEBUG
				IFXTRACE_GENERIC( L" pop\n" ); 
#endif

				// pop
				floodstack.ResizeToExactly( level ); 
			}
		}
	}
}


void IFXSkin::SmoothWeights( I32 iterations, F32 threshhold, 
												F32 weldmax, F32 modelsize )
{
#if IFXSKIN_SMOOTH_DEBUG
	GetVertexWeights().Dump(); 
#endif

//  F32 maxratio = threshhold/modelsize; 
	F32 maxratio = threshhold; 
	weldmax *= modelsize; 

	// map each mesh/vert to n mesh/vert's
	IFXArray <IFXMeshVertexMap > neighbors; 
	// map each mesh/vert to n vert weights
	IFXArray <IFXLongListArray > map; 

	FindNeighbors( m_inmesh, neighbors ); 
	MapWeights( m_inmesh, map ); 

	char progresstext[128]; 

	// smooth
	I32 pass; 
	for( pass = 0; pass < iterations; pass++ )
	{
		sprintf( progresstext, "Smooth Weights %d%%", 100*pass/iterations ); 
		IFXCharacter::StateProgress( progresstext ); 

		BOOL lastpass = ( pass == iterations-1 ); 

		I32 meshid, meshes = m_inmesh->GetNumberMeshes(); 
		for( meshid = 0; meshid < meshes; meshid++ )
		{
			m_inmesh->ChooseMeshIndex( meshid ); 
			I32 vertex, vertices = m_inmesh->GetMaxNumberVertices(); 

			for( vertex = 0; vertex < vertices; vertex++ )
			{
				I32 *mapping; 
				IFXListContext context1, context3; 
				IFXListContext context2; 
				map[meshid][vertex].ToHead( context1 ); 
				while( ( mapping = map[meshid][vertex].PostIncrement( context1 ) )
																	!= NULL )
				{
					I32 boneid = m_vertexweights[*mapping].GetBoneIndex(); 
					IFXBoneNode *bonenode = m_character->LookupBoneIndex( boneid ); 
					IFXASSERT( bonenode ); 

					IFXCylinder &bounds = bonenode->CylindricalBounds(); 
					IFXVector3 &jointscale = bounds.GetScale( 0 ); 
					F32 jointsize = 0.5f*( jointscale[1]+jointscale[2] ); 

					IFXMeshVertex *neighborvertex; 
					neighbors[meshid][vertex].ToHead( context2 ); 
					while( ( neighborvertex = 
							neighbors[meshid][vertex].PostIncrement( context2 ) )
																	!= NULL )
					{
						I32 neighmesh = neighborvertex->GetMeshIndex(); 
						I32 neighvert = neighborvertex->GetVertexIndex(); 
						F32 weight = m_vertexweights[*mapping]
														.GetBoneWeight(); 
						I32 found = -1; 
						F32 neighweight = 0.0f; 

						I32 *neighmapping; 
						map[neighmesh][neighvert].ToHead( context3 ); 
						while( ( neighmapping = map[neighmesh][neighvert]
											.PostIncrement( context3 ) )!= NULL )
						{
							I32 neighboneid = m_vertexweights[*neighmapping]
														.GetBoneIndex(); 

							if( neighboneid == boneid )
							{
								found = *neighmapping; 
								break; 
							}
						}

						if( found >= 0 )
							neighweight = m_vertexweights[found].GetBoneWeight(); 

						F32 delta = weight-neighweight; 

						m_inmesh->ChooseMeshIndex( neighmesh ); 
						const IFXVector3 &neighvector = m_inmesh
												->GetVertexConst( neighvert ); 
						m_inmesh->ChooseMeshIndex( meshid ); 

						IFXVector3 difference; 
						difference.Subtract( m_inmesh->GetVertexConst( vertex ), 
																neighvector ); 
						F32 distance = difference.CalcMagnitude(); 
						if( distance < weldmax )
							distance = 0.0f; 
						else if( lastpass )
							continue; 

						F32 maxdiff = distance*maxratio/jointsize; 

						F32 adjust = ( F32 )fabs( delta )-maxdiff; 
						//* if difference is too abrupt
						if( adjust > 0.0f )
						{
#if IFXSKIN_SMOOTH_DEBUG
	BOOL show = ( meshid == 0 && vertex == 485 ) ||
				( meshid == 9 && vertex == 1 ) ||
				( neighmesh == 0 && neighvert == 485 ) ||
				( neighmesh == 9 && neighvert == 1 ); 

	if( show )
	{
		IFXTRACE_GENERIC( L"vert %d:%d bone %d to %d:%d ", 
								meshid, vertex, boneid, neighmesh, neighvert ); 
		IFXTRACE_GENERIC( L"%.6G-%.6G = %.6G vs %.6G ", weight, neighweight, delta, maxdiff ); 
		IFXTRACE_GENERIC( L"dist %.6G\n", distance ); 
	}
#endif

							//* create new influence if neighbor not already
							if( found < 0 )
							{
								found = m_vertexweights.GetNumberElements(); 

#if IFXSKIN_SMOOTH_DEBUG
	if( show )
								IFXTRACE_GENERIC( L" create %d ", found ); 
#endif

								m_vertexweights.ResizeToAtLeast( found+1 ); 

								m_vertexweights[found]
											.CopyFrom( m_vertexweights[*mapping] ); 
								m_vertexweights[found]
											.SetMeshIndex( neighmesh ); 
								m_vertexweights[found]
											.SetVertexIndex( neighvert ); 
								m_vertexweights[found].SetBoneWeight( 0.0f ); 

								( **neighbors[neighmesh][neighvert]
										.Append( new IFXMeshVertex ) )
										.SetMeshVertex( meshid, vertex ); 
								**( map[neighmesh][neighvert] )
										.Append( new I32 ) = found; 
							}

							//* smooth difference
							adjust *= 0.5; 
							if( weight < neighweight )
								adjust = -adjust; 

							if( lastpass )
							{
								//* on last pass, only copy weights to
								//* make sure they are equal
								if( meshid <= neighmesh && vertex < neighvert )
									m_vertexweights[found].BoneWeight() = 
									m_vertexweights[*mapping].GetBoneWeight(); 
								else
									m_vertexweights[*mapping].BoneWeight() = 
										m_vertexweights[found].GetBoneWeight(); 
							}
							else
							{
								m_vertexweights[*mapping].BoneWeight() -= adjust; 
								m_vertexweights[found].BoneWeight() += adjust; 
							}

#if IFXSKIN_SMOOTH_DEBUG
	if( show )
	{
		IFXTRACE_GENERIC( L" adjust %.6G", adjust ); 
		IFXTRACE_GENERIC( L" %.6G", m_vertexweights[*mapping].GetBoneWeight() ); 
		IFXTRACE_GENERIC( L" %.6G\n", m_vertexweights[found].GetBoneWeight() ); 
	}
#endif
						}
					}
				}
			}
		}

			//* normalize
		for( meshid = 0; meshid < meshes; meshid++ )
		{
			m_inmesh->ChooseMeshIndex( meshid ); 
			I32 vertex, vertices = m_inmesh->GetMaxNumberVertices(); 
			for( vertex = 0; vertex < vertices; vertex++ )
			{
				F32 sum = 0.0f; 
				I32 *mapping; 
				IFXListContext context; 

				map[meshid][vertex].ToHead( context ); 
				while( ( mapping = map[meshid][vertex]
												.PostIncrement( context ) )!= NULL )
					sum += m_vertexweights[*mapping].GetBoneWeight(); 

#if IFXSKIN_STRICT
				IFXASSERT( sum > 0.0f ); 
#endif
				if( sum > 0.0f )
				{
					map[meshid][vertex].ToHead( context ); 
					while( ( mapping = map[meshid][vertex]
												.PostIncrement( context ) )!= NULL )
						m_vertexweights[*mapping].BoneWeight() /= sum; 
				}
#if IFXSKIN_SMOOTH_DEBUG
				else
					IFXTRACE_GENERIC( L"empty %d/%d\n", meshid, vertex ); 
#endif

				/// @todo: rework it
				if( sum < 0.1f )
					sum = 1.0f; 
			}
		}
	}
}


/******************************************************************************
	WARNING overwrites offset caching; use before ComputeVertexOffsets()

	stores weight ordered boneid in vertex and normal offsets
	then does the bonecache-optimised sort

	if GetVertexOrdered() == true, does not optimise bones, 
		so that verts are in ascending order
******************************************************************************/
void IFXSkin::SortVertexWeightsForCache( void )
{
	m_vertexweights.SortOnMeshThenVertex(); 
	if( GetVertexOrdered() )
		return; 

	const I32 maxweights = 6; 
	F32 weights[maxweights]; 
	I32 boneids[maxweights]; 

	I32 j, m, length = m_vertexweights.GetNumberElements(); 
	for( m = 0; m < length; m++ )
	{
		IFXVertexWeight &rVertexWeight = m_vertexweights[m]; 
		I32 vertid = rVertexWeight.GetVertexIndex(); 

		// find bounds of like verts
		I32 n = m; 
		I32 start = n; 
		while( n < length && m_vertexweights[n].GetVertexIndex() == vertid )
			n++; 
		I32 end = n; 

		// reset
		for( n = 0; n < maxweights; n++ )
		{
			weights[n] = 0.0f; 
			boneids[n] = -1; 
		}

		// get ordered weights
		for( n = start; n < end; n++ )
		{
			F32 weight = m_vertexweights[n].GetBoneWeight(); 
			I32 boneid = m_vertexweights[n].GetBoneIndex(); 

			j = maxweights-1; 
			while( j >= 0 && boneids[j] < boneid )
			{
				if( j < maxweights-1 )
				{
					weights[j+1] = weights[j]; 
					boneids[j+1] = boneids[j]; 
				}
				weights[j] = weight; 
				boneids[j] = boneid; 
				j--; 
			}
		}

		// fill in ordered weights
		for( n = 0; n < maxweights; n++ )
		{
			if( boneids[n] >= 0 )
				weights[n] = ( F32 )boneids[n]; 
		}
		// pack
		for( n = start; n < end; n++ )
		{
			m_vertexweights[n].Offset() = &weights[0]; 
			m_vertexweights[n].NormalOffset() = &weights[3]; 
		}

		m = end-1; 
	}

	GetVertexWeights().SortOnMeshThenBonesThenVertex(); 
}


BOOL IFXSkin::GetMasterMeshVertex( U32 meshid, U32 vertex, 
												U32 *mmeshid, U32 *mvertex )
{
	if( meshid >= m_mastervertices.GetNumberElements() ||
			vertex >= m_mastervertices[meshid].GetNumberElements() )
		return false; 

	m_mastervertices[meshid][vertex].GetMeshVertex( mmeshid, mvertex ); 
	return true; 
}


void IFXSkin::RelinkWeights( void )
{
#if IFXSKIN_RELINK_BY_WEIGHTS
	const I32 maxancestors = 8; 
	I32     ancestor[maxancestors]; 
	F32 ancestorcompete[maxancestors]; 
	F32 ancestoranticompete[maxancestors]; 
	I32 ancestors = 0; 
#endif

	I32 m, length = m_vertexweights.GetNumberElements(); 
	//I32 newlength = length; 
	for( m = 0; m < length; m++ )
	{
		IFXVertexWeight &rVertexWeight = m_vertexweights[m]; 
		I32 meshid = rVertexWeight.GetMeshIndex(); 
		I32 vertid = rVertexWeight.GetVertexIndex(); 
		I32 boneid = rVertexWeight.GetBoneIndex(); 
		F32 childweight = rVertexWeight.GetBoneWeight(); 

#if IFXSKIN_RELINK_DEBUG
		BOOL show = ( boneid == 51 && childweight > 0.99 ); 

		if( show )
		{
			IFXTRACE_GENERIC( L"\nvertex = %d:%d childweight = %.6G\n", 
						rVertexWeight.GetMeshIndex(), vertid, childweight ); 
		}
#endif

		if( childweight < 0.0001 )
			continue; 

		IFXBoneNode *bonenode = m_character->LookupBoneIndex( boneid ); 
		IFXASSERT( bonenode ); 
		if( !bonenode )
			continue; 

#if IFXSKIN_RELINK_DEBUG
		if( show )
		{
			IFXTRACE_GENERIC( L"bone %d %ls\n", bonenode->GetBoneIndex(), 
									bonenode->NameConst().Raw8() ); 
		}
#endif

		IFXCoreNode *parent = bonenode->Parent(); 
		if( !parent->IsBone() )
			continue; 

		I32 parentid = ( ( IFXBoneNode * )parent )->GetBoneIndex(); 

		IFXBoneLinks &bonelinks = bonenode->BoneLinks(); 
		I32 numlinks = bonelinks.GetNumberLinks(); 
		if( numlinks < 1 )
			continue; 


		F32 parentweight = 0.0f; 
		F32 parcompeteweight = 0.0f; // sibling/nephew/etc
		BOOL has_descendent = false; 

		//* find weights for the same vertex ( assumes they're sorted )
		I32 n = m; 
		IFXVertexWeight *parententry = NULL; 
		while( n > 0 && m_vertexweights[n-1].GetVertexIndex() == vertid )
			n--; 
		I32 start = n; 
		while( n < length && m_vertexweights[n].GetVertexIndex() == vertid )
			n++; 
		I32 end = n; 

#if IFXSKIN_RELINK_BY_WEIGHTS
		ancestors = 0; 
#endif

		// find all ancestors first
		for( n = start; n < end; n++ )
		{
			if( m_vertexweights[n].GetBoneIndex() == parentid )
			{
				parententry = &m_vertexweights[n]; 
				parentweight = parententry->GetBoneWeight(); 
			}
#if IFXSKIN_RELINK_BY_WEIGHTS
			else
			{
				IFXBoneNode *otherbone = m_character->LookupBoneIndex( 
										m_vertexweights[n].GetBoneIndex() ); 
				IFXASSERT( otherbone ); 
				if( bonenode->HasAncestor( otherbone ) )
				{
					ancestor[ancestors] = n; 
					ancestorcompete[ancestors] = 0.0f; 
					ancestoranticompete[ancestors] = 0.0f; 
					ancestors++; 
				}
			}
#endif
		}

		// then, deal with leaves
		for( n = start; n < end; n++ )
		{
			if( m_vertexweights[n].GetBoneIndex() == parentid )
			{
				//* other is parent
				continue; 
			}
			else
			{
				IFXBoneNode *otherbone = m_character->LookupBoneIndex( 
										m_vertexweights[n].GetBoneIndex() ); 
				IFXASSERT( otherbone ); 
				if( bonenode->HasAncestor( otherbone ) )
				{
					//* other is ancestor
					continue; 
				}
				else if( otherbone->HasAncestor( bonenode ) )
				{
					//* other is descendent
					has_descendent = true; 
					break; 
				}
				else
				{
					//* neither is descendent of the other

#if IFXSKIN_RELINK_BY_WEIGHTS
					//* account for competitors for ancestor's influence
					I32 j; 
					for( j = 0; j < ancestors; j++ )
					{
						IFXBoneNode *ansbone = m_character->LookupBoneIndex( 
							m_vertexweights[ancestor[j]].GetBoneIndex() ); 
						IFXASSERT( ansbone ); 

						if( otherbone->HasAncestor( ansbone ) )
							ancestorcompete[j] += m_vertexweights[n]
														.GetBoneWeight(); 
					}
#endif

					//* competitor for parent
					if( n > m && otherbone->HasAncestor( parent ) )
						parcompeteweight += 
										m_vertexweights[n].GetBoneWeight(); 
				}
			}
		}

#if IFXSKIN_RELINK_BY_WEIGHTS
		/*** Ancestors of this bone who are descendents of another ancestor
			count FOR this bone when competing for influence */
		for( n = 0; n < ancestors; n++ )
		{
			IFXBoneNode *otherbone = m_character->LookupBoneIndex( 
							m_vertexweights[ancestor[n]].GetBoneIndex() ); 
			IFXASSERT( otherbone ); 

			I32 j; 
			for( j = 0; j < ancestors; j++ )
				if( j!= n )
				{
					IFXBoneNode *ansbone = m_character->LookupBoneIndex( 
							m_vertexweights[ancestor[j]].GetBoneIndex() ); 
					IFXASSERT( ansbone ); 

					//* ancestral anti-competitors
					if( otherbone->HasAncestor( ansbone ) )
					{
						F32 est_fraction = ( childweight )/
								( childweight+ancestorcompete[n] ); 
						ancestoranticompete[j] += est_fraction*
							m_vertexweights[ancestor[n]].GetBoneWeight(); 
					}
				}
		}
#endif

		//* if a descendent will do the relinking, we don't have to
		if( has_descendent )
			continue; 

		IFXASSERT( childweight >= 0.0f ); 
		IFXASSERT( parentweight >= 0.0f );      // redundant?

#if IFXSKIN_RELINK_BY_WEIGHTS
		F32 ancestorweight = 0.0f; 
		for( n = 0; n < ancestors; n++ )
		{
			F32 fraction = ( childweight+ancestoranticompete[n] )/
					( childweight+ancestoranticompete[n]+ancestorcompete[n] ); 

			ancestorweight += fraction*
							m_vertexweights[ancestor[n]].GetBoneWeight(); 

#if IFXSKIN_RELINK_DEBUG
			if( show )
			{
				IFXTRACE_GENERIC( L"ans %d: compete = %.6G anticompete = %.6G\n", 
						m_vertexweights[ancestor[n]].GetBoneIndex(), 
						ancestorcompete[n], ancestoranticompete[n] ); 
				IFXTRACE_GENERIC( L"  fraction = %.6G of %.6G adds %.6G\n", 
						fraction, 
						m_vertexweights[ancestor[n]].GetBoneWeight(), 
						fraction*
							m_vertexweights[ancestor[n]].GetBoneWeight() ); 
			}
#endif
		}
#endif

		//* how much of parent weight we are allowed to reassign
		F32 parcompetefraction = parcompeteweight/
											( parcompeteweight+childweight ); 

#if IFXSKIN_RELINK_DEBUG
		if( show )
		{
			IFXTRACE_GENERIC( L"vertex = %d:%d ", 
							rVertexWeight.GetMeshIndex(), vertid ); 
#if IFXSKIN_RELINK_BY_WEIGHTS
			IFXTRACE_GENERIC( L"ancestors = %d ", 
							ancestors ); 
			IFXTRACE_GENERIC( L"ancestorweight = %.6G ", ancestorweight ); 
#endif
			IFXTRACE_GENERIC( L"pre parentweight = %.6G\n", parentweight ); 
			IFXTRACE_GENERIC( L"parcompeteweight = %.6G parcompetefraction = %.6G\n", 
							parcompeteweight, parcompetefraction ); 
		}
#endif

		parentweight *= ( 1.0f-parcompetefraction ); 

		F32 partialweight = parentweight+childweight; 

		F32 linklength = bonelinks.GetLinkLength()/( F32 )numlinks; 
		IFXVector3 offset(0, 0, 0); 
		CalcLocalOffset( boneid, 
						m_vertexweights[m].OffsetConst(), &offset ); 

#if IFXSKIN_RELINK_BY_WEIGHTS
		F32 totalweight = ancestorweight+partialweight; 
		F32 flinkid = ( numlinks+1 )*childweight/totalweight; 
#else
		F32 flinkid = offset[0]/linklength+( numlinks+1 )*0.5f; 

#if IFXSKIN_RELINK_DEBUG
		if( show )
		{
			IFXTRACE_GENERIC( L"offset = %ls linklength = %.6G flinkid = %.6G\n", 
								offset.Out().Raw(), linklength, flinkid ); 
		}
#endif
#endif

		I32 linkid2 = ( I32 )flinkid; 

		//* if not weighted to parent and beyond chain, leave as is
		if( !parententry && linkid2 > numlinks )
			continue; 

		if( linkid2 < 0 )
			linkid2 = 0; 
		if( linkid2 > numlinks )
			linkid2 = numlinks; 
		if( linkid2 == numlinks+1 )     // redundant?
			linkid2 = numlinks; 
		I32 linkid1 = linkid2-1; 

		F32 substart = ( linkid1+1 )/( F32 )( numlinks+1 ); 

#if FALSE //IFXSKIN_RELINK_BY_WEIGHTS
		F32 subfraction = ( childweight/totalweight-substart )*
														( F32 )( numlinks+1 ); 
#else
		F32 subfraction = flinkid-substart*( F32 )( numlinks+1 ); 
		if( subfraction < 0.0f )
			subfraction = 0.0f; 
		if( subfraction > 1.0f )
			subfraction = 1.0f; 

#endif

		//* reuse weight to original bone for weight to first link
		//* make new entry for weight to second link
		IFXVertexWeight &newentry = m_vertexweights.CreateNewElement(); 
		newentry.CopyFrom( rVertexWeight ); 

		if( linkid1 >= 0 )
			newentry.SetBoneIndex( 
								bonelinks.GetLink( linkid1 )->GetBoneIndex() ); 
		else
			newentry.SetBoneIndex( parentid ); 

		if( linkid2 < numlinks )
			rVertexWeight.SetBoneIndex( 
								bonelinks.GetLink( linkid2 )->GetBoneIndex() ); 

		if( fabs( subfraction ) < 0.001f )
			subfraction = 0.0f; 

		//* normal-derived influence fade
		F32 jointfade = 0.0f; 
		if( parententry )
		{
			// jointfade 0.0 = inside radius, no loss  1.0 = double+ radius, full loss
			IFXCylinder &bounds = bonenode->CylindricalBounds(); 
			IFXVector3 &jointscale = bounds.GetScale( 0 ); 
			// average of x and y radius
			F32 average = IFXSQRT( jointscale[1]*jointscale[1]+
									jointscale[2]*jointscale[2] ); 
			F32 jsx = ( offset[0]/average ); 
			if( jsx < 0.0f )
				jsx = 0.0f; 

			// get normal from 'master' pre-replication post-weld vertex
			F32 cosa = 1.0f; 
			U32 mesh0 = meshid, vertex0 = vertid; 

			IFXMeshVertexList &replist = m_replicants[meshid][vertid]; 
			IFXListContext context; 
			IFXMeshVertex *mv; 
			//I32 numreps = replist.GetNumberElements(); 
			replist.ToHead( context ); 
			do 
			{
				mv = replist.PostIncrement( context ); 
				if( mv )
					mv->GetMeshVertex( &mesh0, &vertex0 ); 
				else
				{
					mesh0 = meshid; 
					vertex0 = vertid; 
				}

				m_inmesh->ChooseMeshIndex( mesh0 ); 
				IFXVector3 normal = m_inmesh->GetNormalConst( vertex0 ); 
				IFXVector3 xaxis( 1.0f, 0.0f, 0.0f ), boneaxis; 
				bonenode->StoredTransform().RotateVector( xaxis, boneaxis ); 

				F32 dotproduct = normal.DotProduct( boneaxis ); 
				if( cosa > dotproduct )
					cosa = dotproduct; 
			} while( mv ); 

			cosa -= jsx; 
			if( cosa < 0.0f )
				cosa = 0.0f; 

			jointfade = ( F32 )pow( fabs( cosa ), 2.0f ); 
		}

		IFXASSERT( jointfade >= 0.0f ); 
		IFXASSERT( jointfade <= 1.0f ); 

		F32 jointloss = partialweight*jointfade; 
		partialweight -= jointloss; 

		F32 subweight = partialweight*subfraction; 
		IFXASSERT( subweight >= 0.0f ); 
		IFXASSERT( subfraction >= 0.0f ); 
		IFXASSERT( subfraction <= 1.0f ); 

#if IFXSKIN_RELINK_DEBUG
		if( show )
		{
			IFXTRACE_GENERIC( L"substart = %.6G subfraction = %.6G ", substart, subfraction ); 
#if IFXSKIN_RELINK_BY_WEIGHTS
			IFXTRACE_GENERIC( L"ancestorweight = %.6G ", ancestorweight ); 
#endif
			IFXTRACE_GENERIC( L"parentweight = %.6G childweight = %.6G\n", 
													parentweight, childweight ); 
			IFXTRACE_GENERIC( L"jointfade = %.6G jointloss = %.6G ", jointfade, jointloss ); 
			IFXTRACE_GENERIC( L"link1 = %d/%d subweight = %.6G\n", linkid1, numlinks, subweight ); 
		}
#endif

		newentry.SetBoneWeight( partialweight-subweight ); 
		rVertexWeight.SetBoneWeight( subweight ); 

		if( parententry )
		{
			parententry->BoneWeight() *= parcompetefraction; 

			// WARNING shouldn't just leave for other children to consume
			parententry->BoneWeight() += jointloss; 
		}
	}
}


BOOL IFXSkin::CalcLocalOffset( I32 boneid, 
								const IFXVector3 &world, IFXVector3 *offset )
{
	IFXBoneNode *bonenode = m_character->LookupBoneIndex( boneid ); 
	if( !bonenode )
		return false; 

	bonenode->StoredTransform().ReverseTransformVector( world, *offset ); 
	return true; 
}


/******************************************************************************
	if computetips = false, do not compute tip cross-sections in addition
		to base cross-sections
******************************************************************************/
void IFXSkin::CalculateJointCrossSections( BOOL updatecache, BOOL computetips )
{
	if( updatecache )
	{
		ComputeVertexOffsets(); 
	}

	m_character->ForEachNodeTransformed( IFXPARENTFIRST, 
								&IFXSkin::CalculateJointsForBone, &computetips ); 
}


BOOL IFXSkin::CalculateJointsForBone( IFXCoreNode &node, 
									IFXTransform &transform, IFXVariant state )
{
	if( !node.IsBone() )
		return false; 

	BOOL *computetips = NULL; 
#ifdef _DEBUG
	IFXRESULT result=
#endif

	state.CopyPointerTo( &computetips ); 
	IFXASSERT( result == IFX_OK ); 
	IFXASSERT( computetips ); 

	IFXBoneNode &bonenode = ( IFXBoneNode & )node; 
	I32 index = bonenode.GetBoneIndex(); 
	if( !( index%5 ) )
	{
		char progresstext[128]; 
		sprintf( progresstext, "Calc Joints %d%%", 100*index/
								bonenode.RootCharacter()->GetBoneTableSize() ); 
		IFXCharacter::StateProgress( progresstext ); 
	}

	CalculateJointForBone( node, transform, 0, *computetips ); 
	if( *computetips )
		CalculateJointForBone( node, transform, 1, *computetips ); 

	return false; 
}


void IFXSkin::CalculateJointForBone( IFXCoreNode &node, 
							IFXTransform &transform, BOOL tip, BOOL computetips )
{
	IFXBoneNode &bnode = ( IFXBoneNode & )node; 

	if( !bnode.IsAutoJoint() || bnode.IsBoneLink() )
		return; 

	I32 boneid = bnode.GetBoneIndex(); 
	F32 bonelength = bnode.GetLength(); 

	IFXSkin *skin = bnode.RootCharacter()->GetSkin(); 

	IFXList < I32 > family; 
	family.SetAutoDestruct( true ); 
	IFXListContext longcontext; 
	**( family.Append( new I32 ) ) = boneid; 
	if( bnode.Parent()->IsBone() )
		**( family.Append( new I32 ) ) = ( ( IFXBoneNode * )bnode.Parent() )
															->GetBoneIndex(); 

	IFXBoneNodeList &children = bnode.Children(); 
	BOOL has_children = ( bnode.Children().GetNumberElements() > 0 ); 
	IFXListContext context; 
	IFXBoneNode *child; 
	children.ToHead( context ); 
	while( ( child = children.PostIncrement( context ) ) != NULL )
		**( family.Append( new I32 ) ) = child->GetBoneIndex(); 

	IFXCylinder &bounds = bnode.CylindricalBounds(); 
	IFXVector3 &center = bounds.GetCenter( tip ); 
	IFXVector3 &scale = bounds.GetScale( tip ); 
	IFXVector3 basescale = bounds.GetScale( 0 ); 

	IFXVector3 parentscale; 
	BOOL check_parent = false; 
	if( computetips && bnode.Parent()->IsBone() )
	{
		parentscale = ( ( IFXBoneNode * )bnode.Parent() )
										->CylindricalBounds().GetScale( 1 ); 

		if( parentscale.CalcMagnitude() > bonelength*0.01 )
			check_parent = true; 
	}

	const I32 passes = 16; 
	const F32 expander = 1.4f; 
	const F32 reducescale = 1.0f;   // 0.95f; 
	const F32 unitdeltamax = 0.25f; 
	const F32 deltainfluence = 0.5f; 
	const F32 xfade = 0.5f;             // 0.25f; 

	center.Reset(); 
	scale.Reset(); 

	scale[1] = 0.1f; 
	scale[2] = 0.1f; 

#if IFXFIXED_JOINTS
	scale[1] = IFXFIXED_JOINTSIZE; 
	scale[2] = IFXFIXED_JOINTSIZE; 
	return false; 
#endif

	BOOL has_weights = false; 
	IFXArray <IFXVector3 > offsets; 
	IFXVertexWeights &m_vertexweights = skin->GetVertexWeights(); 
	I32 m, length = m_vertexweights.GetNumberElements(); 
	I32 index; 
	if( length > 0 )
	{
		for( m = 0; m < length; m++ )
		{
#if IFXJOINT_USEMOREWEIGHTS
			index = m_vertexweights[m].GetBoneIndex(); 
			if( family.SearchForContent( longcontext, &index ) != NULL )
#else
			if( m_vertexweights[m].GetBoneIndex() == boneid )
#endif
			{
#if IFXDEFORM_GLOBALORIENTED
				IFXVector3 &offset = offsets.CreateNewElement(); 
				skin->CalcLocalOffset( boneid, 
								m_vertexweights[m].OffsetConst(), &offset ); 
#else
				IFXVector3 &offset = offsets.CreateNewElement(); 
				offset = m_vertexweights[m].OffsetConst(); 
#endif
				if( m_vertexweights[m].GetBoneIndex() == boneid )
					has_weights = true; 
				if( tip )
					offset[0] -= bonelength; 
			}
		}
	}
	else
	{
		const IFXPackVertex *pPackVertex = NULL; 
		const IFXPackBoneWeight *pPackWeight = NULL; 

		I32 weights, n; // , vertex; 
		I32 meshid, meshes = skin->GetInputMeshConst()->GetNumberMeshes(); 
		for( meshid = 0; meshid < meshes; meshid++ )
		{
			skin->GetInputMesh()->ChooseMeshIndex( meshid ); 

			IFXPackWeights &packweights = skin->GetPackVertexWeights()[meshid]; 
			packweights.RewindForRead(); 

			length = packweights.GetNumberVerticesConst(); 
			for( m = 0; m < length; m++ )
			{
				pPackVertex = packweights.NextPackVertexForRead(); 
				weights = pPackVertex->GetNumWeights(); 
				// vertex = pPackVertex->GetVertexIndex(); 

				for( n = 0; n < weights; n++ )
				{
					pPackWeight = packweights.NextPackBoneWeightForRead(); 

					index = pPackWeight->GetBoneIndex(); 
					if( family.SearchForContent( longcontext, &index ) != NULL )
					{
						IFXVector3 &offset = offsets.CreateNewElement(); 
						skin->CalcLocalOffset( boneid, 
										pPackVertex->OffsetConst(), &offset ); 

						if( pPackWeight->GetBoneIndex() == boneid )
							has_weights = true; 
						if( tip )
							offset[0] -= bonelength; 
					}
				}
			}
		}
	}

	if( !has_weights )
	{
		scale.Reset(); 
		return; 
	}

	I32 numberoffsets = offsets.GetNumberElements(); 

	F32 crudescale = 1e6f; 
	IFXVector3 originaloffset, radial; 
	F32 x, angle, axis, delta[2], f[2], change; 
	I32 pass, n, cycle; 

	for( m = 0; m < numberoffsets; m++ )
	{
		radial = offsets[m]; 

		x = ( F32 )fabs( radial[0] ); 
		radial[0] = 0.0f; 

		F32 radius = radial.CalcMagnitude()+x; 
		if( crudescale > radius )
			crudescale = radius; 
	}
	F32 deltamax = unitdeltamax*crudescale; 

	if( numberoffsets < 3 )
		return; 

	scale[1] = crudescale; 
	scale[2] = crudescale; 

	for( pass = 0; pass < passes; pass++ )
	{
		F32 crude2 = deltainfluence*crudescale*( ( passes-pass )/( F32 )passes ); 

		scale[1] *= expander; 
		scale[2] *= expander; 

		delta[0] = 0.0f; 
		delta[1] = 0.0f; 

		// move, then resize
		for( cycle = 0; cycle < 2; cycle++ )
		{
			for( m = 0; m < numberoffsets; m++ )
			{
				originaloffset = offsets[m]; 

				radial.Subtract( originaloffset, center ); 
				x = ( F32 )fabs( radial[0] )*xfade; 
				radial[0] = 0.0f; 

				angle = IFXATAN2( radial[2], radial[1] ); 
				f[0] = ( F32 )fabs( IFXCOS( angle ) )+0.001f; 
				f[1] = ( F32 )fabs( IFXSIN( angle ) )+0.001f; 

				for( n = 0; n < 2; n++ )
				{
					axis = ( ( F32 )fabs( radial[n+1] )+x )/f[n]; 

					if( scale[n+1] > axis )
					{
						if( cycle )
							scale[n+1] = axis; 
						else
						{
							change = crude2*( 1.0f-axis/scale[n+1] ); 

							if( change > 0.0f )
								delta[n] += ( originaloffset[n+1] > 0.0 )?
														-change: change; 
						}
					}
				}
			}

			if( !cycle )
			{
				for( n = 0; n < 2; n++ )
				{
					if( ( F32 )fabs( delta[n] ) > deltamax )
						delta[n] = ( delta[n] > 0.0 )? deltamax: -deltamax; 

					center[n+1] += delta[n]; 
				}
			}

			I32 j; 
			for( j = 1; j < 3; j++ )
			{
				if( scale[j] > bonelength*IFXJOINT_MAXRAD )
					scale[j] = bonelength*IFXJOINT_MAXRAD; 

				if( scale[j] > scale[3-j]*IFXJOINT_MAXASPECT )
					scale[j] = scale[3-j]*IFXJOINT_MAXASPECT; 

				if( ( F32 )fabs( center[j] ) > scale[j]*IFXJOINT_MAXDISPLACE )
					center[j] *= scale[j]*IFXJOINT_MAXDISPLACE/
													( F32 )fabs( center[j] ); 

				if( tip )
				{
					F32 maxdiff = IFXJOINT_MAXTIPCHANGE*bonelength; 
					F32 mindiff = IFXJOINT_MINTIPCHANGE*bonelength; 

					F32 diff = scale[j]-basescale[j]; 
					if( diff < mindiff )
						diff = mindiff; 
					else if( !has_children && diff > 0.0f )
						diff = 0.0f; 
					else if( diff > maxdiff )
						diff = maxdiff; 

					scale[j] = basescale[j]+diff; 
				}
				else if( check_parent )
				{
					if( scale[j] > parentscale[j]*IFXJOINT_MAXCHILDASPECT )
					scale[j] = parentscale[j]*IFXJOINT_MAXCHILDASPECT; 
				}
			}
		}
	}

	scale[1] *= reducescale; 
	scale[2] *= reducescale; 

	if( tip )
		center[0] = bonelength; 
}


void IFXSkin::DeformMesh( BOOL renormalize, BOOL clod )
{
	if( !m_outmesh || !m_outmesh->GetNumberMeshes() )
	{
		return; 
	}

	if( !m_vertexweights.GetNumberElements() &&
		!m_packweightarray.GetNumberElements() )
	{
		return; 
	}


#if !IFXDEFORM_VERTEXORIENTED
	I32 meshid, meshes = m_outmesh->GetNumberMeshes(); 
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_outmesh->ChooseMeshIndex( meshid ); 

		m_outmesh->ZeroAllVertices(); 
		m_outmesh->ZeroAllNormals(); 
	}
#endif

#if IFXDEFORM_VERTEXORIENTED
	TIME_PROGRESS( m_character, IFXBonesManager::TimeBoneCache, true ); 
	I32 lastsize = m_bonecachearray.GetNumberElements(); 
	m_bonecachearray.ResizeToAtLeast( m_character->GetBoneTableSize() ); 
	PrepareBoneCacheArray(); 
	if( ( U32 )lastsize!= m_bonecachearray.GetNumberElements() )
		m_bonecachearray.Collect(); 
	TIME_PROGRESS( m_character, IFXBonesManager::TimeBoneCache, false ); 

	if ( m_bUseVectorUnit )
		ComputeDeformedVerticesPackedSSE( clod );   // note: on Mac this is really Altivec not SSE!
	else
		ComputeDeformedVerticesPacked( clod ); 

#else
	// also stores transforms for AccumulateWeightedVertices()
	m_character->ForEachNodeTransformed( IFXPARENTFIRST|IFXSTORE_XFORM, 
										&IFXSkin::ComputeStretchNormals ); 

	m_character->ForEachNodeTransformed( IFXCHILDFIRST, 
										&IFXSkin::AccumulateWeightedVertices ); 

	m_character->ForEachNodeTransformed( IFXPARENTFIRST, 
										&IFXSkin::TweakWeightedVertices ); 
#endif


	TIME_PROGRESS( m_character, IFXBonesManager::TimeRenormalize, true ); 

	if( renormalize )
		NormalizeOutputNormals(); 

	TIME_PROGRESS( m_character, IFXBonesManager::TimeRenormalize, false ); 
}


void IFXSkin::ComputeVertexOffsets( void )
{
	SortVertexWeightsForCache(); 

	// store up all bone transforms
	m_character->ResetToReference(); 
	m_character->ForEachNodeTransformed( IFXPARENTFIRST|IFXSTORE_XFORM, NULL ); 

	I32 m, length = m_vertexweights.GetNumberElements(); 
	for( m = 0; m < length; m++ )
	{
		IFXVertexWeight &rVertexWeight = m_vertexweights[m]; 

		I32 boneid = rVertexWeight.GetBoneIndex(); 
		I32 meshid = rVertexWeight.GetMeshIndex(); 
		I32 vertexid = rVertexWeight.GetVertexIndex(); 

		IFXBoneNode *bonenode = m_character->LookupBoneIndex( boneid ); 
		if( !bonenode )
			continue; 

		m_inmesh->ChooseMeshIndex( meshid ); 

#if IFXDEFORM_GLOBALORIENTED
		rVertexWeight.Offset() = m_inmesh->GetVertexConst( vertexid ); 
		rVertexWeight.NormalOffset() = m_inmesh->GetNormalConst( vertexid ); 
#else
		IFXVector3 &offset = rVertexWeight.Offset(); 
		IFXVector3 global; 
		global = m_inmesh->GetVertexConst( vertexid ); 
		bonenode->StoredTransform().ReverseTransformVector( global, offset ); 

#if IFXBONECACHE_NORMALS
		IFXVector3 &normaloffset = rVertexWeight.NormalOffset(); 
		global = m_inmesh->GetNormalConst( vertexid ); 

		bone->StoredTransform().ReverseRotateVector( global, normaloffset ); 
#endif

#endif
	}
}


void IFXSkin::ComputePackVertexOffset( U32 meshid, IFXPackVertex *pPackVertex )
{
	I32 vertexid = pPackVertex->GetVertexIndex(); 

	m_inmesh->ChooseMeshIndex( meshid ); 

	F32 *pvert = pPackVertex->Offset(); 
	const F32 *mvert = m_inmesh->GetVertexConst( vertexid ); 

	pvert[0] = mvert[0]; 
	pvert[1] = mvert[1]; 
	pvert[2] = mvert[2]; 

	pvert = pPackVertex->NormalOffset(); 
	mvert = m_inmesh->GetNormalConst( vertexid ); 

	pvert[0] = mvert[0]; 
	pvert[1] = mvert[1]; 
	pvert[2] = mvert[2]; 
}


void IFXSkin::PrepareBoneCacheArray( void )
{
	IFXMatrix4x4 result; 
	IFXBoneNode *bnode; 

	result.Reset(); 

	I32 m, length = m_character->GetBoneTableSize(); 
	for( m = 0; m < length; m++ )
	{
		if( !( bnode = m_character->LookupBoneIndex( m ) ) )
			continue; 

		result.Multiply3x4( 
					bnode->StoredTransform().GetMatrixDataConst(), 
					bnode->ReferenceTransform().GetMatrixInverseDataConst() ); 

		// copy 4x4 into 3x4
		m_bonecachearray[m].GetGlobalMatrix() = result; 
	}
}


/**
	@note   assumes m_vertexweight is meshid/vertexid sorted
*/
void IFXSkin::PackVertexWeights( void )
{
//	if(true)
//	{
//		m_vertexweights;
//	}
//

	I32 const meshes = m_inmesh->GetNumberMeshes(); 

	m_packweightarray.Clear(); 
	m_packweightarray.ResizeToAtLeast( meshes ); 

	I32 meshid; 
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_inmesh->ChooseMeshIndex( meshid ); 
		const I32 numverts = m_inmesh->GetMaxNumberVertices(); 

		const I32 start = m_vertexweights.GetMeshStart( meshid ); 
		IFXASSERT( ( U32 )start < m_vertexweights.GetNumberElements() ); 

		const I32 numweights = m_vertexweights.GetMeshStart( meshid+1 )-start; 

		IFXPackWeights &packweights = m_packweightarray[meshid]; 
		packweights.Allocate( numverts, numweights ); 

		IFXPackVertex *packvert = NULL; 
		IFXPackBoneWeight *pPackWeight = NULL; 
		I32 m, n, vertid = -2; 
		for( m = 0; m < numweights; ++m )
		{
			IFXVertexWeight &rVertexWeight = m_vertexweights[start+m]; 

			if( vertid!= rVertexWeight.GetVertexIndex() )
			{
				vertid = rVertexWeight.GetVertexIndex(); 

				packvert = packweights.NextPackVertexForWrite(); 
				packvert->SetVertexIndex( ( U32 )vertid ); 
				packvert->SetNumWeights( 1 ); 

				for( n = 0; n < 3; n++ )
				{
					packvert->Offset()[n] = rVertexWeight.Offset()[n]; 
					packvert->NormalOffset()[n] = rVertexWeight.NormalOffset()[n]; 
				}
			}
			else
				packvert->IncNumWeights(); 

			pPackWeight = packweights.NextPackBoneWeightForWrite(); 
			pPackWeight->SetBoneIndex( rVertexWeight.GetBoneIndex() ); 
			pPackWeight->SetBoneWeight( rVertexWeight.GetBoneWeight() ); 
		}
	}
}


void IFXSkin::UnpackVertexWeights( void )
{
	const I32 meshes = m_packweightarray.GetNumberElements(); 

	m_vertexweights.Clear(); 

	U32 weightcount = 0; 
	U32 totalweights = 0; 
	U32 m, n; 

	I32 meshid; 
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		IFXPackWeights &packweights = m_packweightarray[meshid]; 

		m_inmesh->ChooseMeshIndex( meshid ); 
		if( m_inmesh->GetNumberVertices() < m_inmesh->GetMaxNumberVertices() )
		{
			IFXTRACE_GENERIC( L"WARNING: %d of %d vertices valid on mesh %d\n", 
									m_inmesh->GetNumberVertices(), 
									m_inmesh->GetMaxNumberVertices(), meshid ); 
		}
		const U32 numverts = packweights.GetNumberVerticesConst(); 
		const U32 numweights = packweights.GetNumberWeightsAllocated(); 
		if( numverts!= m_inmesh->GetMaxNumberVertices() )
		{
			IFXTRACE_GENERIC( L"UnpackVertexWeights() only %d/%d vertices available"
															L" for mesh %d\n", 
							numverts, m_inmesh->GetMaxNumberVertices(), meshid ); 
		}

		totalweights += numweights; 
		m_vertexweights.ResizeToAtLeast( totalweights ); 

		packweights.RewindForRead(); 
		const IFXPackVertex *pPackVertex = NULL; 
		const IFXPackBoneWeight *pPackWeight = NULL; 
		U32 weights; 

		for( m = 0; m < numverts; m++ )
		{
			pPackVertex = packweights.NextPackVertexForRead(); 
			weights = pPackVertex->GetNumWeights(); 
			IFXASSERT( weights > 0 ); 
			for( n = 0; n < weights; n++ )
			{
				pPackWeight = packweights.NextPackBoneWeightForRead(); 

				IFXVertexWeight &rVertexWeight = m_vertexweights[weightcount++]; 
				rVertexWeight.SetMeshIndex( meshid ); 
				rVertexWeight.SetVertexIndex( pPackVertex->GetVertexIndex() ); 
				rVertexWeight.SetBoneIndex( pPackWeight->GetBoneIndex() ); 
				rVertexWeight.SetBoneWeight( pPackWeight->GetBoneWeight() ); 
				rVertexWeight.Offset().Set(  pPackVertex->OffsetConst()[0], 
											pPackVertex->OffsetConst()[1], 
											pPackVertex->OffsetConst()[2] ); 
				rVertexWeight.NormalOffset().Set( 
										pPackVertex->NormalOffsetConst()[0], 
										pPackVertex->NormalOffsetConst()[1], 
										pPackVertex->NormalOffsetConst()[2] ); 
			}
		}
	}
}


/******************************************************************************
	NOTE                    `speed critical'

	WARNING If there is any scaling applied to the matrices, 
			you must activate the compile-time flag IFXM34_UNSCALE
			in IFXMatrix3x4.h.
******************************************************************************/
void IFXSkin::ComputeDeformedVerticesPacked( BOOL clod )
{
	TIME_PROGRESS( m_character, IFXBonesManager::TimeDeformation, true ); 

	F32 accumulator[4]; 
	F32 normaccum[4]; 
	IFXVertexIndexer *outputIndexer = m_outmesh->GetVertexIndexer(); 

	IFXASSERT( m_inmesh->GetNumberMeshes() == m_outmesh->GetNumberMeshes() ); 

	const IFXBoneCacheArray &cbonecachearray = m_bonecachearray; 

	U32 m, n, meshid; 
	const U32 meshes = ( U32 )m_outmesh->GetNumberMeshes(); 
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_outmesh->ChooseMeshIndex( meshid ); 

		const I32 numverts = ( clod )?
			m_outmesh->GetNumberVertices(): m_outmesh->GetMaxNumberVertices(); 

		IFXPackWeights &packweights = m_packweightarray[meshid]; 
		packweights.RewindForRead(); 
		const U32 length = packweights.GetNumberVerticesConst(); 
		const IFXPackVertex *pPackVertex = NULL; 
		const IFXPackBoneWeight *pPackWeight = NULL; 
		U32 weights; 
		U32 length1 = length-1; 

		if( length )
			pPackVertex = packweights.NextPackVertexForRead(); 

		for( m = 0; m < length; m++ )
		{
			/// @todo this has gotta be slow! ( casts off const, too )
			ComputePackVertexOffset( meshid, ( IFXPackVertex * )pPackVertex ); 

			pPackWeight = packweights.NextPackBoneWeightForRead(); 
			weights = pPackVertex->GetNumWeights(); 
			IFXASSERT( weights > 0 ); 

			cbonecachearray.GetElementConst( pPackWeight->GetBoneIndex() )
						.GetGlobalMatrixConst()
						.TransformVertexAndNormal( 
								pPackWeight->GetBoneWeight(), 
								pPackVertex->OffsetConst(), accumulator, 
								pPackVertex->NormalOffsetConst(), normaccum ); 
			/// @todo: Investigate behavior of this code when default motion is used
			for( n = 1; n < weights; n++ )
			{
				pPackWeight = packweights.NextPackBoneWeightForRead(); 

				cbonecachearray.GetElementConst( pPackWeight->GetBoneIndex() )
						.GetGlobalMatrixConst()
						.CumulativeTransformVertexAndNormal( 
								pPackWeight->GetBoneWeight(), 
								pPackVertex->OffsetConst(), accumulator, 
								pPackVertex->NormalOffsetConst(), normaccum ); 
				IFXASSERT( ( ( IFXVector3 )normaccum ).CalcMagnitude() <= 1.1f ); 
			}

			outputIndexer->SetVertexAndNormal( pPackVertex->GetVertexIndex(), 
														accumulator, normaccum ); 

			if( m < length1 )
			{
				pPackVertex = packweights.NextPackVertexForRead(); 
				if( pPackVertex->GetVertexIndex() >= numverts )
				{
					//* move on to next mesh
					m = length;     // faster than break
				}
			}
		}
	}

	TIME_PROGRESS( m_character, IFXBonesManager::TimeDeformation, false ); 
}


/******************************************************************************
	WARNING speed critical
******************************************************************************/
void IFXSkin::ComputeDeformedVertices( BOOL clod )
{
	IFXASSERT( FALSE ); 
}


/******************************************************************************
void IFXSkin::NormalizeOutputNormals( void )

******************************************************************************/
void IFXSkin::NormalizeOutputNormals( void )
{
	IFXASSERT( m_outmesh ); 

	F32 mag2, scale; 

	IFXVertexIndexer *indexer = m_outmesh->GetVertexIndexer(); 
	I32 meshid, meshes = m_outmesh->GetNumberMeshes(); 
	for( meshid = 0; meshid < meshes; meshid++ )
	{
		m_outmesh->ChooseMeshIndex( meshid ); 

		I32 index, normals = m_outmesh->GetNumberNormals(); 
		for( index = 0; index < normals; index++ )
		{
#if IFXSKIN_FASTNORMALIZE
			indexer->MoveToIndex( index ); 
			IFXVector3 *normal = indexer->GetNormal(); 

			mag2 = ( *normal )[0]*( *normal )[0]+( *normal )[1]*( *normal )[1]+
													( *normal )[2]*( *normal )[2]; 

#if IFXSKIN_USE_SQRT_TABLE
			IFXASSERT( ( I32 )( mag2*( IFXSKIN_SQRT_SCALESIZE-1 ) )
													 <IFXSKIN_SQRT_TABLESIZE ); 
			scale = m_invsqrt[( I32 )( mag2*( IFXSKIN_SQRT_SCALESIZE-1 ) )]; 
#else
			scale = 1.0f/( F32 )IFXSQRT( mag2 ); 
#endif
			normal->Scale( scale ); 
#else
			// copy
			IFXVector3 vector = m_outmesh->GetNormalConst( index ); 

			vector.Normalize();         //* sqrt()!

			// copy
			m_outmesh->SetNormal( index, vector.Raw() ); 
#endif
		}
	}
}
