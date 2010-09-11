#ifndef SAMPLER_H
#define SAMPLER_H

#include "stages.h"

#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/update/bounding.h>

namespace vs
{
    class SamplerListener
    {

    public:
        virtual void startingSetup              ( void )                    = 0;
        virtual void setupComplete              ( int povs )                = 0;
        virtual void povProcessed               ( int pov, int samples )    = 0;
        virtual void startingFeatureSampling    ( void )                    = 0;

    };

    template< class MeshType >
    class Sampler
    {

    public:

        typedef typename MeshType::VertexIterator   VertexIterator;
        typedef typename MeshType::CoordType        CoordType;

        static void generateSamples
                ( VSParameters*     params,
                  MeshType*         inputMesh,
                  MeshType*         uniformSamplesMesh,
                  MeshType*         featureSamplesMesh,
                  SamplerListener*  listener = 0 )
        {
            listener->startingSetup();

            // OpenGL initialization
            assert( glGetError() == GL_NO_ERROR );
            glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
            glEnable( GL_DEPTH_TEST );

            // prepare input/output mesh
            vcg::tri::UpdateBounding< MeshType >::Box( *inputMesh );
            uniformSamplesMesh->Clear();
            featureSamplesMesh->Clear();

            // initializes resources
            Resources resources( params );
            resources.initialize();
            resources.fbo->bind();

            // builds stages
            AttributesExtractor< MeshType > extractor       ( inputMesh, &resources );
            ConeFilter                      coneFilter      ( &resources );
            Compactor                       inputCompactor  ( &resources, "out_mask", "input_", "best_" );
            Killer                          killer          ( &resources );
            MaskUpdater                     maskUpdater     ( &resources );
            Compactor                       povCompactor    ( &resources, "pov_alive_mask", "input_", "current_best_" );
            Compactor                       deadsCompactor  ( &resources, "killer_map", "dead_map", "compacted_deads", true );
            DeadMasker                      deadMasker      ( &resources );
            AliveMasker                     aliveMasker     ( &resources );
            Compactor                       aliveCompactor  ( &resources, "alive_mask", "best_", "alive_" );
            FinalCompactor                  finalCompactor  ( &resources );
            FeatureDetector                 detector        ( &resources );

            listener->setupComplete( resources.params->povs );
            GLint* samplesCount = &( resources.buffers[ "best_position" ]->elements );

            // *** sampling ***

            for( int i=0; i<2; i++ )
            {
                if( i==1 )
                {
                    listener->startingFeatureSampling();
                    resources.changeResolution();
                    extractor.currentPov = 0;
                }

                // first pov
                extractor.go();
                if( i==0 ){ coneFilter.go(); }else{ detector.go(); }
                inputCompactor.go();
                listener->povProcessed( 1, *samplesCount );

                // subsequent povs
                while( extractor.nextPov() )
                {
                    extractor.go();
                    if( i==0 ){ coneFilter.go(); }else{ detector.go(); }
                    killer.go();
                    maskUpdater.go();
                    povCompactor.go();
                    deadsCompactor.go();
                    deadMasker.go();
                    aliveMasker.go();
                    aliveCompactor.go();
                    finalCompactor.go();
                    listener->povProcessed( extractor.currentPov + 1, *samplesCount );
                }

                // download samples
                downloadSamples( &resources, (i==0)? uniformSamplesMesh : featureSamplesMesh );
            }

            // ****************

            // finalize resources
            resources.fbo->unbind();
            resources.finalize();
            assert( glGetError() == GL_NO_ERROR );
        }

    private:

        static void downloadSamples( Resources* resources, MeshType* target )
        {
            // download samples from the gpu
            PixelData* position = resources->buffers[ "best_position" ];
            PixelData* normal   = resources->buffers[ "best_normal" ];
            GLfloat* pos = position->download();
            GLfloat* nrm = normal->download();
            GLfloat* posPix = pos;
            GLfloat* nrmPix = nrm;

            // append samples to output mesh
            int samples = position->elements;
            VertexIterator vi = vcg::tri::Allocator< MeshType >::AddVertices( *target, samples );

            while( vi != target->vert.end() )
            {
                (*vi).P() = CoordType( posPix[0], posPix[1], posPix[2] );
                posPix = &(posPix[3]);

                (*vi).N() = CoordType( nrmPix[0], nrmPix[1], nrmPix[2] );
                nrmPix = &(nrmPix[3]);

                vi++;
            }

            // free downloaded samples
            free( pos );
            free( nrm );

            // re-compute bounding box
            vcg::tri::UpdateBounding< MeshType >::Box( *target );
        }
    };

}

#endif // SAMPLER_H
