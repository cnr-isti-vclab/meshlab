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

#ifndef RFX_STATE_H_
#define RFX_STATE_H_

#include <GL/glew.h>

class RfxState
{
public:
	enum StateType {
		RFX_RENDERSTATE, RFX_SAMPLERSTATE
	};

	enum TextureStateRfx {
		GL_TextureWrapS = 1, GL_TextureWrapT, GL_TextureWrapR,
		GL_TextureMinify, GL_TextureMagnify, GL_TextureBorderColor,
		GL_TextureMaxAnisotropyEXT, GL_TextureLODBias = 12
	};

	enum TextureWrapRfx {
		GL_CLAMP_RFX = 1, GL_CLAMP_TO_EDGE_RFX, GL_REPEAT_RFX,
		GL_CLAMP_TO_BORDER_RFX, GL_MIRRORED_REPEAT_RFX
	};

	enum TextureFilterRfx {
		GL_NEAREST_RFX, GL_LINEAR_RFX, GL_NEAREST_MIPMAP_NEAREST_RFX,
		GL_NEAREST_MIPMAP_LINEAR_RFX, GL_LINEAR_MIPMAP_NEAREST_RFX,
		GL_LINEAR_MIPMAP_LINEAR_RFX
	};

	RfxState() {}
	RfxState(StateType type, int state = -1, int value = -1);
	virtual ~RfxState() {}

	void SetState(int _state) { state = _state; }
	void SetValue(float _val) { value = _val; }
	void SetEnvironment(GLint);

private:
	void SetTextureEnvironment(GLint);
	void SetGLEnvironment() { /* STUB */ };
	GLint GLWrapMode();
	GLint GLFilterMode();

	int state;
	float value;
	StateType type;
};

#endif /* RFX_STATE_H_ */
