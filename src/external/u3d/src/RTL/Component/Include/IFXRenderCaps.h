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
// IFXRenderCaps.h
#ifndef IFX_RENDERCAPS_H
#define IFX_RENDERCAPS_H

#include <memory.h>

/**
 *	The IFXRenderCaps object contains a set of flags and values that
 *	determine the capabilities of a rendering device.  These can
 *	be used to make determinations on optimal rendering
 *	algorithms.  All data members beginning with m_b are one bit values.
 *	The 1 (TRUE) value specifies that the capability is present, the 0 (FALSE) value
 *	means that the capability is not present.
 */
class IFXRenderCaps
{
public:

	/**
	 *	Constructor, defaults all values to 0.
	 */
	IFXRenderCaps() {memset(this, 0, sizeof(IFXRenderCaps));}

	/**
	 *	Empty Destructor.
	 */
	~IFXRenderCaps() {}

	// Boolean Capabilities
	U32 m_b16BitColor : 1;	///< Device can render to 16 bit surfaces
	U32 m_b32BitColor : 1;	///< Device can render to 32 bit surfaces
	U32 m_b16BitDepth : 1;	///< Device can use a 16 bit depth buffer
	U32 m_b24BitDepth : 1;	///< Device can use a 24 bit depth buffer
	U32 m_b8BitStencil : 1;	///< Device can use an 8 bit stencil buffer
	U32 m_bCubeTextures : 1; ///< Device supports using cube textures
	U32 m_bTex8888 : 1;	///< Device supports textures with rgba8888 format
	U32 m_bTex8880 : 1;	///< Device supports textures with rgba8880 format
	U32 m_bTex5551 : 1;	///< Device supports textures with rgba5551 format
	U32 m_bTex5650 : 1;	///< Device supports textures with rgba5650 format
	U32 m_bTex5550 : 1;	///< Device supports textures with rgba5550 format
	U32 m_bTex4444 : 1;	///< Device supports textures with rgba4444 format
	U32 m_bAA2X : 1;	///< Device supports 2X anti-aliasing
	U32 m_bAA3X : 1;	///< Device supports 3X anti-aliasing
	U32 m_bAA4X : 1;	///< Device supports 4X anti-aliasing
	U32 m_bAA4XSW : 1;	///< Device supports 4X anti-aliasing through software emulation

	// Integer Value Capabilities
	U32 m_uNumHWTexUnits;	///< Maximum number of textures used in a single rendering pass
	U32 m_uMaxTextureWidth; ///< Maximum texture width supported
	U32 m_uMaxTextureHeight;///< Maximum texture height supported
};

#endif
