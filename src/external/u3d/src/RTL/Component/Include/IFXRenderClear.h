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
// IFXRenderClear.h
// DO NOT INCLUDE THIS DIRECTLY!!!!!
// This is included with IFXRender.h
#ifndef IFX_RENDER_CLEAR_H
#define IFX_RENDER_CLEAR_H

// {BDF04286-3FC3-4fcd-8B4D-2C45CA4DE81C}
IFXDEFINE_GUID(IID_IFXRenderClear,
0xbdf04286, 0x3fc3, 0x4fcd, 0x8b, 0x4d, 0x2c, 0x45, 0xca, 0x4d, 0xe8, 0x1c);

//==============================
// IFXRenderClear
//==============================
/**
 *	Interface to an object containing parameters passed to an IFXRenderContext::Clear() call.
 *
 *	This interface encapsulates all necessary state settings relating to clearing the 
 *	back/depth/stencil buffers associated with an IFXRender object.
 *
 *	@see IFXRenderContext::Clear().
 */
class IFXRenderClear
{
public:	
	//=============================================
	// IFXRenderClear Constructor/Destructor
	//=============================================
	IFXRenderClear();
	virtual ~IFXRenderClear();

	//==============================
	// IFXRenderClear methods
	//==============================
	/**
	 *	Sets whether or not the color (back) buffer is cleared when IFXRenderContext::Clear() is called.
	 *
	 *	@param	bClear	Input BOOL specifying whether or not the back buffer is cleared.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetColorCleared(BOOL bClear);

	/**
	 *	Sets whether or not the depth buffer is cleared when IFXRenderContext::Clear() is called.
	 *
	 *	@param	bClear	Input BOOL specifying whether or not the depth buffer is cleared.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetDepthCleared(BOOL bClear);

	/**
	 *	Sets whether or not the stencil buffer is cleared when IFXRenderContext::Clear() is called.
	 *
	 *	@param	bClear	Input BOOL specifying whether or not the stencil buffer is cleared.
	 *
	 *	@return An IFXRESULT error code - always @b IFX_OK.
	 */
	IFXRESULT IFXAPI SetStencilCleared(BOOL bClear);

	/**
	 *	Sets the color value that the color buffer is cleared to.
	 *
	 *	@param vClearValue	Input IFXVector3 specifying the color that the back buffer should
	 *						be cleared to.  Each color channel is clamped between 0.0 - 1.0
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetColorValue(const IFXVector3& vClearValue);

	/**
	 *	Sets the depth value that the depth buffer is cleared to.
	 *
	 *	@param fClearValue	Input F32 specifying the depth value that the depth buffer is 
	 *						cleared to.  This value is clamped between 0.0 - 1.0 (where 1.0
	 *						is farthest from the camera).
	 *
	 *	@return An IFXRESULT error code - always @b IFX_OK.
	 */
	IFXRESULT IFXAPI SetDepthValue(F32 fClearValue);
	
	/**
	 *	Sets the stencil value that the stencil buffer is cleared to.
	 *
	 *	@param uClearValue	Input U32 specifying the stencil value that the stencil buffer
	 *						is cleared to.  Values are clamped between 0 - 255.
	 *
	 *	@return An IFXRESULT code.
	 *
	 *	@retval	IFX_OK	No error.
	 */
	IFXRESULT IFXAPI SetStencilValue(U32 uClearValue);
		
	/**
	 *	Gets whether or not the back (color) buffer is cleared.
	 *
	 *	@return Boolean value.
	 *
	 *	@retval	TRUE	The color buffer is to be cleared.
	 *	@retval FALSE	The color buffer is not to be cleared.
	 *	@see SetColorCleared().
	 */
	BOOL IFXAPI GetColorCleared() const;

	/**
	 *	Gets whether or not the depth buffer is cleared.
	 *
	 *	@return Boolean value.
	 *
	 *	@retval	TRUE	The depth buffer is to be cleared.
	 *	@retval FALSE	The depth buffer is not to be cleared.
	 *	@see SetDepthCleared().
	 */
	BOOL IFXAPI GetDepthCleared() const;
	
	/**
	 *	Gets whether or not the stencil buffer is cleared.
	 *
	 *	@return Boolean value.
	 *
	 *	@retval	TRUE	The stencil buffer is to be cleared.
	 *	@retval FALSE	The stencil buffer is not to be cleared.
	 *	@see SetStencilCleared().
	 */
	BOOL IFXAPI GetStencilCleared() const;
	
