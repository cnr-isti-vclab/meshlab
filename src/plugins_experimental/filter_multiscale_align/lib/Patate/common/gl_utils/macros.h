/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _PATATE_COMMON_GL_UTILS_MACROS_
#define _PATATE_COMMON_GL_UTILS_MACROS_


#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#ifndef PATATE_ASSERT_NO_GL_ERROR
#define PATATE_ASSERT_NO_GL_ERROR() _assertNoGlError(__FILE__, __LINE__)
#endif

#define PATATE_FIELD_OFFSET(_struct, _field) &(static_cast<_struct*>(0)->_field)

inline const char* glErrorString(GLenum error)
{
    switch(error)
    {
    case GL_NO_ERROR:
        return "NO_ERROR";
    case GL_INVALID_ENUM:
        return "INVALID_ENUM";
    case GL_INVALID_VALUE:
        return "INVALID_VALUE";
    case GL_INVALID_OPERATION:
        return "INVALID_OPERATION";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "INVALID_FRAMEBUFFER_OPERATION";
    case GL_OUT_OF_MEMORY:
        return "OUT_OF_MEMORY";
    }
    return "Unknown error";
}

inline void _assertNoGlError(const char* file, int line)
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR) {
        printf("OpenGL error in %s:%d: %s (0x%x)\n",
               file, line, glErrorString(error), error);
        abort();
    }
}



#endif

