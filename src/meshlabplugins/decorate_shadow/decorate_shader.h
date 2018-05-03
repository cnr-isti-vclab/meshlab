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
#include <GL/glew.h>
#include <QFile>
#include <QImage>
#include <cassert>

//#include <QObject>
#include <common/interfaces.h>
//#include <meshlab/glarea.h>


class GLArea;

#define BLUR_COEF 0.4

/**
  * Base abstract class for all the four decorator methods(shadow mapping, VSM shadow mapping, VSM shadow mapping
  * with blur and Screen Space Ambient Occlusion). It defines the method that should be implemented in the derived
  * class(init(), runShader() and setup()). It defines a set of methods in common beetwen all the derived class too.
  */
class DecorateShader
{
public:
    DecorateShader(){
        this->_initOk = false;

        //default texture size
        this->_texW = 1024;
        this->_texH = 1024;
      }

	virtual ~DecorateShader() {}

    /**
      * Performs init commands.
      * If something went wrong return false, otherwise true.
      * @return false if something went wrong, true otherwise.
      */
    virtual bool init() = 0;

    /**
      * Applies the decoration running the shaders.
      * @param m the mesh model.
      * @param gla GLArea reference.
      */
    virtual void runShader(MeshDocument&, GLArea*) = 0;
    virtual void setShadowIntensity(float f) =0;

protected:
    bool _initOk;
    int _texW,_texH;

    /** The FrameBufferObject handler */
    GLuint _fbo;

    /**
      * Sets up the needed resources(FBO and textures) to apply the shader.
      * @return false if something went wrong, true otherwise.
      */
    virtual bool setup() = 0;

    /**
      * Binds the FBO, clear the color and depth buffer and sets the clear depth value and the viewport size.
      */
    void bind()
    {
        assert(_initOk);
        glClearDepth(1.0);
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glPushAttrib(GL_VIEWPORT_BIT);
        glViewport(0, 0, this->_texW, this->_texH);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }

    /**
      * Performs the glew init. If something went wrong it shows an allert message
      * and returns false, otherwise returns true;
      * @return true if no errors happened during the setup, false otherwise.
      */
    bool initGlew()
	{
        GLenum err = glewInit();
		return (err == GLEW_OK);
    }

    /**
      * Performs the setup of the decorator plugin. If something went wrong
      * it shows an allert message an returns false, otherwise returns true;
      * @return true if no errors happened during the setup, false otherwise.
      */
    bool initSetup()
	{
        return setup();
    }

    /**
    * Unbinds the last frame buffer object attached and pops the viewport attribute.
    */
    void unbind()
    {
        if(!_initOk)
            return;

        glPopAttrib();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
    * Prints the depth map specified by the handler <b>map<b/> in a file named <b>fname</b>
    * $$$$$$ FOR DEBUGGING PURPOSES $$$$$$
    * @param map The handler to the depth map
    * @param fname The name of the new file
    */
    void printDepthMap(GLuint map, const QString &fname){
        if (!this->_initOk)
                return;
        QImage img(this->_texW, this->_texH, QImage::Format_RGB32);

        float *tempFBuf = new float[this->_texW * this->_texH *1 ];
        float *tempFBufPtr = tempFBuf;
        glBindTexture(GL_TEXTURE_2D, map);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, tempFBufPtr);
        for (int i = 0; i < this->_texH; ++i) {
                QRgb *scanLine = (QRgb*)img.scanLine(i);
                for (int j = 0; j < this->_texW; ++j) {
                    const unsigned char val = (unsigned char) (tempFBufPtr[0] * 255.0f);
                        scanLine[j] = qRgb(val, val, val);
                        tempFBufPtr ++;
                }
        }
        delete[] tempFBuf;
        img.mirrored().save(fname, "PNG");
    }

    /**
    * Prints the color texture specified by the handler <b>map<b/> in a file named <b>fname</b>
    * $$$$$$ FOR DEBUGGING PURPOSES $$$$$$
    * @param map The handler to the texture
    * @param fname The name of the new file
    */
    void printColorMap(GLuint map, const QString &fname){
        if (!this->_initOk)
                return;

        QImage img(this->_texW, this->_texH, QImage::Format_RGB32);

     unsigned char *tempBuf = new unsigned char[this->_texW * this->_texH * 3];
        unsigned char *tempBufPtr = tempBuf;
        glBindTexture(GL_TEXTURE_2D, map);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, tempBufPtr);
        for (int i = 0; i < this->_texH; ++i) {
                QRgb *scanLine = (QRgb*)img.scanLine(i);
                for (int j = 0; j < this->_texW; ++j) {
                        scanLine[j] = qRgb(tempBufPtr[0], tempBufPtr[1], tempBufPtr[2]);
                        tempBufPtr += 3;
                }
        }

