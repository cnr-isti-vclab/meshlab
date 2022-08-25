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
	@file	IFXModifier.h

			This header file defines the IFXModifier interface and
			the associated interface identifier.
*/

#ifndef IFXMODIFIER_H
#define IFXMODIFIER_H

#include "IFXMarker.h"

class IFXSceneGraph;
class IFXModifierDataPacket;
class IFXModifierChain;

IFXDEFINE_GUID(IID_IFXModifier,
0x68d1ff05, 0x90f7, 0x4f52, 0x83, 0x21, 0x92, 0xab, 0xce, 0x45, 0xd4, 0xd5);

#define IFX_W_MODIFIER_OUTPUT_NOT_CHANGED \
	MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_MODIFIER, 0X0001)

#define IFX_W_MODIFIER_OVERRIDE_INVALIDATION \
	MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_MODIFIER, 0X0002)


/**
       This interface supplies the services supporting the processing
	   of DataElements within an associated DataPacket as a stage of a
	   ModifierChain's execution sequence.
*/
class IFXModifier : virtual public IFXMarker
{
public:
	/**
	        Associates a new DataPacket with the Modifier.

	@pre    None.

	@post   The provided DataPacket is associated with the Modifier.
	        If the Modifier was already associated with a DataPacket, it is
			first released.

   	@param  pInDataPacket
	         The incoming IFXDataPacket That will provide input to the modifier
			 chain.

 	@param  pInDataPacket
	         The incoming IFXDataPacket to be associated with the Modifier.

	@return Upon success, IFX_OK is returned. Otherwise, one of the 
			following failures are returned: IFX_E_OUT_OF_MEMORY.
	*/
	virtual IFXRESULT IFXAPI  SetDataPacket( 
									IFXModifierDataPacket* pInInputDataPacket,
									IFXModifierDataPacket* pInDataPacket ) = 0;


	/**
	        Hands back the associated DataPacket.

	@param  rpOutDataPacket
	         The IFXModifierDataPacket pointer to contain the address of the
			 Modifier's DataPacket.

	@return Upon success, IFX_OK is returned. Otherwise, one of the 
			following failures are returned: IFX_E_NOT_INITIALIZED.
	*/
	virtual IFXRESULT IFXAPI  GetDataPacket( 
									IFXModifierDataPacket*& rpOutDataPacket ) = 0;

	/**
	        Hands back a pointer to the list of DIDs that the Modifier will add
			or change.

	@param  rpOutOutputs
	         The pointer to contain the address of the consecutive list of
			 resultant DataElement GUID pointers.

	@param  rOutNumberOfOutputs
	         The U32 to contain the number of GUIDs in the @e
			 rpOutOutputs.

	@param  rpOutUnchangedOutputAttrs
	         The array of U32 attributes which were not changed.

	@return Upon success, IFX_OK is returned. Otherwise, one of the 
			following failures are returned: IFX_E_OUT_OF_MEMORY.

	@note    This method can be used before, and after, the Modifier is
	         initialized.

	         The Modifier is responsible for the lifetime of the list.
	*/
	virtual IFXRESULT IFXAPI  GetOutputs( 
									IFXDID**& rpOutOutputs,
									U32&      rOutNumberOfOutputs,
									U32*&	  rpOutUnchangedOutputAttrs ) = 0;


	/**
	        Hands back the list of input DataElements and the list of output
			DataElements, by which, the result of the specified @e pInOutputDID
			is determined.

	@param  pInOutputDID
	         One of the Modifier's output DIDs in the list handed back from
			 @e GetOutputs().

	@param  rppOutInputDependencies
	         The pointer to contain the address of the list of input DIDs, by
			 which, the specified output is determined.

	@param  rOutNumberInputDependencies
	         The U32 to contain the number of GUIDs in @e rppOutInputDependencies.

	@param  rppOutOutputDependencies
	         The pointer to contain the address of the list related output DIDs.

	@param  ruOutNumberOfOutputDependencies
	         The U32 to contain the number of GUIDs in @e rppOutOutputDependencies.

	@return Upon success, IFX_OK is returned.

	@note   The Modifier is responsible for the lifetime of the list.
	*/
	virtual IFXRESULT IFXAPI  GetDependencies(
	                            IFXDID*   pInOutputDID,
	                            IFXDID**& rppOutInputDependencies,
	                            U32&      rOutNumberInputDependencies,
                                IFXDID**& rppOutOutputDependencies,
	                            U32&      ruOutNumberOfOutputDependencies,
								U32*&	  rpOutOutputDepAttrs )=0;

	/**
	        Generate a result from the input DataPacket.

	@param  inDataElementIndex
			The index of the DataElement.

	@param  rpOutData
			The pointer to the output DataElement interface.

	@return Upon success, IFX_OK is returned or 
			IFX_W_MODIFIER_OUTPUT_NOT_CHANGED if the specified output 
			DataElement did not change. Otherwise, one of the following 
			failures are returned: IFX_E_INVALID_ENTRY if the specified 
			DataElement does not exist; or IFX_E_NOT_INITIALIZED if the 
			Modifier, ModifierDataPacket, and/or the ModifierChain was not 
			initialized properly.
	*/
	virtual IFXRESULT IFXAPI  GenerateOutput( 
										U32    inDataElementIndex,
										void*& rpOutData,
										BOOL&  rNeedRelease)=0;

	enum IFXModifierMessage
	{
		DEALLOCATE_INTERMEDIATE_DATA, // Temp data needed to generate outputs.
		DEALLOCATE_ALL_DATA, // Intermediate and output data.
		NEW_MODCHAIN_STATE,
		FORCE_32BIT = 0xFFFFFFFF
	};

	/**
	        Processes a Modifier message.

	@param  eInMessage
			The specific type of notification.

	@param  pMessageContext

	@return Upon success, IFX_OK is returned.
	*/
	virtual IFXRESULT IFXAPI  Notify( 
								IFXModifierMessage eInMessage,
								void*              pMessageContext )=0;

	/**
	        Associates a new ModifierChain with the Modifier.

	@pre    None.

	@post   The provided ModifierChain is associated with the Modifier.
	        If the Modifier was already associated with a ModifierChain, it is
			first released.

	@param  pInModifierChain
			The address of the IFXModifierChain that contains the Modifier.

	@param  inModifierChainIndex
			The index of Modifier in the ModifierChain.

	@return Upon success, IFX_OK is returned. Otherwise, one of the 
			following failures are returned: IFX_E_OUT_OF_MEMORY.
	*/
	virtual IFXRESULT IFXAPI  SetModifierChain(
	                               IFXModifierChain* pInModifierChain,
	                               U32               inModifierChainIndex )=0;

	/**
	        Retrieves the modifier chain that this modifier is associated with.

	@pre    The Modifier has already bein initialized into a modifier chain.

	@post  None.

	@param  ppOutModifierChain
			An AddRefed version of the the modifier chain.

	@return Upon success, IFX_OK is returned. Otherwise, one of the 
			following failures are returned: IFX_E_OUT_OF_MEMORY.
	*/
	virtual IFXRESULT IFXAPI  GetModifierChain( IFXModifierChain** ppOutModifierChain )=0;

	/**
	        Hands back the Modifier's ModifierChain index.

	@param  rOutModifierChainIndex
	         The U32 to contain the Modifier's ModifierChain index.

	@return Upon success, IFX_OK is returned. Otherwise, one of the 
			following failures are returned: IFX_E_NOT_INITIALIZED.
	*/
	virtual IFXRESULT IFXAPI  GetModifierChainIndex( U32& rOutModifierChainIndex )=0;
};


#endif

