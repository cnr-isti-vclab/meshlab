//***************************************************************************
//
//  Copyright (c) 2001 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
// IFXOpenGL.h
#ifndef IFX_OPENGL_H
#define IFX_OPENGL_H

/**
 *  This file will incorporate the IFXOpenGL interface, along with
 *  the platform specific IFXOpenGLXXX specialization and the platform
 *  native OpenGL headers (gl.h, etc.)
 */

// The platform dependent files will include at least
// OpenGL 1.1 support (possibly 1.2) and the windowing
// interface layer (wgl, agl, glX) needed for the platform.
#include "IFXOpenGLOS.h"

#endif // IFX_OPENGL_H

// END OF FILE
