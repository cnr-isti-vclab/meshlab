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
	@file	IFXAuthorMesh.h
*/

#ifndef IFXAuthorMesh_H__
#define IFXAuthorMesh_H__

#include "IFXAuthor.h"
#include "IFXEnums.h"
#include "IFXUnknown.h"
#include "IFXVector3.h"
#include "IFXVector4.h"

// {677D73FB-6CC-4BC7-AC4C-77110EFDA9C7}
IFXDEFINE_GUID(IID_IFXAuthorMesh,
0x677D73FB, 0x6CC, 0x4BC7, 0xAC, 0x4C, 0x77, 0x11, 0x0E, 0xFD, 0xA9, 0xC7);

#define IFX_E_AUTHOR_MESH_LOCKED \
		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_AUTHORGEOM, 0x01)

#define IFX_E_AUTHOR_MESH_NOT_LOCKED \
		MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_AUTHORGEOM, 0x02)

#define IFX_AUTHOR_INVALID_INDEX (U32)-1

/**
*  A structure that contains the data for a face for a given mesh attribute
*  (position, normal, texture coordinate, etc.)
*/
struct IFXAuthorFace
{
	U32& VertexA();
	U32& VertexB();
	U32& VertexC();
	IFXAuthorFace& Set(U32, U32, U32);
	U32 corner[3];

	IFXAuthorFace()
	{
		Set(0,0,0);
	}
};

const U32 IFX_BAD_INDEX32 = 0xFFFFFFFF;

enum IFXMESH_FLAGS
{
	IFXMESH_UNDEFINED			= 0x00000000,
	IFXMESH_UNLOCKED			= 0x00000001,
	IFXMESH_LOCKED				= 0x00000002,
	IFXMESH_ALLOCATED			= 0x00000004,
	IFXMESH_WANTATTRIBUTEMAPS   = 0x00000010,
	IFXMESH_FORCEDWORD			= 0x7FFFFFFF
};

enum IFXAuthorMeshNormalGen
{
	IFXAuthorMeshNormalGen_None = 0,
	IFXAuthorMeshNormalGen_Flat = 1,
	IFXAuthorMeshNormalGen_Smooth = 2,
	IFXAuthorMeshNormalGen_SmoothByMaterial = 3,

	IFXAuthorGeomNormalGen_FORCEDWORD = 0
};

/**
*  Provides high-level information about a mesh; namely, what attributes
*  are present in the mesh and how many elements can be found in each
*  attribute array.
*/
struct IFXAuthorMeshDesc
{
	IFXAuthorMeshDesc();
	U32 NumFaces;			///< Number of faces structures in each face
	U32 NumPositions;		///< Number of position
	U32 NumNormals;			///< Number of Normals
	U32 NumDiffuseColors;	///< Number of Diffuse Colors
	U32 NumSpecularColors;	///< Number of Specular Colors
	U32 NumTexCoords;		///< Number of Texture Coordinates.
	U32 NumMaterials;		///< Number of Materials
	U32 NumBaseVertices;	///< Number of Base Vertices
};

IFXINLINE IFXAuthorMeshDesc::IFXAuthorMeshDesc()
{
	IFXInitStruct(this);
}

class IFXAuthorMesh;

/**
	This is a helper class for IFXAuthorMesh to allow for mesh locks to be
	released automatically when the mesh locker goes out of scope.  These
	locker objects hold a reference to the locked IFXAuthorMesh to insure
	that the IFXAuthorMesh object is valid when it comes time to release it.
	\n
	These objects are intended to be used on the stack(not dynamically
	allocated) as follows:

	@verbatim
	void func(IFXAuthorMesh* pAM)
	{
		IFXAuthorMeshLocker AMLock(pAM);

		// Use the AM functions that require locking


		// AMLock Automatically releases the lock when it destructs
	}
	@endverbatim
*/
class CIFXAuthorMeshLocker
{
public:
	/**
	* Constructs a locker and an optional lock on an author mesh.
	*
	* @param pInAuthorMesh  The mesh to lock
	*/
	CIFXAuthorMeshLocker(IFXAuthorMesh *pInAuthorMesh = NULL);

