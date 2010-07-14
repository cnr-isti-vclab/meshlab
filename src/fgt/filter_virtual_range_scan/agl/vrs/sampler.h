#ifndef SAMPLER_H
#define SAMPLER_H

#include "vrs_parameters.h"
#include "shared_resources.h"

#include "stages/start.h"
#include "stages/compactor.h"
#include "stages/killer.h"
#include "stages/mask_updater.h"
#include "stages/dead_masker.h"
#include "stages/final_compactor.h"
#include "stages/feature_detection.h"
#include "stages/discriminator.h"

#include <vcg/complex/trimesh/allocate.h>

#include <QTime>

namespace agl
{
    namespace vrs
    {
        class SamplerListener
        {
        public:
            virtual void setupComplete( VRSParameters* currentParams ) = 0;
            virtual void povProcessed( int povIndex, int samplesCount ) = 0;
            virtual void texturesDownloaded( void ) = 0;
            virtual void fillingTargetMesh( int samplesCount ) = 0;

        };

        template< class MeshType >
        class Sampler
        {
        public:

            // some typedefs to manipulate the mesh better
            typedef typename MeshType::VertexPointer    VertexPointer;
            typedef typename MeshType::ScalarType       ScalarType;
            typedef typename MeshType::CoordType        CoordType;
            typedef vcg::Point3< ScalarType >           MyPoint;

            Sampler( void )
            {
                currentPovs = 0;
            }

            void generateSamples
                    ( MeshType&         sourceMesh,
                      MeshType&         targetMesh,
                      VRSParameters&    params )
            {
                bool takingMeasures = false;
                int totalTime = 0;
                int elapsed = 0;
                int startStageTime = 0;
                int featureDetectionTime = 0;
                int discriminatorTime = 0;
                int currentViewCompactionTime = 0;
                int killerTime = 0;
                int maskUpdaterTime = 0;
                int deadMapTime = 0;
                int deadMaskTime = 0;
                int survivorsCompactionTime = 0;
                int finalCompactionTime = 0;
                QTime time;

                if( takingMeasures )
                {
                    time.start();
                }

                saveGLState();
                glClearColor( 0.0, 0.0, 0.0, 0.0 );

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    qDebug( "Saving and initializing GL state: %d ms", elapsed );
                }

                SharedResources* res = SharedResources::getInstance();

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    qDebug( "Getting SharedResources instance: %d ms", elapsed );
                }

                Start<MeshType> startStage( &params, &sourceMesh );
                currentPovs = &(startStage.povs);
                FeatureDetection featureDetection( params );
                Discriminator discriminator( params );

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    qDebug( "Building start, feature det. and discr: %d ms", elapsed );
                }

                for( unsigned int l=0; l<listeners.size(); l++ )
                {
                    listeners[l]->setupComplete( &params );
                }

