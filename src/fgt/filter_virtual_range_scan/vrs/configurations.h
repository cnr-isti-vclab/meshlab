#ifndef CONFIGURATIONS_H
#define CONFIGURATIONS_H

#include "utils.h"

namespace vrs
{
    // stores all the render configurations used in virtual range scan
    class Configurations
    {
    public:
        Configurations( PixelDataManager* pdm, int side, int attributeMask )
        {
            this->pdm = pdm;

            /* attributes extraction configuration */
            int attributes = getColorAttachmentsCount( attributeMask );
            FboConfiguration* c = Utils::createFboConfiguration( attributes, side, true );
            configs[ "start" ] = c;
            string names[] = { "depthBlack", "inputPosition", "inputNormal",
                               "inputEyeNormal", "startMask" };
            for( int i=0; i<attributes; i++ )
            {
                pdm->put( names[i], c->at(i).pData );
            }
            FboAttachment& depthAttachment = (*c)[ c->size() - 1 ];

            /* depth fixer configuration */
            c = Utils::createFboConfiguration( 1, side, false );
            configs[ "depth_fixer" ] = c;
            pdm->put( "inputDepth", c->at(0).pData );

            /* mask pyramid */
            c = Utils::createFboConfiguration( 1, side, false );
            configs[ "mask_pyramid" ] = c;
            PixelData* pData = c->at(0).pData;
            pdm->put( "mask_pyramid", pData );
            pData->bind(0);
            glGenerateMipmap( GL_TEXTURE_2D );
            pData->unbind();

            /* best samples */
            c = Utils::createFboConfiguration( attributes - 1, 4, false );
            configs[ "best" ] = c;
            string bestNames[] = { "bestDepth", "bestEyeNormal", "bestNormal", "bestPosition" };
            for( int i=0; i<attributes-1; i++ )
            {
                pdm->put( bestNames[i], c->at(i).pData );
            }

            /* cone filtering (uniform sampling) */
            c = Utils::createFboConfiguration( 1, side, false );
            configs[ "detection" ] = c;
            pdm->put( "outMask", (*c)[0].pData );

            c = Utils::createFboConfiguration( 1, side, false );
            configs[ "best_eye_normal" ] = c;
            pdm->put( "eyeNormalBest", (*c)[0].pData );

            /* killer configuration */
            c = Utils::createFboConfiguration( 2, side, false );
            c->push_back( depthAttachment );
            configs[ "killer" ] = c;
            pdm->put( "killerMap", (*c)[0].pData );
            pdm->put( "deadMap", (*c)[1].pData );

            /* mask updater configuration */
            c = Utils::createFboConfiguration( 1, side, false );
            configs[ "mask_updater" ] = c;
            pdm->put( "updatedMask", (*c)[0].pData );

            /* current bests samples */
            string currentBestNames[] = { "currentBestDepth", "currentBestEyeNormal",
                                          "currentBestNormal", "currentBestPosition" };
            c = Utils::createFboConfiguration( attributes - 1, 4, false );
            configs[ "currentBest" ] = c;
            for( int i=0; i<attributes-1; i++ )
            {
                pdm->put( currentBestNames[i], c->at(i).pData );
            }

            /* compacted dead samples */
            c = Utils::createFboConfiguration( 1, 2, false );
            configs[ "compacted_dead" ] = c;
            pdm->put( "compactedDeadMap", (*c)[0].pData );

            /* dead mask */
            c = Utils::createFboConfiguration( 1, 2, false );
            configs[ "dm" ] = c;
            pdm->put( "deadMask", (*c)[0].pData );

            /* dead mask negation */
            c = Utils::createFboConfiguration( 1, 2, false );
            configs[ "dmneg" ] = c;
            pdm->put( "survivorsMask", (*c)[0].pData );

            /* survivors */
            string aliveNames[] = { "aliveDepth", "aliveEyeNormal",
                                          "aliveNormal", "alivePosition" };
            c = Utils::createFboConfiguration( attributes - 1, 2, false );
            configs[ "alive" ] = c;
            for( int i=0; i<attributes-1; i++ )
            {
                pdm->put( aliveNames[i], c->at(i).pData );
            }

            /* survivors pyramid */
            c = Utils::createFboConfiguration( 1, 2, false );
            configs[ "survivors_pyramid" ] = c;
            pdm->put( "survivorsPyramid", (*c)[0].pData );
        }

        ~Configurations( void )
        {
            for( map< string, FboConfiguration* >::iterator i = configs.begin();
                i != configs.end(); ++i )
            {
                delete (*i).second;
            }
            configs.clear();
        }

        void changeResolution( int side )
        {
            string arr1[] = { "start", "depth_fixer", "mask_pyramid",
                              "detection", "killer", "mask_updater", "best_eye_normal" };

            for( int i=0; i<7; i++ )
            {
                resetConfigSide( arr1[i], side );
            }

            string arr2[] = { "best", "currentBest", "compacted_dead",
                          "dm", "dmneg", "alive", "survivors_pyramid" };
            for( int i=0; i<7; i++ )
            {
                resetConfigSide( arr2[i], 2 );
            }

            PixelData* maskPyramid = (*pdm)[ "mask_pyramid" ];
            maskPyramid->bind( 0 );
            glGenerateMipmap( GL_TEXTURE_2D );
            maskPyramid->unbind();
        }

        FboConfiguration* operator[]( const string& key ){ return configs[key]; }

    private:
        map< string, FboConfiguration* > configs;
        PixelDataManager* pdm;
        
        int getColorAttachmentsCount( int mask )
        {
            return 5;
        }

        void resetConfigSide( string configName, int side )
        {
            FboConfiguration* c = configs[ configName ];

            PixelData* tmp = 0;
            FboAttachment* a = 0;
            vector< GLuint > toDel;
            for( unsigned int i=0; i<c->size(); i++ )
            {
                a = &((*c)[i]);
                tmp = a->pData;

                toDel.push_back( tmp->textureId );
                tmp->textureId = Utils::createTexture( side, a->attachmentPoint == GL_DEPTH_ATTACHMENT );
                tmp->side = side;
                tmp->length = side * side;
                a->lod = 0;
            }

            GLuint arr[ toDel.size() ];
            for( unsigned int i=0; i<toDel.size(); i++ )
            {
                arr[i] = toDel[i];
            }

            glDeleteTextures( toDel.size(), arr );
        }
    };
}

#endif // CONFIGURATIONS_H
