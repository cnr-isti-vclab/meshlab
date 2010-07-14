#ifndef SHADER_OBJECT_H
#define SHADER_OBJECT_H

#include <GL/glew.h>
#include <string>
#include <map>

using namespace std;

namespace agl
{
    namespace shaders
    {
        class shader_object
        {

        public:

            shader_object   ( GLenum type, const string& src );
            ~shader_object  ( void );

            bool    is_ok       ( void );
            string  get_log     ( void );
            GLuint  shader_id   ( void );

        private:

            GLuint  id;
            string  log;
            bool    ok;

            static map< GLenum, string > shader_string;
            static void initialize_map( void );
        };
    }
}

#endif // SHADER_OBJECT_H
