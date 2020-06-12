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
	@file	IFXMetaDataX.h

			This header defines the IFXMetaData interface and
			the associated interface identifier.

	@note	The IFXMetaDataX interface is supported by the CIFXMetaData component. 
			It is expected to be used by the CIFXCoreServices, CIFXCoreServicesRef, 
			CIFXMarker and CIFXDataBlock components, as well as U3D clients..  
			Its component id is named IID_IFXMetaDataX.
*/


#ifndef IFXMetaData_H
#define IFXMetaData_H

//***************************************************************************
//	Includes
//***************************************************************************

#include "IFXUnknown.h"
#include "IFXString.h"

//***************************************************************************
//	Enumerations
//***************************************************************************

enum IFXMetaDataAttribute
{
	IFXMETADATAATTRIBUTE_STRING = 0,
	IFXMETADATAATTRIBUTE_BINARY = 1,
	IFXMETADATAATTRIBUTE_HIDDEN = 2,
	IFXMETADATAATTRIBUTE_DOUBLECLICK = 0x10,
	IFXMETADATAATTRIBUTE_VALUEINRIGHTCLICKMENU = 0x20,
	IFXMETADATAATTRIBUTE_KEYINRIGHTCLICKMENU = 0x40,
	IFXMETADATAATTRIBUTE_ACTION = 0x100,
	IFXMETADATAATTRIBUTE_FILE = 0x200,
	IFXMETADATAATTRIBUTE_MIME = 0x400
};


//***************************************************************************
//	Classes, structures and types
//***************************************************************************


//***************************************************************************
//	Interfaces and interface identifiers
//***************************************************************************

// {0DA0A0A6-B449-479f-A312-A048B2C0AFAB}
IFXDEFINE_GUID(IID_IFXMetaDataX, 
0xda0a0a6, 0xb449, 0x479f, 0xa3, 0x12, 0xa0, 0x48, 0xb2, 0xc0, 0xaf, 0xab);

/**
	This interface is used by a client to manage any related set of meta-data that 
	is typically associated with some component. It fully supports definition of 
	information specifically related to the Meta Data section of the standard block 
	header. It derives from the IFXUnknown interface. 

	@note	Meta-data consists of zero or more key/value pairs that are defined by the client. 
			A key/value pair consists of a key and a value. The key is a string-based reference 
			or description of the associated value. The value is either a string or a sequence 
			of binary bytes (proceeded by the size in bytes). This ends up being saved in the file. 
*/

class IFXMetaDataX : virtual public IFXUnknown
{
	public:
		/**
			Method that returns the number of key/value pairs.*/
		virtual void IFXAPI  GetCountX(U32& rCount)const = 0;

		/**
			Method that hands back the index of the specified key if it exists. 
			If the specified key does not exists, an IFXRESULT is returned 
			instead of an exception being thrown. 
			It is expected that this might be a typical usage scenario.
			Method can return IFX_OK or IFX_E_NO_METADATA result.*/
		virtual IFXRESULT IFXAPI  GetIndex(const IFXString& rKey, U32& rIndex) = 0;

		/**
			Method that copies the key string associated 
			with the key/value pair specified by index into the caller's string.*/
		virtual void IFXAPI  GetKeyX(U32 uIndex, IFXString& rOutKey) = 0;

		/**
			Method that returns the value's enumerated type 
			(either binary or string) associated with the key/value pair specified by index. */
		virtual void IFXAPI  GetAttributeX(U32 uIndex, IFXMetaDataAttribute& rAttribute) = 0;

		/**
		Method that sets the value's enumerated type 
		(either binary or string) associated with the key/value pair specified by index. */
		virtual void IFXAPI  SetAttributeX(U32 uIndex, const IFXMetaDataAttribute& rAttribute) = 0;

		/**
			Method that copies the value binary data (buffer of U8s) associated 
			with the key/value pair specified by index into the caller's buffer. 
			The key/value pair must be of type binary.*/
		virtual void IFXAPI  GetBinaryX(U32 uIndex, U8* pDataBuffer) = 0;

		/**
			Method that returns the U32 size in bytes of the value binary data 
			associated with the key/value pair specified by index. 
			The key/value pair must be of type binary.*/
		virtual void IFXAPI  GetBinarySizeX(U32 uIndex, U32& rSize) = 0;

