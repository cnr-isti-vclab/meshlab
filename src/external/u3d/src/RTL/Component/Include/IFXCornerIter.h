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
	@file	IFXCornerIter.h

			A corner-based iterator for walking the surface
			of a model.  The iterator stores the current mesh index,
			face index, and corner index.  It provides methods
			for going to the clockwise or counterclockwise corner
			on a face and for jumping across to the adjacent face.

	@note	Designed to be very fast.  Use IFXFatCornerIter if you
			need to access the mesh attributes.
*/

#ifndef IFXCORNERITER_H
#define IFXCORNERITER_H

#include "IFXDataTypes.h"
#include "IFXNeighborFace.h"

class IFXNeighborMesh;
class IFXFatCornerIter;


const U32 g_ClockWise[] = {1, 2, 0};
const U32 g_CounterClockWise[] = {2, 0, 1};

IFXINLINE U32 Clockwise(const U32 corner)
{
	IFXASSERT(corner < 3);
	return g_ClockWise[corner];	// equivalent to (corner + 1) % 3
}

IFXINLINE U32 CounterClockwise(const U32 corner)
{
	IFXASSERT(corner < 3);
	return g_CounterClockWise[corner];	// equivalent to (corner + 2) % 3
}

/** 
This class is used as a corner-based iterator for walking the surface
of a model. The iterator stores the current mesh index, face index, and 
corner index.  It provides methods for going to the clockwise or 
counterclockwise corner on a face and for jumping across to the adjacent 
face. (nonlinear, corner-based, iterator) 
*/
class IFXCornerIter
{
public:
    IFXCornerIter();

    U32 GetMeshIndex();		///< Gets the current mesh index.
    U32 GetFaceIndex();		///< Gets the current face index.
    U32 GetCornerIndex();	///< Gets the current corner index.

	/** Gets a pointer to the current IFXNeighborFace structure.
	Use this to preview a jump by calling GetNeighbor()
	or get and set corner and face flags.*/
	IFXNeighborFace* GetCurrentFace();

	/** Gets a pointer to current corner flags. */
	IFXCornerFlags* GetCornerFlags();

	/** Gets a pointer to current face flags. */
	IFXFaceFlags* GetFaceFlags();

	/** Moves from the current corner to the 
	clockwise corner on the same face. */
    void MoveClockwise();

	/** Moves from the current corner to the 
	counterclockwise corner on the same face.*/
    void MoveCounterClockwise();

	/** Move to the next face in the linked-list of faces
	around the edge opposite the current corner.
	The current corner will be opposite the edge that
	was opposite the old corner.	 
	Returns true if mesh boundary crossed.*/
	BOOL JumpAcross();
	
	/** Same as JumpAcross() except the "flags" argument
	will contain all the flags from the jump.
	Returns true if mesh boundary crossed.*/
	BOOL JumpAcross(IFXCornerFlags** ppFlags);

	/** 
		Returns true if iterators have the same mesh and face index.

		@param pIter A pointer to the input Corner Iterator.
	*/
	BOOL IsEqualFace(IFXCornerIter* pIter);

	/** 
		Returns true if iterators have a different mesh or face index.

		@param pIter A pointer to the input Corner Iterator.
	*/
	BOOL IsNotEqualFace(IFXCornerIter* pIter);

	/** 
		Returns true if iterators have the same mesh, face, 
		and corner index.
		
		@param pIter A pointer to the input Corner Iterator.
	*/
	BOOL IsEqualCorner(IFXCornerIter* pIter);

	/** 
		Returns true if iterators have a different mesh, face, 
		or corner index.
		
		@param pIter A pointer to the input Corner Iterator.
	*/
	BOOL IsNotEqualCorner(IFXCornerIter* pIter);

    /** 
		Make this iterator a clone of pSrcIter iterator.

		@param pSrcIter A pointer to the input Corner Iterator.
	*/
    void Clone(IFXCornerIter* pSrcIter);

	/** 
		Make current neighbor link point to the neighbor
		specified by pDstIter iterator.

		@param pDstIter A pointer to the input Corner Iterator.
	*/
	void MoveLink(IFXCornerIter* pDstIter);

private:
    friend class IFXNeighborMesh;	
    friend class IFXFatCornerIter;	

	/** 
		Initialize IFXCornerIter.
		
		@note Can only be initialized by IFXNeighborMesh or IFXFatCornerIter. 

		@param pNeighborMesh A pointer to the input Neighbor Mesh.
	*/
    void Initialize(IFXNeighborMesh* pNeighborMesh,
					U32 mesh, U32 face, U32 corner);

