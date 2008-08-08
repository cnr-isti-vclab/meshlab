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
		// do nothing, unsupported state
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

inline GLint RfxState::GLFunctionMode()
{
	switch ((FunctionRfx)value) {
	case NEVER_RFX:
		return GL_NEVER;
	case LESS_RFX:
		return GL_LESS;
	case EQUAL_RFX:
		return GL_EQUAL;
	case LESS_EQUAL_RFX:
		return GL_LEQUAL;
	case GREATER_RFX:
		return GL_GREATER;
	case NOT_EQUAL_RFX:
		return GL_NOTEQUAL;
	case GREATER_EQUAL_RFX:
		return GL_GEQUAL;
	case ALWAYS_RFX:
		return GL_ALWAYS;
	default:
		return GL_ALWAYS;
	}
}

inline GLint RfxState::GLColorMode()
{
	switch ((ColorRfx)value) {
	case ZERO:
		return GL_ZERO;
	case ONE:
		return GL_ONE;
	case SRC_COLOR:
		return GL_SRC_COLOR;
	case INV_SRC_COLOR:
		return GL_ONE_MINUS_SRC_COLOR;
	case DEST_COLOR:
		return GL_DST_COLOR;
	case INV_DEST_COLOR:
		return GL_ONE_MINUS_DST_COLOR;
	case SRC_ALPHA:
		return GL_SRC_ALPHA;
	case INV_SRC_ALPHA:
		return GL_ONE_MINUS_SRC_ALPHA;
	case DEST_ALPHA:
		return GL_DST_ALPHA;
	case INV_DEST_ALPHA:
		return GL_ONE_MINUS_DST_ALPHA;
	case CONST_COLOR:
		return GL_CONSTANT_COLOR;
	case INV_CONST_COLOR:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	case CONST_ALPHA:
		return GL_CONSTANT_ALPHA;
	case INV_CONST_ALPHA:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	case SRC_ALPHA_SATURATE:
		return GL_SRC_ALPHA_SATURATE;
	default:
		return GL_ZERO;
	}
}

