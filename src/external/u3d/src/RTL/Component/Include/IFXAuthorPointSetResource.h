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
	@file	IFXAuthorPointSetResource.h
*/

#ifndef IFXAuthorPointSetResource_H
#define IFXAuthorPointSetResource_H

#include "IFXModifier.h"
#include "IFXMarkerX.h"
#include "IFXGenerator.h" 
#include "IFXDataBlockQueueX.h"
#include "IFXAuthorPointSet.h"
#include "IFXSkeleton.h"
#include "IFXMeshMap.h"

// {1739E931-DC5B-4f2c-8F3A-E9455082487D}
IFXDEFINE_GUID(IID_IFXAuthorPointSetResource, 
0x1739e931, 0xdc5b, 0x4f2c, 0x8f, 0x3a, 0xe9, 0x45, 0x50, 0x82, 0x48, 0x7d);


/**
	IFXAuthorPointSetResource that will generate an renderable.
 */
class IFXAuthorPointSetResource : virtual public IFXModifier,
								  virtual public IFXGenerator, 
								  virtual public IFXMarkerX
{
public:

	virtual IFXRESULT IFXAPI BuildDataBlockQueue() = 0;
	
	virtual void IFXAPI GetDataBlockQueueX(IFXDataBlockQueueX*& rpDataBlockQueueX) = 0;

	/**
		Retrieves the bounding sphere of the geometry in model space.
	*/
	virtual const IFXVector4& IFXAPI GetBoundingSphere()=0;
	
	/**
		Sets the current bounding sphere.
	*/
	virtual IFXRESULT IFXAPI SetBoundingSphere(const IFXVector4& rBoundingSphere)=0;
	
	/**
	* Retrieves the resource transform of the object.  The resource
	* transform is applied before the model to world transform.
	*/
	virtual const IFXMatrix4x4& IFXAPI GetTransform()=0;
	
	/**
	Sets the resource transform.  The resource
	transform is applied before the model to world transform.
	*/

	virtual IFXRESULT IFXAPI SetTransform(const IFXMatrix4x4& rTransform) = 0;

	/**
	 *	Returns the IFXAuthorPointSet wrapped by this class.
	 *
	 *	@param	rpAuthorPointSet 	The returned reference to the
	 *                              IFXAuthorPointSet
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI GetAuthorPointSet(
									IFXAuthorPointSet*& rpAuthorPointSet) = 0;
	
	/**
	 *	Sets the IFXAuthorPointSet wrapped by this class.  
	 *
	 *	@param	pAuthorPointSet  	The IFXAuthorPointSet to use for this
	 *                              object.
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI SetAuthorPointSet(IFXAuthorPointSet* pAuthorPointSet) = 0;

	/**
	 *	Retrives the associated IFXMeshGroup   
	 *
	 *	@param	ppMeshGroup  	The IFXMeshGroup pointer  to use for this
	 *                              object.
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI GetMeshGroup(IFXMeshGroup** ppMeshGroup) = 0;

	/**
	*  Currently does nothing.
	*/
	virtual IFXRESULT IFXAPI Transfer() = 0;

	/** @name	Bones support
	 */
	//@{
	virtual IFXSkeleton* IFXAPI GetBones( void ) = 0;
	virtual IFXRESULT IFXAPI SetBones( IFXSkeleton* pBonesGen ) = 0;
 	virtual IFXRESULT IFXAPI GetRenderMeshMap(IFXMeshMap** ppRenderLineSetMap) = 0;
 	virtual IFXRESULT IFXAPI SetRenderMeshMap(IFXMeshMap* pRenderLineSetMap) = 0;
	//@}
};

#endif
