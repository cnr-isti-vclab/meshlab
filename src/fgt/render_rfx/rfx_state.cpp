/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include "rfx_state.h"

RfxState::RfxState(StateType _type, int _state, int _value)
	: state(_state), value(_value), type(_type)
{
}

void RfxState::SetEnvironment(GLint target)
{
	if (type == RFX_SAMPLERSTATE)
		SetTextureEnvironment(target);
	else
		SetGLEnvironment();
}

void RfxState::SetTextureEnvironment(GLint target)
{
	switch (state) {
	case GL_TextureWrapS:
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GLWrapMode());
		break;
	case GL_TextureWrapT:
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GLWrapMode());
		break;
	case GL_TextureWrapR:
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GLWrapMode());
		break;

	case GL_TextureMinify:
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GLFilterMode());
		break;
	case GL_TextureMagnify:
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GLFilterMode());
		break;

	case GL_TextureBorderColor:
		// TODO
		break;

	case GL_TextureMaxAnisotropyEXT:
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
		break;

	case GL_TextureLODBias:
		glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, value);
		break;

	default:
		// don't do anything. unsupported state
		break;
	}
}

inline GLint RfxState::GLWrapMode()
{
	switch ((TextureWrapRfx)value) {
	case GL_CLAMP_RFX:
		return GL_CLAMP;
	case GL_CLAMP_TO_EDGE_RFX:
		return GL_CLAMP_TO_EDGE;
	case GL_REPEAT_RFX:
		return GL_REPEAT;
	case GL_CLAMP_TO_BORDER_RFX:
		return GL_CLAMP_TO_BORDER;
	case GL_MIRRORED_REPEAT_RFX:
		return GL_MIRRORED_REPEAT;
	default:
		return GL_CLAMP;
	}
}

inline GLint RfxState::GLFilterMode()
{
	switch ((TextureFilterRfx)value) {
	case GL_NEAREST_RFX:
		return GL_NEAREST;
	case GL_LINEAR_RFX:
		return GL_LINEAR;
	case GL_NEAREST_MIPMAP_NEAREST_RFX:
		return GL_NEAREST_MIPMAP_NEAREST;
	case GL_NEAREST_MIPMAP_LINEAR_RFX:
		return GL_NEAREST_MIPMAP_LINEAR;
	case GL_LINEAR_MIPMAP_NEAREST_RFX:
		return GL_LINEAR_MIPMAP_NEAREST;
	case GL_LINEAR_MIPMAP_LINEAR_RFX:
		return GL_LINEAR_MIPMAP_LINEAR;
	default:
		return GL_NEAREST;
	}
}
