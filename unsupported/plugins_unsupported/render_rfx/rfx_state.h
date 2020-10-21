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

#include <QString>
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

	enum RenderStateRfx {
		GL_CurrentColor = 1, GL_SecondaryColor,
		GL_ClearColor = 4, GL_ClearDepth, GL_ShadeModel, GL_FrontFace, GL_CullMode,
		GL_EdgeFlag,
		GL_DepthNear = 14, GL_DepthFar, GL_FogColor = 17, GL_FogDensity, GL_FogStart,
		GL_FogEnd, GL_FogMode, GL_PointSize, GL_PointMin, GL_PointMax, GL_PointSmooth,
		GL_LineWidth, GL_LineSmooth, GL_PolyFrontMode, GL_PolyBackMode, GL_PolySmooth,
		GL_PolyOffsetFactor, GL_PolyOffsetUnits, GL_PolyOffsetPoint, GL_PolyOffsetLine,
		GL_PolyOffsetFill,
		GL_AlphaEnable = 41, GL_AlphaFunction, GL_AlphaReference, GL_DepthEnable,
		GL_DepthFunction, GL_BlendEnable, GL_BlendColor, GL_BlendSourceRGB,
		GL_BlendDestRGB, GL_BlendSourceAlpha, GL_BlendDestAlpha, GL_BlendEquation,
		GL_WriteMaskColor, GL_WriteMaskDepth, GL_VertexProgramTwoSideARB,
		GL_StencilEnable, GL_StencilFunction, GL_StencilValueMask, GL_StencilReference,
		GL_StencilFail, GL_StencilPassDepthFail, GL_StencilPassDepthPass,
		GL_WriteMaskStencil, GL_ClearStencil
	};

	enum TextureWrapRfx {
		GL_CLAMP_RFX = 1, GL_CLAMP_TO_EDGE_RFX, GL_REPEAT_RFX,
		GL_CLAMP_TO_BORDER_RFX, GL_MIRRORED_REPEAT_RFX
	};

	enum TextureFilterRfx {
		GL_NEAREST_RFX, GL_LINEAR_RFX, GL_NEAREST_MIPMAP_NEAREST_RFX = 3,
		GL_NEAREST_MIPMAP_LINEAR_RFX, GL_LINEAR_MIPMAP_NEAREST_RFX,
		GL_LINEAR_MIPMAP_LINEAR_RFX
	};

	enum FunctionRfx {
		NEVER_RFX = 1, LESS_RFX, EQUAL_RFX, LESS_EQUAL_RFX, GREATER_RFX,
		NOT_EQUAL_RFX, GREATER_EQUAL_RFX, ALWAYS_RFX
	};

	enum ColorRfx {
		ZERO = 1, ONE, SRC_COLOR, INV_SRC_COLOR, DEST_COLOR, INV_DEST_COLOR,
		SRC_ALPHA, INV_SRC_ALPHA, DEST_ALPHA, INV_DEST_ALPHA, CONST_COLOR,
		INV_CONST_COLOR, CONST_ALPHA, INV_CONST_ALPHA, SRC_ALPHA_SATURATE
	};

	RfxState() {}
	RfxState(StateType type, int state = -1, long value = -1);
	virtual ~RfxState() {}

	void SetState(int _state) { state = _state; }
	void SetValue(long _val) { value = _val; }
	void SetEnvironment(GLint texTarget = 0);

	QString GetTextureState() { return TextureStatesStrings[state - 1]; }
	QString GetTextureValue();
	QString GetRenderState();
	QString GetRenderValue();

	static GLfloat* DecodeColor(long);

private:
	void SetTextureEnvironment(GLint);
	void SetGLEnvironment();
	GLint GLWrapMode();
	GLint GLFilterMode();
	GLint GLFunctionMode();
	GLint GLColorMode();
	QString ColorToString(float *thecol)
	{ return "("
		+ QString().setNum(thecol[0]) + ", "
		+ QString().setNum(thecol[1]) + ", "
		+ QString().setNum(thecol[2]) + ", "
		+ QString().setNum(thecol[3]) + ")";
	}
	void GLEnableDisable(GLint);

	int state;
	long value;
	StateType type;
	static const char *TextureStatesStrings[];
	static const char *RenderStatesStrings[];
	static const char *TextureWrapStrings[];
	static const char *TextureFilterStrings[];
	static const char *RenderFunctionStrings[];
	static const char *RenderColorStrings[];
};

#endif /* RFX_STATE_H_ */
