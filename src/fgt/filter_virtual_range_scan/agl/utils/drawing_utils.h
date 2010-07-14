#ifndef DRAWING_UTILS_H
#define DRAWING_UTILS_H

#include <GL/glew.h>
#include <stdlib.h>
#include <memory.h>
#include <Qt>

namespace agl
{
    namespace utils
    {
        class drawing_utils
        {

        public:

            static void save_matrices( void )
            {
                glMatrixMode (GL_PROJECTION);
                glPushMatrix ();
                glLoadIdentity ();
                glMatrixMode (GL_MODELVIEW);
                glPushMatrix ();
                glLoadIdentity ();
            }

            static void draw_fullscreen_quad( void )
            {
                glBegin (GL_QUADS);
                glVertex3i (-1, -1, -1);
                glVertex3i (1, -1, -1);
                glVertex3i (1, 1, -1);
                glVertex3i (-1, 1, -1);
                glEnd ();
            }

            static void restore_matrices( void )
            {
                glMatrixMode (GL_PROJECTION);
                glPopMatrix ();
                glMatrixMode (GL_MODELVIEW);
                glPopMatrix ();
            }

            /*
                feeds the pipeline with <count> vertices whose coordinates are
                (x, y) pair, where 0 <= x < x_max and 0 <= y < y_max. If
                count > x_max * y_max, then only x_max * y_max points are feed
                into the pipeline.
             */
            static void feed_coords( int x_max, int y_max, int count )
            {
                int max_points = x_max * y_max;
                if( count > max_points ) count = max_points;

                int rows = count / x_max;
                int cols = count % x_max;

                glBegin( GL_POINTS );


                int i = 0;
                for( i = 0; i < rows; i++ )
                {
                    for( int j = 0; j < x_max; j++ )
                    {
                        glVertex2i( j, i );
                    }
                }

                for( int j = 0; j < cols; j++ )
                {
                    glVertex2i( j, i );
                }

                glEnd();
            }
        };
    }
}

#endif // DRAWING_UTILS_H
