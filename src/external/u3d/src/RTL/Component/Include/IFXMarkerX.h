//***************************************************************************
//
//  Copyright (c) 2002 - 2006 Intel Corporation
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
	@file	IFXMarkerX.h

			The IFXMarkerX interface provides i/o control of components 
			within the scene graph database.
*/


#ifndef __IFXMARKERX_H__
#define __IFXMARKERX_H__


#include "IFXMarker.h"
#include "IFXEncoderX.h"


IFXDEFINE_GUID(IID_IFXMarkerX,
0x66ea2426, 0x99d2, 0x47b5, 0x9d, 0xc5, 0xf0, 0x7f, 0x6a, 0xe6, 0x0, 0x65);

#define IFX_MAXIMUM_QUALITY_FACTOR	1000
#define IFX_DEFAULT_QUALITY_FACTOR	1000

class IFXMarkerX : virtual public IFXMarker
{
public:
	enum EIFXQualityFactorFlags
	{
		POSITION_QUALITY  = 1 << 1,
		TEXCOORD_QUALITY  = 1 << 2,
		NORMAL_QUALITY	  = 1 << 3,
		DIFFUSE_QUALITY   = 1 << 4,
		SPECULAR_QUALITY  = 1 << 5,
		ALL               = 1 << 31
	};

	/**
		Function that hands back the IFXEncoderX interface pointer to a 
		component that can be used to encode this component.

		@param	rpEncoderX	Reference to the encoder's interface pointer 
							that will be set to a valid value upon success.
	*/
	virtual void IFXAPI  GetEncoderX(IFXEncoderX*& rpEncoderX) = 0;

	/**
		Function used to set the quality factors of one or more indentified 
		quality settings.

		@param	uQualityFactor		New quality factor that must be in the 
									range [0, IFX_MAXIMUM_QUALITY_FACTOR].  
									As quality factors increase, the actual 
									quality also increases.  The default 
									quality factor is 
									IFX_DEFAULT_QUALITY_FACTOR.
		@param	uQualityFactorMask	Mask of EIFXQualityFactorFlags flags 
									that defines the set of quality 
									settings that the new quality factor is 
									to be assigned to upon success.  
									IFXMarkerX::ALL is used to identify the 
									default quality setting.
	*/
	virtual void IFXAPI  SetQualityFactorX(
								U32 uQualityFactor, 
								U32 uQualityFactorMask = (U32)IFXMarkerX::ALL) = 0;

	/**
		Function used to get a single quality factor setting.

		@param	ruQualityFactor		Reference to a value that upon success 
									is initalized with a quality factor 
									setting in the range [0, 
									IFX_MAXIMUM_QUALITY_FACTOR].
		@param	uQualityFactorMask	A single EIFXQualityFactorFlags flag 
									that identifies the quality factor 
									setting to be obtained.  Use 
									IFXMarkerX::ALL to get the default 
									quality setting.  The behavior of 
									other flag combinations are undefined.
	*/
	virtual void IFXAPI  GetQualityFactorX(
								U32& ruQualityFactor, 
								U32 uQualityFactorMask = (U32)IFXMarkerX::ALL) = 0;
};


#endif
