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
// IFXRenderView.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRender.h
#ifndef IFX_RENDER_VIEW_H
#define IFX_RENDER_VIEW_H

//==============================
// IFXRenderView
//==============================
/**
 *	Data storage and interface to pass view parameters to 
 *	IFXRender objects.  This interface controls view port size (within
 *	the rendering window), camera projection characteristics, near and
 *	far clipping planes, and the pixel aspect ratio.
 *
 *	@see IFXRenderContext::SetView()
 */
class IFXRenderView
{
public:
	//=============================================
	// IFXRenderView Constructor/Destructor
	//=============================================
	IFXRenderView();
	virtual ~IFXRenderView();

	//================================
	// IFXRenderView methods
	//================================		
	/**
	 *	Sets the projection mode for creating the projection matrix.
	 *
	 *	@param eMode	Input IFXenum for specifying the projection mode. Valid values are:
	 *					- @b IFX_PERSPECTIVE - Normal camera style projection.
	 *					- @b IFX_ORTHOGRAPHIC - Appears as if camera is infinitely far from the object.
	 *						The view frustum sides are parallel.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The project mode specified is invalid.
	 */
	IFXRESULT IFXAPI SetProjectionMode(IFXenum eMode);

	/**
	 *	Specifies whether to use the custom view frustum dimensions (specified with 
	 *	SetProjectionRect()), or to calculate one.
	 *
	 *	@param bUseProjRect	Input BOOL specifying whether to use the manual projection 
	 *						rect (@b TRUE), or to calculate one based on other parameters, (@b FALSE).
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetUseProjectionRect(BOOL bUseProjRect);

	/**
	 *	Specifies whether to calculate the width of the view frustum, or use specified values.  
	 *
	 *	@param bAutoHorizontal	Input BOOL specifying whether or not to use manually specified
	 *							view frustum widths (@b FALSE), or to calculate them (@b TRUE).
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetAutoHorizontalView(BOOL bAutoHorizontal);

	/**
	 *	Sets the horizontal field of view used to calculate the projection matrix.  This is only
	 *	used when the projection mode is @b IFX_PERSPECTIVE.  
	 *
	 *	@param fFOV	Input F32 describing the vertical field of view in degrees as measured from
	 *				from the center line of the view. This must be in the range 0 - 90.0
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The field of view specified is out of the valid range.
	 */
	IFXRESULT IFXAPI SetFOVx(F32 fFOV);
	
	/**
	 *	Sets the vertical field of view used to calculate the projection matrix.  This is only
	 *	used when the projection mode is @b IFX_PERSPECTIVE.  
	 *
	 *	@param fFOV	Input F32 describing the vertical field of view in degrees as measured from
	 *				from the center line of the view. This must be in the range 0 - 90.0
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The field of view specified is out of the valid range.
	 */
	IFXRESULT IFXAPI SetFOVy(F32 fFOV);
	
	/**
	 *	Sets the vertical number of world space units to appear in the view.  This only applies
	 *	when the projection mode is @b IFX_ORTHOGRAPHIC.  
	 *
	 *	@param fHeight	Input F32 specifying the vertical number of world units to fit
	 *					in the orthographic view from top to bottom.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The value specified was <= 0.
	 */
	IFXRESULT IFXAPI SetOrthoHeight(F32 fHeight);
	
	/**
	 *	Sets the horizontal number of world space units to appear in the view.  This only applies
	 *	when the projection mode is @b IFX_ORTHOGRAPHIC.  
	 *
	 *	@param fWidth	Input F32 specifying the horizontal number of world units to fit
	 *					in the orthographic view from right to left.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The value specified was <= 0.
	 */
	IFXRESULT IFXAPI SetOrthoWidth(F32 fWidth);
	
	/**
	 *	Sets the near clipping plane distance.
	 *
	 *	@param fNearClip	Input F32 specifying the near clipping plane distance in projection space.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The value specified was <= 0.
	 */
	IFXRESULT IFXAPI SetNearClip(F32 fNearClip);

