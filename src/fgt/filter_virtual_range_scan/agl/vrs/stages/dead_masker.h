#ifndef DEAD_MASKER_H
#define DEAD_MASKER_H

#include "stage.h"
#include "../../utils/viewing_info.h"

using namespace agl::utils;

namespace agl
{
    namespace vrs
    {
        class DeadMasker: public Stage
        {
        public:
            DeadMasker( void )
            {
                dmNegShader = createShader( "dead_masker_negation" );
                dmShader = createShader( "dead_masker" );
                inputResolution = res->pdm[ "inputMask" ]->data->get_width();
            }

            ~DeadMasker( void )
            {
                delete dmNegShader;
                delete dmShader;
            }

            virtual void go( void )
            {
                // 1st pass: builds the dead mask negation
                // builds the output configuration
                PixelData* pData = res->pdm[ "bestPosition" ];
                texture2d* bestPrototype = pData->data;
                assert( bestPrototype );
                FboConfiguration* conf = buildConfiguration( bestPrototype, 1 );
                texture2d* deadMaskNegation = (*conf)[ GL_COLOR_ATTACHMENT0 ]->renderTarget;

                // prepares the shader for execution
                PixelData* compDead = res->pdm[ "compactedDeadMap" ];
                assert( compDead );
                texture2d* t = compDead->data;
                ssv.clear();
                ssv.bind_texture( "compactedDeadMap", 0, t );
                dmNegShader->set_variables( &ssv );

                // sets the proper transformations
                drawing_utils::save_matrices();
                int newViewportSide = bestPrototype->get_width();
                glViewport( 0, 0, (GLsizei)newViewportSide, (GLsizei)newViewportSide );
                glMatrixMode( GL_MODELVIEW );
                glLoadIdentity();
                GLdouble scaleFactor = 2.0 / newViewportSide;
                glTranslatef( -1.0, -1.0, 0.0 );
                glScalef( scaleFactor, scaleFactor, 1.0 );

                // generates the dead mask
                res->fbo.setConfiguration( conf );
                dmNegShader->load();
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                drawing_utils::feed_coords( t->get_width(), t->get_height(), compDead->length );
                dmNegShader->unload();

                //res->fbo.saveScreenshots();
                res->fbo.setConfiguration(0);

                // cleanup
                deleteDepth( conf );
                delete conf;

                // 2nd pass-> mask extraction (from negation mask)
                conf = buildConfiguration( deadMaskNegation, 1 );
                texture2d* deadMask = (*conf)[ GL_COLOR_ATTACHMENT0 ]->renderTarget;
                PixelData* deadData = new PixelData( deadMask, pData->length );
                res->pdm.saveData( "deadMask", deadData, true );

                // unload the matrices (but retains the new viewport transformation)
                glMatrixMode( GL_MODELVIEW );
                glLoadIdentity();
                glMatrixMode( GL_PROJECTION );
                glLoadIdentity();

                int texSide = bestPrototype->get_width();
                ssv.clear();
                ssv.bind_texture( "negationMask", 0, deadMaskNegation );
                ssv.set_uniform_vector( "elements", GL_INT, &(pData->length) );
                ssv.set_uniform_vector( "texSide", GL_INT, &texSide );
                dmShader->set_variables( &ssv );

                res->fbo.setConfiguration( conf );
                dmShader->load();
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                drawing_utils::draw_fullscreen_quad();
                dmShader->unload();
                res->fbo.setConfiguration(0);

                drawing_utils::restore_matrices();
                glViewport( 0, 0, (GLsizei)inputResolution, (GLsizei)inputResolution );

                texture2d* confDepth = (*conf)[ GL_DEPTH_ATTACHMENT ]->renderTarget;
                texture2d* texturesToDelete[2] = { confDepth, deadMaskNegation };
                res->pdm.tm.deleteTextures( texturesToDelete, 2 );
                delete conf;
            }

        private:
            shader* dmNegShader;
            shader* dmShader;
            int inputResolution;

        };
    }
}

#endif // DEAD_MASKER_H
