#ifndef STAGE_H
#define STAGE_H

#include "../../shaders/shader_set.h"
#include "../../shaders/simple_shader_variables.h"
#include "../shared_resources.h"
#include "../../utils/drawing_utils.h"
#include "../../agl_global.h"

#include <QDir>
#include <QTextStream>

using namespace agl::shaders;
using namespace agl::utils;

namespace agl
{
    namespace vrs
    {
        class Stage
        {

        public:
            Stage( void )
            {
                res = SharedResources::getInstance();
                simpleVSSource = "void main(){ gl_Position = ftransform(); }";
                glslPath = agl::global::vrs_path + "glsl/";
            }

            virtual ~Stage( void )
            {
                ;
            }

            virtual void go( void )
            {
				;
            }

        protected:
            SharedResources* res;
            simple_shaders_variables ssv;
            string simpleVSSource;
            string glslPath;

            FboConfiguration* buildConfiguration
                    ( texture2d* prototype, int colorAttachmentsCount )
            {
                FboConfiguration* conf = FboConfigurationFactory::createFromTexture
                                         ( &(res->pdm.tm), prototype, colorAttachmentsCount, true, false );
                return conf;
            }
			
			FboConfiguration* buildConfiguration
					( int textureSide, int colorAttachmentsCount )
			{
				FboConfiguration* conf = FboConfigurationFactory::create
					( &(res->pdm.tm), textureSide, textureSide, GL_RGB32F, GL_RGB,
					  GL_FLOAT, colorAttachmentsCount, true, false );
			    return conf;
			}

            void fullscreenPass( FboConfiguration* c, shader* s )
            {
                drawing_utils::save_matrices();

                res->fbo.setConfiguration( c );
                s->set_variables( &ssv );
                s->load();

                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                drawing_utils::draw_fullscreen_quad();

                s->unload();
                drawing_utils::restore_matrices();
            }

            void fullscreenPass( string confName, shader* s, bool screenshots = false )
            {
                drawing_utils::save_matrices();

                res->fbo.setConfiguration( confName );
                s->set_variables( &ssv );
                s->load();

                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                drawing_utils::draw_fullscreen_quad();

                if( screenshots )
                {
                    res->fbo.saveScreenshots();
                }

                s->unload();
                drawing_utils::restore_matrices();
            }

            shader* createShader( string name )
            {
                /*
                shader* s = shader_set::create_from_file_fast
                            ( glslPath + name );
                return s;
                */
                return createShaderFromResource( name );
            }

            shader* createShaderFromResource( string name )
            {
                QString vertexShaderName = QString( name.c_str() ) + ".vert";
                QString fragmentShaderName = QString( name.c_str() ) + ".frag";
                QString vertexShader, fragmentShader;

                readResource( vertexShaderName, vertexShader );
                readResource( fragmentShaderName, fragmentShader );

                return shader_set::create_from_source( vertexShader.toStdString(),
                                                       fragmentShader.toStdString(),
                                                       "" );
            }

            void readResource( QString resName, QString& target )
            {
                QString completeName = QString(":/") + resName;

                QFile f( completeName );
                if(f.open( QFile::ReadOnly ) )
                {
                    QTextStream stream(&f);
                    target = stream.readAll();
                    f.close();
                }
            }

            string getSamplersDeclarations( vector< string >& inputs )
            {
                string s = "";
                char buf[150];
                for( unsigned int i=0; i<inputs.size(); i++ )
                {
                    sprintf( buf, "uniform sampler2D %s;\n", inputs[i].c_str() );
                    s += buf;
                }
                return s;
            }

            void bindInputs( vector< string >& inputs )
            {
                PixelData* tmpData;
                for( unsigned int i=0; i<inputs.size(); i++ )
                {
                    tmpData = res->pdm[ inputs[i] ];
                    assert( tmpData );
                    ssv.bind_texture( inputs[i], (GLint)i, tmpData->data );
                }
            }

            void deleteDepth( FboConfiguration* c )
            {
                FboAttachment* depthAtt = (*c)[ GL_DEPTH_ATTACHMENT ];
                assert( depthAtt );
                texture2d* depthTexture = depthAtt->renderTarget;
                res->pdm.tm.deleteTextures( &depthTexture, 1 );
            }

        };
    }
}

#endif // STAGE_H
