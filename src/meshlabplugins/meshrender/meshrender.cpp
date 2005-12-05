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
/****************************************************************************
  History
$Log$
Revision 1.5  2005/12/05 18:11:28  ggangemi
Added toon shader example

Revision 1.4  2005/12/05 16:52:57  ggangemi
new interfaces

Revision 1.3  2005/12/03 22:50:06  cignoni
Added copyright info

****************************************************************************/
#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "meshrender.h"

using namespace vcg;

void MeshShaderRenderPlugin::Init(QAction *a, MeshModel &m, GLArea *gla) 
{
	if(a->text() == tr("Toon Shader"))
	{
		GLenum err = glewInit();
		if (GLEW_OK == err)
		{
			if (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program)
			{
				v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
				f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
				char *fs = "uniform vec3 DiffuseColor;uniform vec3 PhongColor;uniform float Edge;uniform float Phong;varying vec3 Normal;void main (void){vec3 color = DiffuseColor;float f = dot(vec3(0,0,1),Normal);if (abs(f) < Edge)color = vec3(0);if (f > Phong)color = PhongColor;gl_FragColor = vec4(color, 1);}";
				char *vs = "varying vec3 Normal;void main(void){Normal = normalize(gl_NormalMatrix * gl_Normal);gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;}";
				const char * vv = vs;
				const char * ff = fs;
				glShaderSourceARB(v, 1, &vv,NULL);
				glShaderSourceARB(f, 1, &ff,NULL);
				
				glCompileShaderARB(v);
				glCompileShaderARB(f);
				p = glCreateProgramObjectARB();
				glAttachObjectARB(p,v);
				glAttachObjectARB(p,f);
				glLinkProgramARB(p);

				edge =				 glGetUniformLocationARB(p, "Edge");
				phong =				 glGetUniformLocationARB(p, "Phong");
				diffuseColor = glGetUniformLocationARB(p, "DiffuseColor");
				phongColor =	 glGetUniformLocationARB(p, "PhongColor");
				supported = true;
			}
		}	
    return;
	}
}

void MeshShaderRenderPlugin::Render(QAction *a, MeshModel &m, RenderMode &rm, GLArea *gla) 
{
	if(a->text() == tr("Toon Shader"))
	{
		glUseProgramObjectARB(p);
		glUniform1fARB(edge, 0.64f);
		glUniform1fARB(phong, 0.9540001f);
		glUniform3fARB(diffuseColor, 0.0f, 0.25f, 1.0f);
		glUniform3fARB(phongColor, 0.75f, 0.75f, 1.0f);
    return;
	}
}

Q_EXPORT_PLUGIN(MeshShaderRenderPlugin)
