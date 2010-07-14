#ifndef GL_TRACKBALL_WIDGET_H
#define GL_TRACKBALL_WIDGET_H

/* libs includes */
#include <GL/glew.h>

#include <QGLWidget>
#include <QMouseEvent>

#include <vcg/space/point3.h>
#include <wrap/gui/trackball.h>

using namespace vcg;

namespace agl
{
    namespace vcg_interface
    {
        /* ****************** gl_trackball ******************************************* */

        class gl_trackball
        {

        public:

            gl_trackball( void );
            gl_trackball( gl_trackball& tb );

            bool        active;
            bool        visible;
            Point3f     center;
            float       radius;
            bool        zoom_only;

        };

        /* *************************************************************************** */

        /* ****************** gl_trackball_widget ************************************ */

        class gl_trackball_widget: public QGLWidget
        {

            Q_OBJECT

        public:

            /* -------------- constructors and destructor ---------------------------- */

            gl_trackball_widget( QWidget*               parent              = 0,
                                 const gl_trackball*    trackball_params    = 0,
                                 const QGLWidget*       share_widget        = 0,
                                 Qt::WindowFlags        f                   = 0 );

            gl_trackball_widget( QGLContext*            context,
                                 const gl_trackball*    trackball_params    = 0,
                                 QWidget*               parent              = 0,
                                 const QGLWidget*       share_widget        = 0,
                                 Qt::WindowFlags        f                   = 0 );

            gl_trackball_widget( const QGLFormat&       format,
                                 const gl_trackball*    trackball_params    = 0,
                                 QWidget*               parent              = 0,
                                 const QGLWidget*       share_widget        = 0,
                                 Qt::WindowFlags        f                   = 0 );

            virtual ~gl_trackball_widget( void );

            /* ----------------------------------------------------------------------- */

            /* -------------- public functions --------------------------------------- */

            void set_trackball_parameters   ( gl_trackball& trackball_params );
            void set_trackball_enabled      ( bool on );

            /* ----------------------------------------------------------------------- */

        protected:

            gl_trackball trackball_params;
            Trackball trackball;

            void initialize                 ( const gl_trackball* tp );
            void trackball_transformation   ( void );
            void update_trackball           ( void );

            virtual void mousePressEvent    ( QMouseEvent *event );
            virtual void mouseReleaseEvent  ( QMouseEvent *event );
            virtual void mouseMoveEvent     ( QMouseEvent *event );
            virtual void wheelEvent         ( QWheelEvent *event );

            virtual void mouse_down         ( QMouseEvent *event );
            virtual void mouse_up           ( QMouseEvent *event );
            virtual void mouse_move         ( QMouseEvent *event );
            virtual void wheel_move         ( QWheelEvent *event );

        private:
            int prev_y;

        };

        /* *************************************************************************** */
    }
}


#endif // GL_TRACKBALL_WIDGET_H
