#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "renderer.h"

namespace agl
{
    namespace renderers
    {
        template< class mesh_type >
        class mesh_renderer: public renderer
        {

        public:

            virtual ~mesh_renderer( void ){}

            virtual void set_mesh( mesh_type* m )
            {
                this->m = m;
            }

            virtual void prepare( void ) = 0;
            virtual void render ( void ) = 0;
            virtual void finish ( void ) = 0;

        protected:

            mesh_type* m;

        };
    }
}

#endif // MESH_RENDERER_H
