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
	@file	IFXModifierDataPacket.h

			The header file that defines the IFXModifierDataPacket interface.
*/

#ifndef IFXMODIFIERDATAPACKET_H
#define IFXMODIFIERDATAPACKET_H

#include "IFXDataPacket.h"
#include "IFXModifierDataElementIter.h"

class IFXModifierChain;

// {199BE61C-05EA-4e97-B1F6-3A6F2A088D16}
IFXDEFINE_GUID(IID_IFXModifierDataPacket,
0x199be61c, 0x5ea, 0x4e97, 0xb1, 0xf6, 0x3a, 0x6f, 0x2a, 0x8, 0x8d, 0x16);

#define IFX_E_MODIFIER_DATAPACKET_ENTRY_CONSUMED \
	MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_MODIFIER_DATAPACKET, 0X0001)

struct IFXDidEntry
{
	IFXDID Did;
	U32 Flags;
};


class IFXModifierDataPacket : virtual public IFXDataPacket 
/**
    The IFXModifierDataPacket interface supplies extended DataPacket 
	services that relate to result caching within a ModifierChain.
*/
{
public:

	virtual IFXRESULT IFXAPI  InvalidateDataElement( U32 inInvalidDataElementIndex )=0;
	/**<
	        Notification that the supplied DataElement is invalid.
	  
	@param  inInvalidDataElementIndex
	         The index of the DataElement that is invalid.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The ModifierDataPacket, Modifier2, and/or the ModifierChain was
			  not initialized properly.

	@note	This allows the ModifierChain to keep track of what Modifiers need
	        to be brought up to date when delivering a DataElement.          */


	virtual IFXRESULT IFXAPI  DataElementIsValid( 
											U32   inDataElementIndex,
											BOOL& rbOutIsValid )=0;
	/**<
	        Returns the validity of the DataElement.
	  
	@param  inDataElementIndex
	         The index of the DataElement.

	@param  rbOutIsValid
	         The BOOL to hold TRUE if the specified DataElement is valid, or 
			 FALSE if it is not.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_DATAPACKET_INVALID_INDEX                                  \n
              The specified DataElement does not exist in the DataPacket.  \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The ModifierDataPacket was not initialized properly.           */


	virtual IFXRESULT IFXAPI  GetDataElementChangeCount( 
											U32  inDataElementIndex,
											U32& rOutChangeCount )=0;
	/**<
	        Returns the change count of the DataElement for interpreting 
			validity. It will force the Data Element to be valid if possible.
	  
	@param  inDataElementIndex
	         The index of the DataElement.

	@param  rOutChangeCount
	         The U32 to contain the change count of the DataElement.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_DATAPACKET_INVALID_INDEX                                  \n
              The specified DataElement does not exist in the DataPacket.  \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The Modifier or ModifierDataPacket was not initialized properly. */

	virtual IFXRESULT IFXAPI  GetModifierChain(IFXModifierChain** ppOutModChain) = 0;
	/**<
	        Gets the ModifierChain that this Data packet belongs to. 
	  
	@param  ppOutModChain
	         pointer to the value to receive the Mod chain pointer. If the 
			 function succeeds then the mod chain returned is AddReffed. 

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The Modifier or ModifierDataPacket was not initialized properly. 
-            IFX_E_INVALID_POINTER                                           \n
              ppOutModChain is NULL. */


	virtual IFXRESULT IFXAPI  GetChainIndex(U32* pOutChainIndex) = 0;
	/**<
	        Returns the Index of this DataPacket in the Modifier Chain. 
	  
	@param  pOutChainIndex
	         The pointer to a U32 to receive the index of the DataPacket.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_INVALID_POINTER                                           \n
              pOutChainIndex is NULL.
-            IFX_E_NOT_INITIALIZED                                           \n
              The Modifier or ModifierDataPacket was not initialized properly. */


	// Iterator methods
	virtual IFXRESULT IFXAPI  GetIterator(
									U32 inFlags, 
									IFXModifierDataElementIterator** ppOutIter) = 0;
	/**<
	        Initializes the given iterator to iterate across all of the 
			datapacket elements that support the Flags specified. The iterator 
			must be deleted when you are finished with it. 
	  
	@param  inFlags
	         The Flags of the DataElements you wish to iterate across

	@param  ppOutIter
	         The Iterator to be configured ot iterate across the Items

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_INVALID_POINTER                                           \n
              out_pIter is NULL.
-            IFX_E_NOT_INITIALIZED                                           \n
              The Modifier or ModifierDataPacket was not initialized properly. */


};

#endif
