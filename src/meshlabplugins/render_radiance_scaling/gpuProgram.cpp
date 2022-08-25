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
#include "gpuProgram.h"
#include <iostream>

using namespace std;
      

GPUProgram::GPUProgram(GPUShader* vs,GPUShader* fs,GPUShader *gs,
                       int inputGeometry,int outputGeometry,int outVertices)
  : _vs(vs),
    _fs(fs),
    _gs(gs),
    _inputGeometry(inputGeometry),
    _outputGeometry(outputGeometry),
    _outVertices(outVertices) {

  _programId = glCreateProgram();
  setGeometryParameters(_inputGeometry,_outputGeometry,_outVertices);
  attachAndLink();
    }

GPUProgram::GPUProgram(const string &vsFile,
                       const string &fsFile,
                       const string &gsFile,
                       int inputGeometry,
                       int outputGeometry,
                       int outVertices)
  :_inputGeometry(inputGeometry),
   _outputGeometry(outputGeometry),
   _outVertices(outVertices) {

  _vs = _fs = _gs = NULL;

  if(vsFile!="")
    _vs = new GPUShader(VERT,vsFile);

  if(fsFile!="")
    _fs = new GPUShader(FRAG,fsFile);

  if(gsFile!="")
    _gs = new GPUShader(GEOM,gsFile);

  _programId = glCreateProgram();
  setGeometryParameters(_inputGeometry,_outputGeometry,_outVertices);
  attachAndLink();
   }
      
GPUProgram::~GPUProgram() {
      
  detach();
    
  if(_vs!=NULL) {
    delete _vs;
  }

  if(_fs!=NULL) {
    delete _fs;
  }

  if(_gs!=NULL) {
    delete _gs;
  }
    
  glDeleteProgram(_programId);
}

void GPUProgram::setGeometryParameters(int inputGeometry,int outputGeometry,int outVertices) {
#ifdef GL_EXT_geometry_shader4
  if(GL_EXT_geometry_shader4 && _gs!=NULL && _gs->id()!=0) {
    glProgramParameteriEXT(_programId,GL_GEOMETRY_INPUT_TYPE_EXT,inputGeometry);
    glProgramParameteriEXT(_programId,GL_GEOMETRY_OUTPUT_TYPE_EXT,outputGeometry);
    glProgramParameteriEXT(_programId,GL_GEOMETRY_VERTICES_OUT_EXT,outVertices);
  }
#endif
}

void GPUProgram::attach() {
  if(_vs!=NULL) {
    glAttachShader(_programId,_vs->id()); 
  }

  if(_fs!=NULL) {
    glAttachShader(_programId,_fs->id());
  }

  if(_gs!=NULL) {
    glAttachShader(_programId,_gs->id());       
  }
}

void GPUProgram::detach() {
        
  if(_vs!=NULL) {
    glDetachShader(_programId,_vs->id()); 
  }

  if(_fs!=NULL) {
    glDetachShader(_programId,_fs->id());
  }

  if(_gs!=NULL) {
    glDetachShader(_programId,_gs->id());  
  }
}

bool GPUProgram::link() {
  int linked = 1;

  glLinkProgram(_programId);
      
  glGetObjectParameterivARB(_programId,GL_OBJECT_LINK_STATUS_ARB,&linked);
       
  if(linked)
    return true;
    
  return false;
}

bool GPUProgram::attachAndLink() {
  attach();
  return link();
}

void GPUProgram::reload() {
      
  detach();

  bool allOk = true;
  if(_vs!=NULL) {
    allOk = allOk && _vs->loadAndCompile();      
  }

  if(_fs!=NULL) {
    allOk = allOk && _fs->loadAndCompile();
  }

  if(_gs!=NULL) {
    allOk = allOk && _gs->loadAndCompile();
  }
  
  if(!allOk){
    std::cout << "reload fail, maybe missing file" << std::endl;
  }
  setGeometryParameters(_inputGeometry,_outputGeometry,_outVertices);
  attachAndLink();

  // reload uniforms 
  for(map<string,GLint>::iterator i=_uniformLocations.begin();i!=_uniformLocations.end();i++) {
    _uniformLocations[(*i).first] = glGetUniformLocation(_programId,(*i).first.c_str());
  }

  // reload attributes
  for(map<string,GLint>::iterator i=_attributeLocations.begin();i!=_attributeLocations.end();i++) {
    _uniformLocations[(*i).first] = glGetAttribLocation(_programId,(*i).first.c_str());
  }

  // free textures 
  _textures.clear();
}

void GPUProgram::addUniform(const string &uniformName) {
  GLint location = glGetUniformLocation(_programId,uniformName.c_str());  
    
  _uniformLocations[uniformName] = location;
}

void GPUProgram::addAttribute(const string &attributeName) {
  GLint location = glGetAttribLocation(_programId,attributeName.c_str());  
    
  _attributeLocations[attributeName] = location;
}

bool GPUProgram::haveShaderOfType(SHADER_TYPE type) {
    
  if(type==VERT)
    return _vs!=NULL;
    
  if(type==FRAG)
    return _fs!=NULL;
    
  if(type==GEOM)
    return _gs!=NULL;
    
  cout << "Warning : unknown type !" << endl;
  
  return false;
}

string GPUProgram::filename(SHADER_TYPE type) {
    
  if(type==VERT && _vs!=NULL)
    return _vs->filename();
    
  if(type==FRAG && _fs!=NULL)
    return _fs->filename();
    
  if(type==GEOM && _gs!=NULL)
    return _gs->filename();

  cout << "Warning : unknown type !" << endl;
    
  return "";
}
