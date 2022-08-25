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
	@file  IFXPickObject.h

	The header file that defines the IFXPickObject interface.
*/


#ifndef __IFXPICKOBJECT_H__
#define __IFXPICKOBJECT_H__

#include "IFXUnknown.h"
#include "IFXVector3.h"
#include "IFXModel.h"

// {27B05179-95FE-4df7-ABED-E670D7998B6A}
IFXDEFINE_GUID(IID_IFXPickObject,
0x27b05179, 0x95fe, 0x4df7, 0xab, 0xed, 0xe6, 0x70, 0xd7, 0x99, 0x8b, 0x6a);

/**
	This is the main interface for IFXPickObject.

	@note	The associated IID is named IID_IFXPickObject.
*/
class IFXPickObject : public IFXUnknown
{
public:

	/**
		This method returns the distance between the point of intersection and
		the origin of the ray.

		@return	An F32 containing the distance to the point of intersection.
	*/
	virtual F32 IFXAPI GetDistance() = 0;

	/**
		This method returns the index of the face within the face list of a
		particular mesh.

		@return	A U32 containing the face ID.
	*/
	virtual U32 IFXAPI GetFaceID() = 0;

	/**
		This method is used to get the normal vector for the intersected
		triangle.

		@param	vIntersectNormal	An IFXVector3 reference containing the
									normal vector at the point of intersection.
		@return	void
	*/
	virtual void IFXAPI GetIntersectNormal(IFXVector3& vIntersectNormal) = 0;

	/**
		This method is used to get the point of intersection on the triangle.

		@param	vIntersectPoint	An IFXVector3 reference containing the
								point of intersection.
		@return	void
	*/
	virtual void IFXAPI GetIntersectPoint(IFXVector3& vIntersectPoint) = 0;

	/**
		This method returns the index of the mesh within the meshGroup.

		@return	A U32 containing the mesh ID.
	*/
	virtual U32 IFXAPI GetMeshID() = 0;

	/**
		This method returns the model associated with this IFXPickObject.

		@return	An IFXModel pointer.
	*/
	virtual IFXModel* IFXAPI GetModel() = 0;
	virtual U32 IFXAPI GetInstance() = 0;

	/**
		This method returns the "u" portion of the barycentric coordinate.

		@return	A F32 "u" portion of the barycentric coordinate.
	*/
	virtual F32 IFXAPI GetUCoord() = 0;

	/**
		This method returns the "v" portion of the barycentric coordinate.

		@return	A F32 "v" portion of the barycentric coordinate.
	*/
	virtual F32 IFXAPI GetVCoord() = 0;

	/**
		This method is used to obtain the vertices of the intersected triangle.

		@param	v1	An IFXVector reference representing the first vertex.
		@param	v2	An IFXVector reference representing the second vertex.
		@param	v3	An IFXVector reference representing the third vertex.

		@return	void
	*/
	virtual void IFXAPI GetVertices( IFXVector3& v1,
							  IFXVector3& v2,
							  IFXVector3& v3 ) = 0;

	/**
		This method is used to set the model associated with this IFXPickObject.

		@param	pModel	An IFXModel pointer.

		@return	void
	*/
	virtual void IFXAPI SetModel(IFXModel* pModel) = 0;
	virtual void IFXAPI SetInstance(U32 Instance) = 0;


	/**
		This method is used to set the mesh and face IDs.

		@param	uMeshID	A U32 corresponding to the index of the mesh within
						meshGroup of the underlying geometry.
		@param	uFaceID	A U32 corresponding to the index of the face within
						the face list of the mesh index by uMeshID.

		@return	void
	*/
	virtual void IFXAPI SetIDs( U32 uMeshID,
						 U32 uFaceID ) = 0;

	/**
		This method is used to set the mesh and face IDs.

		@param	fU	A F32 corresponding to the "u" portion of the barycentric
					coordinate.
		@param	fV	A F32 corresponding to the "v" portion of the barycentric
					coordinate.
		@param	fT	A F32 corresponding to the distance between the origin of
					the ray and the point of intersection.

		@return	void
	*/
	virtual void IFXAPI SetUVTCoords( F32 fU,
							   F32 fV,
							   F32 fDistance ) = 0;

	/**
		This method is used to set the vertices of the intersected triangle.

		@param	v1	An IFXVector reference representing the first vertex.
		@param	v2	An IFXVector reference representing the second vertex.
		@param	v3	An IFXVector reference representing the third vertex.

		@return	void
	*/
	virtual void IFXAPI SetVertices( IFXVector3& v1,
							  IFXVector3& v2,
							  IFXVector3& v3 ) = 0;
};

#endif
