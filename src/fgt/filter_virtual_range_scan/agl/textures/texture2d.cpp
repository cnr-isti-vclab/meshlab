#include "texture2d.h"
#include "../utils/error_bin.h"
#include <stdlib.h>

using namespace agl::textures;
using namespace agl::utils;

texture2d::texture2d( int width, int height, const GLvoid *data )
{
    initialize( width, height, GL_RGBA8, GL_RGBA, GL_FLOAT, data );
}

texture2d::texture2d( int width, int height, GLint internal_format,
                      GLenum format, GLenum type, const GLvoid *data )
{
    initialize( width, height, internal_format, format, type, data );
}

texture2d::texture2d( texture2d &t, int lod )
{
    /* get pixels data */
    int pixel_size = t.get_pixel_size();

    GLint width = 0, height = 0;
    bool previously_bound = t.bound;
    if( !previously_bound ) t.bind();
    glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_WIDTH, &width );
    glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_HEIGHT, &height );

    int pixels_number = width * height;
    int texture_size = pixels_number * pixel_size;
    void* tex = malloc( texture_size );

    glGetTexImage( GL_TEXTURE_2D, lod, t.format, t.type, tex );

    if( !previously_bound ) t.unbind();

    /* creates a new texture */
    initialize( width, height, t.get_internal_format(), t.get_format(),
                t.get_type(), tex );

    free( tex );
}

void texture2d::initialize( int width,
                            int height,
                            GLint internal_format,
                            GLenum format,
                            GLenum type,
                            const GLvoid* data )
{
    ok                  = false;
    bound               = false;
    mipmaps_generated   = false;
    err_string          = "no errors";
    my_texture_unit     = -1;

    this->width             = width;
    this->height            = height;
    this->internal_format   = internal_format;
    this->format            = format;
    this->type              = type;
    texture_id              = 0;

    // creates the texture object
    create_texture( width, height, internal_format, format, type, data );
}

texture2d::~texture2d( void )
{
    if( bound ) unbind();

    if( texture_id > 0 )
    {
        glDeleteTextures( 1, &texture_id );
    }
}

GLuint texture2d::get_id( void )
{
    return texture_id;
}

int texture2d::get_width( int lod )
{
    if( lod == 0 || !mipmaps_generated )
    {
        return width;
    }
    else
    {
        bool previously_bound = bound;
        if( !previously_bound ) bind();

        GLint w;
        glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_WIDTH, &w );

        if( !previously_bound ) unbind();

        return (int)w;
    }
}

int texture2d::get_height( int lod )
{
    if( lod == 0 || !mipmaps_generated )
    {
        return height;
    }
    else
    {
        bool previously_bound = bound;
        if( !previously_bound ) bind();

        GLint h;
        glGetTexLevelParameteriv( GL_TEXTURE_2D, lod, GL_TEXTURE_HEIGHT, &h );

        if( !previously_bound ) unbind();

        return (int)h;
    }
}

bool texture2d::is_ok( void )
{
    return ok;
}

string texture2d::get_error_string( void )
{
    return err_string;
}

GLint texture2d::get_internal_format( void )
{
    return internal_format;
}

GLenum texture2d::get_format( void )
{
    return format;
}

GLenum texture2d::get_type( void )
{
    return type;
}

int texture2d::get_components_count( void )
{
    int components = -1;

    switch( internal_format )
    {
    case GL_ALPHA:
    case GL_ALPHA8:
    case GL_ALPHA12:
    case GL_ALPHA16:
    case GL_R:
    case GL_R32F:
    case GL_R16:
    case GL_R16I:
    case GL_R16UI:
    case GL_R32I:
    case GL_R32UI:
    case GL_COMPRESSED_ALPHA:
    case GL_COMPRESSED_INTENSITY:
    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32:

        components = 1;
        break;

    case GL_COMPRESSED_RGB:
    case GL_R3_G3_B2:
    case GL_RGB:
    case GL_RGB4:
    case GL_RGB5:
    case GL_RGB8:
    case GL_RGB10:
    case GL_RGB12:
    case GL_RGB16:
    case GL_RGB32F:
    case GL_RGB16F:

        components = 3;
        break;

    case GL_COMPRESSED_RGBA:
    case GL_RGBA:
    case GL_RGBA32F:
    case GL_RGBA16F:
    case GL_RGBA12:
    case GL_RGBA16:
    case GL_RGBA16UI:
    case GL_RGBA16_EXTENDED_RANGE_SGIX:
    case GL_RGBA16_SIGNED_SGIX:
    case GL_RGBA2:
    case GL_RGBA32I:
    case GL_RGBA32UI:
    case GL_RGBA4:
    case GL_RGBA4_DXT5_S3TC:
    case GL_RGBA4_S3TC:
    case GL_RGBA8:
    case GL_RGBA8I:
    case GL_RGBA8UI:
    case GL_RGBA_DXT5_S3TC:
    case GL_RGBA_EXTENDED_RANGE_SGIX:
    case GL_RGBA_FLOAT_MODE_ARB:
    case GL_RGBA_INTEGER:

        components = 4;
        break;
    }

    return components;
}

