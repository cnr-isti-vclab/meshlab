#ifndef FEATURE_DETECTION_H
#define FEATURE_DETECTION_H

#include "stage.h"
#include "../detectors/feature_detector.h"
#include <math.h>

#define PI 3.1415926535

namespace agl
{
    namespace vrs
    {
        class FeatureDetection: public Stage
        {
        public:

            FeatureDetection( VRSParameters& params )
            {
                p = &params;
                featureDetectorShader = 0;
                if( params.generateUniformSamples ) return;

                texture2d* inputPrototype = res->pdm[ "inputMask" ]->data;
                depthRangeConf = buildConfiguration( inputPrototype, 1 );
                res->fbo.saveConfiguration( "depth_range_configuration", depthRangeConf );

                depthRangeAttachment = (*depthRangeConf)[ GL_COLOR_ATTACHMENT0 ];
                texture2d* depthRangePyramid = depthRangeAttachment->renderTarget;
                depthRangePyramid->generate_mipmaps();

                FboConfiguration* featureConf = buildConfiguration( inputPrototype, 1 );
                res->fbo.saveConfiguration( "feature_detection_configuration", featureConf );

                texture2d* featureMap = (*featureConf)[ GL_COLOR_ATTACHMENT0 ]->renderTarget;
                PixelData* featureData = new PixelData( featureMap );
                res->pdm.saveData( "featureMap", featureData );

                copier = createShader( "copier" );
                depthPyramidShader = createShader( "detectors/depth_range_detector" );
                featureDetectorShader = createShader( "detectors/detector" );

                depthJump = params.maxDepthJump;
                angleThreshold = params.angleThreshold;
            }

            ~FeatureDetection( void )
            {
                if( featureDetectorShader )
                {
                    delete featureDetectorShader;
                    delete depthPyramidShader;
                    delete copier;
                }
            }

            virtual void go( void )
            {
                static int view = 0;
                char b[30];
                sprintf( b, "%d", view++ );

                PixelData* depthData = res->pdm[ "inputDepth" ];

                res->fbo.setConfiguration(0);
                ssv.clear();
                ssv.bind_texture( "inputMap", 0, depthData->data );

                fullscreenPass( "depth_range_configuration", copier );
                //res->fbo.saveScreenshots( b );
                res->fbo.setConfiguration(0);

                texture2d* pyramid = depthRangeAttachment->renderTarget;
                int levels = pyramid->get_mipmap_levels();
                texture2d* inputMask = res->pdm[ "inputMask" ]->data;

                ssv.clear();
                ssv.bind_texture( "pyramid", 0, pyramid );
                ssv.bind_texture( "inputMask", 1, inputMask );

                glPushAttrib( GL_VIEWPORT_BIT );

                for( int i=1; i<levels; i++ )
                {
                    glViewport( 0, 0, pyramid->get_width(i), pyramid->get_height(i));
                    ssv.set_uniform_vector( "level", GL_INT, &i );
                    depthRangeAttachment->lod = i;
                    fullscreenPass( "depth_range_configuration", depthPyramidShader );
                }
                depthRangeAttachment->lod = 0;
                glPopAttrib();

                res->fbo.setConfiguration(0);

                /* uncomment to dump depth range pyramid
                char buf[50];
                for( int i=0; i<levels; i++ )
                {
                    sprintf( buf, "depth_range%d.txt", i );
                    pyramid->download_and_dump_to_file( buf, i );
                }
                */

                // downloads the min and max depth value
                float minDepth, maxDepth;
                client_texture2d_f* pyramidTop = texture2d::download< float >( pyramid, levels - 1 );
                texture_pixel< float >* top = pyramidTop->pixels[0];
                minDepth = top->x();
                maxDepth = top->y();
                delete pyramidTop;

                float smallJump = ( maxDepth - minDepth ) * depthJump;
                float bigJump = ( maxDepth - minDepth ) * p->bigDepthJump;
                texture2d* inputNormal = res->pdm[ "inputNormal" ]->data;
                texture2d* inputEyeNormal = res->pdm[ "inputEyeNormal" ]->data;

                float angleInRadians = ( angleThreshold * PI ) / 180.0;
                float thresholdCosine = cos( angleInRadians );

                angleInRadians = ( (p->frontFacingCone/2.0) * PI ) / 180.0;
                float frontFacingCosine = cos( angleInRadians );

                ssv.clear();
                ssv.bind_texture( "inputMask", 0, inputMask );
                ssv.bind_texture( "inputDepth", 1, depthData->data );
                ssv.bind_texture( "inputNormal", 2, inputNormal );
                ssv.bind_texture( "inputEyeNormal", 2, inputEyeNormal );
                ssv.set_uniform_vector( "smallDepthJump", GL_FLOAT, &smallJump );
                ssv.set_uniform_vector( "bigDepthJump", GL_FLOAT, &bigJump );
                ssv.set_uniform_vector( "thresholdCosine", GL_FLOAT, &thresholdCosine );
                ssv.set_uniform_vector( "frontFacingCosine", GL_FLOAT, &frontFacingCosine );

                fullscreenPass( "feature_detection_configuration", featureDetectorShader, false );

                res->fbo.setConfiguration(0);
            }

        private:
            shader* copier;
            shader* depthPyramidShader;
            shader* featureDetectorShader;
            FboAttachment* depthRangeAttachment;
            FboConfiguration* depthRangeConf;

            VRSParameters* p;
            float depthJump;
            float angleThreshold;
        };
    }
}

#endif // FEATURE_DETECTION_H
