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
// IFXRenderWindow.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRenderContext.h
#ifndef IFX_RENDER_WINDOW_H
#define IFX_RENDER_WINDOW_H

#include "IFXOSRender.h"

//==============================
// IFXRenderWindow
//==============================
/**
*	Interface and data storage struct to pass window parameters to 
*	an IFXRenderContext object.
*
*	@see IFXRenderContext::Initialize(), IFXRenderContext::SetWindow()
*/
class IFXRenderWindow
{
public:
	//=============================================
	// IFXRenderWindow Constructor/Destructor
	//=============================================
	IFXRenderWindow();
	virtual ~IFXRenderWindow();

	/**
	*	Sets the antialiasing mode for the render context.
	*
	*	
	*
	*	@param	eAAMode	Input IFXenum specifying the antialiasing mode to be used
	*					for rendering.  Possible values are:
	*					- @b IFX_NONE: No antialiasing.
	*					- @b Others: Need to determine this.
	*
	*	@return	An IFXRESULT code.
	*
	*	@retval	IFX_OK	No error.
	*	@retval	IFX_E_INVALID_RANGE	The value specified for @a eAAMode is invalid.
	*/
	IFXRESULT IFXAPI SetAntiAliasingMode(IFXenum eAAMode);

	/**
	*	Sets the antialiasing enabled setting for the render context.
	*
	*	
	*
	*	@param	eAAMode	Input IFXenum specifying the antialiasing mode to be used
	*					for rendering.  Possible values are:
	*					- @b IFX_NONE: No antialiasing.
	*					- @b Others: Need to determine this.
	*
	*	@return	An IFXRESULT code.
	*
	*	@retval	IFX_OK	No error.
	*	@retval	IFX_E_INVALID_RANGE	The value specified for @a eAAMode is invalid.
	*/
	IFXRESULT IFXAPI SetAntiAliasingEnabled(IFXenum eAAEnabled);

	/**
	*	Specifies whether or not to use direct to screen (DTS) mode or not.
	*	If bDTS is TRUE, rendering occurs normally (i.e. back buffer is copied directly
	*	to the screen on IFXRenderContext::Swap()).  If bDTS is FALSE, then the data is copied
	*	from the back buffer to the memory location specified by SetWindowPtr().
	*
	*	@param bDTS	Input boolean specifying whether or not to use direct to screen rendering.
	*
	*	@return An IFXRESULT code
	*
	*	@retval	IFX_OK	No error.
	*/
	IFXRESULT IFXAPI SetDTS(BOOL bDTS);	
	/**
	*	Specifies whether or not this object has a transparent background.  This only takes
	*	effect if the DTS is false (see SetDTS()).
	*
	*	@param	bTransparent	Input BOOL that is TRUE if the window is to have a 
	*							transparent background, FALSE otherwise.
	*
	*	@return An IFXRESULT code.
	*
	*	@retval	IFX_OK	No error.
	*/
	IFXRESULT IFXAPI SetTransparent(BOOL bTransparent);

	/**
	*	Sets the size (and position) of the renderable area.  If full screen mode is being
	*	used, then the @a iWidth and @a iHeight parameters specify the screen resolution.  
	*	If windowed mode is used, then all four parameters define where (relative to the 
	*	passed in window or memory location pointer) the rendering occurs.  This, in effect,
	*	creates a sub window region for the renderer to use.  The upper left corner of the
	*	"parent" area is (0,0).
	*
	*	@param	rcWindow	Input IFXRect specifying the size and position of the 
	*						window relative to the parent window.  If fullscreen
	*						mode is enabled, then only the m_Width and m_Height of rcWindow
	*						are used.
	*
	*	@return An IFXRESULT code
	*
	*	@retval	IFX_OK	No error.
	*	@retval	IFX_E_INVALID_RANGE	If full screen mode is used, then the @a rcWindow.m_Width
	*								and/or @a rcWindow.m_Height parameters are not valid 
	*								display resolutions.  If windowed mode is used, then the 
	*								@a rcWindowm_Width or @a rcWindow.m_Height parameters are <= 0.
	*/
	IFXRESULT IFXAPI SetWindowSize(const IFXRect& rcWindow);

