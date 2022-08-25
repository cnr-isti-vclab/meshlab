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

#ifndef IFXCLODManagerInterface_H
#define IFXCLODManagerInterface_H

#include "IFXUnknown.h"
#include "IFXUpdatesGroup.h"

//forward declarations
class IFXMesh;
class IFXMeshGroup;

/**
	This interface contains the IFXResManagerInterface and the 
	IFXMultResManagerInterface classes.
*/
class IFXCLODManager;

/**
	The IFXResManager class is used to change the resolution of a single IFXMesh. 
	If an instance of this class is being used by an IFXCLODManager then you 
	should not change it's resolution here since you may throw off the 
	synchronization provided by the CLODManager.

	Use this class to change the resolution of a mesh with only one material.

	@warning	Initialize must be called before any other method is called.

	@warning	the input mesh is assumed to be at full resolution.
*/
class IFXResManager
{
public:
	virtual ~IFXResManager() {}
	/// @todo	Consistency checking, resolution option.
	/**
		This method initializes the manager for use.

		@param  pInCLODMgr  Pointer to the IFXCLODManager
		@param  meshIndex   Mesh index number
		@return Returns an IFXResult
	*/
	virtual IFXRESULT Initialize(IFXCLODManager* pInCLODMgr, U32 meshIndex) = 0;

	/**
		This method sets the resolution to zero.

		@param  pInCLODMgr  Pointer to the IFXCLODManager
		@param  meshIndex   Mesh index number
		@return Returns an IFXResult
	*/
	virtual IFXRESULT Initialize_ResolutionZero(
								IFXCLODManager* pInCLODMgr, U32 meshIndex) = 0;

	/**
		This method hands back the modified input mesh.

		@param  pIn  Pointer to an IFXMesh input
		@param  pOut Pointer to an IFXMesh output
		@return Returns an IFXResult
	*/
	virtual IFXRESULT UpdateMesh(IFXMesh* pIn, IFXMesh** pOut) = 0;

	/**
		This method sets the resolution range.The resolution range is
		from 0 to the number of vertices in the original IMESH.

		@param  r resolution setting

		@return Returns U32
	*/
	virtual U32 SetResolution(U32 r) = 0;

	/**
		This method gets the resolution range.The resolution range is
		from 0 to the number of vertices in the original IMESH.

		@return Returns U32
	*/
	virtual U32 GetResolution() = 0;

	/**
		This method gets the maximum resolution value.

		@return Returns U32
	*/
	virtual U32 GetMaxResolution() = 0;

	/**
		This method gets the update records.

		@return Returns IFXUpdates
	*/
	virtual IFXUpdates*   GetUpdates() = 0;

	/**
		This method increases the resolution.

		@param  delta Amount of increase
	*/
	virtual void IncreaseResolution(U32 delta) = 0;

	/**
		This method decreases the resolution.

		@param  delta Amount of decrease
	*/
	virtual void DecreaseResolution(U32 delta) = 0;
};

// {1ABA4D0D-8CB6-4b26-A3C7-363BFDF202F3}
IFXDEFINE_GUID(IID_IFXCLODManager,
0x1aba4d0d, 0x8cb6, 0x4b26, 0xa3, 0xc7, 0x36, 0x3b, 0xfd, 0xf2, 0x2, 0xf3);

/**
	The IFXCLODManager class is used to change the resolution of an IFXMeshGroup.
	It does this by adjusting the resolution on each IFXmesh in a synchronous 
	manner so that cracks do not develop between materials. The resolution ranges
	from 0 to the number of vertices in the original mesh.

	@note	If a mesh has only one material, use the IFXResManagerInterface class
			to change resolution because it does away with overhead associated 
			with keeping the resolution of each IFXMesh synchronized.

	@warning	The input mesh is assumed to be at full resolution.
*/
class IFXCLODManager : virtual public IFXUnknown
{
public:

	/// @todo: check consistency, add resolution option?

	/**
		This method initializes the object for use.

		@param  pInMeshGroup	Pointer to an IFXMeshGroup
		@param  pInUpdatesGroup	Pointer to an IFXUpdatesGroup

		@return Returns an IFXResult
	*/
	virtual IFXRESULT IFXAPI Initialize (
									IFXMeshGroup* pInMeshGroup, 
									IFXUpdatesGroup* pInUpdatesGroup) = 0;

	/**
		This method initializes the object for use.

		@param  pCLODManager	Pointer to IFXCLODManagerInterface
		@param  pInMeshGroup	Pointer to an IFXMeshGroup

		@return Returns an IFXResult
	*/
	virtual IFXRESULT IFXAPI Initialize(
									IFXCLODManager* pCLODManager, 
									IFXMeshGroup* pInMeshGroup) = 0;

	/**
		This method sets the resolution to zero.

		@param  pInMeshGroup	Pointer to an IFXMeshGroup
		@param  pInUpdatesGroup	Pointer to an IFXUpdatesGroup

		@return Returns an IFXResult
	*/
	virtual IFXRESULT IFXAPI Initialize_ResolutionZero(
							IFXMeshGroup* pInMeshGroup, 
							IFXUpdatesGroup* pInUpdatesGroup) = 0;

	/**
		This method hands back the modified input mesh.

		@param  pIn  Pointer to an IFXMeshGroup input
		@param  ppOut Pointer to a pointer to an IFXMeshGroup output
		@return Returns an IFXResult
	*/
	virtual IFXRESULT IFXAPI UpdateMesh(IFXMeshGroup* pIn, IFXMeshGroup** ppOut) = 0;

	/**
		This method gets the previous resolution.

		@return Returns U32
	*/
	virtual U32     IFXAPI PrevResolution() = 0;

	/**
		This method sets the resolution range.

		@param  r resolution setting

		@return Returns U32
	*/
	virtual U32     IFXAPI SetResolution(U32 r) = 0;

	/**
		This method gets the resolution range.

		@return Returns U32
	*/
	virtual U32     IFXAPI GetResolution() = 0;

	/**
		This method gets the maximum resolution value.

		@return Returns U32
	*/
	virtual U32     IFXAPI GetMaxResolution() = 0;

	/**
		This method gets the ResManager interface.

		@param  i ResManager interface
		@return Returns IFXResManagerInterface
	*/
	virtual IFXResManager* IFXAPI GetResManager(U32 i) = 0;

	/**
		This method gets the a MeshGroup.

		@return Returns IFXMeshGroup
	*/
	virtual IFXMeshGroup* IFXAPI GetMeshGroup(void)=0;

	/**
		This method gets the an UpdatesGroup.

		@return Returns IFXUpdatesGroup
	*/
	virtual IFXUpdatesGroup* IFXAPI GetUpdatesGroup(void) = 0;
};
#endif
