#ifndef MY_GL_WIDGET_H
#define MY_GL_WIDGET_H

#include "filter_virtual_range_scan.h"

#include <QGLWidget>

using namespace vs;

class MyGLWidget: public QGLWidget
{

public:
    MyGLWidget( VSParameters*   params,
                CMeshO*         inputMesh,
                CMeshO*         uniformSamplesMesh,
                CMeshO*         featureSamplesMesh )
                    : QGLWidget( (QWidget*)0 )
    {
        this->params = params;
        this->inputMesh = inputMesh;
        this->uniformSamplesMesh = uniformSamplesMesh;
        this->featureSamplesMesh = featureSamplesMesh;
        this->setFixedSize( 200, 200 );
        this->result = false;

        updateGL();
    }

    VSParameters*       params;
    CMeshO*             inputMesh;
    CMeshO*             uniformSamplesMesh;
    CMeshO*             featureSamplesMesh;
    SamplerListener*    samplerListener;
    bool                result;

protected:
    virtual void initializeGL( void )
    {
        GLenum result = glewInit();
        assert( result == GLEW_OK );
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_TEXTURE_2D );
        glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

        if( this->context()->isValid() )
        {
            Sampler< CMeshO >::generateSamples
                    ( params, inputMesh, uniformSamplesMesh, featureSamplesMesh, 0 );
            this->result = true;
        }
        else
        {
            this->result = false;
        }        
    }

    virtual void paintGL( void )
    {
        ;
    }

};

#endif // MY_GL_WIDGET_H
