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
	@file	IFXModifierChainInternal.h

			The header file that defines the IFXModifierChainInternal interface.
*/

#ifndef IFXModifierChainInternal_H
#define IFXModifierChainInternal_H

#include "IFXModifierChain.h"
#include "IFXModifierDataPacketInternal.h"


// {3340F0AE-EB91-4910-98BA-5EC8315DB06B}
IFXDEFINE_GUID(IID_IFXModifierChainInternal,
0x3340F0AE, 0xEB91, 0x4910, 0x98, 0xBA, 0x5E, 0xC8, 0x31, 0x5D, 0xB0, 0x6B);

/**
     The IFXModifierChainInternal interface used for internal modifier
     chain communication.
*/
class IFXModifierChainInternal : virtual public IFXModifierChain
{
public:

	virtual IFXRESULT IFXAPI  Invalidate( 
									U32 invalidDataElementIndex,
									U32 modifierIndex ) = 0;
	/**<
			Notification that the supplied DataElement within the DataPacket
			owned by the specified Modifier is invalid.

	@param  invalidDataElementIndex
			The index of the DataElement.

	@param  modifierIndex
			The ModifierChain index of the Modifier that owns the DataPacket
			containing the DataElement.

	@return One of the following IFXRESULT codes:                     \n\n
	-	IFX_OK                                                          \n
		No error.                                                     \n\n
	-	IFX_E_INVALID_ENTRY                                             \n
		The specified DataElement does not exist.                     \n\n
	-	IFX_E_INVALID_RANGE                                             \n
		The specified Modifier does not exist.                        \n\n
	-	IFX_E_NOT_INITIALIZED                                           \n
		The ModifierChain was not initialized properly.

	@note	This allows the ModifierChain to keep track of what Modifiers 
			need to be brought up to date when delivering a DataElement. */


	virtual IFXRESULT IFXAPI  ProcessDependencies( 
									U32 dataElementIndex,
									U32 modifierIndex ) = 0;
	/**<
			Brings all DataElement in the dependency chain up to date.

	@param  dataElementIndex
			The index of the DataElement.

	@param  modifierIndex
			The ModifierChain index of the Modifier that owns the DataPacket
			containing the DataElement.

	@return One of the following IFXRESULT codes:                     \n\n
	-	IFX_OK                                                          \n
		No error.                                                     \n\n
	-	IFX_E_INVALID_ENTRY                                             \n
		The specified DataElement does not exist.                     \n\n
	-	IFX_E_INVALID_RANGE                                             \n
		The specified Modifier does not exist.                        \n\n
	-	IFX_E_NOT_INITIALIZED                                           \n
		The ModifierChain was not initialized properly.                */

	virtual IFXRESULT IFXAPI  AddAppendedModifierChain(
									IFXModifierChainInternal* pInModChain)=0;
	/**<
			Adds an appended modifier chain.

	@param	pInModChain
			Input pointer to an modifier chain, which needs to be appended.

	@return	IFXRESULT code.
	*/

	virtual IFXRESULT IFXAPI  RemoveAppendedModifierChain(
									IFXModifierChainInternal* pInModChain)=0;
	/**<
			Removes an appended modifier chain.

	@param	pInModChain
			Input pointer to an appended modifier chain, which needs to be removed.

	@return	IFXRESULT code.
	*/

	virtual IFXRESULT IFXAPI  RebuildDataPackets(BOOL isReqValidation) = 0;
	/**< 
			When a prepended modifier chain changes this method needs to be
			called.

	@param	isReqValidation

	@return	IFXRESULT code.
	*/

	virtual IFXRESULT IFXAPI  BuildCachedState( 
									IFXModifierDataPacketInternal* pInDP, 
									BOOL isReqValidation) = 0;
	/**< 
			Called by a prepended chain to see if a remove will violate input
			requirements of a appended modifier chain. Also called when a new
			modifier is appended to a prepended chain to test if the add is ok.

	@param	pInDP

	@param	isReqValidation

	@return	IFXRESULT code.
	*/

	virtual IFXRESULT IFXAPI  RestoreOldState() = 0;
	/**< 
		Called when a prepended mod chain is in the middle of building a new
		state and some thing fails, this tells appended chains to back out their
		changed state.
	*/

	virtual IFXRESULT IFXAPI  ClearOldState() = 0;
	/**< 
		Called by a prepended modifier chain when all of the state changed have happened
		successfully, and the backup state can be released.
	*/

	virtual BOOL IFXAPI NeedTime() = 0;
	/**<
		Allows to define that this or appended modifier chain needs time.
	*/

	virtual void IFXAPI RecheckNeedTime() = 0;

	virtual IFXRESULT IFXAPI  GetDEState(
									U32 dataElementIndex, 
									IFXDataElementState** ppDEState) = 0;
	virtual IFXRESULT IFXAPI  GetIntraDeps(IFXIntraDependencies** ppIntraDeps) = 0;
	virtual IFXRESULT IFXAPI  NotifyActive() = 0;
};

#endif
