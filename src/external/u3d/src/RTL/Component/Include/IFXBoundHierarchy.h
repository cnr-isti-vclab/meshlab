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
	@file  IFXBoundHierarchy.h

			The header file that defines the IFXBoundHierarchy interface.
*/

#ifndef __IFXBOUNDHIERARCHY_INTERFACE_H__
#define __IFXBOUNDHIERARCHY_INTERFACE_H__

#include "IFXUnknown.h"
#include "IFXMatrix4x4.h"

// {C7F29C4A-BD0D-4727-B9F3-770E1748FA23}
IFXDEFINE_GUID(IID_IFXBoundHierarchy,
0xc7f29c4a, 0xbd0d, 0x4727, 0xb9, 0xf3, 0x77, 0xe, 0x17, 0x48, 0xfa, 0x23);

enum EIFXBVType
{
	IFX_BV_AABB
};

class CIFXBTreeNode;
class CIFXResultAllocator;
class CIFXCollisionResult;
class IFXMeshGroup;

/**
	This is the main interface for IFXBoundHierarchy.

	@note	The associated IID is named IID_IFXBoundHierarchy.
*/
class IFXBoundHierarchy : virtual public IFXUnknown
{
public:

	/**
		@note	An enumeration that defines the method used to
				split the face list.\n

		@b MIDPOINT - split faces into two buckets at the midpoint of longest
					  side of the box.\n
		@b MEDIAN - split the face list in half to form two buckets.\n
		@b SORTED_MEDIAN - sort the face list, then split in half to form two buckets.\n

	*/
	enum SplitType {MIDPOINT, MEDIAN, SORTED_MEDIAN};

	/**
		This method is used to free anything left on the current
		collisionResult list.

		@return	void.
	*/
	virtual void IFXAPI DeallocateResultList() = 0;

	/**
		This method returns a pointer to the collisionResult list.

		@param	uModelIndex	Index for data in either slot 0 or slot 1.
		@param	ppResultPointer	A pointer to the list of collision results.
		@return	void.
	*/
	virtual IFXRESULT IFXAPI GetCollisionResultPointer( U32 uModelIndex,
												 CIFXCollisionResult** ppResultPointer ) = 0;

	/**
		This method is used to obtain a pointer to the list of
		vertices associated with the underlying geometry.

		@return	IFXVector3 pointer representing the head of the vertex list.
	*/
	virtual IFXVector3* IFXAPI GetVertexList() = 0;

	/**
		This method is used to obtain a pointer to the faceCounts array.
		It's usage pertains to mesh invalidation and boundHierarchy
		recomputation.

		@return	An IFXRESULT code.\n

			@b IFX_E_INVALID_POINTER - Bad pointer or NULL passed in.\n
			@b IFX_OK - No error.
	*/
	virtual IFXRESULT IFXAPI GetFaceCounts(U32** puFaceCounts) = 0;

	/**
		This method returns current worldspace transform of the model.

		@param	uModelIndex	Index for data in either slot 0 or slot 1.
		@param	mTranslation	An IFXVector3 reference to the
								current translation.
		@param	mScaleFactor	An IFXVector3 reference to the
								current scale factor.
		@param	mTargetMatrix	An IFXMatrix4x4 reference to the
								current world transform.

		@return	void.

		@note	The matrix returned from this method is unscaled.  If the
				scale is also required the GetScaleFactor() method can be
				used to obtain it.
	*/
	virtual void IFXAPI GetMatrixComponents( U32           uIndex,
									  IFXVector3&   mTranslation,
									  IFXVector3&   mScaleFactor,
									  IFXMatrix4x4& mTargetMatrix ) = 0;

	/**
		This method returns the number of computed collision results, i.e.
		the number of triangles that overlapped with triangles from another
		model or ray.

		@param	uModelIndex	Index for data in either slot 0 or slot 1.

		@return	void.
	*/
	virtual U32 IFXAPI GetNumResults(U32 uIndex) = 0;

	/**
		This method returns the total number of vertices in the underlying
		geometry.

		@return	U32 refering to the number of vertices.
	*/
	virtual U32 IFXAPI GetNumVerts() = 0;

	/**
		This method is used to obtain a pointer to the positionCounts array.
		It's usage pertains to mesh invalidation and boundHierarchy
		recomputation.

		@return	An IFXRESULT code.\n

			@b IFX_E_INVALID_POINTER - Bad pointer or NULL passed in.\n
			@b IFX_OK - No error.
	*/
	virtual IFXRESULT IFXAPI GetPositionCounts(U32** puPositionCounts) = 0;

