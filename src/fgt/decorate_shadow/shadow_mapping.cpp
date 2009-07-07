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


#include "shadow_mapping.h"

ShadowMapping::ShadowMapping()
{
    this->_texSize = 0;
    this->_depthFrag = 0;
    this->_depthVert = 0;
    this->_depthShaderProgram = 0;
    this->_objectVert = 0;
    this->_objectFrag = 0;
    this->_objectShaderProgram = 0;
    this->_shadowMap = 0;
    this->_initOk=false;
    this->_fbo = 0;
}

ShadowMapping::~ShadowMapping(){}

bool ShadowMapping::Init()
{
    compileLinkSM();
    return true;
}

void ShadowMapping::RunShader(MeshModel& m, GLArea* gla){
        vcg::Box3f bb = m.cm.bbox;
        vcg::Point3f center;
        center = bb.Center();

        GLfloat g_mModelView[16];
        GLfloat g_mProjection[16];

        this->_texSize = bb.Diag();
        (this->_texSize % 2) == 0 ? this->_texSize = this->_texSize + 2 : this->_texSize++;

        glUseProgram(this->_depthShaderProgram);
        GLint uLocWidth = glGetUniformLocation(this->_depthShaderProgram, "width");
        GLint uLocMeshCenter = glGetUniformLocation(this->_depthShaderProgram, "meshCenter");

        glUniform1f(uLocWidth, this->_texSize);
        glUniform3f(uLocMeshCenter, center[0], center[1], center[2]);

        this->Setup();
        this->Bind(m);
        m.Render(vcg::GLW::DMFlat, vcg::GLW::CMPerFace, vcg::GLW::TMPerWedge);
        this->GetQImage();
        this->Unbind();

        glUseProgram(0);

        glUseProgram(this->_objectShaderProgram);
        uLocWidth = glGetUniformLocation(this->_objectShaderProgram, "width");
        uLocMeshCenter = glGetUniformLocation(this->_objectShaderProgram, "meshCenter");

        glUniform1f(uLocWidth, gla->size().width());
        glUniform3f(uLocMeshCenter, center[0], center[1], center[2]);

        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        GLuint loc = glGetUniformLocation(this->_objectShaderProgram, "shadowMap");
        glUniform1i(loc, 1);
        m.Render(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMPerWedge);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glUseProgram(0);
        //glEnable (GL_BLEND);
        //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        int error = glGetError();
}

bool ShadowMapping::Setup()
{
        if (!GLEW_EXT_framebuffer_object) {
                qWarning("FBO not supported!");
                return false;
        }

        if (_initOk)
                return true;

        glGenFramebuffersEXT(1, &_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);

        // depth buffer
        glGenRenderbuffersEXT(1, &(this->_depth));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->_texSize, this->_texSize);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_depth);
        
        // color buffer
        glGenTextures(1, &this->_shadowMap);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //glGenerateMipmapEXT(GL_TEXTURE_2D);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_shadowMap, 0);

        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return _initOk;
}

void ShadowMapping::Bind(MeshModel &m)
{
        assert(_initOk);

        glClearDepth(1.0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, this->_texSize, this->_texSize);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void ShadowMapping::Unbind()
{
        if (!_initOk)
                return;

        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        //glDeleteFramebuffersEXT(1, &_fbo);
}

void ShadowMapping::GetQImage()
{
        if (!_initOk)
                return;

        QImage img(this->_texSize, this->_texSize, QImage::Format_RGB32);

        unsigned char *tempBuf = new unsigned char[this->_texSize * this->_texSize * 3];
        unsigned char *tempBufPtr = tempBuf;
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
        for (int i = 0; i < this->_texSize; ++i) {
                QRgb *scanLine = (QRgb*)img.scanLine(i);
                for (int j = 0; j < this->_texSize; ++j) {
                        scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
                        tempBufPtr += 3;
                }
        }
        delete[] tempBuf;
        img.mirrored().save("./_shadowMapTXT.png", "PNG");
}

bool ShadowMapping::compileLinkSM(){
    GLenum err = glewInit();
    if (GLEW_OK == err) {
        QFile* depthVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/sm/depth.vert"));
        QFile* depthFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/sm/depth.frag"));
        QFile* objVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/sm/object.vert"));
        QFile* objFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/sm/object.frag"));

        depthVert->open(QIODevice::ReadOnly | QIODevice::Text);
        depthFrag->open(QIODevice::ReadOnly | QIODevice::Text);
        objVert->open(QIODevice::ReadOnly | QIODevice::Text);
        objFrag->open(QIODevice::ReadOnly | QIODevice::Text);



                //DEPTH PASS
                QByteArray bArray = depthVert->readAll();
        GLubyte *ShaderSource;
        GLint ShaderLen = (GLint) bArray.length();
        ShaderSource = (GLubyte *)bArray.data();


                this->_depthVert= glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(this->_depthVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
        glCompileShader(this->_depthVert);
        this->printShaderInfoLog(this->_depthVert);
        //delete ShaderSource;

        bArray = depthFrag->readAll();
        ShaderLen = (GLint) bArray.length();
        ShaderSource = (GLubyte *)bArray.data();

                this->_depthFrag = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(this->_depthFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
        glCompileShader(this->_depthFrag);
        this->printShaderInfoLog(this->_depthFrag);
        //delete[] ShaderSource;

                this->_depthShaderProgram = glCreateProgram();
                glAttachShader(this->_depthShaderProgram, this->_depthVert);
                glAttachShader(this->_depthShaderProgram, this->_depthFrag);
        glLinkProgram(this->_depthShaderProgram);
        this->printProgramInfoLog(this->_depthShaderProgram);



                //OBJ PASS
                bArray = objVert->readAll();
        ShaderLen = (GLint) bArray.length();
        ShaderSource = (GLubyte *)bArray.data();

                this->_objectVert= glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(this->_objectVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
        glCompileShader(this->_objectVert);
        this->printShaderInfoLog(this->_objectVert);


                bArray = objFrag->readAll();
        ShaderLen = (GLint) bArray.length();
        ShaderSource = (GLubyte *)bArray.data();

                this->_objectFrag= glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(this->_objectFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
        glCompileShader(this->_objectFrag);
        this->printShaderInfoLog(this->_objectFrag);


                this->_objectShaderProgram = glCreateProgram();
                glAttachShader(this->_objectShaderProgram, this->_objectVert);
                glAttachShader(this->_objectShaderProgram, this->_objectFrag);
        glLinkProgram(this->_objectShaderProgram);
        this->printProgramInfoLog(this->_objectShaderProgram);
    }
    return true;
}


void ShadowMapping::printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

        glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
                printf("%s\n",infoLog);
        free(infoLog);
    }
}

void ShadowMapping::printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

        glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
                printf("%s\n",infoLog);
        free(infoLog);
    }
}
