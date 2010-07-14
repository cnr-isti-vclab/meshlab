#ifndef SOBEL_RENDERER_H
#define SOBEL_RENDERER_H

#include "composite_renderer.h"
#include "../buffers/fbo.h"
#include "../shaders/stubs/sobel_shader.h"

using namespace agl::buffers;
using namespace agl::shaders;

namespace agl
{
    namespace renderers
    {
        template< class mesh_type >
        class sobel_renderer: public composite_renderer< mesh_type >
        {

        public:

            sobel_renderer( renderer< mesh_type >* inner_renderer,
                sobel_shader* ss, bool owner = false );

            virtual ~sobel_renderer( void );

        protected:

            fbo* _fbo;
            sobel_shader* ss;

            virtual void do_prepare( void );
            virtual void do_finish( void );

            void resize_fbo_according_to_viewport( void );

        };
    }
}

#include "sobel_renderer.cpp"

#endif // SOBEL_RENDERER_H
