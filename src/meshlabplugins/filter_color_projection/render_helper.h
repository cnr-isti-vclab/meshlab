/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
#ifndef RENDERHELPER_H
#define RENDERHELPER_H

#include <QString>
#include <QImage>
#include <QGLFramebufferObject>

#include <wrap/gl/shot.h>
#include <wrap/callback.h>

class QGLFramebufferObject;

class RenderHelper {

 public:

  GLuint vbo, nbo, cbo, ibo;  // vertex buffer object (vertices, normals, colors, indices)

  GLint programs[6];
  
  enum RenderingMode {FLAT=0, NORMAL=1, COLOR=2};
  RenderingMode rendmode;

  //buffers for rendered images 
  unsigned char *render; 
  unsigned char *color; 
  unsigned char *normal; 
  floatbuffer   *depth; 

  // min and max for normalization purposes (e.g. weighting)
  float mindepth;
  float maxdepth;

  RenderHelper();
  ~RenderHelper();

  // init
  int initializeGL(vcg::CallBackPos *cb);
  int initializeMeshBuffers(MeshModel *mesh, vcg::CallBackPos *cb);

  // draw & readback
  void renderScene(vcg::Shotf &view, MeshModel *mesh, RenderingMode mode, float camNear, float camFar);

 private:
  
  GLuint createShaderFromFiles(QString basename); // converted into shader/basename.vert .frag
  GLuint createShaders(const char *vert, const char *frag);

};

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

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
    //else
    //  std::cout << " OK!" << std::endl;
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
    //  std::cout << " OK!" << std::endl;
  }

} /* end namespace */

#endif
