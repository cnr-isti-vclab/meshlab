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

#ifndef IFXFATCORNERITER_H
#define IFXFATCORNERITER_H

#include "IFXCornerIter.h"
#include "IFXIterators.h"

class IFXNeighborMesh;
class IFXMeshGroup;

/**
	@file	IFXFatCornerIter.h
			
			(nonlinear, corner-based, iterator)
			IFXFatCornerIter includes all the functionality of IFXCornerIter
			plus the ability to access the mesh attributes (face indices,
			vertex positions, and normals).

	@note	IFXFatCornerIter has more overhead than IFXCornerIter because
			it stores and advances mesh attribute iterators.
*/

class IFXFatCornerIter : public IFXCornerIter
{
public:
    IFXFatCornerIter();

  /** Gets the face structure for the current face.*/
    IFXFace* GetFace();

  /** Gets the vertex U16 of the current corner.*/
  U16 GetVertexIndex();

  /** Gets the vertex position of the current corner.*/
    IFXVector3* GetPosition();

  /** Gets the normal vector of the current corner.

  Returns NULL if current mesh does not have normals.*/
    IFXVector3* GetNormal();

  /** Gets the texture coordinate of the current corner.

  Returns NULL if current mesh does not have the texture layer.
  @note The texture layer was specified when 
  IFXNeighborMesh::GetFatCornerIter() was called.*/
    IFXVector2* GetTexCoord();

  /** Moves from the current corner to the
  clockwise corner on the same face.*/
    void MoveClockwise();

  /** Moves from the current corner to the
  counterclockwise corner on the same face.*/
    void MoveCounterClockwise();

  /** Moves to the next face in the linked-list of faces
  around the edge opposite the current corner.
  The current corner will be opposite the edge that
  was opposite the old corner.

   @retval  TRUE if mesh boundary crossed.*/
    BOOL JumpAcross();

  /** Same as JumpAcross() except the flags argument
   will contain all the flags from the jump.

  @retval TRUE if mesh boundary crossed.*/
  BOOL JumpAcross(IFXCornerFlags** ppFlags);

  /** 
	Makes this iterator like srcIter.
  
	@param pSrcIter A pointer to the input Corner Iterator.
  */
  void Clone(IFXFatCornerIter* pSrcIter);

private:
    friend class IFXNeighborMesh;

  /**
	Initialize IFXFatCornerIter.
		
	@note Can only be initialized by IFXNeighborMesh 
  
	@param pNeighborMesh A pointer to the input Neighbor Mesh.
	@param pMeshGroup A pointer to the input Mesh Group.
  */
  void Initialize(IFXNeighborMesh* pNeighborMesh,
				  U32 mesh, U32 face, U32 corner,
				  IFXMeshGroup* pMeshGroup,
				  U32 textureLayer);

  void LoadMesh();

  
  IFXMeshGroup* m_pMeshGroup; ///< pointer to attached mesh group
  U32 m_textureLayer;     ///< texture layer returned by GetTexCoord()
  IFXFace* m_pCurrFace;   ///< pointer to current face structure
  U16 m_currVertexIndex;  ///< vertex index of the current corner

  /// iterators initialized for the current mesh index
  IFXFaceIter m_faceIter;
  IFXVector3Iter m_positionIter;

  BOOL m_hasNormal;
  IFXVector3Iter m_normalIter;

  BOOL m_hasTexCoord;
  IFXVector2Iter m_texCoordIter;
};



IFXINLINE IFXFatCornerIter::IFXFatCornerIter() :
  m_pMeshGroup(NULL), m_textureLayer(0),
  m_pCurrFace(NULL), m_currVertexIndex(0),
  m_hasNormal(FALSE), m_hasTexCoord(FALSE)
{}

IFXINLINE void IFXFatCornerIter::MoveClockwise()
{
  // make sure iterator was initialized
  IFXASSERT(m_pMeshGroup);
  IFXCornerIter::MoveClockwise();

  // update current vertex U16
  m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());
}


IFXINLINE void IFXFatCornerIter::MoveCounterClockwise()
{
  // make sure iterator was initialized
  IFXASSERT(m_pMeshGroup);
  IFXCornerIter::MoveCounterClockwise();

  // update current vertex index
  m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());
}


IFXINLINE BOOL IFXFatCornerIter::JumpAcross()
{
  // make sure iterator was initialized
  IFXASSERT(m_pMeshGroup);
  BOOL isNewMesh = IFXCornerIter::JumpAcross();

  if (isNewMesh)
  {
    LoadMesh(); // load iterators for new mesh
  }

  // update pointer to current face
  m_pCurrFace = m_faceIter.Index(GetFaceIndex());

  // update current vertex U16
  m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());

  return isNewMesh;
}


IFXINLINE BOOL IFXFatCornerIter::JumpAcross(IFXCornerFlags** ppFlags)
{
  IFXASSERT(m_pMeshGroup);  // make sure iterator was initialized
  BOOL isNewMesh = IFXCornerIter::JumpAcross(ppFlags);

  if (isNewMesh)
  {
    LoadMesh(); // load iterators for new mesh
  }

  // update pointer to current face
  m_pCurrFace = m_faceIter.Index(GetFaceIndex());

  // update current vertex U16
  m_currVertexIndex = m_pCurrFace->Vertex(GetCornerIndex());

  return isNewMesh;
}


IFXINLINE IFXFace* IFXFatCornerIter::GetFace()
{
  IFXASSERT(m_pMeshGroup);  // make sure iterator was initialized
  return m_pCurrFace;
}


IFXINLINE U16 IFXFatCornerIter::GetVertexIndex()
{
  IFXASSERT(m_pMeshGroup);  // make sure iterator was initialized
  return m_currVertexIndex;
}


IFXINLINE IFXVector3* IFXFatCornerIter::GetPosition()
{
  IFXASSERT(m_pMeshGroup);  // make sure iterator was initialized
  return m_positionIter.Index(m_currVertexIndex);
}


IFXINLINE IFXVector3* IFXFatCornerIter::GetNormal()
{
  IFXASSERT(m_pMeshGroup);  // make sure iterator was initialized
  if (m_hasNormal)
    return m_normalIter.Index(m_currVertexIndex);
  else
    return NULL;

}


IFXINLINE IFXVector2* IFXFatCornerIter::GetTexCoord()
{
  IFXASSERT(m_pMeshGroup);  // make sure iterator was initialized
  if (m_hasTexCoord)
    return m_texCoordIter.Index(m_currVertexIndex);
  else
    return NULL;
}


IFXINLINE void IFXFatCornerIter::Clone(IFXFatCornerIter* pSrcIter)
{
  *this = *pSrcIter;
}

#endif
