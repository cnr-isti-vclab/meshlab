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
	value = NULL;
	identifier = _name;
	type = GetUniformType(_type);
	textureLoaded = false;
	textureNotFound = false;
	textureRendered = false;

	_isRmColorVariable = false;

	minVal = 0.0;
	maxVal = 0.0;
}

RfxUniform::~RfxUniform()
{
	if (value) {
		if (type == INT || type == BOOL || type == FLOAT)
			delete value;
		else
			delete[] value;
	}

	// render targets are deleted by rfx_shader
	if (textureLoaded && !textureRendered)
		glDeleteTextures(1, &textureId);
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
		value = new float;
		*value = _value[0];
		break;

	case VEC2:
	case IVEC2:
	case BVEC2:
		value = new float[2];
		memcpy(value, _value, sizeof(float) * 2);
		break;

	case VEC3:
	case IVEC3:
	case BVEC3:
		value = new float[3];
		memcpy(value, _value, sizeof(float) * 3);
		break;

	case VEC4:
	case IVEC4:
	case BVEC4:
	case MAT2:
		value = new float[4];
		memcpy(value, _value, sizeof(float) * 4);
		break;

	case MAT3:
		value = new float[9];
		memcpy(value, _value, sizeof(float) * 9);
		break;

	case MAT4:
		value = new float[16];
		memcpy(value, _value, sizeof(float) * 16);
		break;

	default:
		break;

	}
}

void RfxUniform::SetValue(const QString &texFileName)
{
	textureFile = texFileName;
}

void RfxUniform::LoadTexture()
{
	if (textureRendered) {
		textureNotFound = false;
		textureLoaded = true;
		textureTarget = GL_TEXTURE_2D;

		return;
	}

	if (!QFileInfo(textureFile).exists()) {
		textureNotFound = true;
		return;
	} else {
		textureNotFound = false;
	}

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

	glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTexUnits);

	textureLoaded = (RfxTextureLoader::LoadTexture(textureFile,
	                                               textureStates,
	                                               &textureId)
	                 && texUnit < maxTexUnits);
}

void RfxUniform::PassToShader()
{
	switch (type) {
	case INT:
	case BOOL:
		glUniform1i(location, (GLint)*value);
		break;

	case FLOAT:
		glUniform1f(location, *value);
		break;

	case IVEC2:
	case BVEC2:
		glUniform2i(location, (GLint)value[0], (GLint)value[1]);
		break;

	case VEC2:
		glUniform2f(location, value[0], value[1]);
		break;

	case IVEC3:
	case BVEC3:
		glUniform3i(location, (GLint)value[0], (GLint)value[1], (GLint)value[2]);
		break;

	case VEC3:
		glUniform3f(location, value[0], value[1], value[2]);
		break;

	case IVEC4:
	case BVEC4:
		glUniform4i(location, (GLint)value[0], (GLint)value[1], (GLint)value[2], (GLint)value[3]);
		break;

	case VEC4:
		glUniform4f(location, value[0], value[1], value[2], value[3]);
		break;

	case MAT2:
		glUniformMatrix2fv(location, 1, GL_FALSE, value);
		break;

	case MAT3:
		glUniformMatrix3fv(location, 1, GL_FALSE, value);
		break;

	case MAT4:
		glUniformMatrix4fv(location, 1, GL_FALSE, value);
		break;

	case SAMPLER2D:
	case SAMPLER3D:
	case SAMPLERCUBE:
		if (textureLoaded) {

			if (textureRendered)
				textureId = rTarget->GetTexture();

			glActiveTexture(GL_TEXTURE0 + texUnit);
			glBindTexture(textureTarget, textureId);

			glUniform1i(location, texUnit);
		}
		break;

	default:
		qDebug("don't know what to do with %s", UniformTypeString[type]);
	}
}
