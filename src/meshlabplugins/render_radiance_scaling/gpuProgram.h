/****************************************************************************
* Render Radiance Scaling                                                   *
* Meshlab's plugin                                                          *
*                                                                           *
* Copyright(C) 2010                                                         *
* Vergne Romain, Dumas Olivier                                              *
* INRIA - Institut Nationnal de Recherche en Informatique et Automatique    *
*                                                                           *
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
#ifndef GPUPROGRAM_H
#define GPUPROGRAM_H

#include <GL/glew.h>
#include <iostream>
#include "gpuShader.h"
#include <string>
#include <vector>
#include <map>

class GPUProgram {
    
 public:
  GPUProgram(GPUShader *vs=NULL,GPUShader *fs=NULL,GPUShader *geom=NULL,
             int inputGeometry=0,int outputGeometry=0,int outVertices=0);

  GPUProgram(const std::string &vsFile="",
             const std::string &fsFile="",
             const std::string &gsFile="",
             int inputGeometry=0,
             int outputGeometry=0,
             int outVertices=0);
    
  ~GPUProgram();
    
  void addUniform(const std::string &uniformName);
  void addAttribute(const std::string &attributeName);
  void reload();

  inline GLuint id() const;
  bool haveShaderOfType(SHADER_TYPE type);
  std::string filename(SHADER_TYPE type);

  inline void enable();
  inline void disable();

  inline GLint getUniformLocation(const std::string &uniformName);
  inline GLint getAttributeLocation(const std::string &attributeName);

  inline void setUniform1f(const std::string &uniformName,GLfloat v);
  inline void setUniform2f(const std::string &uniformName,GLfloat v1,GLfloat v2);
  inline void setUniform3f(const std::string &uniformName,GLfloat v1,GLfloat v2,GLfloat v3);
  inline void setUniform4f(const std::string &uniformName,GLfloat v1,GLfloat v2,GLfloat v3,GLfloat v4);
  inline void setUniform1f(GLint loc,GLfloat v);
  inline void setUniform2f(GLint loc,GLfloat v1,GLfloat v2);
  inline void setUniform3f(GLint loc,GLfloat v1,GLfloat v2,GLfloat v3);
  inline void setUniform4f(GLint loc,GLfloat v1,GLfloat v2,GLfloat v3,GLfloat v4);

  inline void setUniform1i(const std::string &uniformName,GLint v);
  inline void setUniform2i(const std::string &uniformName,GLint v1,GLint v2);
  inline void setUniform3i(const std::string &uniformName,GLint v1,GLint v2,GLint v3);
  inline void setUniform4i(const std::string &uniformName,GLint v1,GLint v2,GLint v3,GLint v4);
  inline void setUniform1i(GLint loc,GLint v);
  inline void setUniform2i(GLint loc,GLint v1,GLint v2);
  inline void setUniform3i(GLint loc,GLint v1,GLint v2,GLint v3);
  inline void setUniform4i(GLint loc,GLint v1,GLint v2,GLint v3,GLint v4);

  inline void setUniform1fv(const std::string &uniformName,GLfloat *v,GLsizei count=1);
  inline void setUniform2fv(const std::string &uniformName,GLfloat *v,GLsizei count=1);
  inline void setUniform3fv(const std::string &uniformName,GLfloat *v,GLsizei count=1);
  inline void setUniform4fv(const std::string &uniformName,GLfloat *v,GLsizei count=1);
  inline void setUniform1fv(GLint loc,GLfloat *v,GLsizei count=1);
  inline void setUniform2fv(GLint loc,GLfloat *v,GLsizei count=1);
  inline void setUniform3fv(GLint loc,GLfloat *v,GLsizei count=1);
  inline void setUniform4fv(GLint loc,GLfloat *v,GLsizei count=1);

  inline void setUniform1iv(const std::string &uniformName,GLint *v,GLsizei count=1);
  inline void setUniform2iv(const std::string &uniformName,GLint *v,GLsizei count=1);
  inline void setUniform3iv(const std::string &uniformName,GLint *v,GLsizei count=1);
  inline void setUniform4iv(const std::string &uniformName,GLint *v,GLsizei count=1);
  inline void setUniform1iv(GLint loc,GLint *v,GLsizei count=1);
  inline void setUniform2iv(GLint loc,GLint *v,GLsizei count=1);
  inline void setUniform3iv(GLint loc,GLint *v,GLsizei count=1);
  inline void setUniform4iv(GLint loc,GLint *v,GLsizei count=1);
 
  inline void setUniformMatrix2fv(const std::string &uniformName,GLfloat *v,GLsizei count=1,GLboolean transpose=false);
  inline void setUniformMatrix3fv(const std::string &uniformName,GLfloat *v,GLsizei count=1,GLboolean transpose=false);
  inline void setUniformMatrix4fv(const std::string &uniformName,GLfloat *v,GLsizei count=1,GLboolean transpose=false);
  inline void setUniformMatrix2fv(GLint loc,GLfloat *v,GLsizei count=1,GLboolean transpose=false);
  inline void setUniformMatrix3fv(GLint loc,GLfloat *v,GLsizei count=1,GLboolean transpose=false);
  inline void setUniformMatrix4fv(GLint loc,GLfloat *v,GLsizei count=1,GLboolean transpose=false);

  inline void setUniformTexture(const std::string &uniformName,GLint num,GLenum type,GLuint textureName);
  inline void setUniformTexture(GLint loc,GLint num,GLenum type,GLuint textureName);

  inline void  setUniformBuffer(const std::string &uniformName,GLuint buffer);
  inline void  setUniformBuffer(GLint loc,GLuint buffer);
  inline GLint getUniformBufferSize(const std::string &uniformName);
  inline GLint getUniformBufferSize(GLint loc);

 protected:
  void attach();
  bool link();
  bool attachAndLink();
  void detach();
  void setGeometryParameters(int inputGeometry,int outputGeometry,int outVertices);
    
 private:
        
  GPUShader* _vs;
  GPUShader* _fs;
  GPUShader* _gs;
  GLuint     _programId;
    
  std::map<std::string,GLint>                _uniformLocations;
  std::map<std::string,GLint>                _attributeLocations;
  std::map<GLuint,std::pair<GLenum,GLenum> > _textures;

  int _inputGeometry;
  int _outputGeometry;
  int _outVertices;
};
  
inline void GPUProgram::enable() {
  glUseProgramObjectARB(_programId);
    
  for(std::map<GLuint,std::pair<GLenum,GLenum> >::iterator i=_textures.begin();i!=_textures.end();++i) {
    glActiveTexture((*i).second.first);
    glBindTexture((*i).second.second,(*i).first);
    glEnable((*i).second.second);
  }

}

inline void GPUProgram::disable() {
  for(std::map<GLuint,std::pair<GLenum,GLenum> >::reverse_iterator i=_textures.rbegin();i!=_textures.rend();++i) {
    glActiveTexture((*i).second.first);
    glDisable((*i).second.second);
  }

  glUseProgramObjectARB(0);
}

inline GLuint GPUProgram::id() const {
  return _programId;
}
  
inline void GPUProgram::setUniform1f(const std::string &uniformName,GLfloat v) {
  glUniform1f(_uniformLocations[uniformName],v);
}

inline void GPUProgram::setUniform2f(const std::string &uniformName,GLfloat v1,GLfloat v2) {
  glUniform2f(_uniformLocations[uniformName],v1,v2);
}

inline void GPUProgram::setUniform3f(const std::string &uniformName,GLfloat v1,GLfloat v2,GLfloat v3) {
  glUniform3f(_uniformLocations[uniformName],v1,v2,v3);
}
      
inline void GPUProgram::setUniform4f(const std::string &uniformName,GLfloat v1,GLfloat v2,GLfloat v3,GLfloat v4) {
  glUniform4f(_uniformLocations[uniformName],v1,v2,v3,v4);
}

inline void GPUProgram::setUniform1i(const std::string &uniformName,GLint v) {
  glUniform1i(_uniformLocations[uniformName],v);
}

inline void GPUProgram::setUniform2i(const std::string &uniformName,GLint v1,GLint v2) {
  glUniform2i(_uniformLocations[uniformName],v1,v2);
}

inline void GPUProgram::setUniform3i(const std::string &uniformName,GLint v1,GLint v2,GLint v3) {
  glUniform3i(_uniformLocations[uniformName],v1,v2,v3);
}
      
inline void GPUProgram::setUniform4i(const std::string &uniformName,GLint v1,GLint v2,GLint v3,GLint v4) {
  glUniform4i(_uniformLocations[uniformName],v1,v2,v3,v4);
}

inline void GPUProgram::setUniform1fv(const std::string &uniformName,GLfloat *v,GLsizei count) {
  glUniform1fv(_uniformLocations[uniformName],count,v);
}

inline void GPUProgram::setUniform2fv(const std::string &uniformName,GLfloat *v,GLsizei count) {
  glUniform2fv(_uniformLocations[uniformName],count,v);
}

inline void GPUProgram::setUniform3fv(const std::string &uniformName,GLfloat *v,GLsizei count) {
  glUniform3fv(_uniformLocations[uniformName],count,v);
}

inline void GPUProgram::setUniform4fv(const std::string &uniformName,GLfloat *v,GLsizei count) {
  glUniform4fv(_uniformLocations[uniformName],count,v);
}

inline void GPUProgram::setUniform1iv(const std::string &uniformName,GLint *v,GLsizei count) {
  glUniform1iv(_uniformLocations[uniformName],count,v);
}

inline void GPUProgram::setUniform2iv(const std::string &uniformName,GLint *v,GLsizei count) {
  glUniform2iv(_uniformLocations[uniformName],count,v);
}

inline void GPUProgram::setUniform3iv(const std::string &uniformName,GLint *v,GLsizei count) {
  glUniform3iv(_uniformLocations[uniformName],count,v);
}

inline void GPUProgram::setUniform4iv(const std::string &uniformName,GLint *v,GLsizei count) {
  glUniform4iv(_uniformLocations[uniformName],count,v);
}

inline void GPUProgram::setUniformMatrix2fv(const std::string &uniformName,GLfloat *v,GLsizei count,GLboolean transpose) {
  glUniformMatrix2fv(_uniformLocations[uniformName],count,transpose,v);
}

inline void GPUProgram::setUniformMatrix3fv(const std::string &uniformName,GLfloat *v,GLsizei count,GLboolean transpose) {
  glUniformMatrix3fv(_uniformLocations[uniformName],count,transpose,v);
}

inline void GPUProgram::setUniformMatrix4fv(const std::string &uniformName,GLfloat *v,GLsizei count,GLboolean transpose) {
  glUniformMatrix4fv(_uniformLocations[uniformName],count,transpose,v);
}
  
inline void GPUProgram::setUniformTexture(const std::string &uniformName,GLint num,GLenum type,GLuint textureName) {
  GLenum textureNum;

  const std::map<GLuint,std::pair<GLenum,GLenum> >::iterator it = _textures.find(textureName);

  if(it==_textures.end()) {
    textureNum = GL_TEXTURE0+_textures.size();
  } else {
    textureNum = (*it).second.first;
  }
    
  glPushAttrib(GL_TEXTURE_BIT);
  glActiveTexture(textureNum);
  glBindTexture(type,textureName);
  glEnable(type);
    
  glUniform1i(_uniformLocations[uniformName],num);
  _textures[textureName] = std::pair<GLenum,GLenum>(textureNum,type);

  glDisable(type);
  glPopAttrib();
}

inline void GPUProgram::setUniform1f(GLint loc,GLfloat v) {
  glUniform1f(loc,v);
}

inline void GPUProgram::setUniform2f(GLint loc,GLfloat v1,GLfloat v2) {
  glUniform2f(loc,v1,v2);
}

inline void GPUProgram::setUniform3f(GLint loc,GLfloat v1,GLfloat v2,GLfloat v3) {
  glUniform3f(loc,v1,v2,v3);
}
      
inline void GPUProgram::setUniform4f(GLint loc,GLfloat v1,GLfloat v2,GLfloat v3,GLfloat v4) {
  glUniform4f(loc,v1,v2,v3,v4);
}

inline void GPUProgram::setUniform1i(GLint loc,GLint v) {
  glUniform1i(loc,v);
}

inline void GPUProgram::setUniform2i(GLint loc,GLint v1,GLint v2) {
  glUniform2i(loc,v1,v2);
}

inline void GPUProgram::setUniform3i(GLint loc,GLint v1,GLint v2,GLint v3) {
  glUniform3i(loc,v1,v2,v3);
}
      
inline void GPUProgram::setUniform4i(GLint loc,GLint v1,GLint v2,GLint v3,GLint v4) {
  glUniform4i(loc,v1,v2,v3,v4);
}

inline void GPUProgram::setUniform1fv(GLint loc,GLfloat *v,GLsizei count) {
  glUniform1fv(loc,count,v);
}

inline void GPUProgram::setUniform2fv(GLint loc,GLfloat *v,GLsizei count) {
  glUniform2fv(loc,count,v);
}

inline void GPUProgram::setUniform3fv(GLint loc,GLfloat *v,GLsizei count) {
  glUniform3fv(loc,count,v);
}

inline void GPUProgram::setUniform4fv(GLint loc,GLfloat *v,GLsizei count) {
  glUniform4fv(loc,count,v);
}

inline void GPUProgram::setUniform1iv(GLint loc,GLint *v,GLsizei count) {
  glUniform1iv(loc,count,v);
}

inline void GPUProgram::setUniform2iv(GLint loc,GLint *v,GLsizei count) {
  glUniform2iv(loc,count,v);
}

inline void GPUProgram::setUniform3iv(GLint loc,GLint *v,GLsizei count) {
  glUniform3iv(loc,count,v);
}

inline void GPUProgram::setUniform4iv(GLint loc,GLint *v,GLsizei count) {
  glUniform4iv(loc,count,v);
}

inline void GPUProgram::setUniformMatrix2fv(GLint loc,GLfloat *v,GLsizei count,GLboolean transpose) {
  glUniformMatrix2fv(loc,count,transpose,v);
}

inline void GPUProgram::setUniformMatrix3fv(GLint loc,GLfloat *v,GLsizei count,GLboolean transpose) {
  glUniformMatrix3fv(loc,count,transpose,v);
}

inline void GPUProgram::setUniformMatrix4fv(GLint loc,GLfloat *v,GLsizei count,GLboolean transpose) {
  glUniformMatrix4fv(loc,count,transpose,v);
}
  
inline void GPUProgram::setUniformTexture(GLint loc,GLint num,GLenum type,GLuint textureName) {
  GLenum textureNum;

  const std::map<GLuint,std::pair<GLenum,GLenum> >::iterator it = _textures.find(textureName);

  if(it==_textures.end()) {
    textureNum = GL_TEXTURE0+_textures.size();
  } else {
    textureNum = (*it).second.first;
  }
    
  glPushAttrib(GL_TEXTURE_BIT);
  glActiveTexture(textureNum);
  glBindTexture(type,textureName);
  glEnable(type);
    
  glUniform1i(loc,num);
  _textures[textureName] = std::pair<GLenum,GLenum>(textureNum,type);

  glDisable(type);
  glPopAttrib();
}

inline GLint GPUProgram::getUniformLocation(const std::string &uniformName) {
  return _uniformLocations[uniformName];
}

inline GLint GPUProgram::getAttributeLocation(const std::string &attributeName) {
  return _attributeLocations[attributeName];
}

inline void GPUProgram::setUniformBuffer(const std::string &uniformName,GLuint buffer) {
  glUniformBufferEXT(_programId,_uniformLocations[uniformName],buffer);
}

inline void GPUProgram::setUniformBuffer(GLint loc,GLuint buffer) {
  glUniformBufferEXT(_programId,loc,buffer);
}

inline GLint GPUProgram::getUniformBufferSize(const std::string &uniformName) {
  return glGetUniformBufferSizeEXT(_programId,_uniformLocations[uniformName]);
}

inline GLint GPUProgram::getUniformBufferSize(GLint loc) {
  return glGetUniformBufferSizeEXT(_programId,loc);
}

static inline
void CheckErrorsGL(const char* location=NULL,std::ostream& ostr = std::cerr) {
    
  GLuint errnum;
  const char *errstr;
  while((errnum=glGetError())) {
    ostr << " **************************************************** " << std::endl;
    ostr << " Checking OpenGL Error " << std::endl;
      
    std::cout << " **************************************************** " << std::endl;
    std::cout << " Checking OpenGL Error " << std::endl;
      
    errstr = reinterpret_cast<const char *>(gluErrorString(errnum));
    ostr << errstr;
      
    if(location) ostr << " at " << location;
    ostr << std::endl;
    std::cout << " **************************************************** " << std::endl;
    ostr << " **************************************************** " << std::endl;
  }
}

#endif // GPUPROGRAM_H