	/**
	*	Specifies the "parent" window or memory location for rendering.  If direct to screen (DTS) 
	*   is enabled, then this should be a window pointer.  If DTS is disabled, then this should 
	*	be a memory buffer.
	*
	*	@param pvWindow	An OS and DTS dependent pointer to the rendering area's "parent" area.
	*	@param pvDisplay	This is used under X11 for the X Display* pointer.  This paramter
	*						is ignored under other windowing systems.  Defaults to 0.
	*
	*	@return An IFXRESULT code.
	*
	*	@retval	IFX_OK	No error.
	*	@retval	IFX_E_INVALID_POINTER	@a pvWindow was NULL.  
	*	@todo Should we allow @a pvWindow to be NULL?
	*/
	IFXRESULT IFXAPI SetWindowPtr(IFXHANDLE pvWindow);

	/**
	*	Gets the antialiasing enabled status to be used for rendering.
	*
	*	@return	The antialiasing enabled mode (IFXenum).
	*	@see SetAntiAliasingEnabled()
	*/
	IFXenum IFXAPI GetAntiAliasingEnabled() const;

	/**
	*	Gets the antialiasing mode to be used for rendering.
	*
	*	@return	The antialiasing mode (IFXenum).
	*	@see SetAntiAliasingMode()
	*/
	IFXenum IFXAPI GetAntiAliasingMode() const;

	/**
	*	Gets the direct to screen rendering state.
	*
	*	@return Boolean value.
	*
	*	@retval	TRUE	Direct-to-screen mode is enabled.
	*	@retval	FALSE	Direct-to-screen mode is disabled
	*	@see SetDTS()
	*/
	BOOL IFXAPI GetDTS() const;

	/**
	*	Gets whether or not the window is transparent.
	*
	*	@return Boolean value.
	*
	*	@retval	TRUE	The window is transparent.  If this and DTS is TRUE, then 
	*					the window's background will show through this window.
	*	@retval	FALSE	The window is not transparent.
	*/
	BOOL IFXAPI GetTransparent() const;

	/**
	*	Gets the size and position of the rendering area within the "parent" area.
	*
	*	@return The set window size.
	*	@see SetWindowSize()
	*/
	const IFXRect& IFXAPI GetWindowSize() const;

	/**
	*	Gets the OS and DTS dependent parent area pointer.
	*
	*	@return The parent window or buffer pointer.
	*	@see SetWindowPtr()
	*/
	IFXHANDLE IFXAPI GetWindowPtr() const;
	IFXHANDLE IFXAPI GetHDC() const;

	/**
	*	Gets the Window size and position in screen coordinates (monitor space).
	*	
	*	@param rcIn	Input IFXRect that will receive the window size in monitor space.
	*
	*	@return void
	*/
	void IFXAPI GetWindowSizeVC(IFXRect& rcIn) const;

	/**
	*	Initializes all data to default data.
	*
	*	Default values are:
	*	- <b>AntiAliasing Enabled</b>: IFX_AA_DEFAULT
	*	- <b>AntiAliasing Mode</b>: IFX_AA_DEFAULT
	*	- <b>Direct To Screen</b>: TRUE
	*	- @b Window @b Size: (X,Y,Width,Height) = (0,0,320,240)
	*	- @b Device @b Number: 0 (primary adapter)
	*	- @b Window @b Pointer: NULL
	*	- @b X11 @b Display: NULL
	*/
	void IFXAPI InitData();

	BOOL IFXAPI SetDirtyWindow(IFXRenderWindow& window);

#if defined( RENDERING ) && ( defined( LINUX ) || defined( __linux__ ) )
	XVisualInfo* IFXAPI GetVisual() const;
	Window GetDrawable() const;
	IFXRESULT IFXAPI SetVisual(XVisualInfo* visInfo);
	IFXRESULT IFXAPI SetDrawable(const Window drawable);
#endif

protected:
	IFXenum		m_eAAEnabled;		///< AntiAliasing enabled
	IFXenum		m_eAAMode;			///< AntiAliasing mode
	BOOL		m_bDirectToScreen;	///< Direct to screen BOOL flag
	BOOL		m_bTransparent;		///< Is this window "transparent"?
	IFXRect		m_rcWindow;			///< Size of window or desired resolution for full screen mode
	IFXHANDLE	m_pvWindow;			///< Destination surface (or window) for rendering.
#ifdef WIN32
	IFXHANDLE	m_pDC;				///< handle to a display device context 
#endif
#if defined( RENDERING ) && ( defined( LINUX ) || defined( __linux__ ) )
	Window		m_Window;
	XVisualInfo *m_pVisInfo;
#endif
};


#endif