	/**
	*  Destructs the Locker object. If an author mesh lock is being held,
	*  the lock is automatically released.
	*/
	virtual ~CIFXAuthorMeshLocker();

	/**
	*  Locks an IFXAuthorMesh.  If a lock is already held, that lock is
	*  released and a new lock is obtained.
	*
	*  @param  pInAuthorMesh  The mesh to lock
	*
	*  @return	An IFXRESULT value
	*  @retval	IFX_OK	   No error.
	*/
	IFXRESULT Lock(IFXAuthorMesh* pInAuthorMesh);

	/**
	*  Releases a lock if one is held on an IFXAuthorMesh.
	*
	*  @return	An IFXRESULT value
	*  @retval	IFX_OK	   No error.
	*/
	IFXRESULT Unlock();

private:
	IFXAuthorMesh* m_pAM;
};

/**
*  Used during population of a mesh to track the offsets where the next
*  attribute should be stored.
*/
typedef IFXAuthorMeshDesc IFXAuthorMeshOffsets;


/**
	This is the "author" representation of meshes to be loaded.
	Unlike IFXMeshGroup, which is optimized for rendering
	and not ease of use, this representation should closely mirror
	the mesh format found in many 3D authoring packages.\n
	IFXAuthorMesh uses arrays of elements for each mesh attribute (position,
	normal, texture coordinate, etc.), and arrays of IFXAuthorFace structures
	which specify which three elements of the same attribute are used for each
	face.  Said another way, each face has an IFXAuthorFace structure that
	specifies which vertices it uses in the vertex pool, another
	IFXAuthorFace structure that says which normal it is uses in the normal
	pool for each vertex in the face, etc.\n
	These arrays of IFXAuthorFace structures and mesh attributes must be
	pre-allocated using the Allocate() or Reallocate() methods before
	data may be loaded into the mesh.
*/
class IFXAuthorMesh : public IFXUnknown
{
public:
	/**
	*  Allocates all the necessary space required by the fully-populated
	*  IFXAuthorMesh.
	*
	*  @param pInMeshDesc  The IFXAuthorMeshDesc data structure describing
	*                 the properties of the mesh.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pInMeshDesc was NULL
	*  @retval IFX_E_INVALID_RANGE  The number of materials or positions
	*     was less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  There was insufficiant memory to allocate
	*     the mesh.
	*/
	virtual IFXRESULT IFXAPI Allocate(const IFXAuthorMeshDesc* pInMeshDesc) = 0;

	/**
	*  Reallocates the mesh based on the new mesh description
	*
	*  @param  pInMeshDesc  The new properties of the mesh.
    *
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pInMeshDesc was NULL
	*  @retval IFX_E_INVALID_RANGE  The number of materials or positions
	*     was less than one.
	*  @retval IFX_E_OUT_OF_MEMORY  There was insufficient memory to allocate
	*     the mesh.
	*/
	virtual IFXRESULT IFXAPI Reallocate(const IFXAuthorMeshDesc* pInMeshDesc)	= 0;

	/**
	*  Frees all data associated with the mesh.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*/
	virtual IFXRESULT IFXAPI Deallocate() = 0;

	/**
	*  Creates a complete copy of the IFXAuthorMesh and returns it with
	*  the specified interface.
	*
	*  @param  interfaceID  The IID of the desired mesh interface
	*  @param  ppOutNewAuthorMesh  A pointer to the mesh copy.
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*/
	virtual IFXRESULT IFXAPI Copy(IFXREFIID interfaceID, void** ppOutNewAuthorMesh)= 0;

	/**
	*  Generates mesh normals based on the face and vertex data present
	*  in the mesh.
	*
	*  @param  in_NormalType  The type of normals to generate (none, flat,
	*     smooth, or smooth by material).
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_LOCKED  The mesh was locked with this operation
	*     was requested.  Unlock the mesh and try again.
	*  @retval IFX_E_BAD_PARAM  A bad value was passed in through in_NormalType.
	*/
	virtual IFXRESULT IFXAPI GenerateNormals(IFXAuthorMeshNormalGen in_NormalType)	= 0;

