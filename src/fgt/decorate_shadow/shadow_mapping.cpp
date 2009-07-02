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
    //this->_depth=0;
    this->initOk=false;
    this->fbo = 0;
}

ShadowMapping::~ShadowMapping(){}

bool ShadowMapping::Init()//(int w, int h)
{
//this->_width = 640;
//this->_height = 640;
compileLinkSM();
return true;
}

void ShadowMapping::RunShader(MeshModel& m, GLArea* gla){
        vcg::Box3f bb = m.cm.bbox;
        vcg::Point3f max, min;
        max = bb.max;
        min = bb.min;

        int index = bb.MaxDim();
        if(index == 0) this->_texSize = bb.DimX();
        else if(index == 1) this->_texSize = bb.DimY();
            else this->_texSize = bb.DimZ();
        (this->_texSize % 2) == 0 ? this->_texSize = this->_texSize + 2 : this->_texSize++;

        glUseProgram(this->_depthShaderProgram);

        //PASS UNIFORMS
        GLint uLocNearP = glGetUniformLocation(this->_depthShaderProgram, "near");
        GLint uLocFarP = glGetUniformLocation(this->_depthShaderProgram, "far");
        GLint uLocWidth = glGetUniformLocation(this->_depthShaderProgram, "width");
     //   GLint uLocHeight = glGetUniformLocation(this->_depthShaderProgram, "fViewportHeight");

        glUniform1f(uLocNearP, gla->nearPlane);
        glUniform1f(uLocFarP, gla->farPlane);
        glUniform1f(uLocWidth, this->_texSize);
       // glUniform1f(uLocHeight, gla->size().height());*/
        this->Unbind();
        this->Setup();
        this->Bind(m);
        m.Render(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMPerVert);
        this->GetQImage();
        this->Unbind();

        glUseProgram(0);



         glUseProgram(this->_objectShaderProgram);
        //PASS UNIFORMS
        uLocNearP = glGetUniformLocation(this->_objectShaderProgram, "near");
        uLocFarP = glGetUniformLocation(this->_objectShaderProgram, "far");
        uLocWidth = glGetUniformLocation(this->_objectShaderProgram, "width");
        //uLocHeight = glGetUniformLocation(this->_objectShaderProgram, "fViewportHeight");

        glUniform1f(uLocNearP, gla->nearPlane);
        glUniform1f(uLocFarP, gla->farPlane);
        glUniform1f(uLocWidth, gla->size().width());
        //glUniform1f(uLocHeight, gla->size().height());*/
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        // select modulate to mix texture with color for shading
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        GLuint loc = glGetUniformLocation(this->_objectShaderProgram, "shadowMap");
        glUniform1i(loc, 1);

        m.Render(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMPerVert);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glUseProgram(0);
}

bool ShadowMapping::Setup()
{
        if (!GLEW_EXT_framebuffer_object) {
                qWarning("FBO not supported!");
                return false;
        }

        if (initOk)
                return true;

        glGenFramebuffersEXT(1, &fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

        // depth buffer
        /*glGenRenderbuffersEXT(1, &(this->_depth));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->_texSize, this->_texSize);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_depth);
*/
        // color buffer
        glGenTextures(1, &this->_shadowMap);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glGenerateMipmapEXT(GL_TEXTURE_2D);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_texSize, this->_texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_shadowMap, 0);

        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return initOk;
}

void ShadowMapping::Bind(MeshModel &m)
{
        assert(initOk);

        glClearDepth(1.0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
/*
  cercavo di muovere la mesh nel suo centro
        vcg::Point3f bbc =  m.cm.bbox.Center();
        glMatrixMode(GL_MODELVIEW);
        glTranslatef(-bbc.X(), -bbc.Y(), -bbc.Z());
*/
        /*GLdouble mv[16];
        GLdouble prj[16];
        GLint view[4];

        glGetDoublev(GL_MODELVIEW_MATRIX, mv);
        glGetDoublev(GL_PROJECTION_MATRIX, prj);
        glGetIntegerv(GL_VIEWPORT, view);

        GLdouble fx;
        GLdouble fy;
        GLdouble fz;

        vcg::Point3f p = m.cm.bbox.min;

        GLdouble x,y,z;
        x = p[0];
        y = p[1];
        z = p[2];

        gluProject(x, y, z, mv, prj, view, &fx, &fy, &fz);*/

        glViewport(0, 0, this->_texSize, this->_texSize);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void ShadowMapping::Unbind()
{
        if (!initOk)
                return;

        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        //glDeleteFramebuffersEXT(1, &fbo);
}

void ShadowMapping::GetQImage()
{
        if (!initOk)
                return;// QImage();

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

        img.save("./texture.png", "PNG");
        img.mirrored().save("./textureMirrored.png", "PNG");
        //return img.mirrored();
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

        /*aggiungere controllo compilaz e linking*/


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

