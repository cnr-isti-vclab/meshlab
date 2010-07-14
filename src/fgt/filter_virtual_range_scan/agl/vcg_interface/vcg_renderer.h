#ifndef VCG_RENDERER_H
#define VCG_RENDERER_H

#include "../renderers/mesh_renderer.h"

using namespace agl::renderers;

namespace agl
{
    namespace vcg_interface
    {
        template< class vcg_mesh >
        class vcg_renderer: public mesh_renderer< vcg_mesh >
        {

        public:

            vcg_renderer( void ){}
            virtual ~vcg_renderer( void ){}

            virtual void prepare( void )
            {
                ;
            }

            virtual void render ( void )
            {
                vcg_mesh* m = this->m;
                if( m->vert.size() == 0 ) return;

                typename vcg_mesh::FaceIterator fi;
                int i = 0;
                vcg::Point3f* pp = 0;
                vcg::Point3f* np = 0;

                glBegin( GL_TRIANGLES );

                for( fi = m->face.begin(); fi != m->face.end(); ++fi )
                {
                    for( i = 0; i < 3; i++ )
                    {
                        np = &( (*fi).V(i)->N() );
                        glNormal3f( np->X(), np->Y(), np->Z() );

                        pp = &( (*fi).V(i)->P() );
                        glVertex3f( pp->X(), pp->Y(), pp->Z() );
                    }
                }

                glEnd();
            }

            virtual void finish ( void )
            {
                ;
            }

        };
    }
}

#endif // VCG_RENDERER_H