	/**
	*  Returns the current mesh description.
	*
	*  @return A pointer to an IFXAuthorMeshDesc structure describing the
	*     mesh.
	*/
	virtual const IFXAuthorMeshDesc* IFXAPI GetMeshDesc() = 0;

	/**
	*  Sets the current mesh description (as opposed to the "MaxMeshDesc,"
	*  which is set when the mesh is allocated).
	*
	*  @param pInMeshDesc  The new mesh description
	*
	*  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_RANGE One of the values in pInMeshDesc is
	*     larger than the IFXAuthorMeshDesc used to allocate the mesh,
	*     and/or the number of materials is not the same as when the mesh
	*     was allocated.
	*/
	virtual IFXRESULT IFXAPI SetMeshDesc(const IFXAuthorMeshDesc* pInMeshDesc) = 0;

	/**
	*  Returns the maximum resolution mesh description (ignores current
	*  resolution setting or streaming state).
	*
	*  @return A pointer to an IFXAuthorMeshDesc structure describing the
	*     full-resolution representation of the mesh.
	*/
	virtual const IFXAuthorMeshDesc* IFXAPI GetMaxMeshDesc() = 0;

	/**
	*  Returns the number of texface layers, where texFaces layers are
	*  allocated on demand to save memory.
	*
	*  @param pOutLayers  The number of layers allocated
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
    *  @retval IFX_E_INVALID_POINTER  pOutLayers was NULL
	*/
	virtual IFXRESULT IFXAPI GetNumAllocatedTexFaceLayers(U32* pOutLayers) = 0;

	/**
	*  Locks the mesh so that the array pointers for the individual
	*  attributes can be accessed. Any pointer retrieved after calling this
	*  method should not be used after the matching Unlock() call.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_LOCKED The mesh is already locked.
	*/
	virtual IFXRESULT IFXAPI Lock() = 0;

	/**
	*  Returns the pointer to the Material Definition Array.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param  ppOutMaterials  The pointer to the material definition array
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED  The Mesh is not locked. The mesh
	*       must be locked with Lock() to gain access to the Material
	*       Definition Array.
	*  @retval IFX_E_INVALID_POINTER   ppOutMaterials was NULL.
	*/
	virtual IFXRESULT IFXAPI GetMaterials(IFXAuthorMaterial** ppOutMaterials) = 0;

	/**
	*  Returns the pointer to the Face Position index array.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutPositionFaces  The array of IFXAuthorFaces specifying
	*     the indices of the positions for each vertex on each face. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Position
	*      Face array.
	*  @retval IFX_E_INVALID_POINTER ppOutPositionFaces was NULL.
	*/
	virtual IFXRESULT IFXAPI GetPositionFaces(IFXAuthorFace** ppOutPositionFaces)=  0;

