#ifndef FINAL_COMPACTOR_H
#define FINAL_COMPACTOR_H

#include "stage.h"

namespace agl
{
    namespace vrs
    {
        class FinalCompactor: public Stage
        {
        public:
            FinalCompactor( vector< string >& attributeNames )
            {
                generateFinalCompactorShader( attributeNames );

                PixelData* inputData = res->pdm[ "inputMask" ];
                assert( inputData );
                initialResolution = inputData->data->get_width();
            }

            ~FinalCompactor( void )
            {
                delete finalCompactorShader;
            }

            virtual void go( void )
            {
                // initializes vectors
                bestSurvivors.clear();
                bestSurvivorsNames.clear();
                currentBests.clear();
                currentBestsNames.clear();

                // deletes the best samples so far (they are already copied in survivor-maps)
                deleteOldBestSamples();

                // collect inputs
                res->pdm.getData( "survivor", bestSurvivors, bestSurvivorsNames );
                res->pdm.getData( "currentBest", currentBests, currentBestsNames );

                // calculates the new number of best samples
                int survivorElements = bestSurvivors[0]->length;
                int currentBestElements = currentBests[0]->length;
                int total = survivorElements + currentBestElements;

                // calculates the size of the texture that will contain the best samples
                int outputSide = 1;
                bool ok = false;
                while( !ok )
                {
                    ok = ( outputSide * outputSide >= total );
                    outputSide *= 2;
                }
                outputSide /= 2;

                // builds the fbo configuration
                int numberOfAttributes = bestSurvivors.size();
                FboConfiguration* conf = buildConfiguration( outputSide, numberOfAttributes );

                // saves the PixelData object containing the new best samples textures
                vector< texture2d* > confTextures;
                conf->getTextures( confTextures, true );
                PixelData* pData = 0;
                for( unsigned int i=0; i<confTextures.size(); i++ )
                {
                    pData = new PixelData( confTextures[i], total );
                    res->pdm.saveData( bestSoFarNames[i], pData, true );
                }

                // sets the shader variables
                ssv.clear();
                int survivorsSide = bestSurvivors[0]->data->get_width();
                int currentBestSide = currentBests[0]->data->get_width();
                int textureUnit = 0;
                for( unsigned int i=0; i<confTextures.size(); i++ )
                {
                    ssv.bind_texture( bestSurvivorsNames[i], textureUnit++, bestSurvivors[i]->data );
                    ssv.bind_texture( currentBestsNames[i], textureUnit++, currentBests[i]->data );
                }
                ssv.set_uniform_vector( "totalElements", GL_INT, &total );
                ssv.set_uniform_vector( "survivors", GL_INT, &survivorElements );
                ssv.set_uniform_vector( "currentBests", GL_INT, &currentBestElements );
                ssv.set_uniform_vector( "targetSide", GL_INT, &outputSide );
                ssv.set_uniform_vector( "survivorsSide", GL_INT, &survivorsSide );
                ssv.set_uniform_vector( "currentBestsSide", GL_INT, &currentBestSide );
                finalCompactorShader->set_variables( &ssv );

                // sets the viewport and draws a fullscreen quad
                glViewport( 0, 0, (GLsizei)outputSide, (GLsizei)outputSide );
                fullscreenPass( conf, finalCompactorShader );
                glViewport( 0, 0, (GLsizei)initialResolution, (GLsizei)initialResolution );

                res->fbo.setConfiguration(0);
                deleteDepth( conf );
                delete conf;
            }

        private:
            vector< PixelData* > bestSurvivors;
            vector< string > bestSurvivorsNames;
            vector< PixelData* > currentBests;
            vector< string > currentBestsNames;
            vector< PixelData* > bestSoFar;
            vector< string > bestSoFarNames;
            shader* finalCompactorShader;
            int initialResolution;

            void deleteOldBestSamples( void )
            {
                // obtains and delete the best samples textures because they are
                // going to be replaced by the new ones
                bestSoFar.clear();
                bestSoFarNames.clear();
                res->pdm.getData( "best", bestSoFar, bestSoFarNames );

                vector< texture2d* > pDataTextures;
                for( unsigned int i=0; i<bestSoFar.size(); i++ )
                {
                    pDataTextures.push_back( bestSoFar[i]->data );
                    res->pdm.deleteData( bestSoFarNames[i] );
                }
                res->pdm.tm.deleteTextures( pDataTextures );
            }

            void generateFinalCompactorShader( vector< string >& attributeNames )
            {
                bestSoFar.clear();
                bestSoFarNames.clear();
                res->pdm.getData( "best", bestSoFar, bestSoFarNames );

                vector<string> attributes;
                for( unsigned int i=0; i<bestSoFarNames.size(); i++ )
                {
                    attributes.push_back( bestSoFarNames[i].replace( 0, 4, string("") ) );
                }

                string samplersDeclarations = "";
                string output1 = "", output2 = "";
                string outputTemplate = string("val = texelFetch( %s, texelCoords, 0 );\n") +
                                        "gl_FragData[%d] = vec4( val.xyz, 0.0 );\n";
                const char* templateCC = outputTemplate.c_str();

                char survivorName[30], currentBestName[30];

                char buf[300];
                for( unsigned int i=0; i<attributeNames.size(); i++ )
                {
                    sprintf( survivorName, "survivor%s", attributes[i].c_str() );
                    sprintf( currentBestName, "currentBest%s", attributes[i].c_str() );
                    sprintf( buf, "uniform sampler2D %s;\n", survivorName );
                    samplersDeclarations += buf;
                    sprintf( buf, "uniform sampler2D %s;\n", currentBestName );
                    samplersDeclarations += buf;
                    sprintf( buf, templateCC, survivorName, i );
                    output1 += buf;
                    sprintf( buf, templateCC, currentBestName, i );
                    output2 += buf;
                }

                string ifStatement = string("if( fromSurvivors == 1 ){\n") +
                                     output1 + "\n}else{\n" +
                                     output2 + "}\n";

                QString str;
                readResource( "final_compactor.template", str );

                string fsTemplate = str.toStdString();
                //file_rw::read_file( glslPath + "final_compactor.template", fsTemplate );

                string fsSource = samplersDeclarations + fsTemplate + ifStatement + "}";
                finalCompactorShader = shader_set::create_from_source
                                       ( simpleVSSource, fsSource, "" );
            }

        };
    }
}

#endif // FINAL_COMPACTOR_H
