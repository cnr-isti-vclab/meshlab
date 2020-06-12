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

/**
	@file	IFXMesh.h
*/

#ifndef IFX_MESH_H
#define IFX_MESH_H

#include "IFXMeshGroup.h"
#include "IFXNeighborFace.h"
#include "IFXVertexAttributes.h"
#include "IFXFace.h"
#include "IFXInterleavedData.h"
#include "IFXUnknown.h"
#include "IFXEnums.h"

class IFXUVMapParameters;
class IFXMatrix4x4;

IFXDEFINE_GUID(IID_IFXMesh,
0x177de3bf, 0xf9aa, 0x46ad, 0xa0, 0xac, 0x88, 0x15, 0x28, 0x70, 0x8b, 0xe0);

/// Renderable element types. Mesh can represent object of each of these types.
enum EIFXRenderable
{
	IFXRENDERABLE_ELEMENT_TYPE_MESH,
	IFXRENDERABLE_ELEMENT_TYPE_LINESET,
	IFXRENDERABLE_ELEMENT_TYPE_POINTSET,
	IFXRENDERABLE_ELEMENT_TYPE_GLYPH
};

/**
 	This object represents a single mesh (3D surface). The surface contains indexed 
	triangle lists. The triangle vertices contain a variable
 	number of vertex attributes that are specified upon allocation. These 
 	attributes can include positions, normals, colors, and texture coordinates.
 	The triangles are defined by an array of triangle faces, with each face
 	containing three vertex indices. These indices will be 32 bits in size if the
 	number of allocated vertices are more than 65536, otherwise the indices will
 	be 16 bits in size.
 	
 	The mesh is divided into two or more distinct memory pools: vertices and faces.
 	There could also be additional vertex pools for storing texture coordinates
 	depending upon the settings of the IFXVertexAttributes object passed in during
 	IFXMesh::Allocate(). These pools of memory are contained in IFXInterleavedData
 	objects.  
 */
class IFXMesh : virtual public IFXUnknown
{
public:
	/**
	 	Allocates (or reallocates) memory for the vertex and face pools depending
	 	on the vertex attributes and the number of vertices and faces that were passed in.
	 
	 	@param	vaAttribs	Input IFXVertexAttributes object that specifies what
	 						data is stored in each vertex.
	 
	 	@param	uNumVertices	Input U32 specifying how many vertices to allocate
	 							space for. The size of each vertex is dependent 
	 							on the settings of @a vaAttribs.
	 
	 	@param	uNumFaces		Input U32 specifying the number of faces to allocate 
	 							space for. The size of each face is dependent on
	 							how many vertices are being allocated. If there
	 							are less than 65536 vertices, each face is 6 bytes,
	 							otherwise each face is 12 bytes.
	 
	 	@return	IFXRESULT.
	 
	 	@retval	IFX_OK	No error.
	 	@retval	IFX_E_OUT_OF_MEMORY	There is not enough memory available to complete
	 								this method. The mesh is in an undefined state
	 								when this occurs.
	 */
	virtual IFXRESULT IFXAPI Allocate(	IFXVertexAttributes vaAttribs,
										U32 uNumVertices, 
										U32 uNumFaces )=0;

	/**
	 *	Copies the entire mesh data that lives in @a rSrcMesh into this mesh object.
	 *	Data that exists in the current mesh that does not get replaced with 
	 *	data in @a rSrcMesh will be preserved.  If this mesh object does not have
	 *	enough memory allocated for the data, this mesh will be reallocated.
	 *
	 *	@param	rSrcMesh	Input IFXMesh reference containing mesh data that will
	 *						be copied into this mesh object.
	 *
	 *	@return	IFXRESULT.
	 *	
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_OUT_OF_MEMORY	There is not enough memory available to complete
	 *								this method.  The mesh is in an undefined state
	 *								when this occurs.
	 */
	virtual IFXRESULT IFXAPI Copy( IFXMesh& rSrcMesh )=0;

