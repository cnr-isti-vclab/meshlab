#ifndef CLIENT_TEXTURE2D_H
#define CLIENT_TEXTURE2D_H

#include "texture_pixel.h"
#include "../utils/file_rw.h"

#include <GL/glew.h>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <QImage>

using namespace std;

namespace agl
{
    namespace textures
    {
        class texture2d;

        template< class channel_type >
        class client_texture2d
        {

            friend class texture2d;

        public:

            typedef texture_pixel< channel_type > pixel_type;

            ~client_texture2d( void )
            {
                typename vector< pixel_type* >::iterator i;
                for( i = pixels.begin(); i != pixels.end(); i++ )
                {
                    delete (*i);
                }
                pixels.clear();
            }

            vector< pixel_type* > pixels;

            int get_width( void )
            {
                return width;
            }

            int get_height( void )
            {
                return height;
            }

            GLenum get_channel_type( void )
            {
                return type;
            }

            GLenum get_format( void )
            {
                return format;
            }

            inline pixel_type* get_pixel( int i, int j )
            {
                int index = i * height + j;
                return pixels.at( index );
            }

            QImage* get_image( void )
            {
                channel_type* texdata = &( (*(pixels[0]))[0] );
                int num_bytes = sizeof( channel_type ) * channels * width * height;

                QImage img( width, height, QImage::Format_ARGB32 );
                memcpy( img.bits(), texdata, num_bytes );

                QImage* final_img = new QImage( img.rgbSwapped().mirrored() );
                return final_img;
            }

            void save_to_file( string filename )
            {
                QImage* img = get_image();
                img->save( filename.c_str(), "bmp" );
                delete img;
            }

            void dump_to_file( string filename )
            {
                char buf[ 100 ];
                sprintf( buf, "Texture size: %d x %d\n", width, height );
                string content = buf;

                sprintf( buf, "Channels number: %d\n", channels );
                content += buf;

                sprintf( buf, "Number of bytes per channel: %d\n", sizeof( channel_type ) );

                content += "\nPixels\n------\n";

                int x = 0, y = 0;
                for( unsigned int i=0; i<pixels.size(); i++ )
                {
                    sprintf( buf, "%d)\t\t%s\t(%d, %d)\n", i,
                             pixels[i]->to_string().c_str(), x, y );
                    content += buf;
                    x++;
                    if( x >= width )
                    {
                        x = 0;
                        y++;
                    }
                }

                utils::file_rw::write_file( filename, content );
            }

        private:

            int width;
            int height;
            int channels;
            GLenum type;
            GLenum format;
        };

        typedef client_texture2d< int >     client_texture2d_i;
        typedef client_texture2d< float >   client_texture2d_f;
    }
}

#endif // CLIENT_TEXTURE2D_H
