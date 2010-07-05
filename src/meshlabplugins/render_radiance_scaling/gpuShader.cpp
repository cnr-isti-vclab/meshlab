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
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include <QtGui>
#include <QTextStream>
#include <QString>
#include <QFile>

#include "gpuShader.h"

using namespace std;
      
GPUShader::GPUShader(SHADER_TYPE type,const string &filename,bool printLog)
  : _filename(filename),
    _type(type),
    _shaderId(0),
    _printLog(printLog) {
  _created = createShader();

  loadAndCompile();       
}
            
GPUShader::~GPUShader() {
  if(_created){
    glDeleteShader(_shaderId);
  }
}

bool GPUShader::load() {
  QString res;
  QFile f(QString(_filename.c_str()));

  if (!f.open(QFile::ReadOnly)) {
    std::cerr << "failed to load shader file " << _filename << "\n";
    return false;
  }

  QTextStream stream(&f);
  res = stream.readAll();
  f.close();

  string tmp = res.toStdString();
  const char *s = tmp.c_str();

  glShaderSource(_shaderId,1,&s,NULL);
  
  return true;
}
  

bool GPUShader::compile() { 
  glCompileShader(_shaderId);

  if(_printLog)
    printInfoLog();
  
  return true;
}

void GPUShader::printInfoLog() {
  int   infologLength = 0;
  int   charsWritten  = 0;
  char* infolog;
    
  glGetObjectParameterivARB(_shaderId,GL_OBJECT_INFO_LOG_LENGTH_ARB,&infologLength);
    
  if(infologLength>0) {
    infolog = (char*)malloc(infologLength);
    glGetInfoLogARB(_shaderId,infologLength,&charsWritten,infolog);
    if(infolog[0]!='\0') {
      printf("InfoLog ---> %s\n",_filename.c_str());
      printf("%s",infolog);
    }
    free(infolog);
  }
}

bool GPUShader::loadAndCompile() {
  return _created && load() && compile();
}

bool GPUShader::createShader() {
  switch(_type) {

  case VERT:
    if(GLEW_ARB_vertex_shader)
      _shaderId = glCreateShader(GL_VERTEX_SHADER);
    else { 
      cout << "Warning : vertex shader not supported !" << endl; 
      return false;
    }
    break;
      
  case FRAG:
    if(GLEW_ARB_fragment_shader){
      _shaderId = glCreateShader(GL_FRAGMENT_SHADER);
    }
    else { 
      cout << "Warning : fragment shader not supported !" << endl; 
      return false;
    }
    break;
      
  case GEOM:
#ifdef GL_EXT_geometry_shader4
    if(GL_EXT_geometry_shader4){
      _shaderId = glCreateShader(GL_GEOMETRY_SHADER_EXT);
    }
    else { 
      cout << "Warning : geometry shader not supported !" << endl;  
      return false;
    }
#else 
    cout << "Warning : geometry shader not supported !" << endl;  
    return false;
#endif
    break;
      
  default:
    cout << "Warning : unknown shader type !" << endl;
    return false;
    break;
  }
    
  if(_shaderId==0) {
    cout << "Warning : shader " << _filename << " is not created !" << endl;
    return false;
  }
      
  return true;
} 
