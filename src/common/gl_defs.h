/****************************************************************************
 * MeshLab                                                           o o     *
 * An extendible mesh processor                                    o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005, 2006                                          \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
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

#ifndef GL_DEFS_H
#define GL_DEFS_H

#include "mlexception.h"

// This string is searched for by an error handler, so it must be included
// verbatim in all graphics-related errors.
static const char MESHLAB_GL_ERROR_MSG[] =
    "OpenGL extension initialization failed";
#ifdef MESHLAB_USE_GLAD

#include <glad/glad.h>

static inline bool initializeGLextensions_notThrowing() {
    if (!gladLoadGL()) {
        qWarning(MESHLAB_GL_ERROR_MSG);
        return false;
    }
    return true;
}

static inline void initializeGLextensions() {
    if (!gladLoadGL()) {
        throw MLException(QString("%1\n").arg(MESHLAB_GL_ERROR_MSG));
    }
}

#define MESHLAB_MAKE_EXTENSION_CHECKER(EXT)                                    \
    static inline bool glExtensionsHas##EXT() { return GLAD_GL_##EXT; }

#else

#include <common/gl_defs.h>

static inline bool initializeGLextensions_notThrowing() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        qWarning("%s: %s", MESHLAB_GL_ERROR_MSG,
                 (const char *)glewGetErrorString(err));
    }
    return err == GLEW_OK;
}

static inline void initializeGLextensions() {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw MLException(QString("%1: %2\n")
                              .arg(MESHLAB_GL_ERROR_MSG)
                              .arg((const char *)glewGetErrorString(err)));
    }
}

#define MESHLAB_MAKE_EXTENSION_CHECKER(EXT)                                    \
    static inline bool glExtensionsHas##EXT() { return GLEW_##EXT; }

#endif

MESHLAB_MAKE_EXTENSION_CHECKER(ARB_draw_buffers)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_fragment_program)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_fragment_shader)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_shader_objects)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_shading_language_100)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_texture_cube_map)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_texture_float)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_texture_non_power_of_two)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_vertex_buffer_object)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_vertex_program)
MESHLAB_MAKE_EXTENSION_CHECKER(ARB_vertex_shader)
MESHLAB_MAKE_EXTENSION_CHECKER(EXT_framebuffer_object)
MESHLAB_MAKE_EXTENSION_CHECKER(EXT_gpu_shader4)
MESHLAB_MAKE_EXTENSION_CHECKER(EXT_vertex_shader)

#undef MESHLAB_MAKE_EXTENSION_CHECKER

#endif // !GL_DEFS_H
