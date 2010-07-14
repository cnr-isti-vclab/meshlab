#ifndef START_H
#define START_H

#include "stage.h"
#include "../povs_generator.h"
#include "../vrs_parameters.h"
#include "../../vcg_interface/vcg_renderer.h"

#define MESHLAB_FILTER 1

using namespace agl::vcg_interface;

namespace agl
{
    namespace vrs
    {
        template< class Mesh >
        class Start: public Stage
        {

        public:
            typedef typename Mesh::ScalarType           ScalarType;
            typedef typename vcg::Point3< ScalarType >  MyPoint;

            Start( VRSParameters* params, Mesh* inputMesh )
            {
                // generates the shader
                generateExtractorShader( attributeNames );
                assert( extractor );

                // initializes the renderer
                vcg_r.set_mesh( inputMesh );

                // creates and saves the extraction configuration
                FboConfiguration* startConfiguration = FboConfigurationFactory::create
                                                       ( &(res->pdm.tm),
                                                         params->viewportResolution,
                                                         params->viewportResolution,
                                                         GL_RGB32F,
                                                         GL_RGB,
                                                         GL_FLOAT,
                                                         attributesCount + 1,
                                                         true,
                                                         false );
                res->fbo.saveConfiguration( "attributes extraction", startConfiguration );
                assert( startConfiguration );

                // creates and saves the input PixelData objects
                vector< texture2d* > confTextures;
                startConfiguration->getTextures( confTextures, true );
                assert( confTextures.size() == attributeNames.size() + 1 );

                int dataLength = confTextures[0]->get_width() * confTextures[0]->get_height();
                PixelData* tmpData = 0;

                string tmpName = "";
                for( unsigned int i=0; i<confTextures.size() - 1; i++ )
                {
                    tmpData = new PixelData( confTextures[i] );
                    tmpName = string("input") + attributeNames[i];
                    res->pdm.saveData( tmpName, tmpData );
                }

                int maskIndex = confTextures.size() - 1;
                tmpData = new PixelData();
                tmpData->data = confTextures[ maskIndex ];
                tmpData->length = dataLength;
                res->pdm.saveData( "inputMask", tmpData );

                texture2d* depthTexture = (*startConfiguration)[ GL_DEPTH_ATTACHMENT ]->renderTarget;
                PixelData* depthData = new PixelData( depthTexture );
                res->pdm.saveData( "inputDepthBuffer", depthData );

                // generates povs
                ScalarType eyeDistance = inputMesh->bbox.Diag() / 2;
                PovsGenerator< float >::generatePovs
                        ( params->povs, eyeDistance, inputMesh->bbox.Center(), povs );
                meshCenter = inputMesh->bbox.Center();

                // sets the projection matrix and the viewport transformation
                int resolution = params->viewportResolution;
                glViewport( 0, 0, (GLsizei)resolution, (GLsizei)resolution );

                glMatrixMode( GL_PROJECTION );
                glLoadIdentity();
                glOrtho( -eyeDistance, eyeDistance, -eyeDistance, eyeDistance, -eyeDistance, eyeDistance );

                // initializes the other fields
                currentPov = 0;

                // Meshlab fix
                if( MESHLAB_FILTER )
                {
                    auxConfiguration = buildConfiguration( depthTexture->get_width(), 1 );
                    fixerShader = createShader( "depth_fixer" );
                }
            }

            ~Start( void )
            {
                delete extractor;

                if( MESHLAB_FILTER )
                {
                    delete auxConfiguration;
                    delete fixerShader;
                }
            }