	/**
	 *	Copies a portion of @a rSrcMesh into the current mesh object.  The
	 *	vertex attributes of @a rSrcMesh and this mesh must be identical.  This method 
	 *	allows a fast memcpy() data copy
	 *	rather than working element by element.  This method will clamp the
	 *	number of vertices copied to what is allocated in the target mesh.
	 *
	 *	@param	rSrcMesh	Input Reference to an IFXMesh containing mesh data to
	 *						be copied into this mesh.
	 *	@param	uStartVertex	Vertex index that begins the range of vertex data
	 *							to copy.
	 *	@param	uNumVertices	Number of vertices to copy.
	 *	@param	uStartFace	Face index that begins the range of face data to copy.
	 *	@param	uNumFaces	Number of faces to copy.
	 *
	 *	@return IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The vertex attributes of @a rSrcMesh are not 
	 *									compatible with this mesh.
	 */
	virtual IFXRESULT IFXAPI FastCopy(	IFXMesh& rSrcMesh, 
										U32 uStartVertex,
										U32 uNumVertices,
										U32 uStartFace,
										U32 uNumFaces )=0;


	/**
	 *	Conditionally duplicates the source mesh, copying some interleaved
	 *	data elements and referencing others. This allows meshes to be copied quickly, 
	 *	with only 
	 *	the data arrays that are going to be changed being duplicated. All the other data 
	 *	will be referenced from the source mesh. If an attribute that is present in 
	 *	the source mesh is not included in the Transfer or the Copy Attr Bits, it will
	 *	not be included in the new mesh. 
	 *
	 *	@param	rSrcMesh	Input reference to an IFXMesh containing mesh data to
	 *						be copied into this mesh.
	 *	@param	uTransferAttr	Bits representing the attributes that should be 
	 *			referenced from the Src Mesh. Specifing bits that are not present in 
	 *			Src mesh has no effect. 
	 *	@param	uCopyAttr	Bits representing the attributes of the source mesh that 
	 *			should be copied and not just referenced from the source mesh.
	 *			Specifying bits that reflect attributes not present in the 
	 *			source mesh will cause the new attribute arrays to be created.
	 *			In this case, Texture coordinates inserted this way will default to 
	 *			2D. 
	 *
	 *	@return IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_POINTER	The vertex attributes of @a rSrcMesh are not 
	 *									compatible with this mesh.
	 */
	virtual IFXRESULT IFXAPI TransferData(IFXMesh& rSrcMesh, 
										IFXMeshAttributes uTransferAttr,
										IFXMeshAttributes uCopyAttr )=0;


	/**
	 *	Increases the size of the MaxNumberVertices and 
	 *	MaxNumberFaces. If necessary, new vertex and face memory pools will
	 *	be allocated. All data contained in the mesh will be preserved.
	 *
	 *	If the current number of vertices is less than 65536 and the new 
	 *	number of vertices is greater than 65536, the faces will be converted
	 *	from 16 bit indices to 32 bit indices.  In this case, the index numbers
	 *	that currently exist will be preserved.
	 *
	 *	@param	uNumNewFaces	Number of new faces to increase the face pool by.
	 *	@param	uNumNewVerts	Number of new vertices to increase the vertex
	 *							pool by.
	 *
	 *	@return IFXRESULT.
	 *
	 *	@retval	IFX_OK No error.
	 *	@retval	IFX_E_OUT_OF_MEMORY	There is not enough memory available to complete
	 *								this method.  The mesh is in an undefined state
	 *								when this occurs.
	 */
	virtual IFXRESULT IFXAPI IncreaseSize(U32 uNumNewFaces, U32 uNumNewVerts)=0;

	/**
	 *	Retrieves an iterator to traverse the mesh's vertex data.
	 *
	 *	@param	iter	Output reference to an IFXVertexIterator that can
	 *					be used to iterate over the mesh's vertex data.
	 *
	 *	@return	IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI GetVertexIter(IFXVertexIter& iter)=0;

	/**
	 *	Retrieves an iterator to iterate over the mesh's vertex position data.
	 *
	 *	@param	iter	Output reference to an IFXVector3Iter that can be
	 *					used to iterate over the vertex position data 
	 *					of the mesh.
	 *
	 *	@return IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The mesh does not contain vertex positions.
	 */
	IFXRESULT IFXAPI GetPositionIter(IFXVector3Iter& iter);

	/**
	 *	Retrieves an iterator to iterate over the mesh's vertex normal data.
	 *
	 *	@param	iter	Output reference to an IFXVector3Iter that can be
	 *					used to iterate over the vertex normal data 
	 *					of the mesh.
	 *
	 *	@return An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The mesh does not contain vertex normals.
	 */
	IFXRESULT IFXAPI GetNormalIter(IFXVector3Iter& iter);

