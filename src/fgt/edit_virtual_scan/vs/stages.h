#ifndef STAGES_H
#define STAGES_H

#include "resources.h"
#include "povs_generator.h"
#include "simple_renderer.h"

#include <common/meshmodel.h>

#include <vcg/simplex/vertex/component_ocf.h>

namespace vs
{
    /* stages base class */
    class Stage
    {

    public:
        Stage( Resources* resources )
        {
            this->resources = resources;
        }

        virtual void go( void ) = 0;

    protected:
        Resources*  resources;
        std::vector< PixelData* >   tmpData;
        std::vector< PixelData* >   samplers;
        Program*    prog;

        // data facilities
        void collectAttributesData( QString prefix )
        {
            QString tmpAttribute;
            PixelData* pData = 0;

            for( unsigned int i=0; i<resources->attributes.size(); i++ )
            {
                tmpAttribute = resources->attributes[ i ];
                pData = resources->buffers[ prefix + tmpAttribute ];
                assert( pData );
                tmpData.push_back( pData );
            }
        }

        void collectData( QString dataName )
        {
            PixelData* pData = resources->buffers[ dataName ];
            assert( pData );
            tmpData.push_back( pData );
        }

        void getAttributesDataName( QString prefix, std::vector< QString >& target )
        {
            target.clear();
            for( unsigned int i=0; i<resources->attributes.size(); i++ )
            {
                target.push_back( prefix + resources->attributes[i] );
            }
        }

        // drawing facilities
        void drawFullscreenQuad( void )
        {
            glBegin (GL_QUADS);
            glVertex3i( -1, -1, -1);
            glVertex3i(  1, -1, -1);
            glVertex3i(  1,  1, -1);
            glVertex3i( -1,  1, -1);
            glEnd ();
        }

        void feedCoords( GLint side, GLint count )
        {
            GLint x = 0, y = 0;
            glBegin( GL_POINTS );
            for( GLint i=0; i<count; i++ )
            {
                glVertex2i( x, y );
                x++;
                if( x == side )
                {
                    x = 0;
                    y++;
                }
            }
            glEnd();
        }

        // shader facilities
        void bindSampler( QString samplerName, PixelData* pData )
        {
            const string str = samplerName.toStdString();
            const GLchar* c = (const GLchar*)( str.c_str() );
            GLint loc = glGetUniformLocation( prog->programId, c  );
            GLint textureUnit = (GLint)samplers.size();
            pData->bind( textureUnit );
            glUniform1i( loc, textureUnit );
            samplers.push_back( pData );
        }

        void bindSampler( QString dataName, QString samplerName )
        {
            PixelData* pData = resources->buffers[ dataName ];
            assert( pData && prog );
            bindSampler( samplerName, pData );
        }

        void bindAttributeSamplers( QString prefix )
        {
            QString samplerName;

            for( unsigned int i=0; i<resources->attributes.size(); i++ )
            {
                samplerName = prefix + resources->attributes[i];
                bindSampler( samplerName, samplerName );
            }
        }

        void unbindSamplers( void )
        {
            for( unsigned int i=0; i<samplers.size(); i++ )
            {
                samplers[i]->unbind();
            }
            samplers.clear();
        }

        void uploadFloat( QString uniformName, GLfloat value )
        {
            const string str = uniformName.toStdString();
            const GLchar* c = (const GLchar*)( str.c_str() );
            GLint loc = glGetUniformLocation( prog->programId, c );
            glUniform1f( loc, value );
        }

        void uploadInt( QString uniformName, GLint value )
        {
            const string str = uniformName.toStdString();
            const GLchar* c = (const GLchar*)( str.c_str() );
            GLint loc = glGetUniformLocation( prog->programId, c );
            glUniform1i( loc, value );
        }

        // dumping utils
        void dumpAttributes( QString prefix, QString suffix = "" )
        {
            std::vector< QString > names;
            getAttributesDataName( prefix, names );
            for( unsigned int i=0; i<names.size(); i++ )
            {
                PixelData* pData = resources->buffers[ names[i] ];
                assert( pData );
                pData->dumpToFile( names[i] + suffix + ".txt", 0 );
            }
        }

        void dump( QString dataName, QString prefix = "", int lod = 0 )
        {
            PixelData* pData = resources->buffers[ dataName ];
            assert( pData );
            pData->dumpToFile( prefix + dataName + ".txt", lod );
        }
    };

