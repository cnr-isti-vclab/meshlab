#include "sobel_renderer.h"

using namespace agl::renderers;

template< class mesh_type >
sobel_renderer< mesh_type >::sobel_renderer
        (renderer<mesh_type> *inner_renderer, sobel_shader *ss, bool owner)
            :composite_renderer< mesh_type >( inner_renderer, owner )
{
    _fbo = new fbo( 512, 512, false );
    this->ss = ss;
}

template< class mesh_type >
sobel_renderer< mesh_type >::~sobel_renderer( void )
{
    delete _fbo;
}

template< class mesh_type >
void sobel_renderer< mesh_type >::do_prepare( mesh_type *m )
{
    assert( ss );

    resize_fbo_according_to_viewport();

    _fbo->bind();
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    render( m );    // render-to-texture
    _fbo->unbind();

    texture2d* depth = _fbo->get_texture( GL_DEPTH_ATTACHMENT );
    ss->set_float_map_unit( 0 );

    glActiveTexture( 0 );
    glBindTexture( GL_TEXTURE_2D, depth->get_id() );
    ss->load();

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

template< class mesh_type >
void sobel_renderer< mesh_type >::do_finish( mesh_type *m )
{
    ss->unload();
    glBindTexture( GL_TEXTURE_2D, 0 );
}

template< class mesh_type >
void sobel_renderer< mesh_type >::resize_fbo_according_to_viewport( void )
{
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );

    if( _fbo->get_width() != viewport[2] || _fbo->get_height() != viewport[3] )
    {
        _fbo->resize( viewport[2], viewport[3] );
    }
}
