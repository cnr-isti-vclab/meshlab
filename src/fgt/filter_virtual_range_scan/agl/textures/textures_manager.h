#ifndef TEXTURES_MANAGER_H
#define TEXTURES_MANAGER_H

#include "texture2d.h"

namespace agl
{
    namespace textures
    {
        class TexturesManager
        {

        public:
            TexturesManager( void )
            {
                ;
            }

            ~TexturesManager( void )
            {
                deleteAllTextures();
            }

            void generateTextures( int count, int width, int height,
                                   GLint internalFormat, GLenum format,
                                   GLenum type, const GLvoid* data,
                                   vector< texture2d* >& tv )
            {
                texture2d* tmpTexture = 0;
                for( int i=0; i<count; i++ )
                {
                    tmpTexture = new texture2d( width, height, internalFormat, format, type, data );
                    tv.push_back( tmpTexture );
                    v.push_back( tmpTexture );
                }
            }

            void generateTextures( int count, texture2d* prototype,
                                   vector< texture2d* >& tv )
            {
                texture2d* tmpTexture = 0;
                for( int i=0; i<count; i++ )
                {
                    tmpTexture = new texture2d
                                 ( prototype->get_width(), prototype->get_height(),
                                   prototype->get_internal_format(), prototype->get_format(),
                                   prototype->get_type(), 0 );
                    tv.push_back( tmpTexture );
                    v.push_back( tmpTexture );
                }
            }

            void registerTextures( vector< texture2d* >& tv )
            {
                for( unsigned int i=0; i<tv.size(); i++ )
                {
                    v.push_back( tv[i] );
                }
            }

            void deleteTextures( vector< texture2d* >& tv )
            {
                vector< texture2d* >::iterator vi;

                for( unsigned int i=0; i<tv.size(); i++ )
                {
                    vi = find( v.begin(), v.end(), tv[i] );
                    if( vi != v.end() )
                    {
                        delete (*vi);
                        v.erase( vi );
                    }
                }
            }

            void deleteTextures( texture2d** tArray, int arraySize )
            {
                vector< texture2d* >::iterator ti;
                for( int i=0; i<arraySize; i++ )
                {
                    ti = find( v.begin(), v.end(), tArray[i] );
                    if( ti != v.end() )
                    {
                        delete (*ti);
                        v.erase( ti );
                    }
                }
            }

            void deleteAllTextures( void )
            {
                for( unsigned int i=0; i<v.size(); i++ )
                {
                    delete v[i];
                }
                v.clear();
            }

            int getTexturesCount( void )
            {
                return v.size();
            }

        private:
            vector< texture2d* > v;

        };
    }
}

#endif // TEXTURES_MANAGER_H
