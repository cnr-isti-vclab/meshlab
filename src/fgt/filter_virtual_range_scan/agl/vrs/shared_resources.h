#ifndef SHARED_RESOURCES_H
#define SHARED_RESOURCES_H

#include "../buffers/fbo.h"
#include "../textures/textures_manager.h"
#include "pixel_data_manager.h"

using namespace agl::buffers;

namespace agl
{
    namespace vrs
    {
        class SharedResources
        {

        public:

            static SharedResources* getInstance( void );
            static void deleteInstance( void );

            Fbo                 fbo;
            PixelDataManager    pdm;

            void clearResources( void );

        private:

            static SharedResources* instance;

            SharedResources ( void );
            ~SharedResources( void );

        };
    }
}

#endif // SHARED_RESOURCES_H
