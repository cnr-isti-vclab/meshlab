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

#ifndef SSAO_H
#define SSAO_H

#include <cassert>
#include <QString>
#include <QImage>
#include <QFile>
#include <GL/glew.h>
#include <meshlab/interfaces.h>
#include <meshlab/meshmodel.h>
#include <meshlab/glarea.h>
#include <decorate_shader.h>

#define NOISE_WIDTH 512
#define NOISE_HEIGHT 512

class SSAO : public DecorateShader
{

public:
        SSAO();
        ~SSAO();

        bool init();
        void runShader(MeshModel&, GLArea*);

private:
        bool compileAndLink();
        bool setup();
        void bind();
        void unbind();
        bool loadNoiseTxt();
        void printNoiseTxt();

        GLuint _normalMap;
        GLuint _ssao;
        GLuint _noise;
        GLuint _blurH;
        GLuint _blurV;
        GLuint _depthMap;

        GLuint _depth;

        GLuint _fbo;
        GLuint _fbo2;
        GLuint _normalMapShaderProgram;
        GLuint _normalMapVert, _normalMapFrag;
        GLuint _depthMapShaderProgram;
        GLuint _depthMapVert, _depthMapFrag;
        GLuint _ssaoShaderProgram;
        GLuint _ssaoVert, _ssaoFrag;
        GLuint _blurShaderProgram;
        GLuint _blurVert, _blurFrag;
};

#endif // SSAO_H
