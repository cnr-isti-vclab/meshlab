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
#ifndef TEXTURE_PARAMS_H
#define TEXTURE_PARAMS_H

#include <GL/glew.h>

class TextureParams {
 public:
  TextureParams(const TextureParams &tp);
  TextureParams(GLint minfilter = GL_LINEAR,
		GLint maxfilter = GL_LINEAR,
		GLint wrapr     = GL_CLAMP_TO_EDGE,
		GLint wraps     = GL_CLAMP_TO_EDGE,
		GLint wrapt     = GL_CLAMP_TO_EDGE,
		GLint mode      = GL_REPLACE);
  
  ~TextureParams() {}
  
  inline void setMinfilter(GLint minfilter) {_minfilter = minfilter;}
  inline void setMaxfilter(GLint maxfilter) {_maxfilter = maxfilter;}
  inline void setWrapr    (GLint wrapr    ) {_wrapr     = wrapr;    }
  inline void setWraps    (GLint wraps    ) {_wraps     = wraps;    }
  inline void setWrapt    (GLint wrapt    ) {_wrapt     = wrapt;    }
  inline void setMode     (GLint mode     ) {_mode      = mode;     }
  
  inline GLint minfilter() const {return _minfilter;}
  inline GLint maxfilter() const {return _maxfilter;}
  inline GLint wrapr    () const {return _wrapr;    }
  inline GLint wraps    () const {return _wraps;    }
  inline GLint wrapt    () const {return _wrapt;    }
  inline GLint mode     () const {return _mode;     }
  
 protected:
  GLint _minfilter;
  GLint _maxfilter;
  GLint _wrapr;
  GLint _wraps;
  GLint _wrapt;
  GLint _mode;
};


#endif // TEXTURE_PARAMS_H
