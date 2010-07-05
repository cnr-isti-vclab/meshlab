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
#ifndef GPUSHADER_H
#define GPUSHADER_H

#include <GL/glew.h>
#include <string>

enum SHADER_TYPE {VERT,FRAG,GEOM};

class GPUShader {

 public: 
    
  GPUShader(SHADER_TYPE type,const std::string &filename,bool printLog=true);
  ~GPUShader();
    
  bool load();         
  bool compile();
  bool loadAndCompile();
    
  inline GLuint id() const;
  inline SHADER_TYPE type() const;
  inline std::string filename() const;

 protected:
  std::string _filename;
   
 private:
  SHADER_TYPE _type; 
    
  GLuint      _shaderId;
  bool        _printLog;    
  bool        _created;

  bool createShader();
  void printInfoLog();
};
  
inline GLuint GPUShader::id() const {
  return _shaderId;
}
  
inline SHADER_TYPE GPUShader::type() const {
  return _type;
}

inline std::string GPUShader::filename() const {
  return _filename;
}

#endif // GPUSHADER
