#ifndef BASE_RENDERER_H
#define BASE_RENDERER_H

#include "composite_renderer.h"

namespace agl
{
    namespace renderers
    {
        template < class mesh_type >
        class base_renderer: public composite_renderer< mesh_type >
        {
        public:

            base_renderer( mesh_renderer< mesh_type >* inner_renderer, bool owner = false )
                :composite_renderer< mesh_type >( inner_renderer, owner )
            {
                wireframe = false;
            }

            virtual ~base_renderer( void )
            {
                ;
            }

            void set_wireframe_mode( bool on )
            {
                wireframe = on;
            }

        protected:

            virtual void do_prepare( void )
            {
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                if( wireframe )
                {
                    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                }
            }

            virtual void do_finish( void )
            {
                if( wireframe )
                {
                    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                }
            }

        private:

            bool wireframe;

        };
    }
}

#endif // BASE_RENDERER_H
