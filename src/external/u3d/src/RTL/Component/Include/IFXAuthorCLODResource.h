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
	@file	IFXAuthorCLODResource.h
*/

#ifndef IFXAUTHORCLODRESOURCE_H
#define IFXAUTHORCLODRESOURCE_H

#include "IFXMarkerX.h"
#include "IFXAuthorCLODMesh.h"
#include "IFXAuthorMeshMap.h"
#include "IFXMeshMap.h"
#include "IFXModifier.h"
#include "IFXDataBlockQueueX.h"
#include "IFXGenerator.h"
#include "IFXSkeleton.h"

class IFXUpdatesGroup;
class IFXCLODManager;
class IFXNeighborResControllerInterface;
class IFXUpdatesGroup;
class IFXNeighborMesh;

// Used in both CLOD encoder and decoder for normal calculations
#define IFX_LEN_EPSILON 0.0000001f

// {E284A4A4-9A2F-4110-BFCF-BE32DD8023D7}
IFXDEFINE_GUID(IID_IFXAuthorCLODResource,
0xE284A4A4, 0x9A2F, 0x4110, 0xBF, 0xCF, 0xBE, 0x32, 0xDD, 0x80, 0x23, 0xD7);

/**
 *	It will generate an IFXMeshGroup renderable.
 */
