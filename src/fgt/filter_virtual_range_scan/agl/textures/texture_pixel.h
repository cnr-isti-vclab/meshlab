#ifndef TEXTURE_PIXEL_H
#define TEXTURE_PIXEL_H

#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <string>
#include <stdio.h>
#include <typeinfo>

using namespace std;

namespace agl
{
    namespace textures
    {
        template< class channel_type >
        class texture_pixel
        {

        public:

            texture_pixel( unsigned char channels, channel_type* val, bool copy = true )
            {
                this->channels = channels;
                owner = false;

                if( !val || ( val && copy ) )
                {
                    void* pixel_space = malloc( sizeof(channel_type) * channels );
                    memset( pixel_space, 0, sizeof(pixel_space) );
                    owner = true;
                    this->val = (channel_type*)pixel_space;
                }
                else
                {
                    this->val = val;
                }

                if( val && copy )
                {
                    for( unsigned char i=0; i<channels; i++ )
                    {
                        this->val[i] = val[i];
                    }
                }
            }

            /* copy constructor */
            texture_pixel( const texture_pixel& p )
            {
                this->channels = p.channels;
                this->val = (channel_type*)malloc( sizeof(p.val) );
                memset( this->val, 0, sizeof(this->val) );

                for( unsigned char i=0; i<p.channels; i++ )
                {
                    this->val[i] = p.val[i];
                }
            }

            ~texture_pixel( void )
            {
                if( owner )
                {
                    free( val );
                }
            }

            /* checked access */
            inline channel_type& at( int index )
            {
                assert( index >= 0 && index < channels );
                return val[ index ];
            }

            /* unchecked access */
            inline channel_type& operator[]( int index )
            {
                return val[index];
            }

            inline bool operator==( const texture_pixel& p )
            {
                bool equals = true;
                unsigned char i = 0;

                while( equals && i < channels )
                {
                    equals = ( val[i] == p.val[i] );
                }

                return equals;
            }

            inline string to_string( void )
            {
                string desc = "( ";
                char buf[50];

                string channel_type_name = typeid( channel_type ).name();
                string float_name = typeid( float ).name();
                string format = "%d";
                if( channel_type_name == float_name )
                {
                    format = "%f";
                }
                const char* formatCChar = format.c_str();

                for( unsigned char i = 0; i < channels; i++ )
                {
                    sprintf( buf, formatCChar, val[i] );
                    desc += buf;

                    if( i < channels - 1 )
                    {
                        desc += ",\t";
                    }
                }

                desc += " )";
                return desc;
            }

            inline channel_type& x( void )
            {
                return val[0];
            }

            inline channel_type& y( void )
            {
                return val[1];
            }

            inline channel_type& z( void )
            {
                return val[2];
            }

            inline channel_type& w( void )
            {
                return val[3];
            }

        private:

            channel_type*   val;
            unsigned char   channels;
            bool            owner;

        };

        typedef texture_pixel< int >    i_texture_pixel;
        typedef texture_pixel< float >  f_texture_pixel;
    }
}

#endif // TEXTURE_PIXEL_H
