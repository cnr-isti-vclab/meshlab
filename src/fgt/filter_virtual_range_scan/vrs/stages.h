#ifndef STAGES_H
#define STAGES_H

#include "stage.h"

namespace vrs
{
    /* this stage is used to filter out the pixels that are not facing the
       observer (uniform sampling only) */
    class ConeFilter: public Stage
    {
    public:
        ConeFilter( float coneAngle, SamplerResources* res )
            :Stage( res )
        {
            float angleInRadians = ( (coneAngle/2) * PI ) / 180.0;
            thresholdCosine = cos( angleInRadians );
        }
        ~ConeFilter( void ){}

        virtual void go( void )
        {
            on( "detection", "cone_filter_shader" );
            string samplers[] = { "startMask", "inputEyeNormal" };
            bindPixelData( samplers, 2, samplers );
            setShaderFloat( "thresholdCosine", thresholdCosine );
            Utils::fullscreenQuad();
            off();
        }

    private:
        float thresholdCosine;

    };

    /* the killer stage: output buffers are killerMask and deadMap */
    class Killer: public Stage
    {
    public:
        Killer( SamplerResources* res ):Stage(res){}
        ~Killer( void ){}

        virtual void go( void )
        {
            // generates the "best" eye normal map
            on( "best_eye_normal", "best_normal_map_shader" );
            string sampl[] = { "startMask", "inputEyeNormal" };
            bindPixelData( sampl, 2, sampl );
            Utils::fullscreenQuad();
            off();

            // restores the projection and modelview matrix of the current snapshot
            Utils::restoreMatrices();
            PixelData* tmpData = (*(res->pdm))[ "bestPosition" ];

            on( "killer", "killer_shader" );
            string samplers[] = { "bestPosition", "bestNormal", "outMask", "eyeNormalBest" };
            bindPixelData( samplers, 4, samplers );
            glClear( GL_COLOR_BUFFER_BIT );
            glPointSize( 3.0f );
            Utils::feed_coords( tmpData->side, tmpData->side, tmpData->length );
            glPointSize( 1.0f );
            off();

            // reset matrices
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            glMatrixMode( GL_MODELVIEW );
            glLoadIdentity();
        }
    };

    /* the mask updater stage */
    class MaskUpdater: public Stage
    {
    public:
        MaskUpdater( SamplerResources* res ): Stage( res ){}
        ~MaskUpdater( void ){}

        virtual void go( void )
        {
            on( "mask_updater", "mask_updater_shader" );
            string samplers[] = { "outMask", "killerMap" };
            bindPixelData( samplers, 2, samplers );
            Utils::fullscreenQuad();
            off();
        }
    };

    /* puts 0's where the best samples must be killed */
    class DeadMasker: public Stage
    {
    public:
        DeadMasker( SamplerResources* res ): Stage( res ){}
        ~DeadMasker( void ){}

        virtual void go( void )
        {
            adjustOutputSize();

            // computes the dead mask
            int viewportSide = (*(res->pdm))[ "deadMask" ]->side;
            glMatrixMode( GL_MODELVIEW );
            GLdouble scaleFactor = 2.0 / viewportSide;
            glTranslatef( -1.0, -1.0, 0.0 );
            glScalef( scaleFactor, scaleFactor, 1.0 );

            PixelData* input = (*(res->pdm))[ "compactedDeadMap" ];
            on("dm", "dm_shader" );
            string sampler = "compactedDeadMap";
            bindPixelData( &sampler, 1, &sampler );
            glClear( GL_COLOR_BUFFER_BIT );
            Utils::feed_coords( input->side, input->side, input->length );
            off();

            // computes the dead mask negation
            glMatrixMode( GL_MODELVIEW );
            glLoadIdentity();
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            PixelData* d = (*(res->pdm))[ "bestPosition" ];
            on( "dmneg", "dmneg_shader" );
            sampler = "deadMask";
            bindPixelData( &sampler, 1, &sampler );
            setShaderInt( "elements", d->length );
            setShaderInt( "texSide", d->side );
            Utils::fullscreenQuad();
            off();
        }

    private:
        void adjustOutputSize( void )
        {
            PixelData* tmpData = (*(res->pdm))[ "bestPosition" ];
            PixelData* outData = (*(res->pdm))[ "deadMask" ];
            PixelData* survivorsData = (*(res->pdm))[ "survivorsMask" ];

            if( outData->side < tmpData->side )
            {
                glDeleteTextures( 1, &(outData->textureId) );
                outData->textureId = Utils::createTexture( tmpData->side, false );
                outData->side = tmpData->side;
                outData->length = tmpData->side * tmpData->side;

                glDeleteTextures( 1, &(survivorsData->textureId) );
                survivorsData->textureId = Utils::createTexture( tmpData->side, false );
                survivorsData->side = tmpData->side;
                survivorsData->length = tmpData->length;
            }
        }
    };

    /* compacts the survivor best samples and the best samples of current view */
    class FinalCompactor: public Stage
    {
    public:
        FinalCompactor( SamplerResources* res ): Stage( res )
        {
            vector< string > aliveName, currentBestName;
            vector< PixelData* > aliveData, currentBestData;
            res->pdm->getFromPrefix( "alive", aliveName, aliveData );
            res->pdm->getFromPrefix( "currentBest", currentBestName, currentBestData );
            samplersCount = aliveName.size() + currentBestName.size();
            samplers = new string[ samplersCount ];

            int j = 0;
            for( unsigned int i=0; i<aliveName.size(); i++ )
            {
                samplers[j++] = aliveName[i];
            }

            for( unsigned int i=0; i<currentBestName.size(); i++ )
            {
                samplers[j++] = currentBestName[i];
            }
        }