	/**
	 *	Sets the far clipping plane distance.
	 *
	 *	@param fFarClip	Input F32 specifying the far clipping plane distance in projection space.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The value specified was <= 0.
	 */
	IFXRESULT IFXAPI SetFarClip(F32 fFarClip);
	
	/**
	 *	Sets the pixel aspect ratio.
	 *
	 *	@param fPixelAspect Input F32 specifying the ratio of monitor size to resolution ratio.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The value specified was = 0.
	 */
	IFXRESULT IFXAPI SetPixelAspect(F32 fPixelAspect);
	
	/**
	 *	Sets the projection clip rectangle.  This is the visible rectangle in camera space on the
	 *	near clipping plane that determines the visible area of the view.  This is available as
	 *	an alternative to define the view frustum to SetFieldOfView.
	 *
	 *	@param rcProjection	Input IFXF32Rect specifying the view frustum size at the near
	 *						clipping plane.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	Either @a rcProjection.width or 
	 *								@a rcProjection.height was <= 0.
	 */
	IFXRESULT IFXAPI SetProjectionRect(const IFXF32Rect& rcProjection);
	
	/**
	 *	Sets the drawing area of the following render commands relative to the window (or "sprite").
	 *	Location (0,0) is the upper left corner of the window.
	 *
	 *	@param rcViewport	Input IFXRect struct containing the x offset, y offset,
	 *						width and height of the viewport.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval IFX_E_INVALID_RANGE	The @a rcViewport.width or @a rcViewport.height 
	 *								is less <= 0.
	 */
	IFXRESULT IFXAPI SetViewport(const IFXRect& rcViewport);
	
	
	/**
	 *	Gets the current projection mode.
	 *
	 *	@return The current projection mode - an IFXenum value.
	 *
	 *	@retval	IFX_PERSPECTIVE	Normal camera style projection.
	 *	@retval	IFX_ORTHOGRAPHIC	Appears as if camera is infinitely far from the object.
	 *							The view frustum sides are parallel.

	 *	@see SetProjectionMode()
	 */
	IFXenum IFXAPI GetProjectionMode() const;
	
	/**
	 *	Gets whether or not the automatic horizontal view frustum size is enabled.
	 *
	 *	@return	Boolean value.
	 *
	 *	@retval	TRUE AutoHorizontal is enabled.
	 *	@retval	FALSE AutoHorizontal is disabled.
	 *	@see SetAutoHorizontal()
	 */
	BOOL IFXAPI GetAutoHorizontal() const;

	/**
	 *	Gets whether or not the manual view rect is used or not.
	 *
	 *	@return Boolean value.
	 *
	 *	@retval	TRUE	UseProjectionRect is enabled.
	 *	@retval	FALSE	UseProjectionRect is disabled.
	 *	@see SetUseProjectionRect()
	 */
	BOOL IFXAPI GetUseProjectionRect() const;

	/**
	 *	Gets the currently set horizontal field of view.
	 *
	 *	@return The perspective horizontal field of view.
	 *	@see SetFOVx()
	 */
	F32 IFXAPI GetFOVx() const;
	
	/**
	 *	Gets the currently set vertical field of view.
	 *
	 *	@return The perspective vertical field of view.
	 *	@see SetFOVy()
	 */
	F32 IFXAPI GetFOVy() const;
	
	/**
	 *	Gets the currently set vertical orthographic view volume height.
	 *
	 *	@return The orthographic vertical world unit height.
	 *	@see SetOrthoHeight()
	 */
	F32 IFXAPI GetOrthoHeight() const;
	
	/**
	 *	Gets the currently set horizontal orthographic view volume width.
	 *
	 *	@return The orthographic horizontal world unit width.
	 *	@see SetOrthoWidth()
	 */
	F32 IFXAPI GetOrthoWidth() const;
	
	/**
	 *	Gets the currently set near clip plane value.
	 *
	 *	@return The near clip plane distance.
	 *	@see SetNearClip()
	 */
	F32 IFXAPI GetNearClip() const;
	