	/** Called when we cross a mesh boundary.  Loads in the
	array of neighbor faces for the current mesh index. */
	void LoadMesh();

	U32 m_mesh;			///< current mesh index
	U32 m_face;			///< current face index
	U32 m_corner;		///< current corner index

	/// pointer to our parent neighbor mesh object
	IFXNeighborMesh* m_pNeighborMesh;

	/// current array of neighbor faces for the current mesh
	IFXNeighborFace* m_pNeighborFaces;
};


//
//  inlines
//

IFXINLINE IFXCornerIter::IFXCornerIter() :
	m_mesh(0), m_face(0), m_corner(0),
	m_pNeighborMesh(NULL), m_pNeighborFaces(NULL)
{
}

IFXINLINE U32 IFXCornerIter::GetMeshIndex()
{
	return m_mesh;
}


IFXINLINE U32 IFXCornerIter::GetFaceIndex()
{
	return m_face;
}


IFXINLINE U32 IFXCornerIter::GetCornerIndex()
{
	return m_corner;
}


IFXINLINE IFXNeighborFace* IFXCornerIter::GetCurrentFace()
{
	return &m_pNeighborFaces[m_face];
}


IFXINLINE IFXCornerFlags* IFXCornerIter::GetCornerFlags()
{
	return m_pNeighborFaces[m_face].GetCornerFlags(m_corner);
}


IFXINLINE IFXFaceFlags* IFXCornerIter::GetFaceFlags()
{
	return m_pNeighborFaces[m_face].GetFaceFlags();
}


IFXINLINE void IFXCornerIter::MoveClockwise()
{
	m_corner = Clockwise(m_corner);
}

	
IFXINLINE void IFXCornerIter::MoveCounterClockwise()
{
	m_corner = CounterClockwise(m_corner);
}


IFXINLINE BOOL IFXCornerIter::JumpAcross()
{
	BOOL result = IFX_TRUE;

	U32 newMesh;
	IFXCornerFlags* pFlags;

	m_pNeighborFaces[m_face].GetNeighbor(m_corner, &newMesh, &m_face, &pFlags);
	m_corner = pFlags->cornerIndex;
	
	if (newMesh != m_mesh)
	{
		m_mesh = newMesh;
		LoadMesh();
	}
	else
	{
		result = IFX_FALSE;
	}

	return result;
}


IFXINLINE BOOL IFXCornerIter::JumpAcross(IFXCornerFlags** ppFlags)
{
	U32 newMesh;
	m_pNeighborFaces[m_face].GetNeighbor(m_corner, &newMesh, &m_face, ppFlags);
	m_corner = (*ppFlags)->cornerIndex;
	if (newMesh != m_mesh)
	{
		m_mesh = newMesh;
		LoadMesh();
		return TRUE;
	}
	return FALSE;
}


IFXINLINE BOOL IFXCornerIter::IsEqualFace(IFXCornerIter* pIter)
{
	return ( (m_mesh == pIter->GetMeshIndex()) && 
		     (m_face == pIter->GetFaceIndex()) );
}


IFXINLINE BOOL IFXCornerIter::IsNotEqualFace(IFXCornerIter* pIter)
{
	return ( (m_mesh != pIter->GetMeshIndex()) || 
		     (m_face != pIter->GetFaceIndex()) );
}


IFXINLINE BOOL IFXCornerIter::IsEqualCorner(IFXCornerIter* pIter)
{
	return ( (m_mesh == pIter->GetMeshIndex()) && 
			 (m_face == pIter->GetFaceIndex()) &&
			 (m_corner == pIter->GetCornerIndex()) );
}


IFXINLINE BOOL IFXCornerIter::IsNotEqualCorner(IFXCornerIter* pIter)
{
	return ( (m_mesh != pIter->GetMeshIndex()) || 
			 (m_face != pIter->GetFaceIndex()) ||
			 (m_corner != pIter->GetCornerIndex()) );
}


IFXINLINE void IFXCornerIter::Clone(IFXCornerIter* pSrcIter)
{
	*this = *pSrcIter;
}


IFXINLINE void IFXCornerIter::MoveLink(IFXCornerIter* pDstIter)
{
	IFXNeighborFace* pSrcFace = &m_pNeighborFaces[m_face];

	pSrcFace->SetNeighbor(m_corner,
						  pDstIter->GetMeshIndex(),
						  pDstIter->GetFaceIndex(),
						  pDstIter->GetCornerIndex());
}

#endif
