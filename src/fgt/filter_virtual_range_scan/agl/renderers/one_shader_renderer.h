#ifndef ONE_SHADER_RENDERER_H
#define ONE_SHADER_RENDERER_H

#include <string.h>
#include <stdlib.h>

#include "composite_renderer.h"
#include "../shaders/shader.h"

using namespace agl::shaders;

namespace agl
{
    namespace renderers
    {
        template< class mesh_type >
        class one_shader_renderer: public composite_renderer< mesh_type >
        {
        public:
            one_shader_renderer( mesh_renderer< mesh_type >* inner_renderer,
                                 shader* s,
                                 bool owner = false )
                : composite_renderer< mesh_type >( inner_renderer, owner )
            {
                this->s = s;
            }

            virtual ~one_shader_renderer( void ) {}

            void set_shader( shader* s )
            {
                this->s = s;
            }

        protected:

            virtual void do_prepare( void )
            {
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                s->load();
            }

            virtual void do_finish( void )
            {
                s->unload();
            }

        private:

            shader* s;

        };
    }
}

#endif // LAMBERT_RENDERER_H
