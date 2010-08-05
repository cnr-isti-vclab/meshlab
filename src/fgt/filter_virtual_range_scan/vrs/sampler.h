#ifndef SAMPLER_H
#define SAMPLER_H

#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <wrap/callback.h>

#include "resources.h"
#include "start.h"
#include "compactor.h"
#include "stages.h"

namespace vrs
{
    class SamplerListener
    {
    public:
        virtual void startingSetup( void ) = 0;
        virtual void setupComplete( int povs ) = 0;
        virtual void povProcessed( int pov, int samples ) = 0;
        virtual void startingFeatureSampling( void ) = 0;
    };


    /* the virtual range scan sampler */
    template< class MeshType >
    class Sampler
    {

    public:
        typedef typename MeshType::VertexIterator   VertexIterator;
        typedef typename MeshType::CoordType        CoordType;

        Sampler( VRSParameters* params, MeshType* srcMesh,
                 MeshType* uniformSamplesMesh, MeshType* featureSamplesMesh )
            :res( params )
        {
            this->srcMesh = srcMesh;
            this->uniformSamplesMesh = uniformSamplesMesh;
            this->featureSamplesMesh = featureSamplesMesh;
            this->params = params;
        }

        bool initResources( void )
        {
            return true;
        }

        void generateSamples( void )
        {
            for( unsigned int k = 0; k < listeners.size(); k++ )
            {
                listeners[k]->startingSetup();
            }

            res.init();
            glPushAttrib( GL_ALL_ATTRIB_BITS );
            glClearColor( 0.0, 0.0, 0.0, 0.0 );
            res.fbo->bind();

            // builds stages
            Start< MeshType > start( srcMesh, params->povs, &res );
            lastViews = start.views;
            ConeFilter coneFilter( params->frontFacingCone, &res );
            Compactor inputCompactor( &res );
            inputCompactor.setParameters( "mask_pyramid", "outMask", "mask_pyramid",
                                          "input", "best", "best", "input_compactor" );
            Compactor deadCompactor( &res );
            deadCompactor.setParameters( "mask_pyramid", "killerMap", "mask_pyramid",
                                         "deadMap", "compactedDeadMap", "compacted_dead", "dead_compactor" );
            Compactor aliveCompactor( &res );
            aliveCompactor.setParameters( "survivors_pyramid", "survivorsMask", "survivorsPyramid",
                                          "best", "alive", "alive", "alive_compactor" );

            Killer killer( &res );
            MaskUpdater maskUpdater( &res );
            DeadMasker deadMasker( &res );
            FinalCompactor finalCompactor( &res );
            FeatureDetector detector( &res );
            PixelData* b = (*(res.pdm))[ "bestPosition" ];

            for( unsigned int k = 0; k < listeners.size(); k++ )
            {
                listeners[k]->setupComplete( params->povs );
            }


            for( int i=0; i<2; i++ )
            {
                if( i == 1 )
                {
                    for( unsigned int k = 0; k < listeners.size(); k++ )
                    {
                        listeners[k]->startingFeatureSampling();
                    }

                    inputCompactor.setParameters( "mask_pyramid", "outMask", "mask_pyramid",
                                                  "input", "best", "best", "input_compactor" );
                    res.configs->changeResolution( params->featureResolution );
                    start.currentPov = 0;
                }

                // first view
                start.go();
                if( i == 0 ){ coneFilter.go(); }else{ detector.go(); }
                inputCompactor.go();

                for( unsigned int k = 0; k < listeners.size(); k++ )
                {
                    listeners[k]->povProcessed( 1, b->length );
                }

                inputCompactor.setParameters( "mask_pyramid", "updatedMask", "mask_pyramid",
                                          "input", "current", "currentBest", "input_compactor" );

                // subsequent views
                while( start.nextPov() )
                {
                    start.go();
                    if( i == 0 ){ coneFilter.go(); }else{ detector.go(); }
                    killer.go();
                    maskUpdater.go();
                    inputCompactor.go();
                    deadCompactor.go();
                    deadMasker.go();
                    aliveCompactor.go();
                    finalCompactor.go();

                    for( unsigned int k = 0; k < listeners.size(); k++ )
                    {
                        listeners[k]->povProcessed( start.currentPov + 1, b->length );
                    }
                }

                downloadSamplesToMesh( i==0? uniformSamplesMesh : featureSamplesMesh );
            }

            vcg::tri::UpdateBounding< MeshType >::Box( *uniformSamplesMesh );
            if( featureSamplesMesh != uniformSamplesMesh )
            {
                vcg::tri::UpdateBounding< MeshType >::Box( *featureSamplesMesh );
            }

            res.fbo->unbind();
            glPopAttrib();
            res.free();

            // there is a stack overflow error, probably regarding the projection matrix
            // stack. I can't fix this problem but to reset the error flag.
            glGetError();
        }

        void freeResources( void )
        {
            ;
        }

        vector< SamplerListener* > listeners;
        vector< Point3f > lastViews;

    private:
        VRSParameters* params;
        SamplerResources res;
        MeshType* srcMesh;
        MeshType* uniformSamplesMesh;
        MeshType* featureSamplesMesh;

        void downloadSamplesToMesh( MeshType* target )
        {
            // downloads samples textures
            bool normalOn = params->attributeMask & VRSParameters::NORMAL;
            PixelData* normalData = (normalOn? (*(res.pdm))[ "bestNormal" ] : 0 );
            PixelData* posData = (*(res.pdm))[ "bestPosition" ];


            int pixelSize = 3 * 3 * 4;  // GL_RGB32F
            int dataSide = posData->side;
            int textureSize = pixelSize * dataSide * dataSide;
            GLfloat* pos = (GLfloat*)malloc( textureSize );
            GLfloat* normal = ( normalOn? (GLfloat*)malloc( textureSize ) : 0 );

            posData->bind(0);
            glGetTexImage( GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, pos );
            posData->unbind();

            if( normalOn )
            {
                normalData->bind(0);
                glGetTexImage( GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, normal );
                normalData->unbind();
            }

            // fills target mesh
            int samplesCount = posData->length;
            VertexIterator vi = vcg::tri::Allocator< MeshType >::AddVertices( *target, samplesCount );
            GLfloat* pix = pos;
            GLfloat* nrm = normal;
            while( vi != target->vert.end() )
            {
                (*vi).P() = CoordType( pix[0], pix[1], pix[2] );
                pix = &(pix[3]);

                if( normalOn )
                {
                    (*vi).N() = CoordType( nrm[0], nrm[1], nrm[2] );
                    nrm = &(nrm[3]);
                }

                vi++;
            }

            free( pos );
            if( normalOn ) free( normal );
        }
    };
}

#endif // SAMPLER_H