void RfxState::SetGLEnvironment()
{
	switch (state) {
	case GL_CurrentColor:
	case GL_SecondaryColor:
	case GL_ClearColor:
		//TODO
		break;

	case GL_ClearDepth:
		glClearDepth(value);
		break;

	case GL_ShadeModel:
		glShadeModel((value == 1)? GL_FLAT : GL_SMOOTH);
		break;

	case GL_FrontFace:
		glFrontFace((value == 1)? GL_CCW : GL_CW);
		break;

	case GL_CullMode:
		if (value == 1) {
			glDisable(GL_CULL_FACE);
		} else {
			glEnable(GL_CULL_FACE);
			glCullFace((value == 2)? GL_FRONT :
			            ((value == 3)? GL_BACK : GL_FRONT_AND_BACK));
		}
		break;

	case GL_EdgeFlag:
		glEdgeFlag(value);
		break;

	case GL_DepthNear:
	case GL_DepthFar: {
		GLfloat range[2];
		glGetFloatv(GL_DEPTH_RANGE, range);
		if (state == GL_DepthNear)
			glDepthRange(value, range[1]);
		else
			glDepthRange(range[0], value);
		break;
	}

	case GL_FogColor:
		// TODO
		break;

	case GL_FogDensity:
		glFogi(GL_FOG_DENSITY, value);
		break;

	case GL_FogStart:
		glFogi(GL_FOG_START, value);
		break;

	case GL_FogEnd:
		glFogi(GL_FOG_END, value);
		break;

	case GL_FogMode:
		glEnable(GL_FOG);
		switch ((GLint)value) {
		case 1: // NONE
			glDisable(GL_FOG);
			break;
		case 2: // LINEAR
			glFogi(GL_FOG_MODE, GL_LINEAR);
			break;
		case 3: // EXP
			glFogi(GL_FOG_MODE, GL_EXP);
			break;
		case 4: // EXP2
			glFogi(GL_FOG_MODE, GL_EXP2);
			break;
		default: // UNKNOWN
			break;
		}
		break;

	case GL_PointSize:
		glPointSize(value);
		break;

	case GL_PointMin:
		glPointParameterfv(GL_POINT_SIZE_MIN, &value);
		break;

	case GL_PointMax:
		glPointParameterfv(GL_POINT_SIZE_MAX_ARB, &value);
		break;

	case GL_PointSmooth:
		GLEnableDisable(GL_POINT_SMOOTH);
		break;

	case GL_LineWidth:
		glLineWidth(value);
		break;

	case GL_LineSmooth:
		GLEnableDisable(GL_LINE_SMOOTH);
		break;

	case GL_PolyFrontMode:
		glPolygonMode(GL_FRONT, (value == 1)? GL_POINT :
		                         ((value == 2)? GL_LINE : GL_FILL));
		break;

	case GL_PolyBackMode:
		glPolygonMode(GL_BACK, (value == 1)? GL_POINT :
                                ((value == 2)? GL_LINE : GL_FILL));
		break;

	case GL_PolySmooth:
		GLEnableDisable(GL_POLYGON_SMOOTH);
		break;

	case GL_PolyOffsetFactor: {
		GLfloat units;
		glGetFloatv(GL_POLYGON_OFFSET_UNITS, &units);
		glPolygonOffset(value, units);
		break;
	}

	case GL_PolyOffsetUnits: {
		GLfloat factor;
		glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &factor);
		glPolygonOffset(factor, value);
		break;
	}

	case GL_PolyOffsetPoint:
		GLEnableDisable(GL_POLYGON_OFFSET_POINT);
		break;

	case GL_PolyOffsetLine:
		GLEnableDisable(GL_POLYGON_OFFSET_LINE);
		break;

	case GL_PolyOffsetFill:
		GLEnableDisable(GL_POLYGON_OFFSET_FILL);
		break;

	case GL_AlphaEnable:
		GLEnableDisable(GL_ALPHA_TEST);
		break;

	case GL_AlphaFunction: {
		GLfloat ref;
		glGetFloatv(GL_ALPHA_TEST_REF, &ref);
		glAlphaFunc(GLFunctionMode(), ref);
		break;
	}

	case GL_AlphaReference: {
		GLint func;
		glGetIntegerv(GL_ALPHA_TEST_FUNC, &func);
		glAlphaFunc(func, value);
		break;
	}

	case GL_DepthEnable:
		GLEnableDisable(GL_DEPTH_TEST);
		break;

	case GL_DepthFunction:
		glDepthFunc(GLFunctionMode());
		break;

	case GL_BlendEnable:
		GLEnableDisable(GL_BLEND);
		break;

	case GL_BlendColor:
		// TODO
		break;

	case GL_BlendSourceRGB:
	case GL_BlendDestRGB:
	case GL_BlendSourceAlpha:
	case GL_BlendDestAlpha: {
		GLint srcdst[4];
		glGetIntegerv(GL_BLEND_SRC_RGB, &srcdst[0]);
		glGetIntegerv(GL_BLEND_DST_RGB, &srcdst[1]);
		glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcdst[2]);
		glGetIntegerv(GL_BLEND_DST_ALPHA, &srcdst[3]);

		glBlendFuncSeparate(
			((state == GL_BlendSourceRGB)?   GLColorMode() : srcdst[0]),
			((state == GL_BlendDestRGB)?     GLColorMode() : srcdst[1]),
			((state == GL_BlendSourceAlpha)? GLColorMode() : srcdst[2]),
			((state == GL_BlendDestAlpha)?   GLColorMode() : srcdst[3]));
		break;
	}

	case GL_BlendEquation:
		switch ((GLint)value) {
		case 1: // ADD
			glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
			break;
		case 2: // SUBTRACT
			glBlendEquationSeparate(GL_FUNC_SUBTRACT, GL_FUNC_SUBTRACT);
			break;
		case 3: // REV_SUBTRACT
			glBlendEquationSeparate(GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT);
			break;
		case 4: // MIN
			glBlendEquationSeparate(GL_MIN, GL_MIN);
			break;
		case 5: // MAX
			glBlendEquationSeparate(GL_MAX, GL_MAX);
			break;
		}
		break;

	case GL_WriteMaskColor: {
		GLint val = (GLint)value;
		GLboolean par[4];
		par[3] = (val > 8)? GL_TRUE : GL_FALSE;
		par[2] = (val -= (8 * par[3]) > 4)? GL_TRUE : GL_FALSE;
		par[1] = (val -= (4 * par[2]) > 2)? GL_TRUE : GL_FALSE;
		par[0] = (val -= (2 * par[1]) > 1)? GL_TRUE : GL_FALSE;

		glColorMask(par[0], par[1], par[2], par[3]);
		break;
	}

	case GL_WriteMaskDepth:
		glDepthMask(value);
		break;

	case GL_VertexProgramTwoSideARB:
		GLEnableDisable(GL_VERTEX_PROGRAM_TWO_SIDE_ARB);
		break;

	case GL_StencilEnable:
		GLEnableDisable(GL_STENCIL_TEST);
		break;

	case GL_StencilFunction:
	case GL_StencilValueMask:
	case GL_StencilReference: {
		GLint StFun[3];
		glGetIntegerv(GL_STENCIL_FUNC, &StFun[0]);
		glGetIntegerv(GL_STENCIL_VALUE_MASK, &StFun[1]);
		glGetIntegerv(GL_STENCIL_REF, &StFun[2]);

		glStencilFunc(
			(state == GL_StencilFunction)? GLFunctionMode() : StFun[0],
			(state == GL_StencilValueMask)? value : StFun[1],
			(state == GL_StencilReference)? value : StFun[2]);
		break;
	}

	case GL_StencilFail:
	case GL_StencilPassDepthFail:
	case GL_StencilPassDepthPass: {
		GLint StOp[3];
		glGetIntegerv(GL_STENCIL_FAIL, &StOp[0]);
		glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &StOp[1]);
		glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &StOp[2]);

		// thanks god at least these values are equivalent to OpenGL ones,
		// no mapping needed.
		glStencilOp(
			(state == GL_StencilFail)?          value : StOp[0],
			(state == GL_StencilPassDepthFail)? value : StOp[1],
			(state == GL_StencilPassDepthPass)? value : StOp[2]);
		break;
	}

	case GL_WriteMaskStencil:
		glStencilMask(value);
		break;

	case GL_ClearStencil:
		glClearStencil(value);
		break;

	default:
		// do nothing, unsupported state
		break;
	}
}

inline void RfxState::GLEnableDisable(GLint GLmode)
{
	if (value)
		glEnable(GLmode);
	else
		glDisable(GLmode);
}
