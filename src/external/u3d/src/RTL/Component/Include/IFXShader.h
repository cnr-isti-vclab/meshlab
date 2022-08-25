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
	@file	IFXShader.h
*/

#ifndef __IFXSHADER_H__
#define __IFXSHADER_H__

#include "IFXUnknown.h"

#include "IFXModifierDataPacket.h"
#include "IFXMarkerX.h"
#include "IFXRenderContext.h"
#include "IFXList.h"

// {C941D4E1-CF85-11d3-94BC-00A0C9A0FBAE}
IFXDEFINE_GUID(IID_IFXShader,
0xc941d4e1, 0xcf85, 0x11d3, 0x94, 0xbc, 0x0, 0xa0, 0xc9, 0xa0, 0xfb, 0xae);

class IFXLightSet;
class IFXFrustum;

/**
 *	The IFXShader object is responsible for rendering IFXRenderable elements.  This
 *	can be with normal 3D lighting and material properties, or with non-photorealistic
 *	techniques.  Renderable objects could include meshes (surfaces) and lines.
 */
class IFXShader : virtual public IFXMarkerX
{
public:
	enum IFXShaderOpacity
	{
		IFX_SHADER_OPACITY_FALSE,		///< Shader is translucent
		IFX_SHADER_OPACITY_TRUE,		///< Shader is completely opaque
		IFX_SHADER_OPACITY_DEPENDENT,	///< Shader opacity is dependent on vertex alpha values

		IFX_SHADER_OPACITY_FORCE_32BIT = 0xFFFFFFFF
	};

	/**
	 	Retrieve the opacity type of the objects rendered by this shader.
	 	This is used to do proper ordering of objects that need to be
	 	rendered.
	 
	 	@return	An IFXShaderOpacity value.
	 */
	virtual IFXShaderOpacity IFXAPI Opacity()=0;

	/**
		The IFXShadedElement object contains all data needed to render
		one renderable element.

		Example usage of IFXShadedElement

		@code
		IFXShadedElement* pSE1 = new IFXShadedElement();

		// Add an element
		pShader->ShadedElementList().Append(pSE1);

		// Process all elements
		IFXListContext context;
		pShader->ShadedElementList().ToHead(context);
		while ( IFXShadedElement* pSE = pShader->ShadedElementList().PostIncrement(context) )
			pSE->DoSomething();

		list.DeleteAll(void)
		@endcode
	 */
	struct IFXShadedElement
	{
		IFXModifierDataPacket* pDataPacket;	///< Data store for a particular model.
		U16                    u16Flags;	///< User defined flags
		/**
		 *	The renderable element index within the IFXRenderable object.
		 */
		U16                    u16DataElementIndex;
		/**
		 *	The element of the IFXRenderable object in the @a pDataPacket.
		 */
		U32                    uRenderableElementIndex;
		IFXMatrix4x4*          pfTransform;				///< Local to world transform.
		const IFXLightSet*     pLightSet;				///< Set of lights affecting the object.
		const IFXFrustum*      pFrustum;				///< View frustum of the camera
	};

	/**
	 *	Retrieve a list of IFXShadedElements.  This list is usually empty at the time
	 *	of the call.  The higher-level object will fill this list with IFXShadedElements
	 *	to be rendered with Render().
	 *
	 *	@return	A reference to an IFXList of IFXShadedElements.  This will
	 *			probably be an empty list at the time of the call.
	 */
	virtual IFXList<IFXShadedElement>& IFXAPI ShadedElementList()=0;

	/**
	 *	Render the IFXShadedElements that have been placed in the list returned by
	 *	ShadedElementList().  The value of @a uRenderPass is checked against the
	 *	current render pass flags before rendering.  See SetRenderPassFlags()
	 *	for more information.
	 *
	 *	@param	rRenderContext	Reference to an IFXRenderContext that will be used to
	 *							render the shaded elements.
	 *	@param	uRenderPass	Input U32 specifying the current render pass.  If this
	 *						value doesn't appear current render pass flags, no
	 *						rendering occurs.
	 *
	 *	@return	An IFXRESULT value.
	 *	@retval	IFX_OK	No error.
	 *	@retval	IFX_E_SUBSYSTEM_FAILURE	There was a catastrophic error in the rendering
	 *									system (could be lost device, out of VRAM, etc.)
	 */
	virtual IFXRESULT IFXAPI Render( IFXRenderContext& rRenderContext, U32 uRenderPass )=0;

	/**
	 *	Sets which render passes this shader participates in.  Each bit (1 << n)
	 *	in @a uFlags determines if the shader participates in renderpass n.
	 *
	 *	@param	uFlags	Input 32 bit flag field that determines which render
	 *					passes the shader will produce output for.
	 *
	 *	@return	The render pass flags (same as @a uFlags).
	 */
	virtual U32 IFXAPI SetRenderPassFlags(U32 uFlags)=0;

	/**
	 *	Get the render pass flags for this shader.
	 *	See SetRenderPassFlags for more information.
	 *
	 *	@return The current render pass flags.
	 */
	virtual U32 IFXAPI GetRenderPassFlags() const=0;
};

#endif