	/**
	 *	Gets the currently set far clip plane value.
	 *
	 *	@return The far clip plane distance.
	 *	@see SetFarClip()
	 */
	F32 IFXAPI GetFarClip() const;
	
	/**
	 *	Gets the currently set pixel aspect ratio.
	 *
	 *	@return	The currently set pixel aspect ratio.
	 *	@see SetPixelAspect()
	 */
	F32 IFXAPI GetPixelAspect() const;

	/**
	 *	Gets the manual projection rect.
	 *
	 *	@return The manual view frustum clip rect.
	 *	@see SetProjectionRect()
	 */
	const IFXF32Rect& IFXAPI GetProjectionRect() const;

	/**
	 *	Gets the currently set viewport dimensions.
	 *
	 *	@return The viewport size and position.
	 *	@see SetViewport()
	 */
	const IFXRect& IFXAPI GetViewport() const;
	
	/**
	 *	Sets all member values to their default values.
	 *
	 *	Default values are:
	 *	- @b Projection @b Mode: IFX_PERSPECTIVE
	 *	- <b>Projection Rect</b>: IFXF32Rect(1.0,1.0,1.0,1.0)
	 *	- <b>Use Projection Rect</b>: FALSE
	 *	- <b>Auto Horizontal View</b>: TRUE
	 *	- @b Field @b Of @b View @b X: 40.0
	 *	- @b Field @b Of @b View @b Y: 30.0
	 *	- @b Orthographic @b Height: 200.0
	 *	- @b Orthographic @b Width: 333.0
	 *	- @b Near @b Clip: 1.0
	 *	- @b Far @b Clip: 1000.0
	 *	- @b Pixel @b Aspect: 1.0
	 *	- @b Viewport: IFXRect(0,0,1,1)
	 */
	void IFXAPI InitData();

protected:
	IFXenum		m_eProjectionMode;
	BOOL		m_bAutoHorizontal;
	BOOL		m_bUseProjectionRect;
	F32			m_fFOVx;
	F32			m_fFOVy;
	F32			m_fFOVyDeg;
	F32			m_fOrthoHeight;
	F32			m_fOrthoWidth;
	F32			m_fNearClip;
	F32			m_fFarClip;
	F32			m_fPixelAspect;
	IFXRect	m_rcViewport;
	IFXF32Rect	m_rcProjection;
};

//=======================================
// IFXRenderView Implementation
//=======================================
IFXINLINE IFXRenderView::IFXRenderView()
{
	InitData();
}

IFXINLINE IFXRenderView::~IFXRenderView()
{
	// EMPTY
}

IFXINLINE void IFXRenderView::InitData()
{
	m_eProjectionMode = IFX_PERSPECTIVE3;
	m_bAutoHorizontal = FALSE;
	m_bUseProjectionRect = FALSE;
	m_fFOVx = IFXTO_RAD * 40;
	m_fFOVy = IFXTO_RAD * 30;
	m_fFOVyDeg = 30;
	m_fOrthoHeight = 200;
	m_fOrthoWidth = 333;
	m_fNearClip = 1.0f;
	m_fFarClip = 1000.0f;
	m_rcViewport.Set(0,0,1,1);
	m_rcProjection.Set(1,1,1,1);
	m_fPixelAspect = 1.0f;	
}

