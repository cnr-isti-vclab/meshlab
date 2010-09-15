#ifndef SIMPLE_RENDERER_H
#define SIMPLE_RENDERER_H

#include <GL/glew.h>
#include <vcg/space/point3.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/update/flag.h>
#include <vcg/complex/trimesh/update/color.h>
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
            createBuffers();
        }

        ~SimpleRenderer( void )
        {
            glDeleteBuffers( 3, buffer );
        }

        void render( void )
        {
            /*
            for( int i=0; i<3; i++ )
            {
                assert( glIsBuffer( buffer[i] ) == GL_TRUE );
            }

            glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );         // coordinates
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer[2] ); // indices
            glEnableClientState( GL_NORMAL_ARRAY );
            glEnableClientState( GL_VERTEX_ARRAY );

            glDrawElements( GL_TRIANGLES, elementsToDraw, GL_UNSIGNED_INT,  0 );

            glDisableClientState( GL_NORMAL_ARRAY );
            glDisableClientState( GL_VERTEX_ARRAY );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            */


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
        GLuint      buffer[3];      // coords, normals, colors, indices
        GLsizei     elementsToDraw; // 3 * face_number
        bool        buffersOk;      // true => use vbo's

        void createBuffers( void )
        {
            assert( glGetError() == GL_NO_ERROR );
            glGenBuffers( 3, buffer );

            // prepare buffers
            size_t scalarTypeSize   = sizeof( ScalarType );     // size of scalar type            
            size_t vertexSize       = 3 * scalarTypeSize;       // size of a vertex
            size_t verticesSize     = m->vn * vertexSize;       // size of vertices

            void* vData = malloc( verticesSize );               // vertices buffer
            void* nData = malloc( verticesSize );               // normals buffer
            assert( vData && nData );

            ScalarType* vP = (ScalarType*)vData;
            ScalarType* nP = (ScalarType*)nData;
            map< VertexPointer, unsigned int >* ptrToInt = new map< VertexPointer, unsigned int >();
            unsigned int vertexIndex = 0;

            for( VertexIterator vi = m->vert.begin(); vi != m->vert.end(); ++vi )
            {
                for( int i=0; i<3; i++ )
                {
                    vP[i] = (*vi).P()[i];
                    nP[i] = (*vi).N()[i];
                }

                vP = &( vP[3] );
                nP = &( nP[3] );
                (*ptrToInt)[ &(*vi) ] = vertexIndex;
                vertexIndex++;
            }

            bool floatTypes = ( scalarTypeSize == sizeof(float) );

            // fills coordinates buffer
            glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
            glBufferData( GL_ARRAY_BUFFER, sizeof(vData), vData, GL_STATIC_DRAW );
            glVertexPointer( (GLint)3, floatTypes? GL_FLOAT : GL_DOUBLE, 0, 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            free( vData );

            assert( glGetError() == GL_NO_ERROR );

            // fills normals buffer
            glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
            glBufferData( GL_ARRAY_BUFFER, sizeof(nData), nData, GL_STATIC_DRAW );
            glNormalPointer( floatTypes? GL_FLOAT : GL_DOUBLE, 0, 0 );
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
            free( nData );

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

            /*
            iP = (unsigned int*)iData;
            for( int i=0; i<m->fn; i++ )
            {
                qDebug( "%d)\t(%d, %d, %d)", i, iP[0], iP[1], iP[2] );
                iP = &( iP[3] );
            }
            */

            // fills indices buffer
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffer[2] );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(iData), iData, GL_STATIC_DRAW );
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
            free( iData );
            delete ptrToInt;

            assert( glGetError() == GL_NO_ERROR );
        }

    };
}


#endif // SIMPLE_RENDERER_H
