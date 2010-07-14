#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include "shader_variables.h"

namespace agl
{
    namespace shaders
    {
        class shader
        {

        public:

            virtual ~shader( void ){}

            virtual void    set_variables   ( shader_variables* sv )    = 0;
            virtual void    load            ( void )                    = 0;
            virtual void    unload          ( void )                    = 0;
            virtual bool    is_ok           ( void )                    = 0;
            virtual GLuint  program_id      ( void )                    = 0;

        };
    }
}

#endif // SHADER_H
