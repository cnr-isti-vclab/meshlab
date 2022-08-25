/****************************************************************************
* AutoReg Tool                                                      o o     *
* Automatic Registration of Images on Approximate Geometry        o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
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

#ifndef SHADERUTILS_H
#define SHADERUTILS_H

// Standard headers
#include <stdio.h>
#include <iostream>

#include <GL/glew.h>


namespace ShaderUtils
{

const char *importShaders(const char *filename)
{
	FILE *fp;
	char *text = NULL;

	fp = fopen(filename, "rt");

	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		size_t count = ftell(fp);
		fseek(fp, 0, 0);

		if (count > 0)
		{
			text = new char[count+1];
			count = fread(text, sizeof(char), count, fp);
			text[count] = '\0';
		}

		fclose(fp);
	}

	return text;
}

// Compile shader and provide verbose output
void compileShader(GLuint shader)
{
	static char shlog[2048];
	GLint status;
	int length;

	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetShaderInfoLog(shader, 2048, &length, shlog);
		std::cout << std::endl;
		std::cout << shlog << std::endl;
	}
//	else
//		std::cout << " OK!" << std::endl;
}

// Link shader program and provide verbose output
void linkShaderProgram(GLuint program)
{
	static char proglog[2048];
	GLint status;
	int length;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		glGetProgramInfoLog(program, 2048, &length, proglog);
		std::cout << std::endl;
		std::cout << proglog << std::endl;
	}
	//else
//		std::cout << " OK!" << std::endl;
}

} /* end namespace */

#endif /* SHADERUTILS_H */