        ~FinalCompactor( void )
        {
            delete[] samplers;
        }

        virtual void go( void )
        {
            PixelData* aliveData = (*(res->pdm))[ "alivePosition" ];
            PixelData* currentBestData = (*(res->pdm))[ "currentBestPosition" ];
            int aliveCount = aliveData->length;
            int currentBestCount = currentBestData->length;
            int total = aliveCount + currentBestCount;
            adjustOutputData( total );
            PixelData* outData = (*(res->pdm))[ "bestPosition" ];

            on( "best", "final_compactor_shader" );
            bindPixelData( samplers, samplersCount, samplers );
            setShaderInt( "totalElements", total );
            setShaderInt( "survivors", aliveCount );
            setShaderInt( "currentBests", currentBestCount );
            setShaderInt( "targetSide", outData->side );
            setShaderInt( "survivorsSide", aliveData->side );
            setShaderInt( "currentBestsSide", currentBestData->side );
            Utils::fullscreenQuad();
            off();
        }

    private:
        string* samplers;
        int samplersCount;

        void adjustOutputData( int totalElements )
        {
            FboConfiguration* bestConfig = (*(res->configs))[ "best" ];

            int outSide = getOutputSide( totalElements );
            if( (*bestConfig)[0].pData->side < outSide )
            {
                PixelData* tmpData = 0;
                for( unsigned int i=0; i < bestConfig->size(); i++ )
                {
                    tmpData = (*bestConfig)[i].pData;
                    glDeleteTextures( 1, &(tmpData->textureId) );
                    tmpData->textureId = Utils::createTexture( outSide, false );
                    tmpData->side = outSide;
                }
            }

            for( unsigned int i=0; i < bestConfig->size(); i++ )
            {
                (*bestConfig)[i].pData->length = totalElements;
            }
        }

        // calculates the output texture side (must be 2^n) given a number of elements
        int getOutputSide( int elements )
        {
            int outputSide = 1;
            bool ok = false;
            while( !ok )
            {
                ok = ( outputSide * outputSide >= elements );
                outputSide *= 2;
            }
            outputSide /= 2;
            return outputSide;
        }
    };

    class FeatureDetector: public Stage
    {
    public:
        FeatureDetector( SamplerResources* res ): Stage( res )
        {
            ;
        }

        ~FeatureDetector( void ){}

        virtual void go( void )
        {
            // copies the input depth
            string samplerName = "inputMap", buf = "inputDepth";
            on( "mask_pyramid", "copier_shader" );
            bindPixelData( &buf, 1, &samplerName );
            Utils::fullscreenQuad();
            off();

            // generates depth range pyramid
            int pyramidLevels = getPyramidLevels();
            string samplers[] = { "startMask", "mask_pyramid" };
            string realSamplers[] = { "startMask", "pyramid" };
            FboConfiguration* c = (*(res->configs))[ "mask_pyramid" ];
            for( int i=1; i<pyramidLevels; i++ )
            {
                (*c)[0].lod = i;
                on( "mask_pyramid", "depth_range_builder" );
                bindPixelData( samplers, 2, realSamplers );
                setShaderInt( "level", i );
                glClear( GL_COLOR_BUFFER_BIT );
                Utils::fullscreenQuad();
                off();
            }
            (*c)[0].lod = 0;
            GLfloat minDepth, maxDepth;
            getDepthRange( (*c)[0].pData, pyramidLevels - 1, minDepth, maxDepth );

            // feature detection
            string samplers2[] = { "startMask", "inputDepth", "inputNormal", "inputEyeNormal" };
            GLfloat depthRange = maxDepth - minDepth;
            float smallJump = depthRange * res->params->smallDepthJump;
            float bigJump = depthRange * res->params->bigDepthJump;
            float angleInRadians = ( res->params->angleThreshold * PI ) / 180.0;
            float thresholdCosine = cos( angleInRadians );
            angleInRadians = ( (res->params->frontFacingCone/2.0) * PI ) / 180.0;
            float frontFacingCosine = cos( angleInRadians );

            on( "detection", "detector_shader" );
            bindPixelData( samplers2, 4, samplers2 );
            setShaderFloat( "smallDepthJump", smallJump );
            setShaderFloat( "bigDepthJump", bigJump );
            setShaderFloat( "thresholdCosine", thresholdCosine );
            setShaderFloat( "frontFacingCosine", frontFacingCosine );
            glClear( GL_COLOR_BUFFER_BIT );
            Utils::fullscreenQuad();
            off();
        }

    private:

        int getPyramidLevels( void )
        {
            int levels = 1;
            int tmpSide = (*(res->pdm))[ "startMask" ]->side;
            while( tmpSide > 1 )
            {
                tmpSide /= 2;
                levels++;
            }
            return levels;
        }

        // downloads and returns the pyramid top value
        void getDepthRange( PixelData* pyramid, int maxLevel, GLfloat& min, GLfloat& max )
        {
            GLfloat top[3];
            pyramid->bind(0);
            glGetTexImage( GL_TEXTURE_2D, maxLevel, GL_RGB, GL_FLOAT, top );
            pyramid->unbind();  
            min = top[0];
            max = top[1];
        }

    };
}

#endif // STAGES_H
