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

#include "rfx_glpass.h"

RfxGLPass::~RfxGLPass()
{
	if (shaderLinked)
		glDeleteObjectARB(shaderProgram);

	foreach (RfxState *state, rfxStates)
		delete state;
	rfxStates.clear();

	foreach (RfxUniform *uniform, shaderUniforms)
		delete uniform;
	shaderUniforms.clear();
}

void RfxGLPass::SetShaderSource(const QString &source, bool isFragment)
{
	if (isFragment)
		frag = source;
	else
		vert = source;
}

void RfxGLPass::CompileAndLink()
{
	if (frag.isEmpty() || vert.isEmpty())
		return;

	GLubyte *ShaderSource;
	GLint ShaderLen;

	// compile vertex shader
	ShaderSource = (GLubyte *)new char[vert.length() + 1];
	memcpy(ShaderSource, vert.toLocal8Bit().data(), vert.length());
	ShaderLen = (GLint) vert.length();

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, (const GLchar **)&ShaderSource, &ShaderLen);
	glCompileShader(vertShader);

	delete[] ShaderSource;
	ShaderSource = (GLubyte *)new char[frag.length() + 1];
	memcpy(ShaderSource, frag.toLocal8Bit().data(), frag.length());
	ShaderLen = (GLint) frag.length();

	// compile fragment shader
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, (const GLchar **)&ShaderSource, &ShaderLen);
	glCompileShader(fragShader);

	delete[] ShaderSource;

	// attach shaders to program and link
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);

	// TODO: improve compile/link error handling - integration with qt dialog
	GLint res;
	glGetObjectParameterivARB(shaderProgram, GL_OBJECT_LINK_STATUS_ARB, &res);
	if (!res) {
		printInfoLog(shaderProgram);
	} else {
		// shader link successful. update uniform locations
		// and load textures
		shaderLinked = true;
		foreach (RfxUniform *uni, shaderUniforms) {
			uni->LoadTexture();
			uni->UpdateUniformLocation(shaderProgram);
		}
	}
}

// DEBUGGING for random shader compilation errors
// TODO: improve error handling
#include <cstdlib>
void RfxGLPass::printInfoLog(GLhandleARB obj)
{
	GLint infologLength = 0;
	GLsizei charsWritten = 0;
	char *infoLog;

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

	if (infologLength > 0) {
		infoLog = new char[infologLength];
		glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
			qDebug("%s\n",infoLog);
		delete[] infoLog;
	}
}

void RfxGLPass::Start()
{
	// set gl states
	foreach (RfxState *state, rfxStates)
		state->SetEnvironment();

	// if this pass has no glsl sources,
	// keep using glsl program of the previous pass
	if (!frag.isEmpty() && !vert.isEmpty()) {

		glUseProgram(shaderProgram);

		// pass uniforms
		foreach (RfxUniform *uni, shaderUniforms) {
			uni->PassToShader();
		}
	}
}

RfxUniform* RfxGLPass::getUniform(const QString& uniIdx)
{
	foreach (RfxUniform *u, shaderUniforms) {
		if (u->GetName() == uniIdx)
			return u;
	}

	return NULL;
}
