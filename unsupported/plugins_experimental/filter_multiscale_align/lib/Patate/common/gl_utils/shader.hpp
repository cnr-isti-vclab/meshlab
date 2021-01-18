/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "shader.h"


namespace PatateCommon
{


static const char* pVSName = "VS";
static const char* pTessCSName = "TessCS";
static const char* pTessESName = "TessES";
static const char* pGSName = "GS";
static const char* pFSName = "FS";

inline const char* ShaderType2ShaderName(GLuint _Type)
{
    switch (_Type)
    {
    case GL_VERTEX_SHADER:
        return pVSName;
    case GL_TESS_CONTROL_SHADER:
        return pTessCSName;
    case GL_TESS_EVALUATION_SHADER:
        return pTessESName;
    case GL_GEOMETRY_SHADER:
        return pGSName;
    case GL_FRAGMENT_SHADER:
        return pFSName;
    default:
        assert(0);
    }

    return NULL;
}

Shader::Shader()
    : m_shaderProg(0), m_status(UNINITIALIZED)
{
}


Shader::~Shader()
{
    if(getShaderId())
    {
        destroy();
    }
}


bool Shader::create()
{
    m_shaderProg = glCreateProgram();

    if (m_shaderProg == 0)
    {
        fprintf(stderr, "Error creating shader program\n");
        return false;
    }

    m_status = NOT_COMPILED;
    return true;
}


void Shader::destroy()
{
    PATATE_ASSERT_NO_GL_ERROR();

    glDeleteProgram(m_shaderProg);

    if(glGetError() == GL_NO_ERROR)
        m_shaderProg = 0;
    m_status = UNINITIALIZED;
}


void Shader::use()
{
    glUseProgram(m_shaderProg);
}


void Shader::setGLSLVersionHeader(const std::string& header)
{
    m_versionHeader = header;
}


bool Shader::addShader(GLenum _ShaderType, const char* _pShaderText)
{
    PATATE_ASSERT_NO_GL_ERROR();

    GLuint ShaderObj = glCreateShader(_ShaderType);

    if (ShaderObj == 0)
    {
        fprintf(stderr, "Error creating shader type %d\n", _ShaderType);
        return false;
    }

    m_shaderObjList.push_back(ShaderObj);

    const GLchar* p[2];
    p[0] = m_versionHeader.empty()? 0: &m_versionHeader[0];
    p[1] = _pShaderText;
    GLint Lengths[2];
    Lengths[0] = m_versionHeader.size();
    Lengths[1] = strlen(_pShaderText);
    glShaderSource(ShaderObj, 2, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling %s: '%s'\n", ShaderType2ShaderName(_ShaderType), InfoLog);
        return false;
    }

    glAttachShader(m_shaderProg, ShaderObj);

    return glGetError() == GL_NO_ERROR;
}


bool Shader::addShaderFromFile(GLenum _ShaderType, const char* _pFilename)
{
    FILE* fp;
    size_t filesize;
    char* pShaderText;

    fp = fopen(_pFilename, "rb");

    if(!fp)
    {
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    pShaderText = new char[filesize + 1];
    if(!pShaderText)
    {
        return 0;
    }

    fread(pShaderText, 1, filesize, fp);
    pShaderText[filesize] = 0;
    fclose(fp);

    bool res = addShader(_ShaderType, pShaderText);

    delete [] pShaderText;

    return res;
}


void Shader::clearShaderList()
{
    for (ShaderObjList::iterator it = m_shaderObjList.begin() ; it != m_shaderObjList.end() ; it++)
    {
        glDetachShader(m_shaderProg, *it);
        glDeleteShader(*it);
    }
    m_shaderObjList.clear();
}


bool Shader::finalize()
{
    PATATE_ASSERT_NO_GL_ERROR();

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    m_status = COMPILATION_FAILED;

    glLinkProgram(m_shaderProg);

    glGetProgramiv(m_shaderProg, GL_LINK_STATUS, &Success);
    if (Success == 0)
    {
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        return false;
    }

    glValidateProgram(m_shaderProg);
    glGetProgramiv(m_shaderProg, GL_VALIDATE_STATUS, &Success);
    if (!Success)
    {
        glGetProgramInfoLog(m_shaderProg, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        return false;
    }

    if(glGetError() == GL_NO_ERROR)
    {
        clearShaderList();
        m_status = COMPILATION_SUCCESSFULL;
    }

    return m_status == COMPILATION_SUCCESSFULL;
}


void Shader::bindAttributeLocation(const char* name, unsigned location)
{
    glBindAttribLocation(m_shaderProg, location, name);
}


GLint Shader::getUniformLocation(const char* _pUniformName)
{
    GLint location = glGetUniformLocation(m_shaderProg, _pUniformName);

    if (location < 0)
    {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", _pUniformName);
    }

    return location;
}

GLint Shader::getProgramParam(GLint _param)
{
    GLint ret;
    glGetProgramiv(m_shaderProg, _param, &ret);
    return ret;
}


}
