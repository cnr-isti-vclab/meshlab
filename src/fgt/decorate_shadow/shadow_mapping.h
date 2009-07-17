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

#ifndef SHADOW_MAPPING_H_
#define SHADOW_MAPPING_H_


#include <cassert>
#include <QString>
#include <QImage>
#include <QFile>
#include <GL/glew.h>
#include <meshlab/interfaces.h>
#include <meshlab/meshmodel.h>
#include <meshlab/glarea.h>
#include <decorate_shader.h>

class ShadowMapping : public DecorateShader
{

public:
        ShadowMapping();
		~ShadowMapping();

        bool init();
        void runShader(MeshModel&, GLArea*);

private:
        bool compileAndLink();
        bool setup();
        void bind();
        void unbind();

        GLuint _fbo;
        GLuint _objectShaderProgram;
        GLuint _objectVert, _objectFrag ;
};

#endif /* SHADOW_MAPPING_H_ */
