#ifndef RESOURCES_H
#define RESOURCES_H

#include "shaders.h"
#include "utils.h"
#include "configurations.h"

namespace vrs
{
    class VRSParameters
    {
    public:
        int povs;                           // point of views
        float coneAxis[3];                  // looking-cone axis
        float coneGap;                      // looking-cone gap

        int uniformResolution;              // uniform sampling textures resolution (n x n)
        int featureResolution;              // feature sampling textures resolution (n x n)
        int attributeMask;                  // indicates which attributes to extract
        float smallDepthJump;               // the maximum allowable depth jump to recognize mesh patches continuity
        float angleThreshold;               // the threshold angle (degrees) used in feature detection step
        float bigDepthJump;                 // used to detect big depth offset
        float frontFacingCone;              // the angle (in degrees) used to determine whether a pixel "is facing" the observer

        // the available attributes of each output sample
        enum SampleAttributes
        {
            NORMAL      = 0x00000001,
            POSITION    = 0x00000002
                      };
    };

    class SamplerResources
    {
    public:
        SamplerResources( VRSParameters* params )
        {
            shaders     = 0;
            pdm         = 0;
            configs     = 0;
            fbo         = 0;
            this->params = params;
        }

        ~SamplerResources( void ){}

        bool init( void )
        {
            shaders = new Shaders( params->attributeMask );
            fbo = new Fbo();
            pdm = new PixelDataManager();
            configs = new Configurations( pdm, params->uniformResolution,
                                          params->attributeMask );
            return true;
        }

        void free( void )
        {
            delete configs;
            delete pdm;
            delete fbo;
            delete shaders;
        }

        Shaders* shaders;
        PixelDataManager* pdm;
        Configurations* configs;
        Fbo* fbo;
        VRSParameters* params;
    };
}

#endif // RESOURCES_H