class IFXAuthorCLODResource : virtual public IFXMarkerX, 
							  virtual public IFXModifier,
							  virtual public IFXGenerator
{
public:
	/**
	 *	Returns the IFXAuthorCLODMesh (a multi-resolution representation of
	 *  an IFXAuthorMesh) wrapped by this class.
	 *
	 *	@param	rpAuthorCLODMesh	The returned reference to the
	 *                              IFXAuthorCLODMesh
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  GetAuthorMesh(IFXAuthorCLODMesh*& rpAuthorCLODMesh) = 0;

	/**
	 *	Sets the IFXAuthorCLODMesh (a multi-resolution representation of
	 *  an IFXAuthorMesh) wrapped by this class.  This can be called
	 *  any time a mesh is streaming in, so pAuthorCLODMesh may be an
	 *  intermediate representation of the mesh and its update records.
	 *
	 *	@param	pAuthorCLODMesh  	The IFXAuthorCLODMesh to use for this
	 *                              object.
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  SetAuthorMesh(IFXAuthorCLODMesh* pAuthorCLODMesh) = 0;

	/**
	 *	Sets the IFXAuthorCLODMesh (a multi-resolution representation of
	 *  an IFXAuthorMesh) wrapped by this class.  This is called when
	 *  the mesh has fully streamed into the 3D engine and all mesh data,
	 *  including update records, is now available for rendering.
	 *
	 *	@param	pAuthorCLODMesh  	The IFXAuthorCLODMesh to use for this
	 *                              object.
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  SetAuthorMeshFinal(
									IFXAuthorCLODMesh* pAuthorCLODMesh) = 0;
	
	/**
	 *	Returns the crease angle, which is the minimum angle (in degrees)
	 *  that the clodolution mesh generator will consider belonging
	 *  to a sharp edge between two faces.
	 *
	 *	@param	rfCreaseAngle  	The returned reference to the crease angle.
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  GetCreaseAngle(F32& rCreaseAngle) = 0;

	/**
	 *	Sets the crease angle, which is the minimum angle (in degrees)
	 *  that the clodolution mesh generator will consider belonging
	 *  to a sharp edge between two faces.
	 *
	 *	@param	fCreaseAngle  	The new value for the crease angle.
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 *
	 *  @note Setting this parameter will have no effect until the mesh
	 *  is regenerated from scratch.
	 */
	virtual IFXRESULT IFXAPI  SetCreaseAngle(F32 creaseAngle) = 0;

	// Parameters for compression
	/**
	 *	Returns the normal crease parameter, which is used during compression.
	 *  Normals at the same position which are closer than the crease parameter
	 *  are merged.  The crease parameter is the dot product between the 
	 *  normals (-1 to 1).
	 *
	 *	@param	rNormalCreaseParameter 	The returned reference to the 
	 *                                      normal crease parameter.
	 */
	virtual void IFXAPI  GetNormalCreaseParameter(F32& rNormalCreaseParameter) = 0;
	/**
	 *  Sets the normal crease parameter, which is used during compression.
	 *  Normals at the same position that are closer than the crease parameter
	 *  are merged.  The crease parameter is the dot product between the 
	 *  normals (-1 to 1).
	 *
	 *	@param	normalCreaseParameter 	The new value for the normal crease parameter.
	 *
	 *  @note  Since a mesh, once compressed, will never be re-compressed,
	 *  you must manually destroy the mesh contained in the 
	 *  IFXAuthorCLODResource and regenerate it for this parameter to have
	 *  any effect.
	 */
	virtual void IFXAPI  SetNormalCreaseParameter(F32 normalCreaseParameter) = 0;
	/**
	 *	Returns the normal update parameter, which is used during compression.
     *  Corrections to the predicted normal (determined during the compression
	 *  process) that are smaller than the update value are dropped.  The 
	 *  update parameter is expressed as a dot product between the original 
	 *  authored normal and the predicted normal (-1 to 1).
	 *
	 *	@param	rNormalUpdateParameter 	The returned reference to the 
	 *                                      normal update parameter.
	 */	
	virtual void IFXAPI  GetNormalUpdateParameter(F32& rNormalUpdateParameter) = 0;
	/**
	 *  Sets the normal update parameter, which is used during compression.
     *  Corrections to the predicted normal (determined during the compression
	 *  process) that are smaller than the update value are dropped.  The 
	 *  update parameter is expressed as a dot product between the original 
	 *  authored normal and the predicted normal (-1 to 1).
	 *
	 *	@param	normalUpdateParameter 	The new value for the normal update 
	 *                                  parameter.
	 *
	 *  @note  Since a mesh, once compressed, will never be re-compressed,
	 *  you must manually destroy the mesh contained in the 
	 *  IFXAuthorCLODResource and regenerate it for this parameter to have
	 *  any effect.
	 */
	virtual void IFXAPI  SetNormalUpdateParameter(F32 normalUpdateParameter) = 0;
	/**
	 *	Returns the normal tolerance, which is used during compression.
     *  Normals smaller than this value will be considered the same.  Larger 
	 *  values result in a smaller render mesh but at the sacrifice of accuracy.  
	 *  Smaller values are accurate but result in a bulky mesh. The tolerance
	 *  is expressed as the dot product between the two normals (-1 to 1).	 
	 *
	 *	@param	rNormalTolerance 	The returned reference to the 
	 *                                      normal tolerance.
	 */	
	virtual void IFXAPI  GetNormalTolerance(F32& rNormalTolerance) = 0;
	/**
	 *  Sets the normal tolerance, which is used during compression.
     *  Normals smaller than this value will be considered the same.  Larger 
	 *  values result in a smaller render mesh but at the sacrifice of accuracy.  
	 *  Smaller values are accurate but result in a bulky mesh. The tolerance
	 *  is expressed as the dot product between the two normals (-1 to 1).	 
	 *
	 *	@param	normalTolerance 	The new value for the normal tolerance.
	 *
	 *  @note  Since a mesh, once compressed, will never be re-compressed,
	 *  you must manually destroy the mesh contained in the 
	 *  IFXAuthorCLODResource and regenerate it for this parameter to have
	 *  any effect.
	 */
	virtual void IFXAPI  SetNormalTolerance(F32 normalTolerance) = 0;

	/**
	*  Returns the current resolution (expressed as a ratio of the
	*  current resolution to the maximum possible mesh resolution)
	*  of the mesh.
	*
	*  @return  The current mesh resolution
	*/
	virtual F32 IFXAPI  GetCLODLevel() = 0;

	/**
	*  Sets the current resolution of the mesh (expressed as a ratio of the
	*  current resolution to the maximum possible mesh resolution).
	*
	*  @param  inCLODRatio   The new mesh resolution
	*
	*  @return An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*  @retval  IFX_E_INVALID_RANGE  A level less than zero or greater than
	*                                one was entered.
	*/
	virtual IFXRESULT IFXAPI  SetCLODLevel( F32 inCLODRatio ) = 0;

	// Mesh-reordering maps
	/**
	*  Returns a mapping showing how the original mesh has been reordered or
	*  simplified by the mesh scrubbing, clodolution generation, and 
	*  compression processes.
	*
	*  @param ppAuthorMeshMap  The returned pointer to the mapping
	*
	*  @return  An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*  @retval  IFX_E_NOT_INITIALIZED   The mapping does not exist yet
	*  @retval  IFX_E_INVALID_POINTER   A bad pointer was passed to this routine
	*/
	virtual IFXRESULT IFXAPI  GetAuthorMeshMap(IFXMeshMap** ppAuthorMeshMap) = 0;

	/**
	*  Sets a mapping showing how the original mesh has been reordered or
	*  simplified by the mesh scrubbing, clodolution generation, and 
	*  compression processes.
	*
	*  @param pAuthorMeshMap  The new mapping to adopt
	*
	*  @return  An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*  @retval  IFX_E_INVALID_POINTER   A bad pointer was passed to this 
	*                                   routine.
	*
	*  @note This is intended for internal use only unless you really
	*  know what you are doing.
	*/
	virtual IFXRESULT IFXAPI  SetAuthorMeshMap(IFXMeshMap* pAuthorMeshMap) = 0;
	
	/**
	*  Returns a mapping showing how the compressed mesh has been modified
	*  during its conversion from an IFXAuthorCLODMesh to an IFXMeshGroup.
	*  This conversion process typically involves replication of mesh data
	*  found along material boundaries and the storage of mesh data in
	*  high-performance data structures.
	*
	*  @param ppRenderMeshMap  The returned pointer to the mapping
	*
	*  @return  An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*  @retval  IFX_E_NOT_INITIALIZED   The mapping does not exist yet
	*  @retval  IFX_E_INVALID_POINTER   A bad pointer was passed to this routine
	*/
	virtual IFXRESULT IFXAPI  GetRenderMeshMap(IFXMeshMap** ppRenderMeshMap) = 0;

	/**
	*  Set a mapping showing how the compressed mesh has been modified
	*  during its conversion from an IFXAuthorCLODMesh to an IFXMeshGroup.
	*  This conversion process typically involves replication of mesh data
	*  found along material boundaries and the storage of mesh data in
	*  high-performance data structures.
	*
	*  @param pRenderMeshMap  The new mapping to adopt
	*
	*  @return  An IFXRESULT value.
	*  @retval	IFX_OK	No error.
	*  @retval  IFX_E_INVALID_POINTER   A bad pointer was passed to this routine
	*
	*  @note This is intended for internal use only.
	*/
	virtual IFXRESULT IFXAPI  SetRenderMeshMap(IFXMeshMap* pRenderMeshMap) = 0;

	// Bones support
	virtual IFXSkeleton* IFXAPI GetBones( void ) = 0;
	virtual IFXRESULT IFXAPI SetBones( IFXSkeleton* pBonesGen ) = 0;

	/**
	*  Currently does nothing.
	*/
	virtual IFXRESULT IFXAPI  Transfer() = 0;

	virtual void IFXAPI  GetExcludeNormals(BOOL& bExcludeNormals) = 0;
	virtual void IFXAPI  SetExcludeNormals(BOOL  bExcludeNormals) = 0;

	/**
	 *	Retrieves the associated IFXCLODManager.
	 *
	 *	@return	An IFXCLODManager pointer.
	 */
	virtual IFXCLODManager* IFXAPI GetCLODController() = 0;

	/**
	 *	Retrieves the associated IFXNeighborResControllerInterface.
	 *
	 *	@return	An IFXNeighborResControllerInterface pointer.
	 */
	virtual IFXNeighborResControllerInterface* IFXAPI GetNeighborResController() = 0;

	/**
	 *	Retrieves the enabled status of the CLOD controller.
	 *
	 *	@param	pbOutCLODAuto	A pointer to a boolean to contain the value of
	 *							of the enabled status of the CLOD controller.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_INVALID_POINTER	The value of @a pbOutCLODAuto was 0.
	 */
	virtual IFXRESULT IFXAPI  GetCLODAuto(BOOL* pbOutCLODAuto) = 0;



	/**
	 *	Sets the enabled status of the CLOD controller.
	 *
	 *	@param	bInCLODAuto	A boolean value to be used as the enabled status of
	 *						the CLOD controller.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  SetCLODAuto(BOOL bInCLODAuto) = 0;



	/**
	 *	Retrieves the CLOD Bias value.
	 *
	 *	@param	pOutCLODBias	The address of an F32 value to contain the
	 *							of the CLOD Bias value.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_INVALID_POINTER	The value of @a pOutCLODBias was 0.
	 */
	virtual IFXRESULT IFXAPI  GetCLODBias(F32* pOutCLODBias) = 0;

	/**
	 *	Sets the CLOD Bias value.
	 *
	 *	@param	inCLODBias	An F32 value to be used as the CLOD Bias value.
	 *
	 *	@return	An IFXRESULT value.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI  SetCLODBias(F32 inCLODBias) = 0;


	virtual IFXRESULT IFXAPI  BuildDataBlockQueue() = 0;
	virtual void IFXAPI  GetDataBlockQueueX(IFXDataBlockQueueX*& rpDataBlockQueueX) = 0;

	/** 
		Retrieves the bounding sphere of the geometry in model space.
	*/
	virtual const IFXVector4& IFXAPI GetBoundingSphere() = 0;

	/**
		Sets the current bounding sphere.
	*/
	virtual IFXRESULT IFXAPI  SetBoundingSphere(const IFXVector4& vInBoundingSphere)=0;

	/**
		Retrieves the resource transform of the object.  The resource
		transform is applied before the model to world transform.
	*/
	virtual const IFXMatrix4x4& IFXAPI GetTransform()=0;

	/**
		Sets the resource transform.  The resource
		transform is applied before the model to world transform.
	*/
	virtual IFXRESULT IFXAPI  SetTransform(const IFXMatrix4x4& tInTransform)=0;

	/**
		Retrieves the associated IFXMeshGroup.
	*/
	virtual IFXRESULT IFXAPI  GetMeshGroup(IFXMeshGroup**) = 0;

	/**
		Retrieves the associated IFXUpdatesGroup.
	*/
	virtual IFXRESULT IFXAPI  GetUpdatesGroup(IFXUpdatesGroup**) = 0;

	/**
		Retrieves the associated IFXNeighboorMesh.
	*/
	virtual IFXRESULT IFXAPI  GetNeighborMesh(IFXNeighborMesh**) = 0;
};

#endif
