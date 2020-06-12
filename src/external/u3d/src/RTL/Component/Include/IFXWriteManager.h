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
//
//	IFXWriteManager.h
//
//	DESCRIPTION
//		This header defines the IFXWriteManager interface and the associated
//	interface identifier.
//
//	NOTES
//		The IFXWriteManager interface is supported by the CIFXWriteManager
//	object.  It's IFXCID is defined in IFXCoreCIDs.h and is named
//	CID_IFXWriteManager.
//***************************************************************************
#ifndef IFXWRITEMANAGER_H__
#define IFXWRITEMANAGER_H__

#include "IFXUnknown.h"
#include "IFXCoreServices.h"
#include "IFXWriteBuffer.h"
#include "IFXExportOptions.h"

// {C7C93F01-D804-11d3-A924-00A0C977EB2F}
IFXDEFINE_GUID(IID_IFXWriteManager,
0xc7c93f01, 0xd804, 0x11d3, 0xa9, 0x24, 0x0, 0xa0, 0xc9, 0x77, 0xeb, 0x2f);


/**
	This interface is used to write out a scenegraph to an IFXWriteBuffer.
*/
class IFXWriteManager : public IFXUnknown
{
public:
	/**
	Initializes the Write Manager with the CoreServices object.
	From this the WriteManager obtains a pointer to the IFXSceneGraph object,
	which it needs during the Write() phase.

	@param	pCoreServices	Pointer to the CoreServices object.
	*/
	virtual IFXRESULT IFXAPI  Initialize(IFXCoreServices *pCoreServices) = 0;
	/**
	Writes an IFXSceneGraph out to an IFXWriteBuffer, based on the options supplied
	in exportOptions. The IFXSceneGraph is obtained from the Core Services object
	provided through the call to Initialize().

	@param	pWriteBuffer	An IFXWriteBuffer interface. The WriteManager writes its data out to this interface.
	@param	exportOptions	This bitfield determines which of the sub-components of the scene graph is to be written out.
	*/
	virtual IFXRESULT IFXAPI  Write(IFXWriteBuffer *pWriteBuffer, IFXExportOptions exportOptions = IFXEXPORT_EVERYTHING ) = 0;
	/**
	This method sets the quality factor of the scenegraph elements that are to be written out.

	@param	qualityFactor	The quality factor of the scenegraph elements.
	@param	module			Describes the module to be written out.
	*/
	virtual IFXRESULT IFXAPI  SetQualityFactor(U32 qualityFactor, IFXExportOptions module) = 0;
};

#endif
