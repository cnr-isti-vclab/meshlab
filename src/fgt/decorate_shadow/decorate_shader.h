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
#ifndef DECORATE_SHADER_H
#define DECORATE_SHADER_H
#include <cassert>
#include <QImage>
#include <QFile>
#include <GL/glew.h>
#include <meshlab/meshmodel.h>
#include <meshlab/glarea.h>


class DecorateShader
{
public:
    DecorateShader(){
        this->_initOk = false;
        this->_texSize = 512;
        this->_shadowMap = 0;
	//	this->_color_tex = 0;
    }

    //virtual ~DecorateShader();

    virtual bool init() = 0;
    virtual void runShader(MeshModel&, GLArea*) = 0;

protected:
    bool _initOk;
    int _texSize;
    GLuint _shadowMap;
	// GLuint _color_tex;

    virtual bool compileAndLink() = 0;
    virtual bool setup() = 0;
    virtual void bind() = 0;
    virtual void unbind() = 0;

    void getShadowMap(){
        if (!this->_initOk)
                return;

        QImage img(this->_texSize, this->_texSize, QImage::Format_RGB32);

        float *tempFBuf = new float[this->_texSize * this->_texSize *1 ];
        float *tempFBufPtr = tempFBuf;
        glBindTexture(GL_TEXTURE_2D, this->_shadowMap);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, tempFBufPtr);
        for (int i = 0; i < this->_texSize; ++i) {
                QRgb *scanLine = (QRgb*)img.scanLine(i);
                for (int j = 0; j < this->_texSize; ++j) {
                    const unsigned char val = (unsigned char) (tempFBufPtr[0] * 255.0f);
                        scanLine[j] = qRgb(val, val, val);
                        tempFBufPtr ++;
                }
        }
        delete[] tempFBuf;
        img.mirrored().save("./_shadowMapTXT.png", "PNG");
    }

    bool printShaderInfoLog(GLuint obj){
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
/*
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("Shader failure");
            msgBox.setText(QString("Failed in compiling and linking the shader"));
            int ret = msgBox.exec();
            return false;*/
        }
        return true;
    }

    bool printProgramInfoLog(GLuint obj)
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
/*
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setWindowTitle("Shader program failure");
            msgBox.setText(QString("Failed in compiling and linking the shader program"));
            int ret = msgBox.exec();
            return false;*/
        }
        return true;
    }
};
#endif // DECORATE_SHADER_H
