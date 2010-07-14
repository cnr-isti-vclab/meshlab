#include "fbo.h"

#include <assert.h>
#include <vector>

#include "../utils/viewing_info.h"

using namespace agl::buffers;
using namespace agl::utils;

/*
fbo::fbo( int width, int height, bool empty_fbo )
{
    initialize( width, height, empty_fbo );
}

fbo::fbo( bool empty_fbo )
{
    int w, h;
    viewing_info::get_viewport( 0, 0, &w, &h );
    initialize( w, h, empty_fbo );
}

void fbo::initialize( int w, int h, bool empty )
{
    this->width     = w;
    this->height    = h;
    bound           = false;
    ok              = true;
    attachments     = new map< GLenum, texture2d* >();

    glGenFramebuffers( 1, &fbo_id );

    if( !empty )
    {
        bind();
        attach( GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24,
                GL_DEPTH_COMPONENT, GL_FLOAT );
        attach( GL_COLOR_ATTACHMENT0, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE );
        is_ok();
        unbind();
    }
}

fbo::~fbo( void )
{
    clear();
    glDeleteFramebuffers( 1, &fbo_id );
    delete attachments;
}

void fbo::bind( void )
{
    glBindFramebuffer( GL_FRAMEBUFFER, fbo_id );
    bound = true;
}

void fbo::unbind( void )
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    bound = false;
}

texture2d* fbo::attach( GLenum attachment,
                        GLint  internal_format,
                        GLenum format,
                        GLenum type )
{
    bool previously_bound = bound;
    if( !previously_bound )
    {
        bind();
    }

    detach( attachment );

    texture2d* t = new texture2d( width, height, internal_format, format, type, 0 );

    glFramebufferTexture2D
            ( GL_FRAMEBUFFER,
              attachment,
              GL_TEXTURE_2D,
              t->get_id(),
              0 );

    (*attachments)[ attachment ] = t;

    if( !previously_bound )
    {
        unbind();
    }

    return t;
}

bool fbo::attach( GLenum attachment, texture2d *t, int lod )
{
    bool previously_bound = bound;
    if( !previously_bound ) bind();

    detach( attachment );

    glFramebufferTexture2D
            ( GL_FRAMEBUFFER,
              attachment,
              GL_TEXTURE_2D,
              t->get_id(),
              lod );

    (*attachments)[ attachment ] = t;

    if( !previously_bound ) unbind();
    return is_ok();
}

void fbo::detach( GLenum attachment, bool delete_texture )
{
    bool previously_bound = bound;
    if( !previously_bound )
    {
        bind();
    }

    map< GLenum, texture2d* >::iterator i;
    i = attachments->find( attachment );
    if( i == attachments->end() )
    {
        if( !previously_bound )
        {
            unbind();
        }
        return;    // nothing to detach
    }

    glFramebufferTexture2D
            ( GL_FRAMEBUFFER,
              attachment,
              GL_TEXTURE_2D,
              0,
              0 );

    if( delete_texture )
    {
        delete ( (*i).second );
    }

    attachments->erase( i );

    if( !previously_bound )
    {
        unbind();
    }
}

void fbo::detach( texture2d* t, bool delete_it )
{
    bool previously_bound = bound;
    if( !previously_bound )
    {
        bind();
    }

    map< GLenum, texture2d* >::iterator i = attachments->begin();
    bool found = false;

    while( !found && i != attachments->end() )
    {
        found = ( t == (*i).second );
        ++i;
    }

    if( found )
    {
        --i;
        detach( (*i).first, delete_it );
    }

    if( !previously_bound )
    {
        unbind();
    }
}

void fbo::clear( bool delete_textures )
{
    bool previously_bound = bound;
    if( !previously_bound )
    {
        bind();
    }

    map< GLenum, texture2d* >::iterator i;

    for( i = attachments->begin(); i != attachments->end(); i++ )
    {
        glFramebufferTexture2D
                ( GL_FRAMEBUFFER,
                  (*i).first,
                  GL_TEXTURE_2D,
                  0,
                  0 );

        if( delete_textures )
        {
            delete (*i).second;
        }
    }

    attachments->clear();

    if( !previously_bound )
    {
        unbind();
    }
}

void fbo::resize( int width, int height )
{
    if( this->width == width && this->height == height ) return;

    bool previously_bound = bound;
    if( !previously_bound )
    {
        bind();
    }

    this->width     = width;
    this->height    = height;

    // detach all attachments
    map< GLenum, texture2d* >::iterator i;
    for( i = attachments->begin(); i != attachments->end(); i++ )
    {
        glFramebufferTexture2D
                ( GL_FRAMEBUFFER,
                  (*i).first,
                  GL_TEXTURE_2D,
                  0,
                  0 );
    }

    // resize textures
    for( i = attachments->begin(); i != attachments->end(); i++ )
    {
        ( (*i).second )->resize( width, height );
    }

    // reattach textures
    for( i = attachments->begin(); i != attachments->end(); i++ )
    {
        glFramebufferTexture2D
                ( GL_FRAMEBUFFER,
                  (*i).first,
                  GL_TEXTURE_2D,
                  ( (*i).second )->get_id(),
                  0 );
    }

    is_ok();

    if( !previously_bound )
    {
        unbind();
    }
}

void fbo::resize_according_to_viewport( void )
{
    int w, h;
    viewing_info::get_viewport( 0, 0, &w, &h );

    if( width != w || height != h )
    {
        resize( w, h );
    }
}

bool fbo::is_ok( void )
{
    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    ok = ( status == GL_FRAMEBUFFER_COMPLETE );
    return ok;
}

int fbo::color_attachments_count( void )
{
    return attachments->size();
}

texture2d* fbo::get_texture( GLenum attachment )
{
    map< GLenum, texture2d* >::iterator i = attachments->find( attachment );
    if( i != attachments->end() )
    {
        return (*i).second;
    }
    return 0;
}

texture2d* fbo::acquire( GLenum attachment )
{
    texture2d* t = get_texture( attachment );
    detach( attachment, false );
    return t;
}


int fbo::get_width( void )
{
    return width;
}

int fbo::get_height( void )
{
    return height;
}

const int fbo::DEPTH24_RGB32F   = 0;
const int fbo::DEPTH24_RGBA32F  = 1;

fbo* fbo::setup( int width, int height, int atts, int col_atts_count )
{
    fbo* _fbo = new fbo( width, height, true );

    _fbo->attach( GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT );

    GLenum internal_format = 0;
    GLenum format = 0;

    switch( atts )
    {
    case ( fbo::DEPTH24_RGB32F ):
        internal_format = GL_RGB32F;
        format = GL_RGB;
        break;
    case ( fbo::DEPTH24_RGBA32F ):
        internal_format = GL_RGBA32F;
        format = GL_RGBA;
        break;
    }

    for( int i=0; i<col_atts_count; i++ )
    {
        _fbo->attach( GL_COLOR_ATTACHMENT0 + i, internal_format, format, GL_FLOAT );
    }

    return _fbo;
}
*/




















