#ifndef MY_GL_WIDGET_H
#define MY_GL_WIDGET_H

#include "filter_virtual_range_scan.h"

#include <QGLWidget>

using namespace vrs;

class MyGLWidget: public QGLWidget
{

public:
    MyGLWidget( QWidget* parent ): QGLWidget( parent )
    {
        this->setFixedSize( 200, 200 );
    }

    VRSParameters*      params;
    CMeshO*             inputMesh;
    CMeshO*             uniformSamplesMesh;
    CMeshO*             featureSamplesMesh;
    SamplerListener*    samplerListener;

protected:
    virtual void initializeGL( void )
    {
        /*
        qDebug( "Initializing MyGLWidget..." );
        glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
        glEnable( GL_DEPTH_TEST );

        if( this->context()->isValid() )
        {
            qDebug( "The OpenGL context is valid" );
        }
        else
        {
            qDebug( "The OpenGL context is not valid." );
        }

        glMatrixMode( GL_PROJECTION );
        glLoadIdentity();

        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();

        glViewport( (GLint)0, (GLint)0, (GLsizei)200, (GLsizei)200 );

        glColor3f( 1.0f, 0.0f, 0.0f );
        */

        glewInit();
        glEnable( GL_DEPTH_TEST );
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

        if( this->context()->isValid() )
        {
            qDebug( "The OpenGL context is valid" );
        }
        else
        {
            qDebug( "The OpenGL context is not valid." );
        }
    }

    virtual void paintGL( void )
    {
        /*
        FboConfiguration* conf = Utils::createFboConfiguration( 1, 64, true );
        Fbo fbo;
        fbo.bind();
        fbo.setConfiguration( conf );

        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glBegin( GL_QUADS );
        glVertex3f( -0.5f,  -0.5f,  -0.5f );
        glVertex3f(  0.5f,  -0.5f,  -0.5f );
        glVertex3f(  0.5f,   0.5f,  -0.5f );
        glVertex3f( -0.5f,   0.5f,  -0.5f );
        glEnd();

        fbo.screenshots( "test" );

        fbo.setConfiguration(0);
        fbo.unbind();

        Utils::dumpConfiguration( conf, "start" );
        delete conf;
        */

        Sampler< CMeshO > sampler( params, inputMesh, uniformSamplesMesh, featureSamplesMesh );
        sampler.listeners.push_back( samplerListener );
        sampler.generateSamples();

        hide();
    }

};

#endif // MY_GL_WIDGET_H
