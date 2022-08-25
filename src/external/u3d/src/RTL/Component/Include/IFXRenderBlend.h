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
// IFXRenderBlend.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRender.h
#ifndef IFX_RENDER_BLEND_H
#define IFX_RENDER_BLEND_H

//==============================
// IFXRenderBlend
//==============================
/**
 *	This is the interface to an object that provides frame buffer blend and
 *	alpha test parameters to an IFXRenderContext interface.  This encapsulates all 
 *	state settings for alpha blending and alpha test.
 */
class IFXRenderBlend
{
public:
	//=============================================
	// IFXRenderBlend Constructor/Destructor
	//=============================================
	IFXRenderBlend();
	virtual ~IFXRenderBlend();

	//==============================
	// IFXRenderBlend methods
	//==============================
	/**
	 *	Sets the reference value used in comparisons when alpha test is enabled.
	 *	Alpha test can be enabled by calling: IFXRenderContext::Enable(IFX_FB_ALPHA_TEST).
	 *
	 *	@param uRef	Input F32 specifying the reference value for alpha test
	 *				comparisons.  The input is clamped between 0 - 1.0.
	 *
	 *	@return An IFXRESULT error code.
	 *
	 *	@retval	IFX_OK	No error.
	 *
	 *	@see	IFXRenderContext::Enable(), IFXRenderContext::Disable()
	 */
	IFXRESULT IFXAPI SetReference(F32 fRef);
	
	/**
	 *	Sets the comparison function used for alpha test.
	 *	Alpha test can be enabled by calling: IFXRenderContext::Enable(IFX_FB_ALPHA_TEST).
	 *
	 *	@param eFunc	Input IFXenum specifying the comparison function for alpha testing.
	 *					The valid options are:
	 *					- @b  IFX_NEVER:  The test never passes.  No pixels are drawn.
	 *					- @b  IFX_LESS:  The rendered alpha value must be less than the reference value.
	 *					- @b  IFX_GREATER:  The rendered alpha value must be greater than the ref. value.
	 *					- @b  IFX_EQUAL:  The rendered alpha value must be equal to the reference value.
	 *					- @b  IFX_NOT_EQUAL:  The rendered alpha value must not be equal to the ref. value.
	 *					- @b  IFX_LEQUAL:  The rendered alpha value must be less than or equal to
	 *						the reference value.
	 *					- @b  IFX_GEQUAL:  The rendered alpha value must be greater than or equal to
	 *						the reference value.
	 *					- @b  IFX_ALWAYS:  The test always passes.  No pixels are rejected.
	 *
	 *	@return	An IFXRESULT error code. 
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified comparison function is invalid.
	 *
	 *	@see	IFXRenderContext::Enable(), IFXRenderContext::Disable()
	 */
	IFXRESULT IFXAPI SetTestFunc(IFXenum eFunc);
	
	/**
	 *	Sets the blend function between rendered pixels and the existing
	 *	framebuffer.
	 *	Alpha blending can be enabled by calling: IFXRenderContext::Enable(IFX_FB_BLEND).
	 *
	 *	@param eFunc	Input IFXenum specifying the desired blending 
	 *					function.  Valid values are:
	 *					- @b IFX_FB_ADD: Add the RGB components into the framebuffer
	 *					- @b IFX_FB_MULTIPLY: Multiply the RGB components into the framebuffer
	 *					- @b IFX_FB_ALPHA_BLEND: Linear blend the RGB components into the
	 *						framebuffer based on the rendered alpha value.
	 *					- @b IFX_FB_INV_ALPHA_BLEND: Linear blend the RGB components into
	 *						framebuffer based on the inverse (1.0 - a) of the rendered alpha.
	 *					- @b IFX_FB_ADD_BLEND: Multiply the alpha values into the source color 
	 *						values, then add to the frame buffer.
	 *					- @b IFX_FB_INVISIBLE: Use the existing framebuffer values.  This is 
	 *						useful to render into the Depth buffer only.
	 *
	 *	@return	An IFXRESULT error code. 
	 *
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_INVALID_RANGE	The specified comparison function is invalid.
	 *
	 *	@see	IFXRenderContext::Enable(), IFXRenderContext::Disable()
	 */
	IFXRESULT IFXAPI SetBlendFunc(IFXenum eFunc);

