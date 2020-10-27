/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _PATATE_COMMON_GL_UTILS_SHADER_
#define _PATATE_COMMON_GL_UTILS_SHADER_

#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <list>


#include "macros.h"


namespace PatateCommon {

class Shader
{
public:
    enum Status
    {
        UNINITIALIZED,
        NOT_COMPILED,
        COMPILATION_SUCCESSFULL,
        COMPILATION_FAILED
    };

public:

    inline Shader();
    inline virtual ~Shader();

    inline bool create();
    inline void destroy();

    inline void use();

    inline Status status() const { return m_status; }

    inline void setGLSLVersionHeader(const std::string& header);
    inline bool addShaderFromFile(GLenum _ShaderType, const char* _pFilename);
    inline bool addShader(GLenum _ShaderType, const char* _pShaderText);
    inline void clearShaderList();
    inline bool finalize();

    inline GLuint getShaderId() { return m_shaderProg; }

    inline void bindAttributeLocation(const char* name, unsigned location);
    inline GLint getUniformLocation(const char* _pUniformName);
    inline GLint getProgramParam(GLint _param);
  
protected:
    GLuint m_shaderProg;
    
private:
    typedef std::list<GLuint> ShaderObjList;

    Status m_status;
    std::string m_versionHeader;
    ShaderObjList m_shaderObjList;
};

//#define INVALID_UNIFORM_LOCATION 0xFFFFFFFF


}

#include "shader.hpp"


#endif

