#ifndef COMPOSITE_RENDERER_H
#define COMPOSITE_RENDERER_H

#include "mesh_renderer.h"

namespace agl
{
    namespace renderers
    {
        template< class mesh_type >
        class composite_renderer: public mesh_renderer< mesh_type >
        {

        public:

            composite_renderer( mesh_renderer< mesh_type >* inner_renderer,
                                bool owner = false )
            {
                assert( inner_renderer );
                this->inner_renderer    = inner_renderer;
                this->owner             = owner;
            }

            virtual ~composite_renderer( void )
            {
                if( owner )
                {
                    delete inner_renderer;
                }
            }

            virtual void set_mesh( mesh_type *m )
            {
                this->m = m;
                inner_renderer->set_mesh( this->m );
            }

            virtual void prepare( void )
            {
                inner_renderer->prepare();
                do_prepare();
            }

            virtual void render( void )
            {
                inner_renderer->render();
                do_render();
            }

            virtual void finish( void )
            {
                do_finish();
                inner_renderer->finish();
            }

        protected:

            mesh_renderer< mesh_type >*     inner_renderer;
            bool                            owner;

            virtual void do_prepare ( void ){}
            virtual void do_render  ( void ){}
            virtual void do_finish  ( void ){}

        };
    }
}

#endif // COMPOSITE_RENDERER_H
