#ifndef SHADER_SET_H
#define SHADER_SET_H

#include "shader.h"
#include "shader_object.h"
#include "shader_builder.h"

#include <string>
#include <vector>

using namespace std;

namespace agl
{
    namespace shaders
    {
        class shader_set: public shader
        {

        public:

            const static int VERTEX_SHADER;
            const static int FRAGMENT_SHADER;
            const static int GEOMETRY_SHADER;

            static shader_set* create_from_file
                    ( string v_file,
                      string f_file,
                      string g_file );

            static shader_set* create_from_file_fast
                    ( string filename );

            static shader_set* create_from_source
                    ( string v_source,
                      string f_source,
                      string g_source );

            static shader_set* create_from_builder
                    ( ShaderBuilder* builder );

            virtual ~shader_set( void );

            /* shader implementation */
            virtual void    load            ( void );
            virtual void    unload          ( void );
            virtual bool    is_ok           ( void );
            virtual GLuint  program_id      ( void );
            virtual void    set_variables   ( shader_variables *sv );

        protected:

            GLuint                      program;
            vector< shader_object* >    shaders;
            string                      log;
            bool                        ok;
            shader_variables*           sv;

            shader_set( void );
        };
    }
}

#endif // SHADER_SET2_H