    /* attribute extraction stage */
    class AttributesExtractor: public Stage
    {

    public:
        typedef CMeshO::ScalarType         ScalarType;
        typedef vcg::Point3< ScalarType >  MyPoint;
        typedef vcg::Shot< ScalarType >    ShotType;

        AttributesExtractor( MeshModel* inputMeshModel, Resources* res )
            :Stage( res ), renderer( &(inputMeshModel->cm) )
        {
            this->inputMeshModel = inputMeshModel;
            this->inputMesh = &(inputMeshModel->cm);
            currentPov = 0;
            meshCenter = inputMesh->bbox.Center();

            if( resources->params->useCustomPovs )
            {
                assert( resources->params->customPovs.size() > 0 );
            }
            else
            {
                // generate povs
                MyPoint coneAxis( res->params->coneAxis[0],
                                  res->params->coneAxis[1],
                                  res->params->coneAxis[2] );
                // NB: the cone axis must be inverted due to the ortographic projection,
                // that sets a negative near plane
                coneAxis = -coneAxis;
                coneAxis.Normalize();
                ScalarType coneGap = ( res->params->coneGap / 180.0f ) * PI;
                orthoRadius = ( inputMesh->bbox.Diag() / 2 ) * 1.2;
                PovsGenerator< ScalarType >::generatePovs( res->params->povs, orthoRadius, meshCenter, coneAxis, coneGap, povs );
                generateUpVectors( povs, meshCenter, upVectors );
            }
        }

        virtual void go( void )
        {
            if( resources->params->useCustomPovs )
            {
                Pov& newPov = resources->params->customPovs[ currentPov ];
                ScalarType nearPlane, farPlane;
                GlShot< ShotType >::GetNearFarPlanes( newPov.first, inputMesh->bbox, nearPlane, farPlane );
                GlShot< ShotType >::SetView( newPov.first, nearPlane, farPlane );

                glEnable( GL_SCISSOR_TEST );
                Box2i& scissorBox = newPov.second;
                Point2i min = scissorBox.min;
                int width = scissorBox.DimX();
                int height = scissorBox.DimY();
                glScissor( (GLint)min.X(), (GLint)min.Y(), (GLsizei)width, (GLsizei)height );
            }
            else
            {
                glMatrixMode( GL_PROJECTION );
                glLoadIdentity();
                glOrtho( -orthoRadius, orthoRadius, -orthoRadius, orthoRadius, -orthoRadius, orthoRadius );

                MyPoint& p  = povs[ currentPov ];
                MyPoint& up = upVectors[ currentPov ];
                glMatrixMode( GL_MODELVIEW );
                glLoadIdentity();
                gluLookAt( meshCenter.X(),  meshCenter.Y(), meshCenter.Z(),
                           p.X(),           p.Y(),          p.Z(),
                           up.X(),          up.Y(),         up.Z() );
            }

            // filling all attribute-maps except color
            tmpData.clear();
            collectAttributesData( "input_" );
            collectData( "start_mask" );
            resources->fbo->load( tmpData );
            PixelData* depthBuffer = resources->buffers[ "depth_buffer" ];
            resources->fbo->loadDepth( depthBuffer->textureId );

            Program* startShader = resources->shaders[ "start" ];
            startShader->load();

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            renderer.render( );
            //resources->fbo->screenshots( "start" );

            startShader->unload();
            resources->fbo->unload();

            // generate color-map with the vcg renderer
            tmpData.clear();
            collectData( "input_color" );
            resources->fbo->load( tmpData );
            resources->fbo->loadDepth( depthBuffer->textureId );

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            inputMeshModel->Render
                    ( resources->params->cmDrawMode,
                      resources->params->cmColorMode,
                      resources->params->cmTextureMode );
            /*
            static int k = 0;
            resources->fbo->screenshots( QString::number(k++).toStdString() );
            */

            resources->fbo->unload();

            if( resources->params->useCustomPovs )
            {
                glDisable( GL_SCISSOR_TEST );
            }

            // save and then reset modelview and projection matrix
            glGetFloatv( GL_MODELVIEW_MATRIX, resources->mvMatrix );
            glGetFloatv( GL_PROJECTION_MATRIX, resources->projMatrix );

            if( resources->params->useCustomPovs )
                 GlShot< ShotType >::UnsetView();

            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            glMatrixMode( GL_MODELVIEW );
            glLoadIdentity();
        }

