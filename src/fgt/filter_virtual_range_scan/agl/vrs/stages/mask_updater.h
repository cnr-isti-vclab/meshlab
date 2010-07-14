#ifndef MASK_UPDATER_H
#define MASK_UPDATER_H

#include "stage.h"

namespace agl
{
    namespace vrs
    {
        class MaskUpdater: public Stage
        {
        public:
            MaskUpdater( void )
            {
                texture2d* prototype = res->pdm[ "outMask" ]->data;
                muConf = buildConfiguration( prototype, 1 );
                res->fbo.saveConfiguration( "mask updater configuration", muConf );

                texture2d* updatedMask = (*muConf)[ GL_COLOR_ATTACHMENT0 ]->renderTarget;
                PixelData* pData = new PixelData( updatedMask );
                res->pdm.saveData( "updatedMask", pData );

                muShader = createShader( "mask_updater" );
            }

            ~MaskUpdater( void )
            {
                delete muShader;
            }

            virtual void go( void )
            {
                string inputNames[] = { "outMask", "killerMap" };
                vector< PixelData* > inputData;
                res->pdm.getData( inputNames, 2, inputData );

                ssv.clear();
                ssv.bind_texture( "outMask", 0, inputData[0]->data );
                ssv.bind_texture( "killerMap", 1, inputData[1]->data );

                fullscreenPass( "mask updater configuration", muShader );
            }

        private:
            FboConfiguration* muConf;
            shader* muShader;

        };
    }
}

#endif // MASK_UPDATER_H
