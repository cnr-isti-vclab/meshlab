#ifndef SHADOW_MAP_RENDERER_H
#define SHADOW_MAP_RENDERER_H

#include "composite_renderer.h"
#include "../buffers/fbo.h"
#include "../shaders/stubs/shadow_map_shader.h"

#include <Qt>

using namespace agl::buffers;
using namespace agl::shaders;

namespace agl
{
    namespace renderers
    {
        template< class mesh_type >
        class shadow_map_renderer: public composite_renderer< mesh_type >
        {

        public:
            shadow_map_renderer
            ( renderer< mesh_type >* inner_renderer,
              int fbo_width, 
              int fbo_height,
              bool owner = false )
                : composite_renderer< mesh_type >( inner_renderer, owner ),
                  light_pos( 0.0, 0.0, 0.0 )
            {
                s = 0;
                current_mesh = 0;

                for( int i=0; i<16; i++ )
                {
                    light_projection_matrix[i] = 0.0;
                    light_modelview_matrix[i] = 0.0;
                }

                fb_obj = new fbo( fbo_width, fbo_height, false );
                depth_computed = false;
            }

            virtual ~shadow_map_renderer()
            {
                delete fb_obj;
            }

            void set_shadow_map_shader( shadow_map_shader* s )
            {
                this->s = s;
                s->set_light_matrices( light_modelview_matrix, light_projection_matrix);
            }

            void set_light_position( vcg::Point3f& p )
            {
                light_pos = p;
                depth_computed = false;
            }

            void recompute_depth_map( void )
            {
                depth_computed = false;
            }
            
            void render_area_resized( int width, int height )
            {
                fb_obj->resize( width, height );
                depth_computed = false;
            }

        protected:
        
            virtual void do_prepare( void )
            {
                if( !depth_computed )
                {
                    // render-to-texture
                    fb_obj->bind();
                    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                    glPolygonOffset( 1.1, 4.0 );
                    glEnable( GL_POLYGON_OFFSET_FILL );
                    render( m );
                    glDisable( GL_POLYGON_OFFSET_FILL );
                    fb_obj->unbind();

                    // saves the parameters for the shadow mapping
                    glGetFloatv( GL_PROJECTION_MATRIX, light_projection_matrix );
                    glGetFloatv( GL_MODELVIEW_MATRIX, light_modelview_matrix );

                    if( s )
                    {
                        depth = fb_obj->get_texture( GL_DEPTH_ATTACHMENT_EXT );
                        s->set_depth_unit( 0 );
                    }

                    current_mesh = m;
                    depth_computed = true;
                }

                if( depth )
                {
                    glActiveTexture( GL_TEXTURE0 );
                    glBindTexture( GL_TEXTURE_2D, depth->get_id() );
                }

                if( s )
                {
                    s->load();
                }

                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                return;
            }

            virtual void do_finish( void )
            {
                if( s )
                {
                    s->unload();
                }

                glBindTexture( GL_TEXTURE_2D, 0 );
            }

        private:

            shadow_map_shader*  s;
            texture2d*          depth;
            fbo*                fb_obj;
            mesh_type*          current_mesh;
            bool                depth_computed;
            vcg::Point3f        light_pos;
            GLfloat             light_modelview_matrix[16];
            GLfloat             light_projection_matrix[16];

        };
    }
}

#endif // SHADOW_MAP_RENDERER_H
