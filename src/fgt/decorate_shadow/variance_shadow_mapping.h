/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#ifndef VARIANCE_SHADOW_MAPPING_H
#define VARIANCE_SHADOW_MAPPING_H

#include "shadow_mapping.h"


class VarianceShadowMapping : public ShadowMapping
{

public:
    VarianceShadowMapping();
    ~VarianceShadowMapping();

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

protected:

    /**
      * Sets up the needed resources(FBO and textures) to apply the shader.
      * @return false if something went wrong, true otherwise.
      */
    bool setup();

    GLuint _depth;

    GLuint _depthShaderProgram;
    GLuint _depthVert, _depthFrag;
};


#endif // VARIANCE_SHADOW_MAPPING_H
