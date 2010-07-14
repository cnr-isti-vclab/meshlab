#ifndef KILLER_H
#define KILLER_H

#include "stage.h"
#include "../../utils/viewing_info.h"

using namespace agl::utils;

namespace agl
{
    namespace vrs
    {
        class Killer: public Stage
        {

        public:
            Killer( void )
            {
                killerShader = createShader( "killer" );

                texture2d* outputPrototype = res->pdm[ "outMask" ]->data;
                killerConfiguration = FboConfigurationFactory::createFromTexture
                                      ( &(res->pdm.tm), outputPrototype, 2, false, false );
                PixelData* initialDepthBuffer = res->pdm[ "inputDepthBuffer" ];
                FboAttachment* depthAttachment = new FboAttachment( GL_DEPTH_ATTACHMENT,
                                                                    initialDepthBuffer->data, 0 );
                killerConfiguration->addAttachment( depthAttachment );
                res->fbo.saveConfiguration( "killer-map configuration", killerConfiguration );

                texture2d* killerMap = (*killerConfiguration)[ GL_COLOR_ATTACHMENT0 ]->renderTarget;
                PixelData* killerData = new PixelData( killerMap );
                res->pdm.saveData( "killerMap", killerData );

                texture2d* sampleCoordsMap = (*killerConfiguration)[ GL_COLOR_ATTACHMENT1 ]->renderTarget;
                PixelData* coordsData = new PixelData( sampleCoordsMap );
                res->pdm.saveData( "deadMap", coordsData );
            }

            ~Killer( void )
            {
                delete killerShader;
            }

            virtual void go( void )
            {
                string inputNames[ 4 ] = { "bestPosition", "bestNormal", "inputEyeNormal", "outMask" };
                vector< PixelData* > inputData;
                res->pdm.getData( inputNames, 4, inputData );

                int bestDataSide = inputData[0]->data->get_width();
                int bestElements = inputData[0]->length;

                ssv.clear();
                for( int i=0; i<4; i++ )
                {
                    ssv.bind_texture( inputNames[i], (GLint)i, inputData[i]->data );
                }
                killerShader->set_variables( &ssv );

                res->fbo.setConfiguration( "killer-map configuration" );
                killerShader->load();

                glClear( GL_COLOR_BUFFER_BIT );
                drawing_utils::feed_coords( bestDataSide, bestDataSide, bestElements );

                killerShader->unload();
            }

        private:
            shader* killerShader;
            FboConfiguration* killerConfiguration;

        };
    }
}

#endif // KILLER_H