	/**
	 *	Gets the current clear value for the color buffer.
	 *
	 *	@return The color that the color buffer will be cleared to.
	 *	@see SetColorValue()
	 */
	const IFXVector3& IFXAPI GetColorValue() const;

	/**
	 *	Gets the current clear value for the depth buffer.
	 *
	 *	@return The depth value that the depth buffer will be cleared to.
	 *	@see SetDepthValue()
	 */
	F32 IFXAPI GetDepthValue() const;

	/**
	 *	Gets the current clear value for the stencil buffer.
	 *
	 *	@return The stencil value that the stencil buffer will be cleared to.
	 *	@see SetStencilValue()
	 */
	U32 IFXAPI GetStencilValue() const;
	
	/**
	 *	Initializes all internal state to default values.  Defaults are:
	 *
	 *	- <b>Color buffer cleared</b>: TRUE
	 *	- <b>Depth buffer cleared</b>: TRUE
	 *	- <b>Stencil buffer cleared</b>: TRUE
	 *	- <b>Color clear color</b>: IFXVector3(0.0, 0.0, 0.0)
	 *	- <b>Depth clear value</b>: 1.0
	 *	- <b>Stencil clear value</b>: 0.0
	 */
	void IFXAPI InitData();

protected:
	IFXVector3	m_vColor;
	F32			m_fDepth;
	U32			m_uStencil;
	IFXenum		m_eClearBuffers;
};

//=======================================
// IFXRenderClear Implementation
//=======================================
IFXINLINE IFXRenderClear::IFXRenderClear()
{
	InitData();
}

IFXINLINE IFXRenderClear::~IFXRenderClear()
{
	// EMPTY
}

IFXINLINE void IFXRenderClear::InitData()
{
	m_eClearBuffers = IFX_COLOR_BUFFER | IFX_DEPTH_BUFFER | IFX_STENCIL_BUFFER;
	m_vColor.Set(0,0,0);
	m_fDepth = 1.0f;
	m_uStencil = 0;
}

IFXINLINE IFXRESULT IFXRenderClear::SetColorCleared(BOOL bClear)
{
	IFXRESULT rc = IFX_OK;

	if(bClear)
	{
		m_eClearBuffers |= IFX_COLOR_BUFFER;
	}
	else
	{
		m_eClearBuffers &= ~IFX_COLOR_BUFFER;
	}

	return rc;
}

IFXINLINE IFXRESULT IFXRenderClear::SetDepthCleared(BOOL bClear)
{
	IFXRESULT rc = IFX_OK;
	
	if(bClear)
	{
		m_eClearBuffers |= IFX_DEPTH_BUFFER;
	}
	else
	{
		m_eClearBuffers &= ~IFX_DEPTH_BUFFER;
	}
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderClear::SetStencilCleared(BOOL bClear)
{
	IFXRESULT rc = IFX_OK;
	
	if(bClear)
	{
		m_eClearBuffers |= IFX_STENCIL_BUFFER;
	}
	else
	{
		m_eClearBuffers &= ~IFX_STENCIL_BUFFER;
	}
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderClear::SetColorValue(const IFXVector3& vClearValue)
{
	IFXRESULT rc = IFX_OK;
	
	m_vColor = vClearValue;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderClear::SetDepthValue(F32 fClearValue)
{
	IFXRESULT rc = IFX_OK;
	
	m_fDepth = fClearValue;
	
	return rc;
}

IFXINLINE IFXRESULT IFXRenderClear::SetStencilValue(U32 uClearValue)
{
	IFXRESULT rc = IFX_OK;
	
	m_uStencil = uClearValue;
	
	return rc;
}

IFXINLINE BOOL IFXRenderClear::GetColorCleared() const
{
	return 0 != (m_eClearBuffers & IFX_COLOR_BUFFER);
}

IFXINLINE BOOL IFXRenderClear::GetDepthCleared() const
{
	return 0 != (m_eClearBuffers & IFX_DEPTH_BUFFER);
}

IFXINLINE BOOL IFXRenderClear::GetStencilCleared() const
{
	return 0 != (m_eClearBuffers & IFX_STENCIL_BUFFER);
}

IFXINLINE const IFXVector3& IFXRenderClear::GetColorValue() const
{
	return m_vColor;
}

IFXINLINE F32 IFXRenderClear::GetDepthValue() const
{
	return m_fDepth;
}

IFXINLINE U32 IFXRenderClear::GetStencilValue() const
{
	return m_uStencil;
}


#endif // IFX_RENDER_CLEAR_H

// END OF FILE


