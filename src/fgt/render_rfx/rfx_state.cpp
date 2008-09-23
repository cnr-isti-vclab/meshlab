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

// static members initialization

// the empty strings are "padding" for compatibility with RM enum
const char *RfxState::RenderStatesStrings[] = {
	"GL_CurrentColor", "GL_SecondaryColor", "", "GL_ClearColor",
	"GL_ClearDepth", "GL_ShadeModel", "GL_FrontFace", "GL_CullMode",
	"GL_EdgeFlag",
	"GL_DepthNear", "GL_DepthFar", "", "GL_FogColor", "GL_FogDensity", "GL_FogStart",
	"GL_FogEnd", "GL_FogMode", "GL_PointSize", "GL_PointMin", "GL_PointMax",
	"GL_PointSmooth", "GL_LineWidth", "GL_LineSmooth", "GL_PolyFrontMode",
	"GL_PolyBackMode", "GL_PolySmooth", "GL_PolyOffsetFactor",
	"GL_PolyOffsetUnits", "GL_PolyOffsetPoint", "GL_PolyOffsetLine",
	"GL_PolyOffsetFill",
	"GL_AlphaEnable", "GL_AlphaFunction", "GL_AlphaReference",
	"GL_DepthEnable", "GL_DepthFunction", "GL_BlendEnable", "GL_BlendColor",
	"GL_BlendSourceRGB", "GL_BlendDestRGB", "GL_BlendSourceAlpha",
	"GL_BlendDestAlpha", "GL_BlendEquation", "GL_WriteMaskColor", "GL_WriteMaskDepth",
	"GL_VertexProgramTwoSideARB", "GL_StencilEnable", "GL_StencilFunction",
	"GL_StencilValueMask", "GL_StencilReference", "GL_StencilFail",
	"GL_StencilPassDepthFail", "GL_StencilPassDepthPass", "GL_WriteMaskStencil",
	"GL_ClearStencil"
};

const char *RfxState::RenderFunctionStrings[] = {
	"NEVER", "LESS", "EQUAL", "LESS_EQUAL", "GREATER", "NOT_EQUAL",
	"GREATER_EQUAL", "ALWAYS"
};

const char *RfxState::RenderColorStrings[] = {
	"ZERO", "ONE", "SRC_COLOR", "INV_SRC_COLOR", "DEST_COLOR",
	"INV_DEST_COLOR", "SRC_ALPHA", "INV_SRC_ALPHA", "DEST_ALPHA",
	"INV_DEST_ALPHA", "CONST_COLOR", "INV_CONST_COLOR", "CONST_ALPHA",
	"INV_CONST_ALPHA", "SRC_ALPHA_SATURATE"
};

const char *RfxState::TextureStatesStrings[] = {
	"GL_TEXTURE_WRAP_S", "GL_TEXTURE_WRAP_T", "GL_TEXTURE_WRAP_R",
	"GL_TEXTURE_MIN_FILTER", "GL_TEXTURE_MAG_FILTER",
	"GL_TEXTURE_BORDER_COLOR", "GL_TEXTURE_MAX_ANISOTROPY_EXT",
	"GL_TEXTURE_LOD_BIAS"
};

const char *RfxState::TextureWrapStrings[] = {
	"GL_CLAMP", "GL_CLAMP_TO_EDGE", "GL_REPEAT", "GL_CLAMP_TO_BORDER",
	"GL_MIRRORED_REPEAT"
};

// the empty string in 3rd position is for compatibility with RM enum
const char *RfxState::TextureFilterStrings[] = {
	"GL_NEAREST", "GL_LINEAR", "", "GL_NEAREST_MIPMAP_NEAREST",
	"GL_NEAREST_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_NEAREST",
	"GL_LINEAR_MIPMAP_LINEAR"
};


RfxState::RfxState(StateType _type, int _state, long _value)
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
		glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, DecodeColor(value));
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
	case GL_CurrentColor: {
		GLfloat *res = DecodeColor(value);
		glColor3f(res[0], res[1], res[2]);
		delete res;
	}

	case GL_SecondaryColor: {
		GLfloat *res = DecodeColor(value);
		glSecondaryColor3f(res[0], res[1], res[2]);
		delete res;
		break;
	}

	case GL_ClearColor: {
		GLfloat *res = DecodeColor(value);
		glClearColor(res[0], res[1], res[2], res[3]);
		delete res;
		break;
	}

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

	case GL_FogColor: {
		glFogfv(GL_FOG_COLOR, DecodeColor(value));
		break;
	}

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
		glPointParameterf(GL_POINT_SIZE_MIN, value);
		break;

	case GL_PointMax:
		glPointParameterf(GL_POINT_SIZE_MAX_ARB, value);
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

	case GL_BlendColor: {
		GLfloat *res = DecodeColor(value);
		glBlendColor(res[0], res[1], res[2], res[3]);
		delete res;
		break;
	}

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

GLfloat* RfxState::DecodeColor(long val)
{
	// decodes a color stored in a 32bit integer
	// RM seems to "pack" an RGBA color this way:
	// R: (0, 255) * 2^0  +
	// G: (0, 255) * 2^8  +
	// B: (0, 255) * 2^16 +
	// A: (-1, -128) U (0, 127) * 2^24

	enum Offsets { _R = 1, _G = 256, _B = 65536, _A = 16777216 };
	GLfloat *color = new GLfloat[4];

	// Alpha needs special care due to its range.
	// remove Alpha component from value and remap in range [0, 255]
	if (val > 0) {
		color[3] = (val % _A == 0)? (short)(val / _A) : 0;
	} else {
		color[3] = 0.0f;
		long negVal = val;
		while (negVal < 0) {
			color[3] -= 1.0f;
			negVal += _A;
		}
	}
	val -= (long)(_A * color[3]);
	color[3] += ((color[3] < 0)? 256.0f : 0.0f);

	// R, G and B components are easier to parse
	color[2] = (val > _B)?                           (short)(val / _B) : 0;
	color[1] = ((val -= (int)(_B * color[2])) > _G)? (short)(val / _G) : 0;
	color[0] = ((val -= (int)(_G * color[1])) > _R)? (short)(val)      : 0;

	// now normalize in [0, 1] range
	for (int i = 0; i < 4; ++i)
		color[i] /= 255.0f;

	return color;
}