	/**
	*  Returns the pointer to the Face Normal index array.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutNormalFaces  The array of IFXAuthorFaces specifying
	*     the indices of the normals used by each vertex on each face. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Normal
	*      Face array.
	*  @retval IFX_E_INVALID_POINTER   ppOutNormalFaces was NULL
	*  @retval IFX_E_INVALID_RANGE     The mesh does not contain this
	*      attribute. NumNormals in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetNormalFaces(IFXAuthorFace** ppOutNormalFaces) = 0;

	/**
	*  Returns the pointer to the Face Diffuse index array.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutDiffuseFaces  The array of IFXAuthorFaces specifying
	*     the indices of the diffuse colors used by each vertex on each face. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Diffuse
	*      Faces array.
	*  @retval IFX_E_INVALID_POINTER ppOutDiffuseFaces was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumDiffuseColors in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetDiffuseFaces(IFXAuthorFace** ppOutDiffuseFaces) = 0;

	/**
	*  Returns the pointer to the Face Specular index array
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutSpecularFaces  The array of IFXAuthorFaces specifying
	*     the indices of the specular colors used by each vertex on each face. 
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Specular
	*      Faces array.
	*  @retval IFX_E_INVALID_POINTER ppOutSpecularFaces was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumSpecularColors in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetSpecularFaces(IFXAuthorFace** ppOutSpecularFaces) = 0;


	/**
	*  Returns the pointer to the Face Texture index array.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param index   The index of the texture layer to recover
	*  @param ppOutTexFaces  The array of IFXAuthorFaces specifying
	*     the indices of the texture coordinates used by each vertex 
	*     on each face for the specified texture layer.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Texture
	*      Faces array.
	*  @retval IFX_E_INVALID_POINTER ppOutTexFaces was NULL.
	*  @retval IFX_E_INVALID_RANGE	 The Texture Layer index is invalid. The
	*        value specified for index may be out of range.  This parameter
	*        cannot be less than zero(0) or greater than
	*        IFXMAX_TEX_COORD_LAYERS.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumTexCoords in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetTexFaces(U32 index, IFXAuthorFace** ppOutTexFaces) = 0;

	/**
	*  Returns the pointer to the Face Material index array.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutFaceMaterials  The array of U32es specifying
	*     the material index to use on each face of the mesh
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Material
	*      Faces array.
	*  @retval IFX_E_INVALID_POINTER ppOutFaceMaterials was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumMaterials in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetFaceMaterials(U32** ppOutFaceMaterials)= 0;

	/**
	*  Return the pointer to the Base vertex array.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutBaseVertices  The array of U32es specifying
	*     the indices of the vertices to treat as base vertices.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Base
	*      Vertex array.
	*  @retval IFX_E_INVALID_POINTER ppOutBaseVertices was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumBaseVertices in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetBaseVertices(U32** ppOutBaseVertices)= 0;

	/**
	*  Returns the pointer to the Position Array for this mesh.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutPositions  The array of IFXVector3s containing all the
	*     vertex positions used in this mesh.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED  The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Vertex
	*      Position array.
	*  @retval IFX_E_INVALID_POINTER ppOutPositions was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumPositions in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetPositions(IFXVector3** ppOutPositions) = 0;

	/**
	*  Returns the pointer to the Normal array for this mesh.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutNormals  The array of IFXVector3s containing all the
	*     normals used in this mesh.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED  The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Vertex
	*      Normal array.
	*  @retval IFX_E_INVALID_POINTER ppOutNormals was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumNormals in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetNormals(IFXVector3** ppOutNormals) = 0;

	/**
	*  Returns the pointer to the array of Diffuse Vertex Colors for this mesh.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutDiffuseColors  The array of IFXVector4s containing the
	*     diffuse vertex colors used by this mesh.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED  The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Diffuse
	*      Vertex color array.
	*  @retval IFX_E_INVALID_POINTER ppOutDiffuseColors was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumDiffuseColors in the IFXAuthorMeshDesc is equal to zero(0).
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI GetDiffuseColors(IFXVector4** ppOutDiffuseColors) = 0;

	/**
	*  Returns the pointer to the array of Specular Vertex Colors for this mesh.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutSpecularColor  The array of IFXVector4s containing the
	*     specular vertex colors used by this mesh.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED  The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Specular
	*      Vertex color array.
	*  @retval IFX_E_INVALID_POINTER ppOutSpecularColors was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumSpecularColors in the IFXAuthorMeshDesc is equal to zero(0).
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI GetSpecularColors(IFXVector4** ppOutSpecularColors) = 0;

	/**
	*  Returns the pointer to the array of Texture Coordinates for  this mesh.
	*  The Mesh must be locked to use this method. After
	*  the Mesh is unlocked, any pointer retrieved with this method
	*  is not guaranteed to be valid.
	*
	*  @param ppOutTexCoords  The array of IFXVector4s containing the
	*     texture coordinates used by this mesh.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED  The Mesh is not locked. The
	*      mesh must be locked with Lock() to gain access to the Texture
	*      coordinate array.
	*  @retval IFX_E_INVALID_POINTER ppOutTexCoords was NULL.
	*  @retval IFX_E_INVALID_RANGE   The mesh does not contain this attribute.
	*      NumTexCoords in the IFXAuthorMeshDesc is equal to zero(0).
	*/
	virtual IFXRESULT IFXAPI GetTexCoords(IFXVector4** ppOutTexCoords) = 0;

