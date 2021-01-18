#ifndef MY_GL_WIDGET_H
#define MY_GL_WIDGET_H

#include    "vs/sampler.h"

#include    <QGLWidget>
#include    <QString>

using namespace vs;

class MyGLWidget: public QGLWidget
{

public:
    MyGLWidget( VSParameters*   params,
                MeshModel*      inputMeshModel,
                CMeshO*         uniformSamplesMesh,
                CMeshO*         featureSamplesMesh,
                QGLWidget*      shareWidget )
                    : QGLWidget( (QWidget*)0, shareWidget )
    {
        this->params = params;
        this->inputMeshModel = inputMeshModel;
        this->uniformSamplesMesh = uniformSamplesMesh;
        this->featureSamplesMesh = featureSamplesMesh;
        this->setFixedSize( 200, 200 );
        this->result = false;

        updateGL();
    }

    VSParameters*       params;
    MeshModel*          inputMeshModel;
    CMeshO*             uniformSamplesMesh;
    CMeshO*             featureSamplesMesh;
    SamplerListener*    samplerListener;
    bool                result;
    QString             errorString;

protected:
    virtual void initializeGL( void )
    {
        // glew initialization
        GLenum result = glewInit();
        if( result != GLEW_OK )
        {
            errorString = "Cannot initialize glew.";
            result = false;
            return;
        }

        // check that everything is supported
        if( !checkOpenGLCapabilities() )
        {
            result = false;
            return;
        }

        // OpenGL initialization
        glEnable( GL_DEPTH_TEST );
        glEnable( GL_TEXTURE_2D );
        glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );

        // samples generation
        if( this->context()->isValid() )
        {
            Sampler< CMeshO >::generateSamples
                    ( params, inputMeshModel, uniformSamplesMesh, featureSamplesMesh, 0 );
            this->result = true;
        }
        else
        {
            errorString = "The new OpenGL context is not valid.";
            this->result = false;
        }        
    }

    virtual void paintGL( void )
    {
        ;
    }

private:

    bool checkOpenGLCapabilities( void )
    {
        map< QString, QString > extToErr;
        extToErr[ "GL_ARB_vertex_shader GL_ARB_fragment_shader" ]   = "Vertex and fragment shaders are not supported.";
        extToErr[ "GL_EXT_framebuffer_object" ]                     = "Framebuffer objects are not supported.";
        extToErr[ "GL_ARB_texture_float" ]                          = "Float textures are not supported.";
        extToErr[ "GL_ARB_draw_buffers" ]                           = "Multiple render buffers are not supported.";

        map< QString, QString >::iterator mi = extToErr.begin();
        bool everythingOk = true;
        while( everythingOk && mi != extToErr.end() )
        {
            
            everythingOk = (bool)( glewIsSupported( qPrintable((*mi).first) ) );
            ++mi;
        }

        if( !everythingOk )
        {
            --mi;
            errorString = (*mi).second;
            return false;
        }

        // checks that at least 5 render targets are available
        GLint v = (GLint)0;
        glGetIntegerv( GL_MAX_COLOR_ATTACHMENTS, &v );
        if( v < 5 )
        {
            errorString = "At least 5 render targets must be available for an FBO.";
            return false;
        }

        glGetIntegerv( GL_MAX_DRAW_BUFFERS, &v );
        if( v < 5 )
        {
            errorString = "Cannot write to 5 draw buffers at the same time.";
            return false;
        }

        return true;
    }

};

#endif // MY_GL_WIDGET_H