IFXINLINE IFXRESULT IFXRenderView::SetProjectionMode(IFXenum eMode)
{
	IFXRESULT rc = IFX_OK;

	if(m_eProjectionMode != eMode)
	{
		switch(eMode)
		{
		case IFX_PERSPECTIVE3:
		case IFX_ORTHOGRAPHIC:
			m_eProjectionMode = eMode;
			break;
		case IFX_PERSPECTIVE2:
		case IFX_PERSPECTIVE1:
			rc = IFX_E_UNSUPPORTED;
			break;
		default:
			rc = IFX_E_INVALID_RANGE;
		}
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetAutoHorizontalView(BOOL bAutoHorizontal)
{
	m_bAutoHorizontal = bAutoHorizontal;

	return IFX_OK;
}

IFXINLINE IFXRESULT IFXRenderView::SetUseProjectionRect(BOOL bUseProjRect)
{
	m_bUseProjectionRect =  bUseProjRect;
	
	return IFX_OK;
}

IFXINLINE IFXRESULT IFXRenderView::SetFOVx(F32 fFOV)
{
	IFXRESULT rc = IFX_OK;

	if(fFOV <= 0 || fFOV > 180)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else if(m_fFOVx != fFOV)
	{
		fFOV = IFXTO_RAD * fFOV;
		m_fFOVx = fFOV;
	}
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetFOVy(F32 fFOV)
{
	IFXRESULT rc = IFX_OK;

	
	if(fFOV <= 0 || fFOV > 180)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else if(m_fFOVyDeg != fFOV)
	{
		m_fFOVyDeg = fFOV;
		m_fFOVy = fFOV * IFXTO_RAD;
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetOrthoHeight(F32 fHeight)
{
	IFXRESULT rc = IFX_OK;

	if(fHeight <= 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else if(m_fOrthoHeight != fHeight)
	{
		m_fOrthoHeight = fHeight;
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetOrthoWidth(F32 fWidth)
{
	IFXRESULT rc = IFX_OK;
	
	if(fWidth <= 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else if(m_fOrthoWidth != fWidth)
	{
		m_fOrthoWidth = fWidth;
	}
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetNearClip(F32 fNearClip)
{
	IFXRESULT rc = IFX_OK;
	
	if(fNearClip <= 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else if(m_fNearClip != fNearClip)
	{
		m_fNearClip = fNearClip;
	}
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetFarClip(F32 fFarClip)
{
	IFXRESULT rc = IFX_OK;
	
	if(fFarClip <= 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else 
	{
		m_fFarClip = fFarClip;
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetPixelAspect(F32 fPixelAspect)
{
	IFXRESULT rc = IFX_OK;
	
	if(fPixelAspect == 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else 
	{
		m_fPixelAspect = fPixelAspect;
	}
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetProjectionRect(const IFXF32Rect& rcProjection)
{
	IFXRESULT rc = IFX_OK;

	if(rcProjection.m_X <=0 || rcProjection.m_Y <= 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_rcProjection = rcProjection;
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderView::SetViewport(const IFXRect& rcViewport)
{
	IFXRESULT rc = IFX_OK;

	if(rcViewport.m_Width <= 0 || rcViewport.m_Height <= 0)
	{
		rc = IFX_E_INVALID_RANGE;
	}
	else
	{
		m_rcViewport = rcViewport;
	}

	return rc;
}

IFXINLINE IFXenum IFXRenderView::GetProjectionMode() const
{
	return m_eProjectionMode;
}

IFXINLINE BOOL IFXRenderView::GetAutoHorizontal() const
{
	return m_bAutoHorizontal;
}

IFXINLINE BOOL IFXRenderView::GetUseProjectionRect() const
{
	return m_bUseProjectionRect;
}

IFXINLINE F32 IFXRenderView::GetFOVx() const
{
	return IFXTO_DEG * m_fFOVx;
}

IFXINLINE F32 IFXRenderView::GetFOVy() const
{
	return m_fFOVyDeg;
}

IFXINLINE F32 IFXRenderView::GetOrthoHeight() const
{
	return m_fOrthoHeight;
}

IFXINLINE F32 IFXRenderView::GetOrthoWidth() const
{
	return m_fOrthoWidth;
}

IFXINLINE F32 IFXRenderView::GetNearClip() const
{
	return m_fNearClip;
}

IFXINLINE F32 IFXRenderView::GetFarClip() const
{
	return m_fFarClip;
}

IFXINLINE F32 IFXRenderView::GetPixelAspect() const
{
	return m_fPixelAspect;
}

IFXINLINE const IFXF32Rect& IFXRenderView::GetProjectionRect() const
{
	return m_rcProjection;
}

IFXINLINE const IFXRect& IFXRenderView::GetViewport() const
{
	return m_rcViewport;
}


#endif
