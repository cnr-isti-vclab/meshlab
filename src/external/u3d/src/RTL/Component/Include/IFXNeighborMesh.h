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
// IFXNeighborMesh.h

#ifndef IFX_NEIGHBORMESH_H
#define IFX_NEIGHBORMESH_H

class IFXMeshGroup;

#include "IFXUnknown.h"
#include "IFXNeighborFace.h"
#include "IFXCornerIter.h"
#include "IFXFatCornerIter.h"
#include "IFXVertexMapGroup.h"


IFXDEFINE_GUID(IID_IFXNeighborMesh,
0xfd45358, 0x266e, 0x44c6, 0x92, 0xb2, 0x1e, 0x13, 0x74, 0xa8, 0x0, 0x6f);

class IFXNeighborMesh : virtual public IFXUnknown
{
public:
  /** Allocates memory for NeighborMesh in a parallel array
  * according to the sizes in IFXMeshGroup.
  *
  * @return IFX_E_OUT_OF_MEMORY if a memory allocation fails.*/
    virtual IFXRESULT IFXAPI Allocate(IFXMeshGroup& rMeshGroup)=0;

  /** Frees memory for NeighborMesh.*/
  virtual void IFXAPI Deallocate()=0;

  /** @return TRUE if Allocate() has been successfully called.*/
  virtual BOOL IFXAPI IsAllocated();

  /** @return TRUE if NeighborMesh connectivity is current.
  * @return FALSE if the NeighborMesh connectivity does not
  * match the connectivity in the IFXMeshGroup.
  * (Also known as the dirty flag.)*/
  virtual BOOL IFXAPI IsValid();

  /** Builds the NeighborMesh from the specified Meshgroup and vertex map.*/
  virtual IFXRESULT IFXAPI Build(IFXMeshGroup& rInMeshGroup, IFXVertexMapGroup* pInVertexMap=0)=0;

  /** Needed for Subdiv*/
  virtual IFXRESULT IFXAPI MarkAttributeDiscontinuities(IFXMeshGroup& rMeshGroup)=0;

  /** @return TRUE if the NeighborMesh is current.
   @return FALSE if a mesh was added/deleted from IFXMeshGroup,
  or if an associated IFXMesh has had a change in connectivity
  (if the faces have been changed/added/deleted).*/
  virtual void IFXAPI SetValid(/*in*/ BOOL state);

  /** Returns the number of meshes (partitions) in attached IFXMeshGroup.
  @return 0 if Allocate() has not been called.*/
  virtual U32 IFXAPI GetNumMeshes();

  /** Returns a pointer to the array of neighbor faces.
  This is used to perform an operation that requires iterating
   through all the neighbor faces, for example
  initializing the neighbor links.

    Allocate() must have been called (and successful) before
  this method is used.*/

  /** Calls GetNumMeshes() to determine maximum meshIndex.
  Call GetNumFaces() from the associated IFXMesh to determine
  the maximum index into the IFXNeighborFace array.*/
    virtual IFXNeighborFace* IFXAPI GetNeighborFaceArray(/*in*/ U32 meshIndex);

  /** Gets the lightweight corner iterator for a corner of
   a face on a mesh.  The corner iterator is used for moving
   from one face to its neighbor faces.

   Call GetNumMeshes() to determine maximum meshIndex.*/
  virtual void IFXAPI GetCornerIter(  /*in*/ U32 meshIndex,
                    /*in*/ U32 faceIndex,
                    /*in*/ U32 cornerIndex,
                    /*in-out*/ IFXCornerIter& rCornerIter);

  /** Gets the general purpose corner iterator for a corner of
   a face on a mesh.  This iterator provides methods that access
  face and vertex attribute information on the mesh.  For
  performance reasons, you must specify which texture layer
  you want access to.

  Calls GetNumMeshes() to determine maximum meshIndex.*/
  virtual void IFXAPI GetFatCornerIter( /*in*/ U32 textureLayer,
                      /*in*/ U32 meshIndex,
                      /*in*/ U32 faceIndex,
                      /*in*/ U32 cornerIndex,
                      /*in-out*/ IFXFatCornerIter& rCornerIter);

protected:
  IFXNeighborMesh() {}
  virtual ~IFXNeighborMesh() {}

  IFXMeshGroup* m_pMeshGroup;
  BOOL m_validState;
  U32 m_numMeshes;
  IFXNeighborFace** m_ppNeighborFaces;
};
typedef IFXSmartPtr<IFXNeighborMesh> IFXNeighborMeshPtr;

IFXINLINE BOOL IFXNeighborMesh::IsAllocated()
{
  return BOOL(NULL != m_pMeshGroup);
}


IFXINLINE BOOL IFXNeighborMesh::IsValid()
{
  return m_validState;
}

IFXINLINE void IFXNeighborMesh::SetValid(BOOL state)
{
  m_validState = state;
}

IFXINLINE U32 IFXNeighborMesh::GetNumMeshes()
{
  return m_numMeshes;
}

IFXINLINE IFXNeighborFace* IFXNeighborMesh::GetNeighborFaceArray(/*in*/ U32 meshIndex)
{
  IFXASSERT(m_ppNeighborFaces);
  IFXASSERT(meshIndex < m_numMeshes);
  return m_ppNeighborFaces[meshIndex];
}

IFXINLINE void IFXNeighborMesh::GetCornerIter(/**in*/ U32 meshIndex,
                      /**in*/ U32 faceIndex,
                      /**in*/ U32 cornerIndex,
                      /**in-out*/ IFXCornerIter& rCornerIter)
{
  IFXASSERT(meshIndex < m_numMeshes);
  IFXASSERT(cornerIndex < 3);
  rCornerIter.Initialize(this, meshIndex, faceIndex, cornerIndex);

}

IFXINLINE void IFXNeighborMesh::GetFatCornerIter(/**in*/ U32 textureLayer,
                        /**in*/ U32 meshIndex,
                        /**in*/ U32 faceIndex,
                        /**in*/ U32 cornerIndex,
                        /**in-out*/ IFXFatCornerIter& rCornerIter)
{
  IFXASSERT(meshIndex < m_numMeshes);
  IFXASSERT(cornerIndex < 3);
  rCornerIter.Initialize(this, meshIndex, faceIndex, cornerIndex, m_pMeshGroup, textureLayer);
}


#endif
