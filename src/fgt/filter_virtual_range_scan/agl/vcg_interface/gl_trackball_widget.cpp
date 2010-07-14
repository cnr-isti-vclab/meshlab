#include "gl_trackball_widget.h"

#include <wrap/qt/trackball.h>

using namespace agl::vcg_interface;

/* **************** gl_trackball implementation ************************************** */

gl_trackball::gl_trackball( void )
{
    active      = true;
    visible     = false;
    center      = Point3f( 0.0, 0.0, 0.0 );
    radius      = 1.0;
    zoom_only   = false;
}

gl_trackball::gl_trackball( gl_trackball &tb )
{
    active      = tb.active;
    visible     = tb.visible;
    center      = tb.center;
    radius      = tb.radius;
    zoom_only   = tb.zoom_only;
}

/* *********************************************************************************** */

/* **************** gl_trackball_widget implementation ******************************* */

/* --------------- constructors and destructor --------------------------------------- */

gl_trackball_widget::gl_trackball_widget( QWidget*               parent,
                                          const gl_trackball*    trackball_params,
                                          const QGLWidget*       share_widget,
                                          Qt::WindowFlags        f )
    :QGLWidget( parent, share_widget, f )
{
    initialize( trackball_params );
}

gl_trackball_widget::gl_trackball_widget( QGLContext*            context,
                                          const gl_trackball*    trackball_params,
                                          QWidget*               parent,
                                          const QGLWidget*       share_widget,
                                          Qt::WindowFlags        f )
    :QGLWidget( context, parent, share_widget, f )
{
    initialize( trackball_params );
}

gl_trackball_widget::gl_trackball_widget( const QGLFormat&       format,
                                          const gl_trackball*    trackball_params,
                                          QWidget*               parent,
                                          const QGLWidget*       share_widget,
                                          Qt::WindowFlags        f )
    :QGLWidget( format, parent, share_widget, f )
{
    initialize( trackball_params );
}

gl_trackball_widget::~gl_trackball_widget( void )
{
    ;
}

/* ----------------------------------------------------------------------------------- */

/* -------------------------- public functions --------------------------------------- */

void gl_trackball_widget::set_trackball_parameters( gl_trackball& trackball_params )
{
    this->trackball_params = trackball_params;
    update_trackball();
}

void gl_trackball_widget::set_trackball_enabled( bool on )
{
    trackball_params.active = on;
}

/* ----------------------------------------------------------------------------------- */

/* -------------------------- protected functions ------------------------------------ */

void gl_trackball_widget::initialize( const gl_trackball* tp )
{
    if( tp )
    {
        trackball_params = *tp;
    }
    update_trackball();
    setMouseTracking( true );
}

void gl_trackball_widget::trackball_transformation( void )
{
    trackball.GetView();
    trackball.Apply( trackball_params.visible );
}

void gl_trackball_widget::update_trackball( void )
{
    trackball.center = trackball_params.center;
    trackball.radius = trackball_params.radius;
    trackball.Reset();
}

void gl_trackball_widget::mousePressEvent( QMouseEvent *event )
{
    if( trackball_params.active && !trackball_params.zoom_only )
    {
        Qt::KeyboardModifiers mods = event->modifiers();
        Qt::MouseButton btn = event->button();
        Trackball::Button vcgbtn = QT2VCG( btn, mods );
        trackball.MouseDown( event->x(), height() - event->y(), vcgbtn );
    }
    mouse_down( event );
    updateGL();
}

void gl_trackball_widget::mouseReleaseEvent( QMouseEvent *event )
{
    if( trackball_params.active && !trackball_params.zoom_only )
    {
        Qt::KeyboardModifiers mods = event->modifiers();
        Qt::MouseButton btn = event->button();
        Trackball::Button vcgbtn = QT2VCG( btn, mods );
        trackball.MouseUp( event->x(), height() - event->y(), vcgbtn );
    }
    mouse_up( event );
    updateGL();
}

void gl_trackball_widget::mouseMoveEvent( QMouseEvent *event )
{
    if( trackball_params.active && !trackball_params.zoom_only )
    {
        trackball.MouseMove( event->x(), height() - event->y() );
    }
    mouse_move( event );
    updateGL();
}

void gl_trackball_widget::wheelEvent( QWheelEvent *event )
{
    if( trackball_params.active )
    {
        float notch = ( event->delta() > 0? 1.0 : -1.0 );
        Qt::KeyboardModifiers mods = event->modifiers();
        Trackball::Button btn = QTWheel2VCG( mods );
        trackball.MouseWheel( notch, btn );
    }
    wheel_move( event );
    updateGL();
}

void gl_trackball_widget::mouse_down( QMouseEvent *event )
{
    ;
}

void gl_trackball_widget::mouse_up( QMouseEvent *event )
{
    ;
}

void gl_trackball_widget::mouse_move( QMouseEvent *event )
{
    ;
}

void gl_trackball_widget::wheel_move( QWheelEvent *event )
{
    ;
}

/* ----------------------------------------------------------------------------------- */

/* *********************************************************************************** */
