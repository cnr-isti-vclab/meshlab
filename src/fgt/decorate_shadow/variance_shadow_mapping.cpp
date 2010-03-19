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

#include "decorate_shader.h"
#include "variance_shadow_mapping.h"
#include <common/pluginmanager.h>

VarianceShadowMapping::VarianceShadowMapping():ShadowMapping()
{
    this->_depth = 0;

    this->_depthVert = 0;
    this->_depthFrag = 0;
    this->_depthShaderProgram = 0;
}

VarianceShadowMapping::~VarianceShadowMapping(){
    glDetachShader(this->_depthShaderProgram, this->_depthVert);
    glDetachShader(this->_depthShaderProgram, this->_depthFrag);

    glDeleteShader(this->_depthVert);
    glDeleteShader(this->_depthFrag);
    glDeleteProgram(this->_depthShaderProgram);

    glDeleteFramebuffersEXT(1, &(this->_depth));
}

bool VarianceShadowMapping::init()
{
    if(!this->initGlew() || !this->initSetup())
        return false;

    if(!compileAndLink(
            this->_depthShaderProgram,
            this->_depthVert,
            this->_depthFrag,
            PluginManager::getBaseDirPath().append(QString("/shaders/decorate_shadow/vsm/depthVSM"))) ||
       !compileAndLink(
            this->_shadowMappingProgram,
            this->_shadowMappingVert,
            this->_shadowMappingFrag,
            PluginManager::getBaseDirPath().append(QString("/shaders/decorate_shadow/vsm/objectVSM"))))
        return false;

    return true;
}

void VarianceShadowMapping::runShader(MeshModel& m, GLArea* gla){
    GLfloat g_mModelView[16];
    GLfloat g_mProjection[16];

    this->renderingFromLightSetup(m, gla);

    glMatrixMode(GL_PROJECTION);
        glGetFloatv(GL_PROJECTION_MATRIX, g_mProjection);
    glMatrixMode(GL_MODELVIEW);
        glGetFloatv(GL_MODELVIEW_MATRIX, g_mModelView);

    /***********************************************************/
    //SHADOW MAP Generation
    /***********************************************************/
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    this->bind();

    glUseProgram(this->_depthShaderProgram);
    RenderMode rm = gla->getCurrentRenderMode();
    m.Render(rm.drawMode, vcg::GLW::CMNone, vcg::GLW::TMNone);
    glDisable(GL_POLYGON_OFFSET_FILL);

    this->unbind();

    this->renderingFromLightUnsetup();

    /***********************************************************/
    //SHADOW MAP Generation finished
    /***********************************************************/

    /***********************************************************/
    //OBJECT PASS
    /***********************************************************/
    GLint depthFuncOld;
    glGetIntegerv(GL_DEPTH_FUNC, &depthFuncOld);
    glDepthFunc(GL_LEQUAL);
    vcg::Matrix44f mvpl = (vcg::Matrix44f(g_mProjection).transpose() * vcg::Matrix44f(g_mModelView).transpose()).transpose();
    glUseProgram(this->_shadowMappingProgram);

    GLuint matrixLoc = glGetUniformLocation(this->_shadowMappingProgram, "mvpl");
    glUniformMatrix4fv(matrixLoc, 1, 0, mvpl.V());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->_shadowMap);

    GLuint loc = glGetUniformLocation(this->_shadowMappingProgram, "shadowMap");
    glUniform1i(loc, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m.Render(rm.drawMode, rm.colorMode, vcg::GLW::TMNone);

    glDisable(GL_BLEND);
    glDepthFunc((GLenum)depthFuncOld);
    glUseProgram(0);
}

bool VarianceShadowMapping::setup()
{
        if (!GLEW_EXT_framebuffer_object) {
                qWarning("FBO not supported!");
                return false;
        }

        if (_initOk)
                return true;

        //genero il frame buffer object
        glGenFramebuffersEXT(1, &_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);

        //Generates texture color for variance shadow map
        this->genColorTextureEXT(this->_shadowMap, GL_COLOR_ATTACHMENT0_EXT);

        //Generates render buffer for depth attachment
        this->genDepthRenderBufferEXT(this->_depth);

        //checks for errors
        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return _initOk;
}
