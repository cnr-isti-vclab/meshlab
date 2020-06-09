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
	@file	IFXDecoderX.h
			
			Declaration of IFXDecoderX interface.
			This interface is used by the Load Manager to provide data blocks to a decoder.
*/

#ifndef IFXDecoderX_H
#define IFXDecoderX_H

#include "IFXCoreServices.h"
#include "IFXDataBlockX.h"
#include "IFXNameMap.h"
#include "IFXSceneGraph.h"

struct IFXLoadConfig
{
	IFXLoadConfig() 
	{ 
		m_pCoreServices = NULL;
		m_loadId = 0;
		m_external = 0;
		m_palette = IFXSceneGraph::NUMBER_OF_PALETTES;
		m_units = 1.0f; 
	};

	IFXLoadConfig(	IFXCoreServices *pCS, 
					U32 loadID, 
					BOOL external, 
					IFXSceneGraph::EIFXPalette palette, 
					F64 units = 1.0f ) 
	{
		if( pCS )
		{
			pCS->AddRef();
			m_pCoreServices = pCS;
		}
		m_loadId = loadID;
		m_external = external;
		m_palette = palette;
		m_units = units;
	};

	~IFXLoadConfig() 
	{ 
		IFXRELEASE(m_pCoreServices); 
	};

	/** 
		The decoder may store the core services pointer.
		The decoder may use the core services pointer to access the scene graph
		and palettes and other elements from the core services.
	*/
	IFXCoreServices* m_pCoreServices;

	/** 
		This parameter indicates identifier of loader object that created this decoder.
		This id serves for names collision resolution.
	*/
	U32 m_loadId; 

	/** 
		This parameter indicates if object was loaded from external referenced file or not.
	*/
	BOOL m_external;

    IFXSceneGraph::EIFXPalette m_palette;

	/** 
		Units Scaling Factor describes a value which can be used to convert coordinates
		and lengths from a file to meters. Usage of units should be enabled in file 
		header. 
	*/
	F64 m_units;
};

// {4EC3879A-2D4F-4860-9D53-15A268B2B69C}
IFXDEFINE_GUID(IID_IFXDecoderX,
0x4ec3879a, 0x2d4f, 0x4860, 0x9d, 0x53, 0x15, 0xa2, 0x68, 0xb2, 0xb6, 0x9c);

/** 
	The IFXDecoderX interface is implemented by various decoders used by the CIFXLoadManager.
	Each instance of a decoder is used to decode data blocks for one instance of a scene graph object.
	Each type of decoder is used to decode data blocks for a particular type of object.
*/
class  IFXDecoderX : virtual public IFXUnknown
{
public:
	/**
		Initializes and gets a reference to the coreservices object
		The decoder may perform any necessary initialization procedures.

		@param	pConfig	This parameter stores a set of information required for
						decoder initialization.
	*/
	virtual void IFXAPI  InitializeX( const IFXLoadConfig &pConfig ) = 0;

	/// Provides next block of data to the decoder
	virtual void IFXAPI  PutNextBlockX(IFXDataBlockX &rDataBlockX) = 0;

	/** 
		Transfers information from the data block to the object

		@return One of the following IFXRESULT codes: \n\n
-            IFX_OK \n
              No error; the decoder has transferred all available information to the object. \n\n
-            IFX_OK \n
              There is no object, the object pointer is NULL, or there are no data blocks to decode. \n\n
-            IFX_W_PARTIAL_TRANSFER \n
              The decoder has not finished transferring information to the object. \n
	*/
	virtual void IFXAPI  TransferX(IFXRESULT &rWarningPartialTransfer) = 0;
};

#define IFX_W_PARTIAL_TRANSFER		MAKE_IFXRESULT_PASS( IFXRESULT_COMPONENT_LOADER, 0x0001 )

#endif
