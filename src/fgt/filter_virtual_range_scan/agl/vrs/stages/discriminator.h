#ifndef DISCRIMINATOR_H
#define DISCRIMINATOR_H

#include "stage.h"
#include "../vrs_parameters.h"

namespace agl
{
    namespace vrs
    {
        class Discriminator: public Stage
        {
        public:
            Discriminator( VRSParameters& params )
            {
                discriminatorShader = 0;
                if( params.generateUniformSamples ) return;
                discriminatorShader = createShader( "discriminator" );
                assert( discriminatorShader );
                ssv.clear();
                setThreshold( params.discriminatorThreshold );

                texture2d* inputPrototype = res->pdm[ "inputMask" ]->data;
                FboConfiguration* conf = buildConfiguration( inputPrototype, 1 );
                res->fbo.saveConfiguration( "discriminator", conf );

                texture2d* output = (*conf)[ GL_COLOR_ATTACHMENT0 ]->renderTarget;
                PixelData* pData = new PixelData( output );
                res->pdm.saveData( "outMask", pData );
            }

            ~Discriminator( void )
            {
                if( discriminatorShader )
                {
                    delete discriminatorShader;
                }
            }

            virtual void go( void )
            {
                PixelData* pData = res->pdm[ "featureMap" ];
                assert( pData );
                PixelData* iMask = res->pdm[ "inputMask" ];

                ssv.clear();
                ssv.set_uniform_vector( "threshold", GL_FLOAT, &threshold );
                ssv.bind_texture( "featureMap", 0, pData->data );
                ssv.bind_texture( "inputMask", 1, iMask->data );

                fullscreenPass( "discriminator", discriminatorShader );
            }

            void setThreshold( float threshold )
            {
                this->threshold = threshold;
            }

        private:
            shader* discriminatorShader;
            float threshold;

        };
    }
}

#endif // DISCRIMINATOR_H
