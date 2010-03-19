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

#include "ssao.h"
#include <common/pluginmanager.h>
SSAO::SSAO(float radius):DecorateShader()
{
    this->_radius = radius;

    this->_depth = 0;

    this->_color1 = 0;
    this->_normalMapFrag = 0;
    this->_normalMapVert = 0;
    this->_normalMapShaderProgram = 0;

    this->_depthMap = 0;

    this->_color2= 0;
    this->_ssaoVert = 0;
    this->_ssaoFrag = 0;
    this->_ssaoShaderProgram = 0;

    this->_blurVert = 0;
    this->_blurFrag = 0;
    this->_blurShaderProgram = 0;
    this->_fbo = 0;
    this->_fbo2 = 0;
}

SSAO::~SSAO(){
    glDetachShader(this->_normalMapShaderProgram, this->_normalMapVert);
    glDetachShader(this->_normalMapShaderProgram, this->_normalMapFrag);

    glDeleteShader(this->_normalMapVert);
    glDeleteShader(this->_normalMapFrag);
    glDeleteProgram(this->_normalMapShaderProgram);

    glDetachShader(this->_ssaoShaderProgram, this->_ssaoVert);
    glDetachShader(this->_ssaoShaderProgram, this->_ssaoFrag);

    glDeleteShader(this->_ssaoVert);
    glDeleteShader(this->_ssaoFrag);
    glDeleteProgram(this->_ssaoShaderProgram);

    glDetachShader(this->_blurShaderProgram, this->_blurVert);
    glDetachShader(this->_blurShaderProgram, this->_blurFrag);

    glDeleteShader(this->_blurVert);
    glDeleteShader(this->_blurFrag);
    glDeleteProgram(this->_blurShaderProgram);

    glDeleteTexturesEXT(1, &(this->_color1));
    glDeleteTexturesEXT(1, &(this->_depthMap));

    glDeleteFramebuffersEXT(1, &(this->_depth));
    glDeleteTexturesEXT(1, &(this->_color2));
    glDeleteTexturesEXT(1, &(this->_color2));

    glDeleteFramebuffersEXT(1, &_fbo);
    glDeleteFramebuffersEXT(1, &_fbo2);
}

bool SSAO::init()
{
    if(!this->initGlew() || !this->initSetup())
        return false;

    if(!compileAndLink(
            this->_ssaoShaderProgram,
            this->_ssaoVert,
            this->_ssaoFrag,
            PluginManager::getBaseDirPath().append(QString("/shaders/decorate_shadow/ssao/ssao"))) ||
       !compileAndLink(
            this->_normalMapShaderProgram,
            this->_normalMapVert,
            this->_normalMapFrag,
            PluginManager::getBaseDirPath().append(QString("/shaders/decorate_shadow/ssao/normalMap"))) ||
       !compileAndLink(
            this->_blurShaderProgram,
            this->_blurVert,
            this->_blurFrag,
            PluginManager::getBaseDirPath().append(QString("/shaders/decorate_shadow/ssao/blur"))))
        return false;
    return true;
}