        delete[] tempBuf;
		
        img.mirrored().save(fname, "PNG");
    }

    /**
      * If something wrong happened during the compilation of the shader,
      * whose handler is obj, prints the log.
      * @param obj the shader handler
      */
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
        }
        return true;
    }

    /**
      * If something wrong happened during the compilation of the shader program,
      * whose handler is obj, prints the log.
      * @param obj the shader program handler
      */
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
        }
        return true;
    }

    /**
     * It takes a shader,vertex and fragment files that should be named in the same way
     * except for the extension(e.g. ~/xxxx.vertex ~/xxx.fragment), compile and link it
     * producing a program that could be executed on the GPU.
     * @param program The handler to the program
     * @param vertex The handler to the vertex shader
     * @param fragment The handler to the fragment shader
     * @param path The path to the files.
     * @return true if no errors during compilation and linking occurs
    */
    bool compileAndLink(GLuint& program, GLuint& vertex, GLuint& fragment, QString& path){
        //load the file containing the vertex shader
        QFile vertexShaderFile(path + QString(".vert"));
        bool ret=vertexShaderFile.open(QIODevice::ReadOnly | QIODevice::Text);
        if(!ret)
        {
          qDebug("Unable to open '%s'",qUtf8Printable(path + QString(".vert")));
          return false;
        }

        QByteArray bArray = vertexShaderFile.readAll();
        GLint ShaderLen = (GLint) bArray.length();
        GLubyte* ShaderSource = (GLubyte *)bArray.data();
		
        //create a new vertex shader
        if(vertex==0)
          vertex= glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, (const GLchar **)&ShaderSource, &ShaderLen);
        //compile the vertex shader
        glCompileShader(vertex);
        //print info log about the vertex compilaiton
        if(!this->printShaderInfoLog(vertex))
            return false;

        //close the vertex file
        vertexShaderFile.close();

        //load the file containing the fragment shader
        QFile fragmentShaderFile(path + QString(".frag"));
        fragmentShaderFile.open(QIODevice::ReadOnly | QIODevice::Text);

        bArray = fragmentShaderFile.readAll();
        ShaderLen = (GLint) bArray.length();
        ShaderSource = (GLubyte *)bArray.data();

        //create a new fragment shader
        if(fragment==0)
          fragment= glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, (const GLchar **)&ShaderSource, &ShaderLen);
        //compile the fragment shader
        glCompileShader(fragment);
        //print info log about the fragment compilaiton
        if(!this->printShaderInfoLog(fragment))
            return false;

        //close the fragment file
        fragmentShaderFile.close();

        //create a new shader program with the vertex and fragment shader loaded/compiled above
        if(program==0)
          program = glCreateProgram();
		else
		{
			glDetachShader(program,vertex);
			glDetachShader(program,fragment);
		}
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        if(!this->printProgramInfoLog(program))
            return false;
        return true;
    }

    /**
      * Generates a texture color for the handler <b>tex</b> and attaches it to the FBO
      * at the attachement target defined by <b>attachment</b>. The FBO should be binded before the method is called.
      * @param tex the texture handler
      * @param attachement the FBO attachment target.
      */
    void genColorTextureEXT(GLuint& tex, GLenum attachment){
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  this->_texW, this->_texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex, 0);


    }

    /**
      * Generates a depth render buffer for the handler <b>tex</b> and attaches it to the FBO.
      * The FBO should be binded before the method is called.
      * @param tex the render buffer handler
      */
    void genDepthRenderBufferEXT(GLuint& tex){

        glGenRenderbuffers(1, &tex);
        glBindRenderbuffer(GL_RENDERBUFFER, tex);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->_texW, this->_texH);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, tex);

    }
    
    /**
      * Setup a depth texture and attaches it to the FrameBufferObject. If <b>isShMap</b> is true the texture
      * generated will be a shadow map, otherwise a depth map.
      * The texture format will be GL_DEPTH_COMPONENT24. The FBO should be binded before calling the method
      * @param shmTxt The handler to the texture.
      */
    void genDepthMapTexture24(GLuint& tex, bool isShMap){
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        if(isShMap){
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_LUMINANCE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,  this->_texW, this->_texH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
        return;
    }
    
     /**
      * Setup a depth texture and attaches it to the FrameBufferObject. If <b>isShMap</b> is true the texture
      * generated will be a shadow map, otherwise a depth map.
      * The texture format will be GL_DEPTH_COMPONENT16. The FBO should be binded before calling the method
      * @param shmTxt The handler to the texture.
      */
    void genDepthMapTexture16(GLuint& tex, bool isShMap){
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        if(isShMap){
            glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        }

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16,  this->_texW, this->_texH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex, 0);
        return;
    }
};
#endif // DECORATE_SHADER_H