	/**
		This method returns the splitType used to construct the boundHierarchy.
		geometry.

		@return	U32 referring to the splitType.\n

	*/
	virtual U32 IFXAPI GetSplitType() = 0;

	/**
		This method is used to obtain a pointer to the root node of
		the boundHierarchy.

		@param	ppRoot	A pointer to the root node of the boundHierarchy.

		@return	An IFXRESULT code.\n

			@b IFX_E_INVALID_POINTER - Bad pointer or NULL passed in.\n
			@b IFX_E_NOT_INITIALIZED - The root node is NULL.
			@b IFX_OK - No error.
	*/
	virtual IFXRESULT IFXAPI GetRoot(CIFXBTreeNode** ppRoot) = 0;

	/**
		This method returns current scale of the model.

		@param	uModelIndex	Index for data in either slot 0 or slot 1.
		@param	vScaleFactor	An IFXVector3 reference referring to
								the current scale factor.

		@return	void.
	*/
	virtual void IFXAPI GetScaleFactor( U32         uIndex,
								 IFXVector3& vScaleFactor ) = 0;

	/**
		This method returns transpose of the current worldspace transform.

		@param	uModelIndex	Index for data in either slot 0 or slot 1.
		@param	mTransposeMatrix	An IFXMatrix4x4 reference to the current
									transpose of the world transform.

		@return	void.
	*/
	virtual void IFXAPI GetTransposeMatrix( U32           uIndex,
				 				     IFXMatrix4x4& mTransposeMatrix ) = 0;

	/**
		This method returns current world transform of the model.

		@param	uModelIndex		Index for data in either slot 0 or slot 1.
		@param	mWorldMatrix	An IFXMatrix4x4 reference to the
								current world transform.

		@return	void.

		@note	The matrix returned from this method is unscaled.  If the
				scale is also required the GetScaleFactor() method can be
				used to obtain it.
	*/
	virtual void IFXAPI GetWorldMatrix( U32           uIndex,
								 IFXMatrix4x4& mWorldMatrix ) = 0;

	/**
		This method initializes the boundHierarchy.

		@param	pMeshGroup	A pointer to the IFXMeshGroup associated with
							the underlying geometry.
		@param	ppVertexList	An IFXVector3 reference to the head of
								the vertex list.

		@return	An IFXRESULT code.\n

			@b IFX_E_INVALID_POINTER - Bad pointer or NULL passed in.\n
			@b IFX_E_UNDEFINED - pMeshGroup contains no mesh information.\n
			@b IFX_CANCEL - pMeshGroup contains contains valid mesh information
							but each mesh has no face or vertex data.  This
							case is not treated as an error because is could
							simply be due to streaming of geometry.
			@b IFX_OK - No error.
	*/
	virtual IFXRESULT IFXAPI InitHierarchy( IFXMeshGroup* pMeshGroup,
									 IFXVector3**  ppVertexList ) = 0;

	/**
		This method computes the potential intersection of a box and a sphere.

		@param	pHierarchy	A pointer to the IFXBoundHierarchy associated with
							the sphere model.
		@param	fRadiusSquared	An F32 representing the squared radius of the
								sphere.
		@param	vSphereCentroid	An IFXVector3 representing the world-space
								center of the sphere.
		@param	vContactPoint	An IFXVector3 representing the point of contact
								between the box and the sphere.
		@param	vContactNormal	An IFXVector3 array representing the normals for
								each model at the point of contact.

		@return	An IFXRESULT code.\n

			@b IFX_E_INVALID_POINTER - Bad pointer or NULL passed in.\n
			@b IFX_E_NOT_INITIALIZED - Root node of the boundHierarchy has
									   not been initialized.\n
			@b IFX_FALSE - No error, no collision.
			@b IFX_TRUE - No error, valid collision found.
	*/
	virtual IFXRESULT IFXAPI IntersectBoxSphere(IFXBoundHierarchy*  pHierarchy,
										 F32                 fRadiusSquared,
		 								 IFXVector3&         vSphereCentroid,
										 IFXVector3&         vContactPoint,
										 IFXVector3          vContactNormal[2]) = 0;

	/**
		This method computes the potential intersection of two arbitrary three
		dimensional polygonal models.

		@param	pHierarchy	A pointer to the IFXBoundHierarchy associated with
							the second model.

		@return	An IFXRESULT code.\n

			@b IFX_E_INVALID_POINTER - Bad pointer or NULL passed in.\n
			@b IFX_E_NOT_INITIALIZED - Root node of the boundHierarchy has
									   not been initialized.\n
			@b IFX_FALSE - No error, no collision.
			@b IFX_TRUE - No error, valid collision found.
	*/
	virtual IFXRESULT IFXAPI IntersectHierarchy(IFXBoundHierarchy* pHierarchy) = 0;