	/**
	 *	Retrieves an iterator to iterate over the mesh's vertex texture
	 *	coordinate data.  The @a iter parameter can be any type of iterator.
	 *	However, if the iterator type is bigger than the size of 
	 *	the texture coordinates (i.e. an IFXVector3Iter
	 *	to iterate over two component texture coordinates),
	 *	then this method will fail.
	 *
	 *	@param	iter	Output reference to an IFXIterator that can be
	 *					used to iterate over the texture coordinate data 
	 *					of the mesh.  
	 *
	 *	@param	uLayer	Input U32 representing which texture coordinate layer
	 *					(0-7) to iterate over.  The default is 0.
	 *					
	 *	@return An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The mesh does not contain the specified
	 *								texture coordinate layer @a uLayer, or
	 *								the @a iter object is too big for the 
	 *								allocated texture coordinates.
	 */
	IFXRESULT IFXAPI GetTexCoordIter(IFXIterator& iter, U32 uLayer=0);
	
	/**
	 *	Retrieves a vector iterator that will iterate over a portion of the vertex
	 *	data associated with the mesh.  This method will fail if the  
	 *	iterator type (say IFXVector4Iter) that is passed in is bigger than the data it is iterating
	 *	over (for example, Positions, which are IFXVector3 in size).
	 *
	 *	@param	eType	Input IFXenum value that specifies what the
	 *					iterator should iterate over.  Possible values are:
	 *					- @b IFX_MESH_POSITION: Iterate over vertex positions.
	 *					- @b IFX_MESH_NORMAL: Iterate over vertex normals.
	 *					- @b IFX_MESH_DIFFUSE_COLOR: Iterate over vertex diffuse colors.
	 *					- @b IFX_MESH_SPECULAR_COLOR: Iterate over vertex specular colors.
	 *					- @b IFX_MESH_TC0: Iterate over texture coordinate layer 0.
	 *					- @b IFX_MESH_TC1: Iterate over texture coordinate layer 1.
	 *					- @b IFX_MESH_TC2: Iterate over texture coordinate layer 2.
	 *					- @b IFX_MESH_TC3: Iterate over texture coordinate layer 3.
	 *					- @b IFX_MESH_TC4: Iterate over texture coordinate layer 4.
	 *					- @b IFX_MESH_TC5: Iterate over texture coordinate layer 5.
	 *					- @b IFX_MESH_TC6: Iterate over texture coordinate layer 6.
	 *					- @b IFX_MESH_TC7: Iterate over texture coordinate layer 7.
	 *
	 *	@param	iter	Output IFXIterator reference that will be set up to iterate over
	 *					the data specified with @a eType.
	 *
	 *	@return	IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	Either the attribute specified by @a eType does not
	 *								exist in the mesh, or the passed in @a iter is too
	 *								big for the data element that would be iterated over.
	 */
	virtual IFXRESULT IFXAPI GetVectorIter(IFXenum eType, IFXIterator& iter)=0;

	/**
	 *	Retrieves an IFXFaceIter that will work with both 16 and 32 bit vertex indices.
	 *	This iterator can be used to iterate over the faces in this mesh.  This will
	 *	be slower than using GetU16FaceIter() or GetU32FaceIter().
	 *
	 *	@param	iter	Ouptut IFXFaceIter reference that will iterate over the faces
	 *					contained in the mesh, regardless of face index size.
	 *
	 *	@return	IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI GetFaceIter(IFXFaceIter& iter)=0;

	/**
	 *	Retrieves an IFXU16FaceIter to iterate over the mesh's face data.  This
	 *	method will fail if the mesh contains 32 bit face indices.  The 
	 *	IFXU16FaceIter is much faster than using IFXFaceIter for the same data.
	 *
	 *	@param	iter	Output IFXU16FaceIter that can be used to iterate over
	 *					this mesh's face data.
	 *
	 *	@return IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The mesh contains 32 bit face indices, which 
	 *								are incompatible with IFXU16FaceIter.
	 */
	virtual IFXRESULT IFXAPI GetU16FaceIter(IFXU16FaceIter& iter)=0;

	/**
	 *	Retrieves an IFXU32FaceIter to iterate over the mesh's face data.  This
	 *	method will fail if the mesh contains 16 bit face indices.  The 
	 *	IFXU16FaceIter is much faster than using IFXFaceIter for the same data.
	 *
	 *	@param	iter	Output IFXU32FaceIter that can be used to iterate over
	 *					this mesh's face data.
	 *
	 *	@return IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The mesh contains 16 bit face indices, which 
	 *								are incompatible with IFXU32FaceIter.
	 */
	virtual IFXRESULT IFXAPI GetU32FaceIter(IFXU32FaceIter& iter)=0;