	/**
	 *	Gets the currently set reference value for alpha test comparisons.
	 *
	 *	@return The currently set reference value.
	 *	@see SetReference().
	 */
	F32 IFXAPI GetReference() const;
	
	/**
	 *	Gets the currently set function used to blend rendered pixels into the framebuffer.
	 *	
	 *	@return The currently set alpha blend function.
	 *	@see SetBlendFunc().
	 */
	IFXenum IFXAPI GetBlendFunc() const;
	
	/**
	 *	Gets the currently set function used for the alpha test comparison.
	 *
	 *	@return The currently set alpha test comparison function.
	 *	@see SetTestFunc().
	 */
	IFXenum IFXAPI GetTestFunc() const;

	
	/**
	 *	Initializes data to default values.  Default values are:
	 *
	 *	- <b>Alpha Test Referene</b>: 0
	 *	- <b>Alpha Test Function</b>: IFX_ALWAYS
	 *	- <b>Alpha Blend Function</b>: IFX_FB_ALPHA_BLEND
	 */
	void IFXAPI InitData();

protected:
	F32		m_fRef;
	IFXenum	m_eBlendFunc;
	IFXenum	m_eTestFunc;	
};

//=======================================
// IFXRenderBlend Implementation
//=======================================
IFXINLINE IFXRenderBlend::IFXRenderBlend()
{
	InitData();
}

IFXINLINE IFXRenderBlend::~IFXRenderBlend()
{
	// EMPTY
}

IFXINLINE void IFXRenderBlend::InitData()
{
	m_fRef = 0;
	m_eBlendFunc = IFX_FB_ALPHA_BLEND;
	m_eTestFunc = IFX_ALWAYS;
}

IFXINLINE IFXRESULT IFXRenderBlend::SetReference(F32 fRef)
{
	IFXRESULT rc = IFX_OK;
	
	if(fRef > 1)
		fRef = 1;
	if(fRef < 0)
		fRef = 0;
	
	m_fRef = fRef;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderBlend::SetBlendFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;
	
	switch(eFunc)
	{
	case IFX_FB_ADD:
	case IFX_FB_MULT:
	case IFX_FB_ALPHA_BLEND:
	case IFX_FB_INV_ALPHA_BLEND:
	case IFX_FB_ADD_BLEND:
	case IFX_FB_INVISIBLE:
		m_eBlendFunc = eFunc;
		break;
	default:
//		rc = IFX_E_INVALID_RANGE;
        m_eBlendFunc = IFX_ALWAYS;
		break;
	}
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderBlend::SetTestFunc(IFXenum eFunc)
{
	IFXRESULT rc = IFX_OK;
	
	switch(eFunc)
	{
	case IFX_NEVER:
	case IFX_LESS:
	case IFX_GREATER:
	case IFX_EQUAL:
	case IFX_NOT_EQUAL:
	case IFX_LEQUAL:
	case IFX_GEQUAL:
	case IFX_ALWAYS:
		m_eTestFunc = eFunc;
		break;
	default:
//		rc = IFX_E_INVALID_RANGE;
        m_eTestFunc = IFX_ALWAYS;
		break;
	}
	
	return rc;
}

IFXINLINE F32 IFXRenderBlend::GetReference() const
{
	return m_fRef;
}

IFXINLINE IFXenum IFXRenderBlend::GetBlendFunc() const
{
	return m_eBlendFunc;
}

IFXINLINE IFXenum IFXRenderBlend::GetTestFunc() const
{
	return m_eTestFunc;
}

#endif // IFX_RENDER_BLEND_H

// END OF FILE

