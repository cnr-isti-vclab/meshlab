#ifndef SHADER_VARIABLES_H
#define SHADER_VARIABLES_H

#include <GL/glew.h>

namespace agl
{
    namespace shaders
    {
        class shader_variables
        {

        public:

            virtual ~shader_variables   ( void )                {}
            virtual void set            ( GLuint program_id )   = 0;
            virtual void unset          ( GLuint program_id )   = 0;

        };
    }
}

#endif // SHADER_STARTER_H
