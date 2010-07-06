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
#ifndef TEXTURE_2D_H
#define TEXTURE_2D_H

#include <GL/glew.h>

#include <string>
#include "textureFormat.h"
#include "textureParams.h"

template<typename T = float> 
  class Texture2D {
 public:

 Texture2D(const TextureFormat &tf=TextureFormat(),
           const TextureParams &tp=TextureParams(),
           T* map=NULL,
           int id=-1);

 Texture2D(const Texture2D &tex);

 ~Texture2D();

 inline GLuint        id    () const;
 inline TextureFormat format() const;
 inline TextureParams params() const;
 inline void          bind  () const;

 protected:
 GLuint        _id;
 TextureFormat _format;
 TextureParams _params;
};

template<typename T>
inline GLuint Texture2D<T>::id() const {
  return _id;
}

template<typename T>
inline TextureFormat Texture2D<T>::format() const {
  return _format;
}

template<typename T>
inline TextureParams Texture2D<T>::params() const {
  return _params;
}

template<typename T>
inline void Texture2D<T>::bind() const {

  glBindTexture(_format.target(),_id);
}

template<typename T>
Texture2D<T>::Texture2D(const Texture2D<T> &tex)
: _id(tex.id()),
  _format(tex.format()),
  _params(tex.params()) {

}

template<typename T>
Texture2D<T>::Texture2D(const TextureFormat &tf,const TextureParams &tp,T* map,int id)
: _id(id),
  _format(tf),
  _params(tp) {

  assert(_format.target()==GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_2D);

  if(id<0 || glIsTexture(id)==GL_FALSE) {
    glGenTextures(1,&_id);
  } else {
    _id = id;
  }

  glBindTexture(_format.target(),_id);
    
  if(_format.mipmapmode()==TextureFormat::MIPMAP_GLU_AUTOM) {
      
    gluBuild2DMipmaps(_format.target(),
                      _format.internalformat(),
                      _format.width(),
                      _format.height(),
                      _format.format(),
                      _format.type(),
                      (const GLvoid *)map);
  } else {
    
    glTexImage2D(_format.target(),
                 _format.level(),
                 _format.internalformat(),
                 _format.width(),
                 _format.height(),
                 _format.border(),
                 _format.format(),
                 _format.type(),
                 (const GLvoid *)map);
   
    if(_format.mipmapmode()==TextureFormat::MIPMAP_FBO_AUTOM) {
      assert(map==NULL || map==0);
      glGenerateMipmapEXT(_format.target());
    }
    
  }

  glTexParameteri(_format.target(),GL_TEXTURE_MIN_FILTER,_params.minfilter());
  glTexParameteri(_format.target(),GL_TEXTURE_MAG_FILTER,_params.maxfilter()); 
  glTexParameteri(_format.target(),GL_TEXTURE_WRAP_S,_params.wraps());
  glTexParameteri(_format.target(),GL_TEXTURE_WRAP_T,_params.wrapt());

  //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,_params.mode());
  }
  
template<typename T>
Texture2D<T>::~Texture2D() {	
  glDeleteTextures(1,&_id);
}

typedef Texture2D<float>         FloatTexture2D;
typedef Texture2D<unsigned char> UbyteTexture2D;
typedef Texture2D<unsigned int>  UintTexture2D;

#endif // TEXTURE_2D_H
