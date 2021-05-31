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

#include <meshlab/glarea.h>
#include "decorate_shader.h"
#include "variance_shadow_mapping_blur.h"
#include <common/globals.h>

VarianceShadowMappingBlur::VarianceShadowMappingBlur(float intensity):VarianceShadowMapping(intensity)
{
    this->_blurH = 0;
    this->_blurV = 0;
    this->_blurVert = 0;
    this->_blurFrag = 0;
    this->_blurShaderProgram = 0;
}

VarianceShadowMappingBlur::~VarianceShadowMappingBlur(){
    glDetachShader(this->_blurShaderProgram, this->_blurVert);
    glDetachShader(this->_blurShaderProgram, this->_blurFrag);

    glDeleteShader(this->_blurVert);
    glDeleteShader(this->_blurFrag);
    glDeleteProgram(this->_blurShaderProgram);

    glDeleteTextures(1, &(this->_blurH));
    glDeleteTextures(1, &(this->_blurV));
}

bool VarianceShadowMappingBlur::init()
{
    if(!this->initGlew() || !this->initSetup())
        return false;

    if(!compileAndLink(
            this->_depthShaderProgram,
            this->_depthVert,
            this->_depthFrag,
            meshlab::defaultShadersPath() + "/decorate_shadow/vsmb/depthVSM") ||
       !compileAndLink(
            this->_shadowMappingProgram,
            this->_shadowMappingVert,
            this->_shadowMappingFrag,
            meshlab::defaultShadersPath() + "/shaders/decorate_shadow/vsmb/objectVSM") ||
       !compileAndLink(
            this->_blurShaderProgram,
            this->_blurVert,
            this->_blurFrag,
            meshlab::defaultShadersPath() + "/shaders/decorate_shadow/vsmb/blurVSM"))
        return false;
    return true;
}


void VarianceShadowMappingBlur::runShader(MeshDocument& md, GLArea* gla){
    GLfloat g_mModelView[16];
    GLfloat g_mProjection[16];

    MLSceneGLSharedDataContext* ctx = NULL;
    if ((gla == NULL) || (gla->mvc()  == NULL)) 
            return;
    ctx = gla->mvc()->sharedDataContext();
    if (ctx == NULL)
        return;

    this->renderingFromLightSetup(md, gla);
    glMatrixMode(GL_PROJECTION);
        glGetFloatv(GL_PROJECTION_MATRIX, g_mProjection);
    glMatrixMode(GL_MODELVIEW);
        glGetFloatv(GL_MODELVIEW_MATRIX, g_mModelView);
    /***********************************************************/
    //GENERAZIONE SHADOW MAP
    /***********************************************************/
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);

    this->bind();
    glUseProgram(this->_depthShaderProgram);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MLRenderingData dt;
	MLRenderingData::RendAtts atts;
	atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
	atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;
	atts[MLRenderingData::ATT_NAMES::ATT_FACENORMAL] = true;
	dt.set(MLRenderingData::PR_SOLID, atts);

	for(MeshModel *m: md.meshIterator())
	{
		if ((m != NULL) && (m->visible))
		{
			ctx->drawAllocatedAttributesSubset(m->id(),gla->context(),dt);
		}
	}
	glDisable(GL_POLYGON_OFFSET_FILL);

    this->renderingFromLightUnsetup();

    /***********************************************************/
    //BLURRING horizontal
    /***********************************************************/
    glUseProgram(this->_blurShaderProgram);

    GLfloat scale = 1/(this->_texW * BLUR_COEF);

    GLuint scaleLoc = glGetUniformLocation(this->_blurShaderProgram, "scale");
    glUniform2f(scaleLoc, scale, 0.0);

    glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
    GLuint loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
    glUniform1i(loc, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
        glEnd();

    /***********************************************************/
    //BLURRING vertical
    /***********************************************************/
    glUniform2f(scaleLoc, 0.0, scale);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, this->_blurH);
    loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
    glUniform1i(loc, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glVertex3f( 1.0f, -1.0f, 0.0f);
            glVertex3f(-1.0f,  1.0f, 0.0f);
            glVertex3f( 1.0f,  1.0f, 0.0f);
        glEnd();
    this->unbind();

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

    GLuint shadowIntensityLoc = glGetUniformLocation(this->_shadowMappingProgram, "shadowIntensity");
    glUniform1f(shadowIntensityLoc, this->_intensity);

    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->_blurV);


    loc = glGetUniformLocation(this->_shadowMappingProgram, "shadowMap");
    glUniform1i(loc, 0);

    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE); // to avoid the fact that when saving a snapshot we get semitransparent shadowed areas.
    for(MeshModel *m: md.meshIterator())
    {
        if ((m != NULL) && (m->visible))
        {
			ctx->drawAllocatedAttributesSubset(m->id(),gla->context(),dt);
        }
    }
    glPopAttrib();
    glUseProgram(0);

    glDepthFunc((GLenum)depthFuncOld);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

bool VarianceShadowMappingBlur::setup()
{
    if (!GLEW_EXT_framebuffer_object) {
            qWarning("FBO not supported!");
            return false;
    }

    if (_initOk)
            return true;

    //genero il frame buffer object
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    //Generates texture color for variance shadow map
    this->genColorTextureEXT(this->_shadowMap, GL_COLOR_ATTACHMENT0);

    //Generates texture color for horizontal blur
    this->genColorTextureEXT(this->_blurH, GL_COLOR_ATTACHMENT1);

    //Generates texture color for vertical blur
    this->genColorTextureEXT(this->_blurV, GL_COLOR_ATTACHMENT2);

    //Generates render buffer for depth attachment
    this->genDepthRenderBufferEXT(this->_depth);

    //Defines 3 different draw buffer for the FBO
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffersARB(3, drawBuffers);

    //checks for errors
    int err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    _initOk = (err == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return _initOk;
}