        bool nextPov( void )
        {
            currentPov++;
            int si = (resources->params->useCustomPovs)?resources->params->customPovs.size():(int)povs.size();
            return ( currentPov < si );
        }

        int                         currentPov;

    private:
        MeshModel*                  inputMeshModel;
        CMeshO*                     inputMesh;
        SimpleRenderer< CMeshO >    renderer;
        std::vector< MyPoint >      povs;
        std::vector< MyPoint >      upVectors;
        MyPoint                     meshCenter;
        ScalarType                  orthoRadius;
        int                         dummyPov;

        void generateUpVectors
                ( std::vector< MyPoint >& povs,
                  MyPoint center,
                  std::vector< MyPoint >& target )
        {
            srand( 12345 );
            target.clear();
            MyPoint dir;
            MyPoint up;
            bool ok = false;

            for( unsigned int i=0; i<povs.size(); i++ )
            {
                dir = (povs[i] - center).Normalize();

                ok = false;
                while( !ok )
                {
                    up.X() = rand() % 1000;
                    up.Y() = rand() % 1000;
                    up.Z() = rand() % 1000;
                    up.Normalize();
                    ok = ( vcg::math::Abs(up.dot( dir )) < 0.8 );
                }

                target.push_back( up );
            }
        }
    };

    // a general compactor stage that uses hystogram pyramids
    class Compactor: public Stage
    {

    public:
        Compactor( Resources* res, QString pyramid, QString inputPrefix, QString outputPrefix, bool oneInput = false )
            : Stage( res )
        {
            this->pyramidName   = pyramid;
            this->inputPrefix   = inputPrefix;
            this->outputPrefix  = outputPrefix;
            this->oneInput      = oneInput;
            compactorShader     = generateCompactionShader();
        }

        virtual void go( void )
        {
            buildPyramid();
            compact();
        }

    private:
        QString     pyramidName;
        QString     inputPrefix;
        QString     outputPrefix;
        PixelData*  pyramid;
        Program*    compactorShader;
        bool        oneInput;

        void buildPyramid( void )
        {
            pyramid = resources->buffers[ pyramidName ];
            assert( pyramid );
            pyramid->generateMipmaps();
            int levels = pyramid->getMipmapLevels();
            tmpData.clear();
            tmpData.push_back( pyramid );
            prog = resources->shaders[ "level_builder" ];
            prog->load();
            bindSampler( pyramidName, "pyramid" );

            for( int i=1; i<levels; i++ )
            {
                resources->fbo->load( tmpData, i );
                uploadInt( "level", (GLint)i );
                drawFullscreenQuad();
                resources->fbo->unload();
            }

            unbindSamplers();
            prog->unload();

            /*
            for( int i=0; i<5; i++ )
            {
                pyramid->dumpToFile( QString("pyramid") + QString::number(i) + ".txt", i );
            }
            */
        }

        void compact( void )
        {
            // prepare output buffers
            GLint elements = getPyramidElements();
            tmpData.clear();

            if( oneInput )
            {
                collectData( outputPrefix );
            }
            else
            {
                collectAttributesData( outputPrefix );
            }

            for( unsigned int i=0; i<tmpData.size(); i++ )
            {
                tmpData[i]->resizeToFit( elements );
            }
            resources->fbo->load( tmpData );

            // load shader and set uniforms
            prog = compactorShader;
            prog->load();
            samplers.clear();

            if( oneInput )
            {
                bindSampler( inputPrefix, inputPrefix );
            }
            else
            {
                bindAttributeSamplers( inputPrefix );
            }
            bindSampler( "pyramid", pyramid );
            uploadInt( "samples", elements );
            uploadInt( "max_lod", (GLint)(pyramid->getMipmapLevels() - 1) );
            QString tmpName = oneInput? outputPrefix : (outputPrefix + resources->attributes[0]);
            PixelData* outputPrototype = resources->buffers[ tmpName ];
            assert( outputPrototype );
            uploadInt( "tex_side", outputPrototype->side );

            glClear( GL_COLOR_BUFFER_BIT );
            drawFullscreenQuad();

            // free resources
            unbindSamplers();
            pyramid->unbind();
            prog->unload();
            resources->fbo->unload();

            // update elements
            std::vector< QString > outputNames;

            if( oneInput )
            {
                outputNames.push_back( outputPrefix );
            }
            else
            {
                getAttributesDataName( outputPrefix, outputNames );
            }

            for( unsigned int i=0; i<outputNames.size(); i++ )
            {
                resources->buffers[ outputNames[i] ]->elements = elements;
                //resources->buffers[ outputNames[i] ]->dumpToFile( outputNames[i] + ".txt" );
            }
        }