void SSAO::runShader(MeshModel& m, GLArea* gla){

        /***********************************************************/
        //NORMAL MAP and DEPTH MAP generation
        /***********************************************************/
        this->bind();
        glUseProgram(this->_normalMapShaderProgram);
        RenderMode rm = gla->getCurrentRenderMode();

        vcg::Matrix44f mProj, mInverseProj;
        glMatrixMode(GL_PROJECTION);
        glGetFloatv(GL_PROJECTION_MATRIX, mProj.V());
        glMatrixMode(GL_MODELVIEW);

        mProj.transposeInPlace();
        mInverseProj = vcg::Inverse(mProj);

        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m.Render(vcg::GLW::DMFlat, vcg::GLW::CMNone, vcg::GLW::TMNone);
        glUseProgram(0);

        /***********************************************************/
        //SSAO PASS
        /***********************************************************/
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo2);
        glUseProgram(this->_ssaoShaderProgram);

        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_noise);
        GLuint noiseloc = glGetUniformLocation(this->_ssaoShaderProgram, "rnm");
        glUniform1i(noiseloc, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->_color1);
        GLuint loc = glGetUniformLocation(this->_ssaoShaderProgram, "normalMap");
        glUniform1i(loc, 1);


        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, this->_depthMap);
        loc = glGetUniformLocation(this->_ssaoShaderProgram, "depthMap");
        glUniform1i(loc, 2);

        GLuint radiusLoc = glGetUniformLocation(this->_ssaoShaderProgram, "rad");
        glUniform1f(radiusLoc, this->_radius);

        GLuint matrixLoc = glGetUniformLocation(this->_ssaoShaderProgram, "proj");
        glUniformMatrix4fv(matrixLoc, 1, 0, mProj.transpose().V());

        GLuint invMatrixLoc = glGetUniformLocation(this->_ssaoShaderProgram, "invProj");
        glUniformMatrix4fv(invMatrixLoc, 1, 0, mInverseProj.transpose().V());

        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBegin(GL_TRIANGLE_STRIP);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f( 1.0f, -1.0f, 0.0f);
                glVertex3f(-1.0f,  1.0f, 0.0f);
                glVertex3f( 1.0f,  1.0f, 0.0f);
            glEnd();
        glUseProgram(0);

        /***********************************************************/
        //BLURRING horizontal
        /***********************************************************/
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
        glUseProgram(this->_blurShaderProgram);

        float blur_coef = 0.8;
        GLfloat scale = 1/(this->_texSize * blur_coef);

        GLuint scaleLoc = glGetUniformLocation(this->_blurShaderProgram, "scale");
        glUniform2f(scaleLoc, scale, 0.0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_color2);
        loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
        glUniform1i(loc, 0);

        glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
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
        this->unbind();
        glUniform2f(scaleLoc, 0.0, scale);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, this->_color1);
        loc = glGetUniformLocation(this->_blurShaderProgram, "scene");
        glUniform1i(loc, 0);

            glBegin(GL_TRIANGLE_STRIP);
                glVertex3f(-1.0f, -1.0f, 0.0f);
                glVertex3f( 1.0f, -1.0f, 0.0f);
                glVertex3f(-1.0f,  1.0f, 0.0f);
                glVertex3f( 1.0f,  1.0f, 0.0f);
            glEnd();

        glUseProgram(0);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
}

bool SSAO::setup()
{
    if (!GLEW_EXT_framebuffer_object) {
            qWarning("FBO not supported!");
            return false;
    }

    if (_initOk)
            return true;

    //genero i 2 framebuffer object che mi servono.
    glGenFramebuffersEXT(1, &_fbo);
    glGenFramebuffersEXT(1, &_fbo2);

    //attacco il primo...adesso le modifiche andranno a modificare solo _fbo
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);

    //Generates first color texture
    this->genColorTextureEXT(this->_color1, GL_COLOR_ATTACHMENT0_EXT);

    this->genDepthMapTexture24(this->_depthMap, false);

    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};

    glDrawBuffersARB(0, drawBuffers);

    int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);

    if(!this->_initOk)
        return this->_initOk;

    //attacco il secondo fbo...adesso le modifiche andranno a modificare solo _fbo2
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo2);

    //Generates first color texture
    this->genColorTextureEXT(this->_color2, GL_COLOR_ATTACHMENT0_EXT);

    //Generates render buffer for depth attachment
    this->genDepthRenderBufferEXT(this->_depth);

    GLenum drawBuffers2[] = {GL_COLOR_ATTACHMENT0};

    glDrawBuffersARB(0, drawBuffers2);

    this->loadNoiseTxt();

    err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
    return _initOk;
}

bool SSAO::loadNoiseTxt(){

    QImage image = QImage();
    QString textureName = QString(":/rand.png");
    if (QFile(textureName).exists())
    {
        image = QImage(textureName);
        noiseWidth=image.width();
        noiseHeight=image.height();

        QImage tmpGL = QGLWidget::convertToGLFormat(image);
        image = QImage(tmpGL);
    }
    else {
        qDebug("Warning failed to load noise texture!");
        assert(0);
        return false;
    }

    // Creates The Texture
    glGenTextures(1, &(this->_noise));
    glBindTexture(GL_TEXTURE_2D, this->_noise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, noiseWidth , noiseHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());

    return true;
}

void SSAO::printNoiseTxt(){
    QImage img(noiseWidth , noiseHeight, QImage::Format_RGB32);

    unsigned char *tempBuf = new unsigned char[noiseWidth * noiseHeight * 3];
    unsigned char *tempBufPtr = tempBuf;
    glBindTexture(GL_TEXTURE_2D, this->_noise);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
    for (int i = 0; i < noiseWidth; ++i) {
            QRgb *scanLine = (QRgb*)img.scanLine(i);
            for (int j = 0; j < noiseHeight; ++j) {
                    scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
                    tempBufPtr += 3;
            }
    }

    delete[] tempBuf;

    img.mirrored().save("_noise.png", "PNG");
}
