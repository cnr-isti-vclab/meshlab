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

#include "decorate_shader.h"

class SSAO : public DecorateShader
{

public:
    SSAO(float);
    ~SSAO();

    /**
      * Performs init commands.
      * Initializes GLEW and performs setup, thes compiles and links the shaders.
      * If something went wrong return false, otherwise true.
      * @return false if something went wrong, true otherwise.
      */
    bool init();


    /**
      * Applies the decoration running the shaders.
      * @param m the mesh model.
      * @param gla GLArea reference.
      */
    void runShader(MeshModel&, GLArea*);

private:

    /**
      * Sets up the needed resources(FBO and textures) to apply the shader.
      * @return false if something went wrong, true otherwise.
      */
    bool setup();

    /**
      * Loads the noise texture used in the SSAO pass.
      * @return false if something went wrong, true otherwise.
      */
    bool loadNoiseTxt();

    /**
      * Prints the noise texture.
      * $$$$$$ FOR DEBUGGING PURPOSES $$$$$$
      */
    void printNoiseTxt();

    float _radius;

    int noiseWidth;
    int noiseHeight;

    GLuint _color1;
    GLuint _color2;
    GLuint _noise;
    GLuint _depthMap;

    GLuint _depth;

    GLuint _fbo2;
    GLuint _normalMapShaderProgram;
    GLuint _normalMapVert, _normalMapFrag;
    GLuint _ssaoShaderProgram;
    GLuint _ssaoVert, _ssaoFrag;
    GLuint _blurShaderProgram;
    GLuint _blurVert, _blurFrag;
};

#endif // SSAO_H