        Program* generateCompactionShader( void )
        {
            string simpleVertexProgram = "void main(){ gl_Position = ftransform(); }";
            std::vector< QString > inputs;
            if( oneInput )
            {
                inputs.push_back( inputPrefix );
            }
            else
            {
                getAttributesDataName( inputPrefix, inputs );
            }

            string offsets = "const ivec2 offset[4]={ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(1, 1)};";
            string shaderTemplate = getCompactionTemplate();
            string samplersDeclarations = "", outputString = "";

            string outputTemplate = string("val = texelFetch( %s, org_map_coords, 0 );\n") +
                                    "gl_FragData[%d] = val;\n\n";
            const char* cc = outputTemplate.c_str();
            char buf[250];

            for( unsigned int i=0; i<inputs.size(); i++ )
            {
                sprintf( buf, "uniform sampler2D %s;\n", inputs[i].toStdString().c_str() );
                samplersDeclarations += buf;
                sprintf( buf, cc, inputs[i].toStdString().c_str(), i );
                outputString += buf;
            }

            string fragmentProgram = offsets + samplersDeclarations
                                     + shaderTemplate + outputString + "}";

            return new Program( simpleVertexProgram, fragmentProgram );
        }

        /* this is the template used to istantiate the compaction shader. A compaction
                   shader is composed of the offsets string, the samplers declarations, this
                   template and the output string.
                 */
        string getCompactionTemplate( void )
        {
            const static string templateStr = STRINGFY(

                    uniform sampler2D pyramid;
            uniform int samples;
            uniform int tex_side;
            uniform int max_lod;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                int key_index = coords.y * tex_side + coords.x;

                if( key_index >= samples ) discard;

                ivec2 org_map_coords = ivec2( 0, 0 );
                ivec2 map_coords = ivec2( 0, 0 );
                float pyramid_val = 0.0;
                float lower_bound = 0.0;
                float upper_bound = 0.0;
                bool found = false;
                int offset_index = 0;
                float descent_value = 0.0;

                vec4 val = vec4( 0.0, 0.0, 0.0, 0.0 );
                for( int i=max_lod; i>=0; i-- )
                {
                    org_map_coords *= 2;
                    offset_index = 0;
                    lower_bound = descent_value;
                    found = false;
                    while( !found && offset_index < 4 )
                    {
                        map_coords = org_map_coords + offset[ offset_index ];
                        if ( i > 0 )
                        {
                            pyramid_val = texelFetch( pyramid, map_coords, i ).x;
                        }
                        else
                        {
                            pyramid_val = 1.0f - (texelFetch( pyramid, map_coords, i ).x);
                        }
                        upper_bound = lower_bound + pyramid_val;
                        found = ( key_index >= lower_bound && key_index < upper_bound );
                        offset_index++;
                        descent_value = lower_bound;
                        lower_bound = upper_bound;
                    }

                    org_map_coords = map_coords;
                }
            });

            return templateStr.substr( 0, templateStr.length() - 1 );
        }

