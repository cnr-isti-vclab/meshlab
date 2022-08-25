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

@file  IFXDataPacket.h

	The header file that defines the IFXDataPacket interface. */

#ifndef __IFXDATAPACKET_INTERFACES_H__
#define __IFXDATAPACKET_INTERFACES_H__
#include "IFXUnknown.h"

// {AA44881C-50A7-45fe-995D-9BCDE86C66C1}
IFXDEFINE_GUID(IID_IFXDataPacket,
0xaa44881c, 0x50a7, 0x45fe, 0x99, 0x5d, 0x9b, 0xcd, 0xe8, 0x6c, 0x66, 0xc1);

#define IFX_E_DATAPACKET_INVALID_INDEX \
	MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_DATAPACKET, 0x0001)

#define IFX_E_DATAPACKET_ELEMENT_NOT_FOUND \
	MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_DATAPACKET, 0x0002)

#define IFX_E_DATAPACKET_NO_INTEREST_BIT_PROVIDED \
	MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_DATAPACKET, 0x0003)

#define IFX_W_DATAPACKET_DUPLICATE_ENTRY \
	MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_DATAPACKET, 0x0004)

#define IFX_E_DATAPACKET_LOCKED \
	MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_DATAPACKET, 0x0005)


class IFXDataPacket : virtual public IFXUnknown
/**
       The IFXDataPacket interface supplies the services for managing
       a list of void* DataElements that are uniquely identified by a GUID.

-       DataElement Value Types                                              \n
          The value of a DataElement within a DataPacket is an untyped void*
          which can only be practically interpreted with a priori knowledge
          of the DataElement's GUID type.

-       DataElement GUID collisions                                          \n
          DataElement collisions are handled with a same name replacement
          policy.  For example, a value added as a new DataElement in the
          first Modifier would be replaced by a value set by a subsequent
          Modifier that generated a DataElement with the same IFXREFGUID.    */
{
public:

// ACCESS:

	virtual IFXRESULT IFXAPI  GetDataElementAspectBit( IFXREFGUID rInDataElementGUID,
	                                           U32&       ruOutBitField )=0;
	/**<
	        Returns the subject's aspect of interest bit field that cooresponds
	        to the DataElement identified by the supplied @e rInDataElementGUID.

	@param  rInDataElementGUID
	         The DataElement GUID.

	@param  ruOutBitField
	         The U32 to contain the AspectBit value.

	@pre    The DataPacket contains the specified DataElement.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_DATAPACKET_ENTRY_NOT_FOUND                                \n
              The specified DataElement does not exist in the DataPacket. \n\n
-            IFX_E_INVALID_RANGE                                             \n
              The specified DataElement is not within the first 32 DataElements. \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The DataPacket was not initialized properly.                   */



	virtual IFXRESULT IFXAPI  GetDataElementIndex( IFXREFGUID rInDataElementGUID,
	                                       U32&       ruOutIndex )=0;
	/**<
	        Hands back the index that cooresponds to the DataElement
	        identified by the supplied @e rInDataElementGUID.

	@param  rInDataElementGUID
	         The IFXGUID that identifies the DataElement.

	@param  ruOutIndex
	         The U32 to contain the index.

	@pre    The DataPacket contains the DataElement specified by @e
	        ruInDataElementIndex.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_DATAPACKET_ELEMENT_NOT_FOUND                              \n
              The specified DataElements does not exist in the DataPacket. \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The DataPacket was not initialized properly.                   */


	virtual IFXRESULT IFXAPI  GetDataElement( U32    uInDataElementIndex,
	                                  void** ppOutDataElement )=0;
	/**<
	        Hands back the value of the specified DataElement.

	@param  uInDataElementIndex
	         The DataElement index.

	@param  rOutDataElementValue
	         The void* to contain the value of the DataElement.

	@pre    The DataPacket contains the DataElement specified by @e
	        ruInDataElementIndex.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_DATAPACKET_INVALID_INDEX                                  \n
              The DataElement was NOT in the ModelDataPacket.              \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The DataPacket was not initialized properly.                   \n

	@note   The U32 index is retrieved by GetDataElementIndex().             */


	virtual IFXRESULT IFXAPI  GetDataElement( U32       uInDataElementIndex,
	                                  IFXREFIID desiredInterface,
	                                  void**    ppOutDataElement )=0;
	/**<
	        Hands back the value of the specified DataElement cast to the
			desired interface.

	@param  uInDataElementIndex
	         The DataElement index.

	@param  desiredInterface
	         The IID of the desired interface.

	@param  rOutDataElementValue
	         The void* to contain the value of the DataElement.

	@pre    The DataPacket contains the DataElement specified by @e
	        ruInDataElementIndex.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_DATAPACKET_INVALID_INDEX                                  \n
              The DataElement was NOT in the ModelDataPacket.              \n\n
-            IFX_E_UNSUPPORTED                                               \n
              The DataElement does NOT support the desired interface.      \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The DataPacket was not initialized properly.                   \n

	@note   The U32 index is retrieved by GetDataElementIndex().             */


	virtual IFXRESULT IFXAPI  GetDataPacketSize( U32& ruOutNumberOfDataElements )=0;
	/**<
	        Hands back the current number of DataElements in the DataPacket.

	@param  ruOutNumberOfDataElements
	         A reference to the U32 that will contain the size.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The DataPacket was not initialized properly.                   */


	virtual IFXRESULT IFXAPI  GetDataElementGUID( U32       uInDataElementIndex,
	                                      IFXGUID*& rpOutDataElementGUID )=0;
	/**<
	        Hands back the value of the specified DataElement.

	@param  uInDataElementIndex
	         The DataElement index.

	@param  rpOutDataElementValue
	         A pointer to contain the address of the GUID.

	@pre    The DataPacket contains the DataElement specified by @e
	        ruInDataElementIndex.

	@return One of the following IFXRESULT codes:                          \n\n
-            IFX_OK                                                          \n
              No error.                                                    \n\n
-            IFX_E_DATAPACKET_INVALID_INDEX                                  \n
              The DataElement was NOT in the ModelDataPacket.              \n\n
-            IFX_E_NOT_INITIALIZED                                           \n
              The DataPacket was not initialized properly.                   \n

	@note   The U32 index is retrieved by GetDataElementIndex().             */
};

#endif //#define __IFXDATAPACKET_INTERFACES_H__

