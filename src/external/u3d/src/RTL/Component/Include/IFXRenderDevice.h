//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
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
// IFXRenderDevice.h

#ifndef IFX_RENDER_DEVICE_H
#define IFX_RENDER_DEVICE_H

#include "IFXRenderCaps.h"

/**
 *	Interface ID for the IFXRenderDevice interface.
 */
// {ADB22CE4-899D-46d1-8CDF-C37B8D81543C}
IFXDEFINE_GUID(IID_IFXRenderDevice,
0xadb22ce4, 0x899d, 0x46d1, 0x8c, 0xdf, 0xc3, 0x7b, 0x8d, 0x81, 0x54, 0x3c);

/**
 *	The IFXRenderDevice object represents the encapsulation of a
 *	single video card.  Most methods and settings are through
 *	the CIFXRenderDevice interface.  In fact, this interface is
 *	only used by the implementation of the IFXRenderServices
 *	interface, so in most cases it can be ignored.
 */
class IFXRenderDevice : virtual public IFXUnknown
{
public:
	//=========================
	// IFXRenderDevice Methods
	//=========================
	/**
	 *	Gets whether or not this renderer type is supported for the given device number.
	 *
	 *	@param	uDeviceNum	The logical device number of the video card that will
	 *						perform the rendering.
	 *
	 *	@param	bSupported	Output BOOL that is set to TRUE if the renderer is supported
	 *						for @a uDeviceNum.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI GetSupported(U32 uDeviceNum, BOOL& bSupported)=0;

	/**
	 *	Initializes this device object for this specified physical rendering
	 *	device.
	 *
	 *	@param	uDeviceNum	Input device number (0 based index) representing
	 *						which rendering device to encapsulate.
	 *
	 *	@return	IFXRESULT
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI Initialize(U32 uDeviceNum)=0;

	/**
	 *	Deletes the static mesh given by @a uInMeshId from the device's
	 *	VRAM.
	 *
	 *	@param	uInMeshId	Mesh Id of a static mesh to be deleted from
	 *						a device's video memory.
	 *
	 *	@return IFXRESULT
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI DeleteMesh(U32 uInMeshId)=0;

	/**
	 *	Delete the texture given by @a uInhId from the device's
	 *	VRAM.
	 *
	 *	@param	uInMeshId	Mesh Id of a texture to be deleted from
	 *						a device's video memory.
	 *
	 *	@return IFXRESULT
	 *	@retval	IFX_OK	No error.
	 */
	virtual IFXRESULT IFXAPI DeleteTexture(U32 uInId)=0;

	/**
	 *	Get the capabilities of this rendering device.
	 *
	 *	@note See the description of IFXRenderCaps for more information.
	 *
	 *	@param	rCaps	Reference to an IFXRenderCaps object to be
	 *					filled with the capability information for
	 *					this device.
	 */
	virtual IFXRESULT IFXAPI GetCaps(IFXRenderCaps& rCaps) const=0;

protected:

	IFXRenderDevice() {}			///< Constructor
	virtual ~IFXRenderDevice() {}	///< Destructor
};
/**
 *	Smart pointer type for IFXRenderDevice objects.
 */
typedef IFXSmartPtr<IFXRenderDevice> IFXRenderDevicePtr;

#endif // IFX_RENDER_DEVICE_H
