#ifndef RENDERER_H
#define RENDERER_H

#include <vcg/space/point3.h>

namespace agl
{
    namespace renderers
    {
        class renderer
        {

        public:

            virtual ~renderer( void ){}

            virtual void prepare( void ) = 0;
            virtual void render ( void ) = 0;
            virtual void finish ( void ) = 0;

        };
    }
}

#endif // RENDERER_H