                startStage.go();

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    startStageTime = elapsed;
                }

                vector< string > inputAttributes;
                vector< string > bestAttributes;
                vector< string > currentBestAttributes;
                vector< string > bestSurvivors;
                startStage.getAttributeNames( "input", inputAttributes );
                startStage.getAttributeNames( "best", bestAttributes );
                startStage.getAttributeNames( "currentBest", currentBestAttributes );
                startStage.getAttributeNames( "survivor", bestSurvivors );

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    qDebug( "Building various pixel data names: %d ms", elapsed );
                }

                if( params.generateUniformSamples )
                {
                    PixelData* maskData = res->pdm[ "inputMask" ];
                    PixelData* maskCopy = new PixelData( maskData->data );
                    res->pdm.saveData( "outMask", maskCopy );
                }
                else
                {
                    featureDetection.go();

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        featureDetectionTime = elapsed;
                    }

                    discriminator.go();

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        discriminatorTime = elapsed;
                    }
                }

                Compactor compactor;
                compactor.inputs = inputAttributes;
                compactor.outputs = bestAttributes;
                compactor.mask = "outMask";
                compactor.go();
                //dumpData( "bestPosition", 0 );

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    currentViewCompactionTime = elapsed;
                }

                PixelData* bestPos = res->pdm["bestPosition"];
                for( unsigned int l=0; l<listeners.size(); l++ )
                {
                    listeners[l]->povProcessed( 0, bestPos->length );
                }

                // loop
                Killer killer;
                MaskUpdater maskUpdater;
                DeadMasker deadMasker;
                FinalCompactor finalCompactor( startStage.attributeNames );

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    qDebug( "Building killing stages: %d ms", elapsed );
                }

                int i=1;
                while( startStage.nextPov() )
                {
                    startStage.go();
                    //dumpData( "inputPosition", i );

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        if( elapsed > startStageTime )
                        {
                            startStageTime = elapsed;
                        }
                    }

                    if( !params.generateUniformSamples )
                    {
                        featureDetection.go();

                        if( takingMeasures )
                        {
                            elapsed = time.restart();
                            totalTime += elapsed;
                            if( elapsed > featureDetectionTime )
                            {
                                featureDetectionTime = elapsed;
                            }
                        }


                        discriminator.go();

                        if( takingMeasures )
                        {
                            elapsed = time.restart();
                            totalTime += elapsed;
                            if( elapsed > discriminatorTime )
                            {
                                discriminatorTime = elapsed;
                            }
                        }
                    }

                    killer.go();
                    //dumpData( "killerMap", i );

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        if( elapsed > killerTime )
                        {
                            killerTime = elapsed;
                        }
                    }

                    maskUpdater.go();
                    //dumpData( "updatedMask", i );

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        if( elapsed > maskUpdaterTime )
                        {
                            maskUpdaterTime = elapsed;
                        }
                    }

                    compactor.inputs = inputAttributes;
                    compactor.outputs = currentBestAttributes;
                    compactor.mask = "updatedMask";
                    compactor.go();
                    //dumpData( "currentBestPosition", i );

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        if( elapsed > currentViewCompactionTime )
                        {
                            currentViewCompactionTime = elapsed;
                        }
                    }

                    compactor.inputs.clear();
                    compactor.inputs.push_back( "deadMap" );
                    compactor.outputs.clear();
                    compactor.outputs.push_back( "compactedDeadMap" );
                    compactor.mask = "killerMap";
                    compactor.go();
                    //dumpData( "compactedDeadMap", i );

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        if( elapsed > deadMapTime )
                        {
                            deadMapTime = elapsed;
                        }
                    }

                    deadMasker.go();
                    //dumpData( "deadMask", i );

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        if( elapsed > deadMaskTime )
                        {
                            deadMaskTime = elapsed;
                        }
                    }

                    compactor.inputs = bestAttributes;
                    compactor.outputs = bestSurvivors;
                    compactor.mask = "deadMask";
                    compactor.go();
                    //dumpData( "survivorPosition", i );

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        if( elapsed > survivorsCompactionTime )
                        {
                            survivorsCompactionTime = elapsed;
                        }
                    }

                    finalCompactor.go();
                    //dumpData( "bestPosition", i );

                    if( takingMeasures )
                    {
                        elapsed = time.restart();
                        totalTime += elapsed;
                        if( elapsed > finalCompactionTime )
                        {
                            finalCompactionTime = elapsed;
                        }
                    }

                    bestPos = res->pdm["bestPosition"];
                    for( unsigned int l=0; l<listeners.size(); l++ )
                    {
                        listeners[l]->povProcessed( i, bestPos->length );
                    }

                    i++;
                }

                //res->pdm.printState();

                fillTargetMesh( targetMesh, params.attributeMask );

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    qDebug( "Filling target mesh: %d ms", elapsed );
                }

                SharedResources::deleteInstance();

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    qDebug( "Deleting SharedResources instance: %d ms", elapsed );
                }

                restoreGLState();

                if( takingMeasures )
                {
                    elapsed = time.restart();
                    totalTime += elapsed;
                    qDebug( "Restoring GL state: %d ms", elapsed );

                    qDebug( "Stages measurements\n------------------------\n" );
                    qDebug( "Start stage: %d ms", startStageTime );

                    if( !params.generateUniformSamples )
                    {
                        qDebug( "Features detection: %d ms", featureDetectionTime );
                        qDebug( "Discriminator: %d ms", discriminatorTime );
                    }

                    qDebug( "Killer: %d ms", killerTime );
                    qDebug( "Mask updating: %d ms", maskUpdaterTime );
                    qDebug( "Current view compaction: %d ms", currentViewCompactionTime );
                    qDebug( "Dead map construction: %d ms", deadMapTime );
                    qDebug( "Dead mask construction: %d ms", deadMaskTime );
                    qDebug( "Survivors compaction: %d ms", survivorsCompactionTime );
                    qDebug( "Final compaction time: %d ms", finalCompactionTime );

                    qDebug( "**************************************\n" );

                    //res->pdm.printState();
                }
            }

            void addSamplerListener( SamplerListener* sl )
            {
                listeners.push_back( sl );
            }

            vector< MyPoint >* currentPovs;

        private:

            // the OpenGL state that is saved and then restored
            GLfloat cc[4];                  // clear color
            GLfloat modelviewMatrix[16];
            GLfloat projectionMatrix[16];

            // listeners vector
            vector< SamplerListener* > listeners;

            void saveGLState( void )
            {
                glGetFloatv( GL_COLOR_CLEAR_VALUE, cc );
                glGetFloatv( GL_MODELVIEW_MATRIX, modelviewMatrix );
                glGetFloatv( GL_PROJECTION_MATRIX, projectionMatrix );
            }

            void restoreGLState( void )
            {
                glClearColor( cc[0], cc[1], cc[2], cc[3] );

                glMatrixMode( GL_PROJECTION );
                glLoadMatrixf( projectionMatrix );

                glMatrixMode( GL_MODELVIEW );
                glLoadMatrixf( modelviewMatrix );
            }

            void dumpData( string dataName, int postfixNumber = -1 )
            {
                SharedResources* res = SharedResources::getInstance();
                PixelData* pData = res->pdm[ dataName ];
                char buf[100];
                if( postfixNumber != -1 )
                {
                    sprintf( buf, "%s%d.txt", dataName.c_str(), postfixNumber );
                }
                else
                {
                    sprintf( buf, "%s.txt", dataName.c_str() );
                }
                pData->data->download_and_dump_to_file( buf );
            }

            void fillTargetMesh( MeshType& m, int attributeMask  )
            {
                SharedResources* res = SharedResources::getInstance();

                client_texture2d<float>* position = 0, *normal = 0;

                PixelData* prototype = res->pdm["bestPosition"];
                position = texture2d::download< float >( prototype->data );

                if( attributeMask & VRSParameters::NORMAL )
                {
                    normal = texture2d::download< float >( res->pdm["bestNormal"]->data );
                }

                for( unsigned int l=0; l<listeners.size(); l++ )
                {
                    listeners[l]->texturesDownloaded();
                }


                int numberOfSamples = prototype->length;
                typename MeshType::VertexIterator vi;
                vi = vcg::tri::Allocator<MeshType>::AddVertices( m, numberOfSamples );
                typename MeshType::VertexPointer vp;
                texture_pixel< float >* pixel;

                for( int i=0; i<numberOfSamples; i++ )
                {
                    vp = &(*vi);

                    pixel = position->pixels[i];
                    vp->P() = CoordType( pixel->x(), pixel->y(), pixel->z() );

                    if( normal )
                    {
                        pixel = normal->pixels[i];
                        vp->N() = CoordType( pixel->x(), pixel->y(), pixel->z() );
                    }

                    if( i % 100 == 0 )
                    {
                        for( unsigned int l=0; l<listeners.size(); l++ )
                        {
                            listeners[l]->fillingTargetMesh( i );
                        }
                    }

                    vi++;
                }

                for( unsigned int l=0; l<listeners.size(); l++ )
                {
                    listeners[l]->fillingTargetMesh( numberOfSamples );
                }

                delete position;
                if( normal ) delete normal;
            }

        };
    }
}

#endif // SAMPLER_H