int texture2d::get_pixel_size( void )
{
    int channel_size = 0;
    switch( type )
    {
    case GL_UNSIGNED_BYTE:
        channel_size = sizeof( GLubyte );
        break;
    case GL_BYTE:
        channel_size = sizeof( GLbyte );
        break;
    case GL_INT:
        channel_size = sizeof( GLint );
        break;
    case GL_UNSIGNED_INT:
        channel_size = sizeof( GLuint );
        break;
    case GL_FLOAT:
        channel_size = sizeof( GLfloat );
        break;
    }

    return ( get_components_count() * channel_size );
}

bool texture2d::are_mipmaps_generated( void )
{
    return mipmaps_generated;
}

void texture2d::generate_mipmaps( void )
{
    if( mipmaps_generated ) return;
    bool previously_bound = bound;
    if( !previously_bound ) bind();
    glGenerateMipmap( GL_TEXTURE_2D );
    if( !previously_bound ) unbind();
    mipmaps_generated = true;
}

int texture2d::get_mipmap_levels( void )
{
    if( !mipmaps_generated ) return 1;

    int levels = 0;
    int w = width;

    do
    {
        while( w >= 1 )
        {
            w /= 2;
            levels++;
        }

        w = get_width( levels );
    }while( w > 1 );

    return levels;
}

void texture2d::download_and_dump_to_file( string filename, int lod )
{
    if( type == GL_FLOAT )
    {
        client_texture2d_f* ct = texture2d::download< float >( this, lod );
        ct->dump_to_file( filename );
        delete ct;
    }
    else if( type == GL_INT )
    {
        client_texture2d_i* ct = texture2d::download< int >( this, lod );
        ct->dump_to_file( filename );
        delete ct;
    }
    else
    {
        client_texture2d< unsigned char >* ct = texture2d::download< unsigned char >( this, lod );
        ct->dump_to_file( filename );
        delete ct;
    }
}

void texture2d::bind( int texture_unit )
{
    if( bound ) unbind();
    if( texture_unit != -1 )
    {
        my_texture_unit = texture_unit;   
    }
    else
    {
        my_texture_unit = 0;
    }

    GLenum unit = GL_TEXTURE0 + (GLint)texture_unit;
    glActiveTexture( unit );
    glBindTexture( GL_TEXTURE_2D, texture_id );
    bound = true;
}

void texture2d::unbind( bool reset_texture_unit )
{
    if( my_texture_unit != -1 )
    {
        glActiveTexture( GL_TEXTURE0 + (GLint)my_texture_unit );
        if( reset_texture_unit )
        {
            my_texture_unit = -1;
        }
    }
    glBindTexture( GL_TEXTURE_2D, 0 );
    bound = false;
}

void texture2d::resize( int width, int height )
{
    if( this->width == width && this->height == height ) return;
    if( bound ) unbind();
    if( texture_id == 0 ) return;

    glDeleteTextures( 1, &texture_id );

    this->width     = width;
    this->height    = height;

    create_texture( width, height, internal_format, format, type, 0 );
}

void texture2d::create_texture( int width,
                                int height,
                                GLint internal_format,
                                GLenum format,
                                GLenum type,
                                const GLvoid* data )
{
    error_bin* bin  = error_bin::get_instance();
    bin->reset();

    glGenTextures( 1, &texture_id );
    glBindTexture( GL_TEXTURE_2D, texture_id );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    glTexImage2D( GL_TEXTURE_2D,
                  0,
                  internal_format,
                  (GLsizei)width,
                  (GLsizei)height,
                  0,
                  format,
                  type,
                  data );

    glBindTexture( GL_TEXTURE_2D, 0 );

    bin->check();
    ok = ( bin->is_empty() );

    if( !ok )
    {
        err_string = bin->get_last();
    }
}








