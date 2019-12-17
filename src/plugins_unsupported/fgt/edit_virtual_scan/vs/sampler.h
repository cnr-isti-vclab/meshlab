#ifndef SAMPLER_H
#define SAMPLER_H

#include "stages.h"

#include <vcg/complex/allocate.h>
#include <vcg/complex/algorithms/update/bounding.h>
#include <common/meshmodel.h>

#include <QTime>

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
                  MeshModel*        inputMeshModel,
                  MeshType*         uniformSamplesMesh,
                  MeshType*         featureSamplesMesh,
                  SamplerListener*  listener = 0 )
        {
            if( listener ){ listener->startingSetup(); }

            // OpenGL initialization
            GLenum err = glGetError();
            if( err != GL_NO_ERROR )
            {
                const GLubyte* errStr = gluErrorString( err );
                qDebug( "OpenGL error: %s", (const char*)errStr );
                assert( 0 );
            }

            glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
            glEnable( GL_DEPTH_TEST );
            glEnable( GL_TEXTURE_2D );

            // prepare input/output mesh
            vcg::tri::UpdateBounding< CMeshO >::Box( inputMeshModel->cm );
            uniformSamplesMesh->Clear();
            featureSamplesMesh->Clear();

            // initializes resources
            Resources resources( params );
            resources.initialize();
            resources.fbo->bind();

            // builds stages
            AttributesExtractor             extractor       ( inputMeshModel, &resources );
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

            GLint* samplesCount = &( resources.buffers[ "best_position" ]->elements );
            if( listener ){ listener->setupComplete( resources.params->povs ); }

            // *** sampling ***

            for( int i=0; i<2; i++ )
            {
                if( i==1 )
                {
                    if( listener ){ listener->startingFeatureSampling(); }
                    resources.changeResolution();
                    extractor.currentPov = 0;
                }

                // first pov
                extractor.go();
                if( i==0 ){ coneFilter.go(); }else{ detector.go(); }
                inputCompactor.go();
                if( listener ){ listener->povProcessed( 1, *samplesCount ); }

                // subsequent povs
                while( extractor.nextPov() )
                {
                    extractor.go();

                    if( i==0 )
                    {
                        coneFilter.go();
                    }
                    else
                    {
                        detector.go();
                    }

                    killer.go();
                    maskUpdater.go();
                    povCompactor.go();
                    deadsCompactor.go();
                    deadMasker.go();
                    aliveMasker.go();
                    aliveCompactor.go();
                    finalCompactor.go();

                    if( listener ){ listener->povProcessed( extractor.currentPov + 1, *samplesCount ); }
                }

                // download samples
                downloadSamples( &resources, (i==0)? uniformSamplesMesh : featureSamplesMesh );
            }

            // ****************

            // finalize resources
            resources.fbo->unbind();
            resources.finalize();

            err = glGetError();
            if( err != GL_NO_ERROR )
            {
                const GLubyte* errStr = gluErrorString( err );
                qDebug( "OpenGL error: %s", (const char*)errStr );
                assert( 0 );
            }
        }

    private:

        static void downloadSamples( Resources* resources, MeshType* target )
        {
            // download samples from the gpu
            PixelData* position = resources->buffers[ "best_position" ];
            PixelData* normal   = resources->buffers[ "best_normal" ];
            PixelData* color    = resources->buffers[ "best_color" ];
            GLfloat* pos = position->download();
            GLfloat* nrm = normal->download();
            GLfloat* col = color->download();

            GLfloat* posPix = pos;
            GLfloat* nrmPix = nrm;
            GLfloat* colPix = col;

            // append samples to output mesh
            int samples = position->elements;
            VertexIterator vi = vcg::tri::Allocator< MeshType >::AddVertices( *target, samples );

            while( vi != target->vert.end() )
            {
                (*vi).P() = CoordType( posPix[0], posPix[1], posPix[2] );
                posPix = &(posPix[3]);

                (*vi).N() = CoordType( nrmPix[0], nrmPix[1], nrmPix[2] );
                nrmPix = &(nrmPix[3]);

                for( int i=0; i<3; i++ )
                {
                    (*vi).C()[i] = (unsigned char)( colPix[i] * 255 );
                }
                (*vi).C()[3] = (unsigned char)255;
                colPix = &( colPix[3] );

                vi++;
            }

            // free downloaded samples
            free( pos );
            free( nrm );
            free( col );

            // re-compute bounding box
            vcg::tri::UpdateBounding< MeshType >::Box( *target );
        }
    };

}

#endif // SAMPLER_H
