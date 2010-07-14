#ifndef PIXEL_DATA_H
#define PIXEL_DATA_H

#include "../textures/texture2d.h"

using namespace agl::textures;

namespace agl
{
    namespace vrs
    {
        class PixelData
        {
        public:

            PixelData( void )
            {
                data    = 0;
                length  = -1;
            }

            PixelData( texture2d* data )
            {
                this->data      = data;
                this->length    = data->get_width() * data->get_height();
            }

            PixelData( texture2d* data, int length )
            {
                this->data      = data;
                this->length    = length;
            }

            texture2d*  data;
            int         length;

        };
    }
}

#endif // PIXEL_DATA_H