	/**
		This method computes the potential intersection of a box and a sphere.

		@param	pHierarchy	A pointer to the IFXBoundHierarchy associated with
							the second model.

		@param	vMin		An IFXVector3 array corresponding to the minimum
							point of the axis aligned bounding boxes.
		@param	vMax		An IFXVector3 array corresponding to the maximum
							point of the axis aligned bounding boxes.

		@return	An IFXRESULT code.\n

			@b IFX_E_INVALID_POINTER - Bad pointer or NULL passed in.\n
			@b IFX_E_NOT_INITIALIZED - Root node of the boundHierarchy has
									   not been initialized.\n
			@b IFX_FALSE - No error, no collision.
			@b IFX_TRUE - No error, valid collision found.
	*/
	virtual IFXRESULT IFXAPI IntersectHierarchyQuick(IFXBoundHierarchy*  pHierarchy,
											  IFXVector3          vMin[2],
											  IFXVector3          vMax[2]) = 0;

	/**
		This method computes the potential intersection of ray and the
		underlying geometry.

		@param	pvOrigin	An IFXVector3 corresponding to the origin of
							the ray.
		@param	pvDirection	An IFXVector3 corresponding to the normalized
							direction of the ray.
		@param	uPickType	A U32 referring to the type of pick operation.\n

				@b 0 - no picking for this model
				@b 1 - test on front facing triangles
				@b 2 - test only back facing triangles
				@b 3 - test front and back facing triangles

		@param	ppResultPointer	A CIFXCollisionResult pointer pointing to
								the list of	collision results.

		@return	An IFXRESULT code.\n

			@b IFX_E_INVALID_POINTER - Bad pointer or NULL passed in.\n
			@b IFX_FALSE - No error, no collision.
			@b IFX_TRUE - No error, valid collision found.
	*/
	virtual IFXRESULT IFXAPI IntersectRay(IFXVector3&           vOrigin,
								   IFXVector3&           vDirection,
								   U32                   uPickType,
								   CIFXCollisionResult** ppResultPointer) = 0;

	/**
		This method is used to set the current worldspace transform.  It also
		is responsible to computing the transpose matrix.

		@param	uModelIndex	Index for data in either slot 0 or slot 1.
		@param	mMatrix	An IFXMatrix4x4 reference to the current
						world transform.
		@param	vScale	An IFXVector3 referring to the current scale.

		@return	void.
	*/
	virtual void IFXAPI SetMatrix( U32           uModelIndex,
							IFXMatrix4x4& mMatrix,
							IFXVector3&   vScale ) = 0;

	/**
		This method is used to set the collision result, i.e. the data
		for a specific triangle. This method is used for setting results
		from a two object mesh/mesh collision.

		@param	uModelIndex	Index for data in either slot 0 or slot 1.
		@param	uMeshID	A U32 indicating which mesh in the meshGroup this
						face is from.
		@param	uFaceID	A U32 indicating which face in the meshes facelist
						this face is from.
		@param	vIntersectPoint	An IFXVector3 reference identifying the
								intersection point for this triangle -
								computed as the sum of the three vertices
								and not scaled (done later to avoid multiple
								divides).
		@param	vIntersectionNormal	An IFXVector3 reference containing the
									computed normal vector for the triangle.

		@return	void.
	*/
 	virtual IFXRESULT IFXAPI SetResult( U32          uModelIndex,
								 U32          uMeshID,
								 U32          uFaceID,
				  				 IFXVector3&  vIntersectPoint,
								 IFXVector3&  vIntersectNormal ) = 0;

	/**
		This method is used to set the collision result, i.e. the data
		for a specific triangle. This method is used for setting results
		of a ray/object intersection.

		@param	uMeshID	A U32 indicating which mesh in the meshGroup this
						face is from.
		@param	uFaceID	A U32 indicating which face in the meshes facelist
						this face is from.
		@param	vVerts	An IFXVector3 array identifying the vertices of
						the intersected triangle.
		@param	fU	A F32 representing the U part of the barycentric coordinate.
		@param	fV	A F32 representing the V part of the barycentric coordinate.
		@param	fT	A F32 representing the distance from the ray origin to the
					point of intersection.

		@return	void.
	*/
	virtual IFXRESULT IFXAPI SetResult(U32         uMeshID,
								U32         uFaceID,
								IFXVector3  vVerts[3],
								F32         fU,
								F32         fV,
								F32         fT) = 0;

	/**
		This method is used to set the splitType.

		@param	uSplitType	A U32 refering to the split type.\n

		@return	void

	*/
	virtual void IFXAPI SetSplitType(SplitType uSplitType) = 0;
};

#endif
