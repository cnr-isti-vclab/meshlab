#ifndef SOBEL_DETECTOR_H
#define SOBEL_DETECTOR_H

#include "feature_detector.h"

namespace agl
{
    namespace vrs
    {
        class SobelDetector: public FeatureDetector
        {
        public:
            SobelDetector( void )
                :FeatureDetector( "sobel" )
            {
                ;
            }

            ~SobelDetector( void )
            {
                ;
            }

            virtual int getInputAttribute( void )
            {
                return VRSParameters::NORMAL;
            }

            virtual int getAuxiliaryAttributes( void )
            {
                return 0;
            }
        };
    }
}

#endif // SOBEL_DETECTOR_H