		/**
			Method that copies the value string associated with the key/value pair 
			specified by index into the caller's string. 
			The specified key/value pair must be of type string.*/
		virtual void IFXAPI  GetStringX(U32 uIndex, IFXString& rValue) = 0; 

		/**
			Method that returns a BOOL that if TRUE indicates that 
			the specified key/value pair is serialized when 
			the CIFXWriteManager is instructed to write meta-data for 
			the associated component's style of component. 
			Otherwise if FALSE, it is not serialized when serialization occurs. 
			The key/value pair is specified by index.*/
		virtual void IFXAPI  GetPersistenceX(U32 uIndex, BOOL& rPersistence) = 0;

		/**
			Method that adds a new key/value pair or replaces an existing key/value pair 
			using the the specified key and U8 binary data buffer. 
			The U32 size in bytes of the data buffer is also specified, as well as 
			the persistence BOOL. The binary data is copied out of the caller's buffer 
			and into a buffer managed by the component that exposes this interface.*/
		virtual void IFXAPI  SetBinaryValueX(const IFXString& rKey, U32 size, const U8* pData) = 0;

		/**
			Method that adds a new key/value pair or replaces an existing key/value pair 
			using the the specified key, string value and persistence BOOL. 
			The string is copied from the caller's string to one managed by 
			the component that exposes this interface.*/
		virtual void IFXAPI  SetStringValueX(const IFXString& rKey, const IFXString& rValue) = 0;

		/**
			Method that sets if the specified key/value pair is serialized when 
			the CIFXWriteManager is instructed to write meta-data for the associated 
			component's style of component. If TRUE, it's serialized. Otherwise, it is 
			not serialized. The key/value pair is specified by index.*/
		virtual void IFXAPI  SetPersistenceX(U32 uIndex, BOOL value) = 0;

		/**
			Method that deletes the key/value pair that is specified by index.*/
		virtual void IFXAPI  DeleteX(U32 uIndex) = 0; 

		/**
			Method that delete all of the key/value pairs that exist. 
			It can be successfully called even when no key/value pairs exist.*/
		virtual void IFXAPI  DeleteAll() = 0; 

		/**
			Method that Appends pSource Meta Data to the existing one.*/
		virtual void IFXAPI  AppendX(IFXMetaDataX* pSource) = 0;

		/**
			Method that returns the "packed" string including key and all additional attributes .*/
		virtual void IFXAPI  GetEncodedKeyX(U32 uIndex, IFXString& rOutKey) = 0;

		/**
			Method that returns the number of additional attributes .*/
		virtual void IFXAPI  GetSubattributesCountX(U32 uIndex, U32& rCountSubattributes) = 0;

		/**
			Method that hands back the index of the specified additional attribute if it exists.
			If the specified additional attribute does not exist, an IFXRESULT is returned 
			instead of an exception being thrown. 
			It is expected that this might be a typical usage scenario.
			Method can return IFX_OK or IFX_E_NO_METADATA result.*/
		virtual IFXRESULT IFXAPI  GetSubattributeIndex(U32 uIndex, const IFXString& rSubattributeName, U32& rSubattributeIndex) = 0;

		/**
			Method that copies the name of the additional attribute specified by indexes into the caller's string. */
		virtual void IFXAPI  GetSubattributeNameX(U32 uIndex, U32 uSubattributeIndex, IFXString& rSubattributeName) = 0;

		/**
			Method that copies the value string associated with the additional attribute
			specified by indexes into the caller's string. NULL means that no value exist. */
		virtual void IFXAPI  GetSubattributeValueX(U32 uIndex, U32 uSubattributeIndex, IFXString*& pSubattributeValue) = 0;

		/**
			Method that adds a new additional attribute or replaces an existing additional attribute 
			using the the specified index, name and string value. If pSubattributeValue equals to NULL,
			this means that additional attribute has no value. */
		virtual void IFXAPI  SetSubattributeValueX(U32 uIndex, const IFXString& rSubattributeName, const IFXString* pSubattributeValue) = 0;

		/**
			Method that deletes additional attribute that is specified by index. */
		virtual void IFXAPI  DeleteSubattributeX(U32 uIndex, U32 uSubattributeIndex) = 0;

		/**
			Method that delete all of the additinal attributes that exist. 
			It can be successfully called even when no additinal attributes exist. */
		virtual void IFXAPI  DeleteAllSubattributes(U32 uIndex) = 0;
};

#endif
