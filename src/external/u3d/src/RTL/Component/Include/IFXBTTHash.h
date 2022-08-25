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
	@file	IFXBTTHash.h
*/

//  DESCRIPTION
//
//  It is very important to understand the memory management policy
//  of the hash.  This hash is completely responsible for allocating
//  and deallocating ALL of the memory used in the hash entries and the
//  the hash array itself.  So the pointers passed to the hash
//  via the Initialize(), Add() and Find() methods must all represent
//  pre-allocated structs, which the hash will use to copy data into
//  or from.  In other words, the pointers do not imply either the transfer
//  of ownership of allocated memory OR the exposure of actual hash
//  entries to the user of the interface.  Thus, for example, the
//  IFXBTTHashEntry pointer passed as an argument in Add() must be an
//  IFXBTTHashEntry struct that's been created prior to the call to Add()
//  and should subsequently be destroyed at some point after the call to
//  Add().  It will merely be used by the hash within the Add() method
//  to create a new IFXBTTHashEntry that is identical.  Similarly for
//  the IFXBTTHashEntry array provided in the Initialize() method -- this
//  array will not be retained by the hash, it will be used to create
//  a new array with identical entries.  The calling function is responsible
//  for the memory of the array argument, while the hash will be responsible
//  for the memory of the array that it creates and manages.
//
//  The short version of this is: "If you alloc it, you dealloc it."  The
//  BTT hash doesn't hold onto anything.
//
//***************************************************************************

#ifndef IFXBTTHASH_H
#define IFXBTTHASH_H

#include "IFXSceneGraph.h"


// {0E238F14-2CEC-453a-8F60-0F63A3880953}
IFXDEFINE_GUID(IID_IFXBTTHash,
			   0xe238f14, 0x2cec, 0x453a, 0x8f, 0x60, 0xf, 0x63, 0xa3, 0x88, 0x9, 0x53);

#define IFX_E_HASHMAPU32_ID_NOT_FOUND \
	MAKE_IFXRESULT_FAIL( IFXRESULT_COMPONENT_CORE_SERVICES, 0x00100 )


class IFXBTTHash : public IFXUnknown
{
public:

	class IFXBTTHashEntry
	{
	public:

		// default constructor
		IFXBTTHashEntry()
		{
			blockType = 0;
			paletteType = IFXSceneGraph::NUMBER_OF_PALETTES;
			flags = 0;
		}

		// paramaterized constructor
		IFXBTTHashEntry( U32      in_blockType,
			IFXSceneGraph::EIFXPalette in_paletteType,
			U32      in_flags,
			const IFXCID*  in_decoderCID )

		{
			blockType = in_blockType;
			paletteType = in_paletteType;
			flags = in_flags;
			decoderCID = *in_decoderCID;
		}

		~IFXBTTHashEntry()
		{
			if ( flags & NEWBLOCKTYPE )
			{
			}
		}

		IFXRESULT IFXAPI Initialize()
		{
			return IFX_OK;
		}

		U32 blockType;  // NOTE: this is the hash 'key'
		IFXSceneGraph::EIFXPalette paletteType;
		U32 flags;
		IFXCID decoderCID;
	};

	enum EIFXBTTFlags
	{
		CONTINUATION  = 1<<0,
		CHAIN     = 1<<1,
		NEWBLOCKTYPE  = 1<<2,
		ALL       = 0xFFFFFFFF
	};

	virtual IFXRESULT IFXAPI  Initialize () = 0 ;
	virtual IFXRESULT IFXAPI  Initialize ( 
									U32	uHashSize,
									U32 numEntries,
				 const IFXBTTHashEntry* pEntryList ) = 0 ;

	virtual IFXRESULT IFXAPI  Add  ( const IFXBTTHashEntry* pEntry ) = 0 ;
	virtual IFXRESULT IFXAPI  Delete ( U32 ukey ) = 0 ;
	virtual IFXRESULT IFXAPI  Find   ( U32 ukey, IFXBTTHashEntry* pEntry ) = 0 ;
};

#endif
