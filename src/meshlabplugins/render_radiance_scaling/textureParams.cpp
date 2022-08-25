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
#include "textureParams.h"

TextureParams::TextureParams(GLint minfilter,
			     GLint maxfilter,
			     GLint wrapr,
			     GLint wraps,
			     GLint wrapt,
			     GLint mode) 
  : _minfilter(minfilter),
    _maxfilter(maxfilter),
    _wrapr(wrapr),
    _wraps(wraps),
    _wrapt(wrapt),
    _mode(mode) {
  
  }

TextureParams::TextureParams(const TextureParams &tp) 
  : _minfilter(tp.minfilter()),
    _maxfilter(tp.maxfilter()),
    _wrapr(tp.wrapr()),
    _wraps(tp.wraps()),
    _wrapt(tp.wrapt()),
    _mode(tp.mode()) {
  
}

