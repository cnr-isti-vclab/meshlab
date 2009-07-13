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

ShadowMapping::ShadowMapping():DecorateShader()
{
    this->_objectVert = 0;
    this->_objectFrag = 0;
    this->_objectShaderProgram = 0;
    this->_fbo = 0;
}

ShadowMapping::~ShadowMapping(){
    glDetachShader(this->_objectShaderProgram, this->_objectVert);
    glDetachShader(this->_objectShaderProgram, this->_objectFrag);

    glDeleteShader(this->_objectVert);
    glDeleteShader(this->_objectFrag);
    glDeleteProgram(this->_objectShaderProgram);

    glDeleteFramebuffersEXT(1, &_fbo);
}

bool ShadowMapping::init()
{
    GLenum err = glewInit();
    if (!GLEW_OK == err){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("GLEW init failure");
        msgBox.setText(QString("Init GLEW failed."));
        int ret = msgBox.exec();
        return false;
    }
    if(!this->setup()){
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("FBO Setup failure");
        msgBox.setText(QString("Failed in creating a Frame Buffer Object."));
        int ret = msgBox.exec();
        return false;
    }
    return compileAndLink();
}

void ShadowMapping::runShader(MeshModel& m, GLArea* gla){
        vcg::Box3f bb = m.cm.bbox;
        vcg::Point3f center;
        center = bb.Center();

        GLfloat g_mModelView[16];
        GLfloat g_mProjection[16];

        int diag = bb.Diag();

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

            glGetFloatv(GL_PROJECTION_MATRIX, g_mProjection);
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
            glGetFloatv(GL_MODELVIEW_MATRIX, g_mModelView);

            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(4.0, 4.0);


            this->bind();
            RenderMode rm = gla->getCurrentRenderMode();
            m.Render(rm.drawMode, vcg::GLW::CMNone, vcg::GLW::TMNone);
            glDisable(GL_POLYGON_OFFSET_FILL);
            this->getShadowMap();
            this->unbind();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);

        GLint depthFuncOld;
        glGetIntegerv(GL_DEPTH_FUNC, &depthFuncOld);
        glDepthFunc(GL_LEQUAL);
        vcg::Matrix44f mvpl = (vcg::Matrix44f(g_mProjection).transpose() * vcg::Matrix44f(g_mModelView).transpose()).transpose();
        glUseProgram(this->_objectShaderProgram);

        GLuint matrixLoc = glGetUniformLocation(this->_objectShaderProgram, "mvpl");
        glUniformMatrix4fv(matrixLoc, 1, 0, mvpl.V());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        GLuint loc = glGetUniformLocation(this->_objectShaderProgram, "shadowMap");
        glUniform1i(loc, 0);

        m.Render(rm.drawMode, rm.colorMode, vcg::GLW::TMNone);
        //m.Render(vcg::GLW::DMSmooth, vcg::GLW::CMPerVert, vcg::GLW::TMPerWedge);
        glDepthFunc((GLenum)depthFuncOld);
        glUseProgram(0);
        int error = glGetError();
}

bool ShadowMapping::setup()
{
        if (!GLEW_EXT_framebuffer_object) {
                qWarning("FBO not supported!");
                return false;
        }

        if (_initOk)
                return true;

        glGenTextures(1, &this->_shadowMap);
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,  this->_texSize, this->_texSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glGenFramebuffersEXT(1, &_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, this->_shadowMap, 0);
        //cosi specifichi che il colore non importa, che il fbo non ha niente sull'attachment colore
        GLenum drawBuffers[] = {GL_NONE};
        glDrawBuffersARB(1, drawBuffers);

        int err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
        _initOk = (err == GL_FRAMEBUFFER_COMPLETE_EXT);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        return _initOk;
}

void ShadowMapping::bind()
{
        assert(_initOk);

        glClearDepth(1.0);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, _fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, this->_texSize, this->_texSize);
        glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMapping::unbind()
{
        if (!_initOk)
                return;

        glPopAttrib();
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
        //glDeleteFramebuffersEXT(1, &_fbo);
}

bool ShadowMapping::compileAndLink(){
    QFile* objVert = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/sm/object.vert"));
    QFile* objFrag = new QFile(MainWindowInterface::getBaseDirPath() + QString("/../fgt/decorate_shadow/shader/sm/object.frag"));

    objVert->open(QIODevice::ReadOnly | QIODevice::Text);
    objFrag->open(QIODevice::ReadOnly | QIODevice::Text);

    QByteArray bArray = objVert->readAll();
    GLint ShaderLen = (GLint) bArray.length();
    GLubyte* ShaderSource = (GLubyte *)bArray.data();

    this->_objectVert= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(this->_objectVert, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_objectVert);
    if(!this->printShaderInfoLog(this->_objectVert))
        return false;

    bArray = objFrag->readAll();
    ShaderLen = (GLint) bArray.length();
    ShaderSource = (GLubyte *)bArray.data();

    this->_objectFrag= glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(this->_objectFrag, 1, (const GLchar **)&ShaderSource, &ShaderLen);
    glCompileShader(this->_objectFrag);
    if(!this->printShaderInfoLog(this->_objectFrag))
        return false;

    this->_objectShaderProgram = glCreateProgram();
    glAttachShader(this->_objectShaderProgram, this->_objectVert);
    glAttachShader(this->_objectShaderProgram, this->_objectFrag);
    glLinkProgram(this->_objectShaderProgram);
    if(!this->printProgramInfoLog(this->_objectShaderProgram))
        return false;

    return true;
}
