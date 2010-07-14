#ifndef VIEWING_INFO_H
#define VIEWING_INFO_H

#include <GL/glew.h>
#include <QImage>
#include <math.h>
#include <string>
#include <stdio.h>

#define PI 3.1415926535

using namespace std;

namespace agl
{
    namespace utils
    {
        class viewing_info
        {

        public:

            static void get_viewport( int* x, int* y, int* w, int* h )
            {
                GLint viewport[4];
                glGetIntegerv( GL_VIEWPORT, viewport );

                if( x ) *x = viewport[0];
                if( y ) *y = viewport[1];
                if( w ) *w = viewport[2];
                if( h ) *h = viewport[3];
            }

            static void screenshot( const QString& dest, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE )
            {
                int w, h;
                get_viewport( 0, 0, &w, &h );
                QImage img( w, h, QImage::Format_ARGB32 );
                glReadPixels( 0, 0, w, h, format, type, img.bits() );
                img = img.rgbSwapped().mirrored();
                img.save( dest, "bmp" );
            }

            static float calculate_fov( float distance, float obj_size )
            {
                float half_fov = atan2( obj_size / 2, distance );
                float half_deg_fov = ( half_fov * 180 ) / PI;
                return 2 * half_deg_fov;
            }

            static void dump_matrices( string filename = "" )
            {
                GLfloat modelview[16];
                glGetFloatv( GL_MODELVIEW_MATRIX, modelview );

                GLfloat projection[16];
                glGetFloatv( GL_PROJECTION_MATRIX, projection ); 

                string mview_dump = matrix_to_string( modelview, 4, true );
                string proj_dump = matrix_to_string( projection, 4, true );

                string tot = "\nModelview Matrix\n----------------\n" + mview_dump + "\n"
                             "Projection Matrix\n-----------------\n" + proj_dump + "\n";

                qDebug( tot.c_str() );
            }

            static string matrix_to_string( GLfloat* matrix, int side, bool transpose )
            {
                // transposes the matrix if necessary;
                GLfloat* to_dump = matrix;
                string d;
                char buf[20];
                GLfloat tmp;
                for( int i=0; i<side; i++ )
                {
                    for( int j=0; j<side; j++ )
                    {
                        if( transpose )
                        {
                            tmp = to_dump[ j * side + i ];
                        }
                        else
                        {
                            tmp = to_dump[ i * side + j ];
                        }
                        sprintf( buf, "%f\t", tmp );
                        d += buf;
                    }
                    d += "\n";
                }
                return d;
            }

            static void project( GLdouble x, GLdouble y, GLdouble z,
                                 GLdouble* rx = 0, GLdouble* ry = 0, GLdouble* rz = 0 )
            {
                GLdouble    mv[16];
                GLdouble    pr[16];
                GLint       vp[4];

                glGetDoublev( GL_MODELVIEW_MATRIX, mv );
                glGetDoublev( GL_PROJECTION_MATRIX, pr );
                glGetIntegerv( GL_VIEWPORT, vp );

                GLdouble resultX = 0.0, resultY = 0.0, resultZ = 0.0;
                gluProject( x, y, z, mv, pr, vp, &resultX, &resultY, &resultZ );

                if( rx ) *rx = resultX;
                if( ry ) *ry = resultY;
                if( rz ) *rz = resultZ;
            }

            static void dumpProjection( GLdouble x, GLdouble y, GLdouble z )
            {
                GLdouble rx, ry, rz;
                project( x, y, z, &rx, &ry, &rz );
                qDebug( "(%f, %f, %f) -> (%f, %f, %f)", x, y, z, rx, ry, rz );
            }

        };
    }
}

#endif // VIEWING_INFO_H