	/**
	*  Unlock the mesh. Any pointers retrieved since the Lock call
	*  should not be used after this call.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_AUTHOR_MESH_NOT_LOCKED  The Mesh is not locked.
	*/
	virtual IFXRESULT IFXAPI Unlock() = 0;

	/**
	*  Retrieves a specific material by index.
	*
	*  @param index  The index of the material to return
	*  @param pOutMaterial  A pointer to the material.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER pOutMaterial was NULL.
	*  @retval IFX_E_INVALID_RANGE The index is invalid
	*/
	virtual IFXRESULT IFXAPI GetMaterial(U32 index,
							IFXAuthorMaterial* pOutMaterial)	= 0;


	/**
	*  Sets a specific material face.
	*
	*  @param index  The index of the material to set
	*  @param pInMaterial  A pointer to the material to assign to this index.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER pInMaterial was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number of
	*      materials used by the mesh.
	*/
	virtual IFXRESULT IFXAPI SetMaterial(
									U32 index, 
									const IFXAuthorMaterial* pInMaterial) = 0;

	/**
	*  Retrieves a specific Position face index.
	*
	*  @param index  The index of the position face to return
	*  @param pOutPositionFace  The returned face containing position indices
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutPositionFace was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of position faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetPositionFace(
									U32 index,
									IFXAuthorFace* pOutPositionFace) = 0;

	/**
	*  Sets a specific Position Face index.
	*
	*  @param index  The index of the position face to set
	*  @param pInPositionFace  The position face containing the position
	*							index data to set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInPositionFace was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of position faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetPositionFace(
									U32 index,
									const IFXAuthorFace* pInPositionFace) = 0;

	/**
	*  Retrieves a specific Normal face index.
	*
	*  @param index  The index of the normal face to return
	*  @param pOutNormalFace  The returned face containing normal indices
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutNormalFace was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of normal faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetNormalFace(
									U32 index,
									IFXAuthorFace* pOutNormalFace) = 0;


	/**
	*  Set a specific Normal Face index.
	*
	*  @param index  The index of the normal face to return
	*  @param pInNormalFace  The normal face containing the normal
	*							index data to set.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutNormalFace was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of normal faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetNormalFace(
									U32 index,
									const IFXAuthorFace* pInNormalFace) = 0;

	/**
	*  Retrieves a specific Diffuse vertex color face index.
    *
	*  @param index  The index of the diffuse face to return
	*  @param pOutDiffuseFace  The diffuse face containing the diffuse vertex
	*                           color indices.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutDiffuseFace was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of diffuse vertex color faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetDiffuseFace(
									U32 index,
									IFXAuthorFace* pOutDiffuseFace) = 0;
	/**
	*  Set a specific Diffuse Face index.
    *
	*  @param index  The index of the diffuse face to set
	*  @param pInDiffuseFace  The diffuse face containing the diffuse vertex
	*                           color index data to set
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInDiffuseFace was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of diffuse vertex color faces in the mesh.
	*
	*/
	virtual IFXRESULT IFXAPI SetDiffuseFace(
									U32 index, 
									const IFXAuthorFace* pInDiffuseFace) = 0;

	/**
	*  Retrieves a specific Specular face index.
	*
	*  @param index  The index of the specular face to return
	*  @param pOutSpecularFace  The specular face containing the specular
	*                           vertex color indices.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutSpecularFace was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of specular vertex color faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetSpecularFace(
									U32 index,
									IFXAuthorFace* pOutSpecularFace) = 0;

	/**
	*  Set a specific Specular Face index.
    *
	*  @param index  The index of the specular face to set
	*  @param pInSpecularFace  The specular face containing the specular
	*                           vertex color index data to set
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInSpecularFace was NULL
	*  @retval IFX_E_INVALID_RANGE  The index is greater than the number
	*     of specular vertex color faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetSpecularFace(
									U32 index,
									const IFXAuthorFace* pInSpecularFace) = 0;


	/**
	*  Retrieves a specific Texture face index.
	*
	*  @param layer  The texture coordinate layer this face is from
	*  @param index  The index of the texture face to return
	*  @param pOutTextureFace  The texture face containing the texture
	*                           coordinate indices
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutTextureFace was NULL
	*  @retval IFX_E_INVALID_RANGE  index is greater than the number
	*     of texture coordinate faces in the mesh, the mesh has no texture
	*     coordinates, or layer is greater than IFXMAX_TEX_COORD_LAYERS.
	*  @retval IFX_E_OUT_OF_MEMORY  We were not able to allocate memory for
	*     a new texture layer.
	*/
	virtual IFXRESULT IFXAPI GetTexFace(
								U32 layer, U32 index,
								IFXAuthorFace* pOutTextureFace) = 0;

