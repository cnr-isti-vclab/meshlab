#ifndef SIMPLE_RENDERER_H
#define SIMPLE_RENDERER_H

#include <GL/glew.h>
#include <vcg/space/point3.h>
#include <vcg/complex/complex.h>
#include <vcg/complex/algorithms/update/flag.h>
#include <vcg/complex/algorithms/update/color.h>
#include <wrap/gl/trimesh.h>

#include "assert.h"
#include <Qt>

#include <map>

namespace vs
{
    template< class MeshType >
    class SimpleRenderer
    {

    public:
        SimpleRenderer( MeshType* m )
        {
            assert(m && m->vert.size() > 0 && m->face.size() > 0 );
            this->m         = m;
            this->buffersOk = false;
            //createBuffers();
        }

        ~SimpleRenderer( void )
        {
            //glDeleteBuffers( 4, buffer );
        }

        void render( void )
        {
            /*
            for( int i=0; i<4; i++ )
            {
                assert( glIsBuffer( buffer[i] ) == GL_TRUE );
            }

            glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );         // coordinates
            glVertexPointer( (GLint)3, GL_FLOAT, 0, 0 );
            glEnableClientState( GL_VERTEX_ARRAY );

            glBindBuffer( GL_ARRAY_BUFFER, buffer[1] );         // normals
            glNormalPointer( GL_FLOAT, 0, 0 );
            glEnableClientState( GL_NORMAL_ARRAY );

            glBindBuffer( GL_ARRAY_BUFFER, buffer[2] );         // colors
            glColorPointer( (GLint)4, GL_UNSIGNED_BYTE, 0, 0 );
            glEnableClientState( GL_COLOR_ARRAY );

            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer[3] ); // indices

            glDrawElements( GL_TRIANGLES, elementsToDraw, GL_UNSIGNED_INT,  0 );

            glDisableClientState( GL_COLOR_ARRAY );
            glDisableClientState( GL_NORMAL_ARRAY );
            glDisableClientState( GL_VERTEX_ARRAY );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            */

            // immediate-mode rendering
            if( m->vert.size() == 0 ) return;

            typename MeshType::FaceIterator fi;
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

    private:

        typedef typename MeshType::VertexIterator   VertexIterator;
        typedef typename MeshType::VertexType       VertexType;
        typedef typename MeshType::VertexPointer    VertexPointer;
        typedef typename MeshType::FaceIterator     FaceIterator;
        typedef typename MeshType::ScalarType       ScalarType;

        MeshType*   m;
        GLuint      buffer[4];      // coords, normals, colors, indices
        GLsizei     elementsToDraw; // 3 * face_number
        bool        buffersOk;      // true => use vbo's

        // create the buffer objects to hold mesh data
        void createBuffers( void )
        {
            assert( glGetError() == GL_NO_ERROR );
            glGenBuffers( 4, buffer );

            // prepare buffers
            size_t scalarTypeSize   = sizeof( ScalarType );     // size of scalar type            
            size_t vertexSize       = 3 * scalarTypeSize;       // size of a vertex
            size_t verticesSize     = m->vn * vertexSize;       // size of vertices

            void* vData = malloc( verticesSize );               // vertices buffer
            void* nData = malloc( verticesSize );               // normals buffer
            assert( vData && nData );


            // assume that the color type is Color4b
            size_t colorSize = 4 * m->vn;
            void* cData = malloc( colorSize );
            assert( cData );
            bool perVertexColor = m->HasPerVertexColor();

            // if per-vertex color is not available, we use per-mesh color
            vcg::Color4b defaultColor = m->C();

            ScalarType*     vP = (ScalarType*)vData;
            ScalarType*     nP = (ScalarType*)nData;
            unsigned char*  cP = (unsigned char*)cData;
            map< VertexPointer, unsigned int >* ptrToInt = new map< VertexPointer, unsigned int >();
            unsigned int vertexIndex = 0;

            for( VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi )
            {
                for( int i=0; i<3; i++ )
                {
                    vP[i] = (*vi).P()[i];
                    nP[i] = (*vi).N()[i];
                }

                for( int i=0; i<4; i++ )
                {
                    cP[i] = (perVertexColor? (*vi).C()[i] : defaultColor[i] );
                }

                vP = &( vP[3] );
                nP = &( nP[3] );
                cP = &( cP[4] );
                (*ptrToInt)[ &(*vi) ] = vertexIndex;
                vertexIndex++;
            }

            // fills coordinates buffer
            glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
            glBufferData( GL_ARRAY_BUFFER, verticesSize, vData, GL_STATIC_DRAW );
            glVertexPointer( (GLint)3, GL_FLOAT, 0, 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            free( vData );

            assert( glGetError() == GL_NO_ERROR );

            // fills normals buffer
            glBindBuffer( GL_ARRAY_BUFFER, buffer[1] );
            glBufferData( GL_ARRAY_BUFFER, verticesSize, nData, GL_STATIC_DRAW );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            free( nData );

            assert( glGetError() == GL_NO_ERROR );

            // fills colors buffer
            glBindBuffer( GL_ARRAY_BUFFER, buffer[2] );
            glBufferData( GL_ARRAY_BUFFER, colorSize, cData, GL_STATIC_DRAW );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            free( cData );

            assert( glGetError() == GL_NO_ERROR );

            // prepare indices buffer
            elementsToDraw = (GLsizei)( m->fn * 3 );            // every face has three indices
            size_t indicesSize = ((size_t)elementsToDraw) * sizeof(unsigned int);
            void* iData = malloc( indicesSize );
            assert( iData );
            unsigned int* iP = (unsigned int*)iData;
            for( FaceIterator fi = m->face.begin(); fi != m->face.end(); ++fi )
            {
                for( unsigned int i = 0; i < 3; i++ )
                {
                    iP[ i ] = (*ptrToInt)[ (*fi).V(i) ];
                }
                iP = &( iP[3] );
            }

            // fills indices buffer
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer[3] );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, indicesSize, iData, GL_STATIC_DRAW );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            free( iData );
            delete ptrToInt;

            assert( glGetError() == GL_NO_ERROR );
        }

    };
}


#endif // SIMPLE_RENDERER_H
