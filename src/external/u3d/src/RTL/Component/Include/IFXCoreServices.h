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
	@file	IFXCoreServices.h

			Declaration of IFXCoreServices interface. This interface is used
			to manage the references to the other non-singleton components
*/
#ifndef __IFXCORESERVICES_H__
#define __IFXCORESERVICES_H__

#include "IFXUnknown.h"
#include "IFXString.h"
#include "IFXPalette.h"

enum 
{
	IFXPROFILE_BASE				= 0x00000000,
	IFXPROFILE_EXTENSIBLE		= 0x00000002,
	IFXPROFILE_NOCOMPRESSION	= 0x00000004,
	IFXPROFILE_UNITSSCALE		= 0x00000008
};

#define PALETTE_DEFAULT_GROUP		L""
#define PALETTE_DEFAULT_CAMERA		L""
#define PALETTE_DEFAULT_MODEL		L""
#define PALETTE_DEFAULT_LIGHT		L""
#define PALETTE_DEFAULT_MOTION		L""
#define PALETTE_DEFAULT_IFX_MOTION	L""
#define PALETTE_DEFAULT_TEXTURE		L""
#define PALETTE_DEFAULT_SHADER		L""
#define PALETTE_DEFAULT_MATERIAL	L""

// {AE842701-A9E5-11d3-98A8-00A0C9902D25}
IFXDEFINE_GUID(IID_IFXCoreServices,
0xae842701, 0xa9e5, 0x11d3, 0x98, 0xa8, 0x0, 0xa0, 0xc9, 0x90, 0x2d, 0x25);

/** This interface is used to manage the references to the other non-singleton components.*/
class IFXCoreServices : public IFXUnknown
{
public:
	/** Initializes CoreServices. */
	virtual IFXRESULT IFXAPI  Initialize(U32 uProfile, F64 units = 1.0f) = 0;

	/** Gets Scheduler. */
	virtual IFXRESULT IFXAPI  GetScheduler(IFXREFIID interfaceId, void** ppv) = 0;
	/** Gets Notification Manager. */
	virtual IFXRESULT IFXAPI  GetNotificationManager(IFXREFIID interfaceId, void** ppv) = 0;
	/** Gets Scenegraph. */
	virtual IFXRESULT IFXAPI  GetSceneGraph(IFXREFIID interfaceId, void** ppv) = 0;

	virtual IFXRESULT IFXAPI  GetWeakInterface( IFXCoreServices** ppCoreServices ) = 0;
	/** Gets Name Map. */
	virtual IFXRESULT IFXAPI  GetNameMap( IFXREFIID interfacId, void** ppv ) = 0;
	/** Gets File Reference palette. */
	virtual IFXRESULT IFXAPI  GetFileReferencePalette( IFXPalette** ppv ) = 0;

	virtual IFXRESULT IFXAPI  SetBaseURL(const IFXString& sURL) = 0;
	virtual IFXRESULT IFXAPI  GetBaseURL(IFXString& sURL) = 0;

	virtual IFXRESULT IFXAPI  GetProfile(U32& rProfile) = 0;
	virtual IFXRESULT IFXAPI  GetUnits(F64& rUnits) = 0;
};


#endif