	/**
	*  Sets a specific Texture Face index.
	*
	*  @param layer  The texture coordinate layer into which to
	*               place the face
	*  @param index  The index of the texture face to set
	*  @param pOutTextureFace  The texture face containing the texture
	*                           coordinate indices to set in the mesh
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInTextureFace was NULL
	*  @retval IFX_E_INVALID_RANGE  index is greater than the number
	*     of texture coordinate faces in the mesh, the mesh has no texture
	*     coordinates, or layer is greater than IFXMAX_TEX_COORD_LAYERS.
	*  @retval IFX_E_OUT_OF_MEMORY  We were not able to allocate memory for
	*     a new texture layer.
	*/
	virtual IFXRESULT IFXAPI SetTexFace(
								U32 layer, U32 index,
								const IFXAuthorFace* pInTextureFace) = 0;

	/**
	*  Retrieves a specific Material face ID.
	*
	*  @param index   The index of the face for which we want the material ID
	*  @param pOutuFaceMaterial  The returned material ID.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutuFaceMaterial was NULL
	*  @retval IFX_E_INVALID_RANGE  index was larger than the number of
	*               faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetFaceMaterial(U32 index, U32* pOutuFaceMaterial)= 0;

	/**
	*  Sets a specific Face Material ID
	*
	*  @param index   The index of the face on which we want to set the
	*           material ID.
	*  @param faceMaterialID  The material ID to set on the face.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_RANGE  index was larger than the number of
	*               faces in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetFaceMaterial(U32 index, U32 faceMaterialID) = 0;

	/**
	*  Retrieves a specific Base Vertex (which is the index of a vertex
	*  to treat as a base vertex in clodolution calculations).
	*
	*  @param index   The index of the base vertex for which we want the
	*                  vertex ID
	*  @param pOutBaseVertex  The returned vertex ID.
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutBaseVertex was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of base
	*      vertices in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetBaseVertex(U32 index, U32* pOutBaseVertex)= 0;

	/**
	*  Sets a Base Vertex to a specific vertex ID
	*
	*  @param index   The index of the base vertex to set
	*                  the vertex ID
	*  @param baseVertex  The vertex ID to load into this base vertex
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of base
	*      vertices in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetBaseVertex(U32 index, U32 baseVertex) = 0;

	/**
	*  Retrieves a specific vertex position from the mesh.
	*
	*  @param index   The index of the vertex position to return
	*  @param pOutVector3  The position of the specified vertex
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       vertex positions stored in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetPosition(U32 index, IFXVector3* pOutVector3) = 0;

	/**
	*  Sets a specific vertex position for this mesh.
	*
	*  @param index   The index of the vertex position to set
	*  @param pInVector3  The new position value for this vertex
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       vertex positions stored in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetPosition(U32 index, const IFXVector3* pInVector3) = 0;

	/**
	*  Retrieves a specific Normal from this mesh.
	*
	*  @param index   The index of the normal to return
	*  @param pOutVector3  The requested normal
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       normals stored in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetNormal(U32 index, IFXVector3* pOutVector3) = 0;

	/**
	*  Sets a specific Normal for this mesh.
	*
	*  @param index   The index of the normal to set
	*  @param pInVector3  The new value of the normal
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInVector3 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       normals stored in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetNormal(U32 index, const IFXVector3* pInVector3) = 0;

	/**
	*  Retrieves a specific Diffuse vertex color from this mesh.
	*
	*  @param index   The index of the Diffuse vertex color to return
	*  @param pOutColor  The requested color
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       diffuse vertex colors stored in the mesh.
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI GetDiffuseColor(U32 index, IFXVector4* pOutColor) = 0;

	/**
	*  Sets a specific Diffuse vertex color for this mesh.
	*
	*  @param index   The index of the diffuse vertex color to set
	*  @param pInColor  The new diffuse vertex color to store
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       diffuse vertex colors stored in the mesh.
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI SetDiffuseColor(U32 index, IFXVector4* pInColor) = 0;

	/**
	*  Retrieves a specific Specular vertex color from this mesh.
	*
	*  @param index   The index of the specular vertex color to return
	*  @param pInColor  The requested color value
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       specular vertex colors stored in the mesh.
	*
	*  @note  Colors are represented in BGRA order.
	*/
	virtual IFXRESULT IFXAPI GetSpecularColor(U32 index, IFXVector4* pOutColor)= 0;