            virtual void go( void )
            {
                glMatrixMode( GL_MODELVIEW );
                glLoadIdentity();

                MyPoint p = povs[ currentPov ];
                gluLookAt( meshCenter.X(), meshCenter.Y(), meshCenter.Z(),
                           p.X(), p.Y(), p.Z(),
                           0.0, 1.0, 0.0 );

                res->fbo.setConfiguration( "attributes extraction" );
                extractor->load();

                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

                if( !MESHLAB_FILTER )
                {
                    const GLfloat zClear[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
                    glClearBufferfv(GL_COLOR, 0, zClear );
                }

                vcg_r.render();

                /*
                char buf[50];
                sprintf( buf, "pov%d", currentPov );
                res->fbo.saveScreenshots( buf );
                */

                extractor->unload();

                if( MESHLAB_FILTER )
                {
                    FboConfiguration* c = res->fbo.getConfiguration();
                    texture2d* depth = (*c)[GL_COLOR_ATTACHMENT0]->renderTarget;

                    res->fbo.setConfiguration(0);
                    PixelData* maskData = res->pdm["inputMask"];
                    PixelData* depthData = res->pdm["inputDepth"];
                    ssv.clear();
                    ssv.bind_texture( "inputMask", 0, maskData->data );
                    ssv.bind_texture( "inputDepth", 1, depth );
                    fullscreenPass( auxConfiguration, fixerShader );
                    //res->fbo.saveScreenshots( string(buf) + "depth" );
                    res->fbo.setConfiguration(0);
                    texture2d* newDepth = (*auxConfiguration)[GL_COLOR_ATTACHMENT0]->renderTarget;
                    depthData->data = newDepth;
                }
            }

            bool nextPov( void )
            {
                currentPov++;
                return !( currentPov >= (int)povs.size() );
            }

            void setCurrentPov( int pov )
            {
                currentPov = pov;
            }

            void getAttributeNames( string prefix, vector< string >& target )
            {
                for( unsigned int i=0; i<attributeNames.size(); i++ )
                {
                    target.push_back( prefix + attributeNames[i] );
                }
            }

            vector< string > attributeNames;
            vector< MyPoint > povs;

        private:
            VRSParameters* params;
            vcg_renderer< Mesh > vcg_r;
            shader* extractor;
            int attributesCount;
            MyPoint meshCenter;
            int currentPov;

            // only for Meshlab (OpenGL version too old!)
            FboConfiguration* auxConfiguration;
            shader* fixerShader;

            void generateExtractorShader( vector< string >& attNames )
            {
                attributesCount = 0;
                string vars, vsBody, fsBody;
                char buf[ 50 ];

                attNames.push_back( "Depth" );
                fsBody += "float d = gl_FragCoord.z;\n";
                fsBody += "gl_FragData[0] = vec4( d, d, d, 0.0 );\n";
                attributesCount++;

                attNames.push_back( "Position" );
                vars += "varying vec4 pos;\n";
                vsBody += "pos = gl_Vertex;\n";
                sprintf( buf, "gl_FragData[%d] = vec4( pos.xyz, 0.0 );\n", attributesCount );
                fsBody += buf;
                attributesCount++;

                attNames.push_back( "Normal" );
                vars += "varying vec3 norm;\n";
                vsBody += "norm = gl_Normal;\n";
                fsBody += "vec3 the_norm = normalize( norm );\n";
                sprintf( buf, "gl_FragData[%d] = vec4( the_norm, 0.0 );\n", attributesCount );
                fsBody += buf;
                attributesCount++;

                attNames.push_back( "EyeNormal" );
                vars += "varying vec3 eye_norm;\n";
                vsBody += "eye_norm = gl_NormalMatrix * gl_Normal;\n";
                fsBody += "vec3 the_eye_norm = normalize( eye_norm );\n";
                sprintf( buf, "gl_FragData[%d] = vec4( the_eye_norm, 0.0 );\n", attributesCount );
                fsBody += buf;
                attributesCount++;

                sprintf( buf, "gl_FragData[%d] = vec4( 1.0, 0.0, 0.0, 0.0);\n", attributesCount );
                fsBody += buf;

                vsBody += "gl_Position = ftransform();\n";
                string vsSource = vars + "\nvoid main()\n{\n" + vsBody + "\n}";
                string fsSource = vars + "\nvoid main()\n{\n" + fsBody + "\n}";
                extractor = shader_set::create_from_source( vsSource, fsSource, "" );
            }

        };
    }
}

#endif // START_H