	/**
	 *	Retrieves an IFXLineIter that will work with both 16 and 32 bit vertex indices.
	 *	This iterator can be used to iterate over the lines in this mesh.  This will
	 *	be slower than using GetU16LineIter() or GetU32LineIter()
	 *
	 *	@param	iter	Ouptut IFXLineIter reference that will iterate over the Lines
	 *					contained in the mesh, regardless of Line index size.
	 *
	 *	@return	IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI GetLineIter(IFXLineIter& iter)=0;

	/**
	 *	Retrieves an IFXU16LineIter to iterate over the mesh's Line data.  This
	 *	method will fail if the mesh contains 32 bit Line indices.  The 
	 *	IFXU16LineIter is much faster than using IFXLineIter for the same data.
	 *
	 *	@param	iter	Output IFXU16LineIter that can be used to iterate over
	 *					this mesh's Line data.
	 *
	 *	@return IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The mesh contains 32 bit Line indices, which 
	 *								are incompatible with IFXU16LineIter.
	 */
	virtual IFXRESULT IFXAPI GetU16LineIter(IFXU16LineIter& iter)=0;

	/**
	 *	Retrieves an IFXU32LineIter to iterate over the mesh's Line data.  This
	 *	method will fail if the mesh contains 16 bit Line indices.  The 
	 *	IFXU16LineIter is much faster than using IFXLineIter for the same data.
	 *
	 *	@param	iter	Output IFXU32LineIter that can be used to iterate over
	 *					this mesh's Line data.
	 *
	 *	@return IFXRESULT.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The mesh contains 16 bit Line indices, which 
	 *								are incompatible with IFXU32LineIter.
	 */
	virtual IFXRESULT IFXAPI GetU32LineIter(IFXU32LineIter& iter)=0;


	virtual IFXRESULT IFXAPI GetMeshData(IFXenum eMeshAttribute, IFXInterleavedData*& pMeshData)=0;

	virtual IFXRESULT IFXAPI SetMeshData(IFXenum eMeshAttribute, IFXInterleavedData* pMeshData)=0;

	virtual IFXRESULT IFXAPI GetInterleavedVertexData(IFXInterleavedData*& pMeshData)=0;

	virtual IFXRESULT IFXAPI PurgeRenderData()=0;

	/**
	 *	Returns the current version word on a mesh or mesh element.  By
	 *  comparing this value to a cached version word, we can tell when
	 *  that element changes, and only update data depending on that
	 *  element when it changes.
	 */
	virtual IFXRESULT IFXAPI GetVersionWord(IFXenum eMeshAttribute, 
											U32& uVersionWord)=0;

	/**
	 *  Updates the version word on a mesh attribute, marking it as dirty to
	 *  any other parts of the system that are watching for that mesh
	 *  attribute to change.
	 */
	virtual IFXRESULT IFXAPI UpdateVersionWord(IFXenum eMeshAttribute)=0;
	
	/**
	 *	
	 */
	virtual IFXVertexAttributes IFXAPI GetAttributes()=0;
	
	/**
	 *	
	 */
	virtual U32 IFXAPI GetNumAllocatedVertices()=0;
	
	/**
	 *	
	 */
	virtual U32 IFXAPI GetMaxNumVertices()=0;
	
	/**
	 *	
	 */
	virtual U32 IFXAPI GetNumVertices()=0;

	/**
	 *	
	 */
	virtual U32 IFXAPI GetNumAllocatedFaces()=0;
	
	/**
	 *	
	 */
	virtual U32 IFXAPI GetMaxNumFaces()=0;
	
	/**
	 *	
	 */
	virtual U32 IFXAPI GetNumFaces()=0;


	/**
	 *	
	 */
	virtual IFXRESULT	IFXAPI AllocateLines(U32 uNumLines) = 0;

	/**
	 *	
	 */
	virtual U32 IFXAPI GetNumAllocatedLines()=0;
	
	/**
	 *	
	 */
	virtual U32 IFXAPI GetMaxNumLines()=0;
	
