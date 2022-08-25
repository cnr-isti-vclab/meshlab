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
#include "textureFormat.h"


TextureFormat::TextureFormat(GLenum  target,
			     GLsizei width,
			     GLsizei height,
			     GLenum  internalformat,
			     GLenum  format,
			     GLenum  type,
			     GLsizei depth,
			     int     mipmapmode,
			     GLint   level,
			     GLint   border)
  : _target(target),
    _width(width),
    _height(height),
    _internalformat(internalformat),
    _format(format),
    _type(type),
    _depth(depth),
    _mipmapmode(mipmapmode),
    _level(level),
    _border(border) {
  
  
    }

TextureFormat::TextureFormat(const TextureFormat &tf)
  : _target(tf.target()),
    _width(tf.width()),
    _height(tf.height()),
    _internalformat(tf.internalformat()),
    _format(tf.format()),
    _type(tf.type()),
    _depth(tf.depth()),
    _mipmapmode(tf.mipmapmode()),
    _level(tf.level()),
    _border(tf.border()) {
  
}

