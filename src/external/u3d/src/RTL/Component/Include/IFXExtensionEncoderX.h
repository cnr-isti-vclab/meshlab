//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
	@file	IFXExtensionEncoderX.h

			Declaration of IFXExtensionEncoder interface.
*/

#ifndef IFXEXTENSIONENCODERX_H__
#define IFXEXTENSIONENCODERX_H__

#include "IFXEncoderX.h"

struct IFXExtensionData
{
	IFXExtensionData() :
		m_continuationBlockTypesCount(0),
		m_extensionURLCount(0),
		m_extensionURLs(NULL),
		m_extensionInfo(L""), // empty
		m_modifierType(0) // NODE
	{}

	IFXCID m_cid;
	IFXString m_extensionName;
	IFXString m_vendorName;
	U32 m_continuationBlockTypesCount;
	U32 m_extensionURLCount;
	IFXString* m_extensionURLs;
	IFXString m_extensionInfo;
	U32 m_modifierType;
};

// {8A216599-1602-4914-A5DA-7C49F9BBD300}
IFXDEFINE_GUID(IID_IFXExtensionEncoderX,
			   0x8a216599, 0x1602, 0x4914, 0xa5, 0xda, 0x7c, 0x49, 0xf9, 0xbb, 0xd3, 0x0);

/**
	This interface supplies the services supporting the encoding of
	extension modifier.

	@note	Every extension modifier encoder (New Object) should support 
			this interface.
*/
class IFXExtensionEncoderX : virtual public IFXEncoderX
{
public:
	/**
		Return a set of data describing this particular extension, e.g. decoder's CID,
		associated with this encoder-decoder pair. This data will be used for encoding.

		@note	IFXExtensionData structure should be set in the GetExtensionDataX method.
				If not, default IFXExtensionData structure settings will be used. 
	*/
	virtual void IFXAPI GetExtensionDataX( IFXExtensionData &rExtData ) = 0;

	/// @note	Write Manager uses this method to set up a blockType
	virtual void IFXAPI SetBlockTypesX( U32* pBlockTypes, const U32 blockTypeCount ) = 0;
};

#endif
