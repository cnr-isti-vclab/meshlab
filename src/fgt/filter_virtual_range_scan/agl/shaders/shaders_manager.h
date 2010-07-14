#ifndef SHADERS_MANAGER_H
#define SHADERS_MANAGER_H

#include "shader.h"

#include <map>
#include <string>

using namespace std;

namespace agl
{
    namespace shaders
    {
        class shaders_manager
        {

        public:
            shaders_manager     ( void );
            shaders_manager     ( string shaders_path );
            ~shaders_manager    ( void );

            shader*  get_shader          ( string shader_name );
            bool     register_shader     ( string shader_name, shader* s );
            void     clear               ( void );

        private:

            typedef map< string, shader* >::iterator shaders_it;
            map< string, shader* > shaders;
            string shaders_path;

        };
    }
}

#endif // SHADERS_MANAGER_H
