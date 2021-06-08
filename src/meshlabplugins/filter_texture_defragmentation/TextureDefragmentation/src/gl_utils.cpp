/*******************************************************************************
    Copyright (c) 2021, Andrea Maggiordomo, Paolo Cignoni and Marco Tarini

    This file is part of TextureDefrag, a reference implementation for
    the paper ``Texture Defragmentation for Photo-Reconstructed 3D Models''
    by Andrea Maggiordomo, Paolo Cignoni and Marco Tarini.

    TextureDefrag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TextureDefrag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TextureDefrag. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#include "gl_utils.h"
#include "logging.h"

#include <iostream>
#include <fstream>
#include <memory>

#include <QImage>
#include <QFileInfo>

void CheckGLError()
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::stringstream ss;
        ss << "OpenGL error " << error  << " ";
        if (error == GL_INVALID_VALUE) ss << "GL_INVALID_VALUE";
        if (error == GL_INVALID_OPERATION) ss << "GL_INVALID_OPERATION";
        LOG_ERR << ss.str();
    }
}

std::string ReadShader(const char *path)
{
    std::ifstream sf(path);
    if (sf.is_open()) {
        std::stringstream ss;
        while (sf.good()) {
            std::string s;
            std::getline(sf, s);
            ss << s << std::endl;
        }
        return ss.str();
    } else {
        LOG_ERR << "Unable to read shader file " << path;
        return "";
    }
}

uint32_t CompileShaders(const char **vs_text, const char **fs_text)
{
    GLint status;
    char infoLog[1024] = {0};

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, vs_text, NULL);
    glCompileShader(vs);
    glGetShaderInfoLog(vs, 1024, NULL, infoLog);
    if (*infoLog) {
        LOG_DEBUG << infoLog;
        memset(infoLog, 0, 1024);
    }
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        LOG_ERR << "Vertex shader compilation failed";
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, fs_text, NULL);
    glCompileShader(fs);
    glGetShaderInfoLog(fs, 1024, NULL, infoLog);
    if (*infoLog) {
        LOG_DEBUG << infoLog;
        memset(infoLog, 0, 1024);
    }
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        LOG_ERR << "Fragment shader compilation failed";
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);
    glGetProgramInfoLog(program, 1024, NULL, infoLog);
    if (*infoLog) {
        LOG_DEBUG << infoLog;
    }
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        LOG_ERR << "Shader program link failed";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    CheckGLError();

    return program;
}