	/**
	 *	
	 */
	virtual U32 IFXAPI GetNumLines()=0;

	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI SetMaxNumVertices(U32 uNumVertices)=0;

	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI SetMaxNumFaces(U32 uNumFaces)=0;

	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI SetMaxNumLines(U32 uNumLines)=0;

	
	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI SetNumVertices(U32 uNumVertices)=0;
	
	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI SetNumFaces(U32 uNumFaces)=0;

	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI SetNumLines(U32 uNumLines)=0;

	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI GetBoundingSphere(IFXVector4& vBoundSphere)=0;
	
	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI SetBoundingSphere(const IFXVector4& vBoundSphere)=0;
	
	/**
	 *	
	 */
	virtual IFXRESULT IFXAPI CalcBoundingSphere()=0;

	/**
	 Indicates whether the color is BGR or not.	
	 */
	virtual BOOL IFXAPI IsColorBGR(U32 uColorNum=0)=0;

	/**
	 Switches the diffuse color channel from BGR to RGB and vice versa.	
	 */
	virtual IFXRESULT IFXAPI SwizzleDiffuseColors()=0;

	/**
	 Switches the specular color channel from BGR to RGB and vice versa.	
	 */
	virtual IFXRESULT IFXAPI SwizzleSpecularColors()=0;

	/**
	 Gets the unique ID number for this mesh
	 */
	virtual U32 IFXAPI GetId() const =0;

	virtual U32 IFXAPI NewId() = 0;

	virtual void IFXAPI SetId( U32 id ) = 0;


	// These next 3 are for caching results of automatic texture coordinate generators

	/**
	 *	Sets the set of render texture coordinates in use. This should be a combination of IFX_MESH_RENDER_TCx_BIT flags.
	 */
	virtual IFXRESULT IFXAPI SetRenderTexCoordsInUse(IFXMeshAttributes eTexCoordBits)=0;

	/**
	 *	Gets the set of render texture coordinates in use.
	 */
	virtual IFXMeshAttributes IFXAPI GetRenderTexCoordsInUse()=0;

	/**
	 *	Sets the number of texture coordinates in use.
	 */
	virtual IFXRESULT IFXAPI SetNumTextureCoords(U32 uNumTextureCoords)=0;

	/**
	 *	Gets the number texture coordinates in use.
	 */
	virtual U32 IFXAPI GetNumTextureCoords() const=0;

	/**
	 *	Gets the IFXUVMapParameters last used for this texture layer.
	 *
	 *	@param	uTexLayer 0 based texture layer index (0-IFX_MAX_TEXUNITS).
	 */
	virtual IFXUVMapParameters& IFXAPI GetUVMapParameters(U32 uTexLayer)=0;

	virtual IFXRESULT IFXAPI ReverseNormals()=0;

	/**
	Returns the offset transform for a mesh.

	@param pMatrix   The pointer to the transformation matrix

	@return An IFXRESULT value.
	@retval  IFX_OK   No error.
	*/
	virtual IFXRESULT IFXAPI  GetOffsetTransform( IFXMatrix4x4* pMatrix )const=0;
	/**
	Sets the offset transform for a mesh.

	@param pMatrix   The pointer to the transformation matrix

	@return An IFXRESULT value.
	@retval  IFX_OK   No error.
	*/
	virtual IFXRESULT IFXAPI  SetOffsetTransform( IFXMatrix4x4* pMatrix )=0;

protected:
	IFXMesh() {}
	virtual ~IFXMesh() {}

public:
	/**
	Returns the type of renderable which produced the mesh.
	@return An EIFXRenderable value.
	*/
	virtual EIFXRenderable IFXAPI GetRenderableType() const =0;
	/**
	Sets the type of renderable producing the mesh.

	@param type   The renderable type

	@return An IFXRESULT value.
	@retval  IFX_OK   No error.
	*/
	virtual void IFXAPI SetRenderableType( EIFXRenderable type ) = 0;
};
typedef IFXSmartPtr<IFXMesh> IFXMeshPtr;

// inlined methods
IFXINLINE IFXRESULT IFXMesh::GetPositionIter(IFXVector3Iter& iter)
{
	return GetVectorIter(IFX_MESH_POSITION, iter);
}

IFXINLINE IFXRESULT IFXMesh::GetNormalIter(IFXVector3Iter& iter)
{
	return GetVectorIter(IFX_MESH_NORMAL, iter);
}

IFXINLINE IFXRESULT IFXMesh::GetTexCoordIter(IFXIterator& iter, U32 uLayer)
{
	return GetVectorIter(IFXenum(IFX_MESH_TC0 + uLayer), iter);
}

#endif
