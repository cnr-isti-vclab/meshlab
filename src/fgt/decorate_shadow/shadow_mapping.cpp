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

#include "decorate_shader.h"
#include "shadow_mapping.h"
#include <common/pluginmanager.h>

ShadowMapping::ShadowMapping():DecorateShader()
{
    this->_shadowMappingVert = 0;
    this->_shadowMappingFrag = 0;
    this->_shadowMappingProgram = 0;
    this->_fbo = 0;
}

ShadowMapping::~ShadowMapping(){
    glDetachShader(this->_shadowMappingProgram, this->_shadowMappingVert);
    glDetachShader(this->_shadowMappingProgram, this->_shadowMappingFrag);

    glDeleteShader(this->_shadowMappingVert);
    glDeleteShader(this->_shadowMappingFrag);
    glDeleteProgram(this->_shadowMappingProgram);

    glDeleteTextures(1, &(this->_shadowMap));

    glDeleteFramebuffersEXT(1, &_fbo);
}

bool ShadowMapping::init()
{
    if(!this->initGlew() || !this->initSetup())
        return false;

    return compileAndLink(
            this->_shadowMappingProgram,
            this->_shadowMappingVert,
            this->_shadowMappingFrag,
            PluginManager::getBaseDirPath().append(QString("/shaders/decorate_shadow/sm/object")));
}

void ShadowMapping::renderingFromLightSetup(MeshModel& m, GLArea* gla){
    vcg::Box3f bb = m.cm.bbox;
    vcg::Point3f center;
    center = bb.Center();

    float diag = bb.Diag();

    GLfloat lP[4];
    glGetLightfv(GL_LIGHT0, GL_POSITION, lP);
    vcg::Point3f light = -vcg::Point3f(lP[0],lP[1],lP[2]);

    vcg::Matrix44f tm = gla->trackball.Matrix();

    glMatrixMode(GL_PROJECTION);

    glPushMatrix();

        glLoadIdentity();
        glOrtho(-(diag/2),
                 diag/2,
                 -(diag/2),
                 diag/2,
                 -(diag/2),
                 diag/2);

    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
        vcg::Point3f u, v;
        //mi seleziona automaticamente un upvector che mi eviti casi degeneri...nel caso vada bene 010 sceglie quello
        vcg::GetUV(light, u, v, vcg::Point3f(0,-1,0));
        glLoadIdentity();
        gluLookAt(0, 0, 0, light[0], light[1], light[2], v[0], v[1], v[2]);

        //get the rotation matrix from the trackball
        vcg::Matrix44f rotation;
        vcg::Similarityf track = gla->trackball.track;
        track.rot.ToMatrix(rotation);
        glMultMatrixf(rotation.transpose().V());

        //traslate the model in the center
        glTranslatef(-center[0],-center[1],-center[2]);
}


void ShadowMapping::renderingFromLightUnsetup(){
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void ShadowMapping::runShader(MeshModel& m, GLArea* gla){
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

    //first rendering to get shadowMap from the light point of view
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(4.0, 4.0);

    //binding the FBO
    this->bind();

    RenderMode rm = gla->getCurrentRenderMode();
    m.Render(rm.drawMode, rm.colorMode,rm.textureMode);
    glDisable(GL_POLYGON_OFFSET_FILL);

    //unbinding the FBO
    this->unbind();

    this->renderingFromLightUnsetup();

    /***********************************************************/
    //SHADOW MAP Generation finished
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

bool ShadowMapping::setup()
{
        if (!GLEW_EXT_framebuffer_object) {
                qWarning("FBO not supported!");
                return false;
        }

        if (_initOk)
                return true;
        
        glGenFramebuffersEXT(1, &_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);

        this->genDepthMapTexture24(this->_shadowMap, true);

        //we don't need a color attachment
        GLenum drawBuffers[] = {GL_NONE};
        glDrawBuffersARB(1, drawBuffers);
		
        glReadBuffer(GL_NONE);

        //checks for fbo creation errors
        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return _initOk;
}