	/**
	*  Sets a specific Specular vertex color of this mesh.
	*
	*  @param index   The index of the specular vertex color to set
	*  @param pInColor  The new specular vertex color to store
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInColor was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       specular vertex colors stored in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetSpecularColor(U32 index, IFXVector4* pInColor)= 0;

	/**
	*  Retrieves a specific Texture Coordinate from this mesh.
	*
	*  @param index   The index of the texture coordinate to return
	*  @param pOutVector4  The requested texture coordinate
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pOutVector4 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       texture coordinates stored in the mesh.
	*/
	virtual IFXRESULT IFXAPI GetTexCoord(U32 index, IFXVector4* pOutVector4) = 0;

	/**
	*  Sets specific Texture Coordinate for this mesh.
	*
	*  @param index   The index of the texture coordinate to set
	*  @param pInVector4  The new texture coordinate value to store
	*
    *  @return An IFXRESULT value.
	*  @retval IFX_OK	No error.
	*  @retval IFX_E_INVALID_POINTER  pInVector4 was NULL
	*  @retval IFX_E_INVALID_RANGE  index is larger than the number of
	*       texture coordinates stored in the mesh.
	*/
	virtual IFXRESULT IFXAPI SetTexCoord(U32 index, const IFXVector4* pInVector4) = 0;

	virtual IFXVector4 IFXAPI CalcBoundSphere() = 0;
};

IFXINLINE U32& IFXAuthorFace::VertexA()
{
	return corner[0];
}

IFXINLINE U32& IFXAuthorFace::VertexB()
{
	return corner[1];
}

IFXINLINE U32& IFXAuthorFace::VertexC()
{
	return corner[2];
}

IFXINLINE IFXAuthorFace& IFXAuthorFace::Set(U32 in_a, U32 in_b, U32 in_c)
{
	corner[0] = in_a;
	corner[1] = in_b;
	corner[2] = in_c;
	return *this;
}

IFXINLINE CIFXAuthorMeshLocker::CIFXAuthorMeshLocker(IFXAuthorMesh* pInAM)
{
	m_pAM = NULL;
	Lock(pInAM);
}

IFXINLINE CIFXAuthorMeshLocker::~CIFXAuthorMeshLocker()
{
	Unlock();
}

IFXINLINE IFXRESULT CIFXAuthorMeshLocker::Lock(IFXAuthorMesh* pInAM)
{
	IFXRESULT ir = IFX_OK;
	Unlock();
	m_pAM = pInAM;
	if(m_pAM)
	{
		m_pAM->AddRef();
		ir = m_pAM->Lock();
		if(IFXFAILURE(ir))
		{
			m_pAM->Release();
			m_pAM = NULL;
		}
	}
	return ir;
}

IFXINLINE IFXRESULT CIFXAuthorMeshLocker::Unlock()
{
	IFXRESULT ir = IFX_OK;
	if(m_pAM)
	{
		ir = m_pAM->Unlock();
		m_pAM->Release();
		m_pAM = NULL;
	}
	return ir;
}

#endif
