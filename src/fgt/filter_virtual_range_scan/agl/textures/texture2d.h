#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <GL/glew.h>
#include <string>
#include <math.h>

#include "client_texture2d.h"

using namespace std;

namespace agl
{
    namespace textures
    {
        class texture2d
        {

        public:

            texture2d( int width, int height, const GLvoid* data = 0 );
            texture2d( texture2d& t, int lod = 0 );

            texture2d( int width,
                       int height,
                       GLint internal_format,
                       GLenum format,
                       GLenum type,
                       const GLvoid* data );

            ~texture2d( void );

            GLuint  get_id                      ( void );
            int     get_width                   ( int lod = 0 );
            int     get_height                  ( int lod = 0 );
            bool    is_ok                       ( void );
            string  get_error_string            ( void );
            GLint   get_internal_format         ( void );
            GLenum  get_format                  ( void );
            GLenum  get_type                    ( void );
            int     get_components_count        ( void );
            int     get_pixel_size              ( void );
            bool    are_mipmaps_generated       ( void );
            void    generate_mipmaps            ( void );
            int     get_mipmap_levels           ( void );
            void    download_and_dump_to_file   ( string filename, int lod = 0 );

            void    bind                ( int texture_unit = -1 );
            void    unbind              ( bool reset_texture_unit = true );
            void    resize              ( int width, int height );

            template< class scalar_type >
            static client_texture2d<scalar_type>* download( texture2d* t, int lod = 0 )
            {
                int my_lod = lod;
                if( !t->mipmaps_generated ) my_lod = 0;
                client_texture2d< scalar_type >* ct = new client_texture2d< scalar_type >();

                /* get pixels data */
                int pixel_size = t->get_pixel_size();

                GLint width = 0, height = 0;
                bool previously_bound = t->bound;
                if( !previously_bound ) t->bind( 0 );
                glGetTexLevelParameteriv( GL_TEXTURE_2D, my_lod, GL_TEXTURE_WIDTH, &width );
                glGetTexLevelParameteriv( GL_TEXTURE_2D, my_lod, GL_TEXTURE_HEIGHT, &height );

                int pixels_number = width * height;
                int texture_size = pixels_number * pixel_size;
                void* tex = malloc( texture_size );

                glGetTexImage( GL_TEXTURE_2D, my_lod, t->format, t->type, tex );

                if( !previously_bound ) t->unbind();

                scalar_type* tex_data = (scalar_type*)tex;
                texture_pixel< scalar_type >* cur_pixel;
                scalar_type* pix_data = 0;
                int channels = t->get_components_count();

                for( int i=0; i<pixels_number; i++ )
                {
                    pix_data = tex_data + i * channels;
                    cur_pixel = new texture_pixel< scalar_type >( channels, pix_data, false );
                    ct->pixels.push_back( cur_pixel );
                }

                ct->width = width;
                ct->height = height;
                ct->format = t->format;
                ct->type = t->type;
                ct->channels = channels;

                return ct;
            }

        protected:

            GLuint  texture_id;
            int     width;
            int     height;
            GLint   internal_format;
            GLenum  format;
            GLenum  type;
            bool    ok;
            bool    bound;
            string  err_string;
            int     my_texture_unit;
            bool    mipmaps_generated;

            void initialize( int width,
                             int height,
                             GLint internal_format,
                             GLenum format,
                             GLenum type,
                             const GLvoid* data );

            void create_texture( int width,
                                 int height,
                                 GLint internal_format,
                                 GLenum format,
                                 GLenum type,
                                 const GLvoid* data = 0 );

        };
    }
}


#endif // TEXTURE2D_H