        GLint getPyramidElements( void )
        {
            GLint maxLevel = pyramid->getMipmapLevels() - 1;
            GLfloat* top = pyramid->download( maxLevel );
            GLint elements = 0;

            if( top[1] < (GLfloat)0.5f )
            {
                elements = (GLint)(top[0]);
            }
            else
            {
                elements = (GLint)0;
            }

            delete top;
            return elements;
        }
    };

    class ConeFilter: public Stage
    {

    public:
        ConeFilter( Resources* res ): Stage( res )
        {
            float coneAngle = res->params->frontFacingConeU;
            float rads = ( (coneAngle/2) * PI ) / 180.0f;
            thresholdCosine = (GLfloat)(cos( rads ));
        }

        virtual void go()
        {
            tmpData.clear();
            collectData( "out_mask" );
            resources->fbo->load( tmpData );
            prog = resources->shaders[ "cone_filter" ];
            prog->load();
            samplers.clear();
            bindSampler( "start_mask", "start_mask" );
            bindSampler( "input_eye_normal", "input_eye_normal" );
            uploadFloat( "thresholdCosine", thresholdCosine );

            drawFullscreenQuad();

            unbindSamplers();
            prog->unload();
            resources->fbo->unload();
        }

    private:
        GLfloat thresholdCosine;

    };

    class Killer: public Stage
    {

    public:
        Killer( Resources* res ) : Stage( res )
        {

        }

        virtual void go()
        {
            PixelData* bestPosition = resources->buffers[ "best_position" ];
            GLint samplesSoFar = bestPosition->elements;

            // prepare output
            tmpData.clear();
            collectData( "killer_map" );
            collectData( "dead_map" );
            resources->fbo->load( tmpData );

            // reloads the depth buffer and the modelview and projection matrices
            PixelData* depthBuffer = resources->buffers[ "depth_buffer" ];
            resources->fbo->loadDepth( depthBuffer->textureId );

            glMatrixMode( GL_PROJECTION );
            glLoadMatrixf( resources->projMatrix );
            glMatrixMode( GL_MODELVIEW );
            glLoadMatrixf( resources->mvMatrix );

            // prepare shader
            prog = resources->shaders[ "killer" ];
            prog->load();
            bindSampler( "best_position", "bestPosition" );
            bindSampler( "best_normal", "bestNormal" );
            bindSampler( "out_mask", "outMask" );
            bindSampler( "input_eye_normal", "inputEyeNormal" );
            bindSampler( "input_depth", "inputDepth" );

            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );     // depth buffer is not cleared, otherwise
                                                // we cannot do shadow mapping
            feedCoords( bestPosition->side, samplesSoFar );

            unbindSamplers();
            prog->unload();
            resources->fbo->unload();

            // reset matrices
            glMatrixMode( GL_PROJECTION );
            glLoadIdentity();
            glMatrixMode( GL_MODELVIEW );
            glLoadIdentity();
        }
    };

    class MaskUpdater: public Stage
    {

    public:
        MaskUpdater( Resources* res ) : Stage( res )
        {
            ;
        }

        virtual void go( void )
        {
            tmpData.clear();
            collectData( "pov_alive_mask" );
            resources->fbo->load( tmpData );

            prog = resources->shaders[ "mask_updater" ];
            prog->load();
            bindSampler( "out_mask", "out_mask" );
            bindSampler( "killer_map", "killer_map" );

            drawFullscreenQuad();

            unbindSamplers();
            prog->unload();
            resources->fbo->unload();
        }

    };

    class DeadMasker: public Stage
    {

    public:
        DeadMasker( Resources* res ): Stage( res )
        {
            ;
        }

        virtual void go( void )
        {
            PixelData* bestPosition = resources->buffers[ "best_position" ];
            PixelData* deadMask = resources->buffers[ "dead_mask" ];
            deadMask->resize( bestPosition->side );

            PixelData* compactedDeads = resources->buffers[ "compacted_deads" ];
            GLint deadsCount = compactedDeads->elements;

            tmpData.clear();
            tmpData.push_back( deadMask );
            resources->fbo->load( tmpData );

            // adjust matrices to make the scattering work
            glMatrixMode( GL_MODELVIEW );
            glLoadIdentity();
            GLfloat scaleFactor = 2.0f / bestPosition->side;
            glTranslatef( -1.0f, -1.0f, 0.0f );
            glScalef( scaleFactor, scaleFactor, 1.0f );

            prog = resources->shaders[ "dead_masker" ];
            prog->load();
            bindSampler( "compacted_deads", "compactedDeadMap" );

            glClear( GL_COLOR_BUFFER_BIT );
            feedCoords( compactedDeads->side, deadsCount );

            unbindSamplers();
            prog->unload();
            resources->fbo->unload();

            glMatrixMode( GL_MODELVIEW );
            glLoadIdentity();
        }

    };

    class AliveMasker: public Stage
    {

    public:
        AliveMasker( Resources* res ): Stage( res )
        {
            ;
        }

        virtual void go( void )
        {
            PixelData* bestPosition = resources->buffers[ "best_position" ];
            PixelData* aliveMask = resources->buffers[ "alive_mask" ];
            aliveMask->resize( bestPosition->side );

            tmpData.clear();
            tmpData.push_back( aliveMask );
            resources->fbo->load( tmpData );

            prog = resources->shaders[ "alive_masker" ];
            prog->load();
            bindSampler( "dead_mask", "deadMask" );
            uploadInt( "elements", bestPosition->elements );
            uploadInt( "texSide", bestPosition->side );

            drawFullscreenQuad();

            unbindSamplers();
            prog->unload();
            resources->fbo->unload();
        }

    };

    class FinalCompactor: public Stage
    {

    public:
        FinalCompactor( Resources* res ): Stage( res )
        {
            ;
        }

        virtual void go( void )
        {
            PixelData* currentBestPosition  = resources->buffers[ "current_best_position" ];
            PixelData* alivePosition        = resources->buffers[ "alive_position" ];
            GLint samplesCount = currentBestPosition->elements + alivePosition->elements;

            tmpData.clear();
            collectAttributesData( "best_" );
            for( unsigned int i=0; i<tmpData.size(); i++ )
            {
                tmpData[i]->resizeToFit( samplesCount );
                tmpData[i]->elements = samplesCount;
            }

            resources->fbo->load( tmpData );
            prog = resources->shaders[ "final_compactor" ];
            prog->load();
            bindAttributeSamplers( "current_best_" );
            bindAttributeSamplers( "alive_" );
            uploadInt( "totalElements", samplesCount );
            uploadInt( "survivors", alivePosition->elements );
            uploadInt( "currentBests", currentBestPosition->elements );
            uploadInt( "targetSide", tmpData[0]->side );
            uploadInt( "survivorsSide", alivePosition->side );
            uploadInt( "currentBestsSide", currentBestPosition->side );

            glClear( GL_COLOR_BUFFER_BIT );
            drawFullscreenQuad();

            unbindSamplers();
            prog->unload();
            resources->fbo->unload();
        }

    };

    class FeatureDetector: public Stage
    {

    public:
        FeatureDetector( Resources* res ): Stage( res )
        {
            ;
        }

        virtual void go( void )
        {
            // retrieves the depth range
            tmpData.clear();
            collectData( "input_depth" );
            tmpData[0]->generateMipmaps();
            prog = resources->shaders[ "depth_range_detector" ];
            prog->load();
            bindSampler( "input_depth", "input_depth" );

            for( int i=1; i<tmpData[0]->getMipmapLevels(); i++ )
            {
                resources->fbo->load( tmpData, i );
                uploadInt( "level", i );
                drawFullscreenQuad();
                resources->fbo->unload();
            }

            unbindSamplers();
            prog->unload();

            GLfloat* top = tmpData[0]->download( tmpData[0]->getMipmapLevels() - 1 );
            GLfloat depthRange = top[1] - top[0];
            delete top;

            // performs feature detection
            GLfloat smallJump   = depthRange * resources->params->smallDepthJump;
            GLfloat bigJump     = depthRange * resources->params->bigDepthJump;
            float   rads        = ( resources->params->angleThreshold * PI ) / 180.0f;
            GLfloat thrCos      = (GLfloat)( cos( rads ) );
            rads                = ((resources->params->frontFacingConeF / 2.0f) * PI) / 180.0f;
            GLfloat ffCos       = (GLfloat)( cos( rads ) );

            tmpData.clear();
            collectData( "out_mask" );

            resources->fbo->load( tmpData );
            prog = resources->shaders[ "feature_detector" ];
            prog->load();
            bindSampler( "start_mask", "startMask" );
            bindSampler( "input_depth", "inputDepth" );
            bindSampler( "input_normal", "inputNormal" );
            bindSampler( "input_eye_normal", "inputEyeNormal" );
            uploadFloat( "smallDepthJump", smallJump );
            uploadFloat( "bigDepthJump", bigJump );
            uploadFloat( "thresholdCosine", thrCos );
            uploadFloat( "frontFacingCosine", ffCos );

            drawFullscreenQuad();

            unbindSamplers();
            prog->unload();
            resources->fbo->unload();
        }

    };

}

#endif // STAGES_H
