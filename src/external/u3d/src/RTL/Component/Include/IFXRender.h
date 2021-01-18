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
// IFXRender interface class
#ifndef IFX_RENDER_H
#define IFX_RENDER_H

#include "IFXRenderContext.h"

// {CF0D9FBD-AA95-42ee-9F46-A2158D04326E}
IFXDEFINE_GUID(IID_IFXRender,
			   0xcf0d9fbd, 0xaa95, 0x42ee, 0x9f, 0x46, 0xa2, 0x15, 0x8d, 0x4, 0x32, 0x6e);

/**
	IFXRender - main render device interface.

	This interface represents a single rendering context on one video card.
	The IFXRenderContext interface represents a virtual window that could
	span multiple video cards and multiple threads.  The implementation
	for IFXRenderContext uses IFXRender objects to do the actual rendering
	for each child window that lies completely on one device and runs in
	one thread.

	@todo: How does this relate to the IFXRenderServices object?
*/
class IFXRender : virtual public IFXRenderContext
{
public:
	using IFXRenderContext::Initialize;
	/**
	*  Initializes (or reinitializes) the IFXRender instance with the specified
	*  parameters.
	*
	*  @param  uDeviceNum  The logical device number of the video card that will
	*            perform the rendering.  The default is 0 (the primary
	*            device).
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*
	*
	*/
	virtual IFXRESULT IFXAPI Initialize(U32 uDeviceNum)=0;

	/**
	*  Enables this renderer as the active renderer.  This must be called before making
	*  any rendering calls!
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK  No error.
	*/
	virtual IFXRESULT IFXAPI MakeCurrent()=0;

	/**
	*  Sets the current window parameters.
	*
	*  @param  rWindow An input reference to an IFXRenderWindow object specifying the
	*          parameters for the window.
	*
	*  @return An IFXRESULT code.
	*
	*  @retval IFX_OK No error
	*/
	virtual IFXRESULT IFXAPI SetWindow(IFXRenderWindow& rWindow)=0;
};

typedef IFXSmartPtr<IFXRender> IFXRenderPtr;

#endif
