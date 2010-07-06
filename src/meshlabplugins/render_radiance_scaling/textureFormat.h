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

#ifndef TEXTURE_FORMAT_H
#define TEXTURE_FORMAT_H

#include <GL/glew.h>

class TextureFormat {
 public:
  static const int MIPMAP_MANUAL    = 0;
  static const int MIPMAP_GLU_AUTOM = 1;
  static const int MIPMAP_FBO_AUTOM = 2;
  
  TextureFormat(GLenum  target         = GL_TEXTURE_2D,
		GLsizei width          = 0,
		GLsizei height         = 0,
		GLenum  internalformat = GL_RGBA,
		GLenum  format         = GL_RGBA,
		GLenum  type           = GL_FLOAT,
		GLsizei depth          = 0,
		int     mipmapmode     = MIPMAP_MANUAL,
		GLint   level          = 0,
		GLint   border         = 0);
  
  TextureFormat(const TextureFormat &tf);
  
  inline void setTarget        (GLenum  target        ) {_target         = target;        } 
  inline void setWidth         (GLsizei width         ) {_width          = width;         }
  inline void setHeight        (GLsizei height        ) {_height         = height;        }
  inline void setInternalformat(GLenum  internalformat) {_internalformat = internalformat;}
  inline void setFormat        (GLenum  format        ) {_format         = format;        }
  inline void setType          (GLenum  type          ) {_type           = type;          }
  inline void setDepth         (GLsizei depth         ) {_depth          = depth;         }
  inline void setmipmapmode    (int     mipmapmode    ) {_mipmapmode     = mipmapmode;    }
  inline void setLevel         (GLint   level         ) {_level          = level;         }
  inline void setBorder        (GLint   border        ) {_border         = border;        }
  
  inline GLenum  target        () const {return _target;        }
  inline GLsizei width         () const {return _width;         }
  inline GLsizei height        () const {return _height;        }
  inline GLenum  internalformat() const {return _internalformat;}
  inline GLenum  format        () const {return _format;        }
  inline GLenum  type          () const {return _type;          }
  inline GLsizei depth         () const {return _depth;         }
  inline int     mipmapmode    () const {return _mipmapmode;    }
  inline GLint   level         () const {return _level;         }
  inline GLint   border        () const {return _border;        }
  
 protected:
  GLenum  _target;
  GLsizei _width;
  GLsizei _height;
  GLenum  _internalformat;
  GLenum  _format;
  GLenum  _type;
  GLsizei _depth;
  int     _mipmapmode;
  GLint   _level;
  GLint   _border;
};


#endif // TEXTURE_FORMAT_H
