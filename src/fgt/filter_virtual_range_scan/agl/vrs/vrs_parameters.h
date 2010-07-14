#ifndef VRS_PARAMETERS_H
#define VRS_PARAMETERS_H

#include "detectors/feature_detector.h"

namespace agl
{
    namespace vrs
    {
        class VRSParameters
        {
        public:
            int povs;                           // point of views
            int viewportResolution;             // the snapshots will be n x n textures
            int attributeMask;                  // indicates which attributes to extract
            bool generateUniformSamples;        // indicates wheter to generate uniform or feature samples
            float maxDepthJump;                 // the maximum allowable depth jump in feature detection stage
            float angleThreshold;               // the threshold angle (degrees) used in feature detection step
            float bigDepthJump;                 // used to detect big depth offset
            float frontFacingCone;              // the angle (in degrees) used to determine whether a pixel "is facing"
                                                // the observer

            vector< pair< string, FeatureDetector* > > detectors;
            float discriminatorThreshold;

            // the available attributes of each output sample
            enum SampleAttributes
            {
                NORMAL      = 0x00000001,
                POSITION    = 0x00000002
            };

        };
    }
}

#endif // VRS_PARAMETERS_H
