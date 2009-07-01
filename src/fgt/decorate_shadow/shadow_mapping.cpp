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
    this->_height = 0;
    this->_width = 0;
    this->_depthFrag = 0;
    this->_depthVert = 0;
    this->_depthShaderProgram = 0;
    this->_objectVert = 0;
    this->_objectFrag = 0;
    this->_objectShaderProgram = 0;
    this->_shadowMap = 0;
    this->_depth=0;
    this->initOk=false;
    this->fbo = 0;
}

ShadowMapping::~ShadowMapping(){}

bool ShadowMapping::Init(int w, int h)
{
this->_width = 640;
this->_height = 640;
compileLinkSM();
return true;
}

void ShadowMapping::RunShader(MeshModel& m, GLArea* gla){

        glUseProgram(this->_depthShaderProgram);

        //PASS UNIFORMS
        GLint uLocNearP = glGetUniformLocation(this->_depthShaderProgram, "near");
        GLint uLocFarP = glGetUniformLocation(this->_depthShaderProgram, "far");
        GLint uLocWidth = glGetUniformLocation(this->_depthShaderProgram, "width");
     //   GLint uLocHeight = glGetUniformLocation(this->_depthShaderProgram, "fViewportHeight");

        glUniform1f(uLocNearP, 200);
        glUniform1f(uLocFarP, 200);
        glUniform1f(uLocWidth, gla->size().width());
       // glUniform1f(uLocHeight, gla->size().height());*/
        this->Unbind();
        this->Setup();
        this->Bind();
        m.Render(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMPerVert);
        this->GetQImage();
        /*glDetachShader(this->_depthShaderProgram, this->_depthVert);
        glDeleteShader(this->_depthVert);

        glDetachShader(this->_depthShaderProgram, this->_depthFrag);
        glDeleteShader(this->_depthFrag);

        glDeleteProgram(this->_depthShaderProgram);

        err = glGetError();*/
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
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);

        GLuint loc = glGetUniformLocation(this->_objectShaderProgram, "shadowMap");
        glUniform1i(loc, 1);

        m.Render(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMPerVert);
glDisable(GL_TEXTURE_2D);
glDisable(GL_BLEND);
        /*glDetachShader(this->_objectShaderProgram, this->_objectVert);
        glDeleteShader(this->_objectVert);
        glDetachShader(this->_objectShaderProgram, this->_objectVert);
        glDeleteShader(this->_objectFrag);
        glDeleteProgram(this->_objectShaderProgram);*/
        glUseProgram(0);

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
        glGenRenderbuffersEXT(1, &(this->_depth));
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, this->_depth);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, this->_width, this->_height);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, this->_depth);

        // color buffer
        glGenTextures(1, &this->_shadowMap);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  this->_width, this->_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_shadowMap, 0);


        // set texture state based on the first uniform that will use RT

        /*QList<int> k = passStates.keys();
        for (int i = 0; i < k.size(); ++i) {
                if (k.at(i) > pass) {
                        foreach (RfxState *s, passStates.value(i))
                                s->SetEnvironment(GL_TEXTURE_2D);
                }
        }*/

        //case GL_TextureWrapS:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        //	break;
        //case GL_TextureWrapT:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        //	break;
        //case GL_TextureWrapR:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        //	break;

        //case GL_TextureMinify:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //	break;
        //case GL_TextureMagnify:
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //	break;

        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return initOk;
}

void ShadowMapping::Bind()
{
        assert(initOk);

        //bool colClear = passOptions.value(pass).colorClear;
        //bool depClear = passOptions.value(pass).depthClear;

        /*if (colClear) {
                const float *cols = passOptions.value(pass).colorClearVal;
                glClearColor(cols[0], cols[1], cols[2], cols[3]);
        }
*/
        //if (depClear)
        glClearDepth(1.0);

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, this->_width, this->_height);

        //if (colClear || depClear)
        //        glClear(passOptions.value(pass).clearMask);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void ShadowMapping::Unbind()
{
        if (!initOk)
                return;

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        glPopAttrib();
}


//bool ShadowMapping::ClearBuffers(){return false;}

void ShadowMapping::GetQImage()
{
        if (!initOk)
                return;// QImage();

        QImage img(this->_width, this->_height, QImage::Format_RGB32);

        unsigned char *tempBuf = new unsigned char[this->_width * this->_height * 3];
        unsigned char *tempBufPtr = tempBuf;
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
        for (int i = 0; i < this->_height; ++i) {
                QRgb *scanLine = (QRgb*)img.scanLine(i);
                for (int j = 0; j < this->_width; ++j) {
                        scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
                        tempBufPtr += 3;
                }
        }
        delete[] tempBuf;

        img.save("./texture.png", "PNG");
        img.mirrored().save("./textureMirrored.png", "PNG");
        //return img.mirrored();
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

