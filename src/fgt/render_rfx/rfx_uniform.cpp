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

#include "rfx_uniform.h"

RfxUniform::RfxUniform(const QString &_name, const QString &_type)
{
	identifier = _name;
	type = GetUniformType(_type);
	textureLoaded = false;
	textureNotFound = false;
}

RfxUniform::~RfxUniform()
{
}

void RfxUniform::UpdateUniformLocation(GLuint programId)
{
	location = glGetUniformLocation(programId, identifier.toLocal8Bit().data());
}

// static member initialization
const char *RfxUniform::UniformTypeString[] = {
	"int", "float", "bool",
	"vec2", "vec3", "vec4",
	"ivec2", "ivec3", "ivec4",
	"bvec2", "bvec3", "bvec4",
	"mat2", "mat3", "mat4",
	"sampler1D", "sampler2D", "sampler3D", "samplerCube",
	"sampler1DShadow", "sampler2DShadow"
};

RfxUniform::UniformType RfxUniform::GetUniformType(const QString& stringType)
{
	int i;
	for (i = 0; i < TOTAL_TYPES; ++i) {
		if (stringType == UniformTypeString[i])
			break;
	}

	return (UniformType)i;
}

void RfxUniform::SetValue(float _value[16])
{
	switch (type) {
	case INT:
	case BOOL:
	case FLOAT:
		value = _value[0];
		break;

	case VEC2:
	case IVEC2:
	case BVEC2:
		memcpy(vec2, _value, sizeof(float) * 2);
		break;

	case VEC3:
	case IVEC3:
	case BVEC3:
		memcpy(vec3, _value, sizeof(float) * 3);
		break;

	case VEC4:
	case IVEC4:
	case BVEC4:
		memcpy(vec4, _value, sizeof(float) * 4);
		break;

	case MAT2:
		memcpy(mat2, _value, sizeof(float) * 4);
		break;

	case MAT3:
		memcpy(mat3, _value, sizeof(float) * 9);
		break;

	case MAT4:
		memcpy(mat4, _value, sizeof(float) * 16);
		break;

	default:
		break;

	}
}

void RfxUniform::SetValue(const QString &texFileName)
{
	textureFile = texFileName;
	if (!QFileInfo(textureFile).exists()) {
		qDebug("WARNING: texture file (%s) not found.", textureFile.toStdString().c_str());
		textureNotFound = true;
	}
}

void RfxUniform::LoadTexture(QGLContext *ctx)
{
	switch (type) {
	case SAMPLER2D:
		textureTarget = GL_TEXTURE_2D;
		break;
	case SAMPLER3D:
		textureTarget = GL_TEXTURE_3D;
		break;
	case SAMPLERCUBE:
		textureTarget = GL_TEXTURE_CUBE_MAP;
		break;
	default:
		return;
	}

	if (textureNotFound)
		return;

	QImage Tex;
	if (Tex.load(textureFile) && ctx != NULL) {
		if (textureFile.endsWith(".dds"))
			textureId = ctx->bindTexture(textureFile);
		else
			textureId = ctx->bindTexture(Tex, textureTarget);

		// set texture states
		foreach (RfxState *state, textureStates)
			state->SetEnvironment(textureTarget);

		if (texUnit < GL_MAX_TEXTURE_COORDS)
			textureLoaded = true;
	}
}

void RfxUniform::PassToShader()
{
	switch (type) {
	case INT:
	case BOOL:
		glUniform1i(location, value);
		break;

	case FLOAT:
		glUniform1f(location, value);
		break;

	case IVEC2:
	case BVEC2:
		glUniform2i(location, vec2[0], vec2[1]);
		break;

	case VEC2:
		glUniform2f(location, vec2[0], vec2[1]);
		break;

	case IVEC3:
	case BVEC3:
		glUniform3i(location, vec3[0], vec3[1], vec3[2]);
		break;

	case VEC3:
		glUniform3f(location, vec3[0], vec3[1], vec3[2]);
		break;

	case IVEC4:
	case BVEC4:
		glUniform4i(location, vec4[0], vec4[1], vec4[2], vec4[3]);
		break;

	case VEC4:
		glUniform4f(location, vec4[0], vec4[1], vec4[2], vec4[3]);
		break;

	case MAT2:
		glUniformMatrix2fv(location, 1, GL_FALSE, mat2);
		break;

	case MAT3:
		glUniformMatrix3fv(location, 1, GL_FALSE, mat3);
		break;

	case MAT4:
		glUniformMatrix4fv(location, 1, GL_FALSE, mat4);
		break;

	case SAMPLER2D:
	case SAMPLER3D:
	case SAMPLERCUBE:
		if (textureLoaded) {
			glActiveTexture(GL_TEXTURE0 + texUnit);
			glEnable(textureTarget);
			glBindTexture(textureTarget, textureId);

			glUniform1i(location, texUnit);
		}
		break;

	default:
		qDebug("don't know what to do with %s", UniformTypeString[type]);
	}
}
