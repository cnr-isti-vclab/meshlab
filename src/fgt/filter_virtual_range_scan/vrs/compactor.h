#ifndef COMPACTOR_H
#define COMPACTOR_H

#include "stage.h"

namespace vrs
{
    class Compactor: public Stage
    {
    public:
        Compactor( SamplerResources* res ):Stage(res)
        {
            copierSampler = "inputMap";
            pyramidSampler = "pyramid";
        }

        ~Compactor  ( void ){}

        virtual void go()
        {
            // copies the pyramid base
            adjustPyramidSize();
            FboConfiguration* c = (*(res->configs))[ pyramidConfig ];
            on( pyramidConfig, "copier_shader" );
            bindPixelData( &pyramidSource, 1, &copierSampler );
            Utils::fullscreenQuad();
            off();

            pyramidLevels = getPyramidLevels();

            // generates the pyramid
            for( int i=1; i<pyramidLevels; i++ )
            {
                (*c)[0].lod = i;
                on( pyramidConfig, "level_builder_shader" );
                bindPixelData( &pyramidName, 1, &pyramidSampler );
                setShaderInt( "level", i );
                Utils::fullscreenQuad();
                off();
            }
            (*c)[0].lod = 0;

            // compaction
            PixelData* pyramid = (*c)[0].pData;
            int elements = getPyramidTop( pyramid, pyramidLevels - 1 );
            int outputSide = getOutputSide( elements );

            adjustOutputData( outputPrefix, outputSide, elements );
            on( outputConfig, outputShader );
            bindPixelData( inputPrefix );
            int pyramidUnit = toCompactNames.size();
            pyramid->bind( pyramidUnit );
            GLint loc = glGetUniformLocation( p->programId, "pyramid" );
            glUniform1i( loc, pyramidUnit );

            for( unsigned int i=0; i<toCompactNames.size(); i++ )
            {
                toCompactData[i]->bind( i );
                loc = glGetUniformLocation( p->programId, toCompactNames[i].c_str() );
                glUniform1i( loc, i );
            }

            setShaderInt( "samples", elements );
            setShaderInt( "max_lod", pyramidLevels - 1 );
            setShaderInt( "tex_side", outputSide );

            glClear( GL_COLOR_BUFFER_BIT );
            Utils::fullscreenQuad();

            for( unsigned int i=0; i<toCompactNames.size(); i++ )
            {
                toCompactData[i]->unbind();
            }
            pyramid->unbind();
            off();
        }

        void setParameters( string pyramidConfig, string pyramidSource,
                            string pyramidName, string toCompactPrefix,
                            string outputPrefix, string outputConfig,
                            string outputShader )
        {
            this->pyramidConfig = pyramidConfig;
            this->pyramidSource = pyramidSource;
            this->pyramidName = pyramidName;
            this->inputPrefix = toCompactPrefix;
            res->pdm->getFromPrefix( toCompactPrefix, toCompactNames, toCompactData );
            this->outputPrefix = outputPrefix;
            this->outputConfig = outputConfig;
            this->outputShader = outputShader;
        }

        string  inputPrefix;
        string  pyramidConfig;
        string  outputConfig;
        string  pyramidSource;
        string  pyramidName;
        vector< string > toCompactNames;
        vector< PixelData* > toCompactData;
        string  outputPrefix;
        string  outputShader;
        string  copierSampler;
        string  pyramidSampler;
        int     pyramidLevels;

    private:

        // downloads and returns the pyramid top value
        int getPyramidTop( PixelData* pyramid, int topLevel )
        {
            int pixelSize = 3 * 3 * 4;          // GL_RGB32F
            void* top = malloc( pixelSize );    // 1 pixels for the top level
            pyramid->bind(0);
            glGetTexImage( GL_TEXTURE_2D, pyramidLevels - 1, GL_RGB, GL_FLOAT, top );
            pyramid->unbind();
            int elements = (int)(*((GLfloat*)top));
            free(top);
            return elements;
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

        void adjustOutputData( string outPrefix, int outputSide, int elements )
        {
            // checks if the current output textures size is sufficient
            vector< string > outputNames;
            vector< PixelData* > outputData;
            res->pdm->getFromPrefix( outPrefix, outputNames, outputData );
            if( outputData[0]->side < outputSide )
            {   // enlarges output textures
                PixelData* pData = 0;
                for( unsigned int i=0; i<outputData.size(); i++ )
                {
                    pData = outputData[i];
                    glDeleteTextures( 1, &(pData->textureId) );
                    pData->textureId = Utils::createTexture( outputSide, false );
                    pData->side = outputSide;
                }
            }

            // updates elements count in PixelData objects
            for( unsigned int i=0; i<outputData.size(); i++ )
            {
                outputData[i]->length = elements;
            }
        }

        void adjustPyramidSize( void )
        {
            PixelData* src = (*(res->pdm))[ pyramidSource ];
            PixelData* pyramid = (*(res->pdm))[ pyramidName ];

            if( src->side != pyramid->side )
            {
                glDeleteTextures( 1, &(pyramid->textureId) );
                pyramid->textureId = Utils::createTexture( src->side, false );
                pyramid->side = src->side;
                pyramid->length = src->side * src->side;

                pyramid->bind(0);
                glGenerateMipmap( GL_TEXTURE_2D );
                pyramid->unbind();
            }
        }

        int getPyramidLevels( void )
        {
            int levels = 1;
            int tmpSide = (*(res->pdm))[ pyramidName ]->side;
            while( tmpSide > 1 )
            {
                tmpSide /= 2;
                levels++;
            }
            return levels;
        }
    };
}

#endif // COMPACTOR_H