inline void RfxState::GLEnableDisable(GLint GLmode)
{
	if (value)
		glEnable(GLmode);
	else
		glDisable(GLmode);
}

/***
 * conversion functions from enum to strings for gui use
 */
QString RfxState::GetTextureValue()
{
	switch (state) {
	case GL_TextureWrapS:
	case GL_TextureWrapT:
	case GL_TextureWrapR:
		return TextureWrapStrings[value - 1];

	case GL_TextureMinify:
	case GL_TextureMagnify:
		return TextureFilterStrings[value];

	case GL_TextureBorderColor: {
		return ColorToString(DecodeColor(value));
	}

	case GL_TextureMaxAnisotropyEXT:
	case GL_TextureLODBias:
		return QString().setNum(value);

	default:
		return "";
	}
}

QString RfxState::GetRenderState()
{
	// enum values are not contiguous, so play with offsets
	// when looking up in strings table

	if (state < 10)
		return RenderStatesStrings[state - 1];
	else if (state > 13 && state < 36)
		return RenderStatesStrings[9 + (state - 14)];
	else if (state > 40)
		return RenderStatesStrings[31 + (state - 41)];
	else
		return "";
}

QString RfxState::GetRenderValue()
{
	switch (state) {
	case GL_CurrentColor:
	case GL_SecondaryColor:
	case GL_ClearColor:
	case GL_FogColor:
	case GL_BlendColor:
		return ColorToString(DecodeColor(value));

	case GL_ClearDepth:
	case GL_DepthNear:
	case GL_DepthFar:
	case GL_FogDensity:
	case GL_FogStart:
	case GL_FogEnd:
	case GL_PointSize:
	case GL_PointMin:
	case GL_PointMax:
	case GL_LineWidth:
	case GL_PolyOffsetFactor:
	case GL_PolyOffsetUnits:
	case GL_AlphaReference:
	case GL_StencilValueMask:
	case GL_StencilReference:
	case GL_ClearStencil:
	case GL_WriteMaskStencil:
		return QString().setNum(value);

	case GL_EdgeFlag:
	case GL_PointSmooth:
	case GL_LineSmooth:
	case GL_PolySmooth:
	case GL_PolyOffsetPoint:
	case GL_PolyOffsetLine:
	case GL_PolyOffsetFill:
	case GL_AlphaEnable:
	case GL_DepthEnable:
	case GL_BlendEnable:
	case GL_WriteMaskDepth:
	case GL_VertexProgramTwoSideARB:
	case GL_StencilEnable:
		return (value == 1)? "TRUE" : "FALSE";

	case GL_ShadeModel:
		return (value == 1)? "FLAT_SHADE" : "SMOOTH_SHADE";

	case GL_FrontFace:
		return (value == 1)? "CCW" : "CW";

	case GL_CullMode:
		switch (value) {
		case 1:
			return "NONE";
		case 2:
			return "FRONT";
		case 3:
			return "BACK";
		case 4:
			return "FRONT_AND_BACK";
		default:
			return "";
		}

	case GL_FogMode:
		switch (value) {
		case 1:
			return "NONE";
		case 2:
			return "LINEAR";
		case 3:
			return "EXP";
		case 4:
			return "EXP2";
		default:
			return "";
		}

	case GL_PolyFrontMode:
	case GL_PolyBackMode:
		return (value == 1)? "POINTS" : ((value == 2)? "LINES" : "FILL");

	case GL_AlphaFunction:
	case GL_DepthFunction:
	case GL_StencilFunction:
		return RenderFunctionStrings[value - 1];

	case GL_BlendSourceRGB:
	case GL_BlendDestRGB:
	case GL_BlendSourceAlpha:
	case GL_BlendDestAlpha:
		return RenderColorStrings[value - 1];

	case GL_BlendEquation:
		switch (value) {
		case 1:
			return "ADD";
		case 2:
			return "SUBTRACT";
		case 3:
			return "REV_SUBTRACT";
		case 4:
			return "MIN";
		case 5:
			return "MAX";
		default:
			return "";
		}

	case GL_StencilFail:
	case GL_StencilPassDepthFail:
	case GL_StencilPassDepthPass:
		switch (value) {
		case GL_ZERO:
			return "ZERO";
		case GL_KEEP:
			return "KEEP";
		case GL_REPLACE:
			return "REPLACE";
		case GL_INCR:
			return "INCR";
		case GL_DECR:
			return "DECR";
		case GL_INVERT:
			return "INVERT";
		case GL_INCR_WRAP:
			return "INCR_WRAP";
		case GL_DECR_WRAP:
			return "DECR_WRAP";
		default:
			return "";
		}

	case GL_WriteMaskColor: {
		GLint val = (GLint)value;
		QString retCol;
		if (val > 8) {
			retCol.append("ALPHA ");
			val -= 8;
		}
		if (val > 4) {
			retCol.append("BLUE ");
			val -= 4;
		}
		if (val > 2) {
			retCol.append("GREEN ");
			val -= 2;
		}
		if (val > 1) {
			retCol.append("RED");
			val -= 1;
		}

		return retCol;
	}

	default:
		return "";
	}
}
