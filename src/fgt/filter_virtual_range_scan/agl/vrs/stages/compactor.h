#ifndef COMPACTOR_H
#define COMPACTOR_H

#include "stage.h"
#include <QDir>

namespace agl
{
    namespace vrs
    {
        class Compactor: public Stage
        {
        public:
            Compactor( void )
            {
                pyramidBuilder = createShader( "level_builder" );

                PixelData* pData = res->pdm[ "inputMask" ];
                initialResolution = pData->data->get_width();
            }

            ~Compactor( void )
            {
                delete pyramidBuilder;
            }

            virtual void go( void )
            {
                compact( generatePyramid() );
            }

            string mask;
            vector< string > inputs;
            vector< string > outputs;

        private:

            shader* pyramidBuilder;
            int initialResolution;

            texture2d* generatePyramid( void )
            {
                PixelData* maskData = res->pdm.getData( mask );
                FboConfiguration* conf = buildConfiguration( maskData->data, 1 );

                int vs = maskData->data->get_width();
                glViewport( 0, 0, (GLsizei)vs, (GLsizei)vs );

                copyMask( conf, maskData->data );

                FboAttachment* att = conf->getAttachment( GL_COLOR_ATTACHMENT0 );
                texture2d* pyramid = att->renderTarget;
                pyramid->generate_mipmaps();
                int levels = pyramid->get_mipmap_levels();

                ssv.clear();
                ssv.bind_texture( "pyramid", 0, pyramid );

                for( int i=1; i<levels; i++ )
                {
                    ssv.set_uniform_vector( "level", GL_INT, &i );
                    att->lod = i;
                    fullscreenPass( conf, pyramidBuilder );
                }
                att->lod = 0;
                res->fbo.setConfiguration(0);

                res->pdm.tm.deleteTextures( &( (*conf)[ GL_DEPTH_ATTACHMENT ]->renderTarget ), 1 );
                delete conf;
                return pyramid;
            }

            void copyMask( FboConfiguration* c, texture2d* m )
            {
                string fsSource = string( "uniform sampler2D mask;\n") +
                                  "void main(){ ivec2 coords = ivec2( gl_FragCoord.xy );\n" +
                                  "vec4 texel = texelFetch( mask, coords, 0 );\n" +
                                  "gl_FragColor = vec4( texel.x, 0.0, 0.0, 0.0);}";
                shader* copier = shader_set::create_from_source( simpleVSSource, fsSource, "" );

                ssv.clear();
                ssv.bind_texture( "mask", 0, m );
                fullscreenPass( c, copier );
                delete copier;
            }

            void compact( texture2d* pyramid )
            {
                shader* compactorShader = generateCompactorShader();
                int outs = (int)inputs.size();

                int maxMipmapLevel = pyramid->get_mipmap_levels() - 1;
                client_texture2d_f* ct = texture2d::download< float >( pyramid, maxMipmapLevel );
                int elements = (int)ct->pixels[0]->x();
                delete ct;

                int outputSide = 1;
                bool ok = false;
                while( !ok )
                {
                    ok = ( outputSide * outputSide >= elements );
                    outputSide *= 2;
                }
                outputSide /= 2;

                glViewport( 0, 0, (GLsizei)outputSide, (GLsizei)outputSide );

                FboConfiguration* conf = buildConfiguration( outputSide, outs );

                ssv.clear();
                bindInputs( inputs );
                ssv.bind_texture( "pyramid", (GLint)inputs.size(), pyramid );
                ssv.set_uniform_vector( "samples", GL_INT, &elements );
                ssv.set_uniform_vector( "tex_side", GL_INT, &outputSide );
                ssv.set_uniform_vector( "max_lod", GL_INT, &maxMipmapLevel );

                fullscreenPass( conf, compactorShader );

                vector< texture2d* > confTextures;
                conf->getTextures( confTextures, true );

                PixelData* tmpData = 0;
                for( int i=0; i<outs; i++ )
                {
                    tmpData = new PixelData( confTextures[i], elements );
                    res->pdm.saveData( outputs[i], tmpData, true );
                }

                res->fbo.setConfiguration( 0 );
                pyramid->unbind();
                res->pdm.tm.deleteTextures( &pyramid, 1 );
                deleteDepth( conf );
				delete conf;
				delete compactorShader;

                glViewport( 0, 0, (GLsizei)initialResolution, (GLsizei)initialResolution );
            }

            shader* generateCompactorShader( void )
            {
                string samplersDeclaration = getSamplersDeclarations( inputs );
                string outputString = "";
                string outputTemplate = string("val = texelFetch( %s, org_map_coords, 0 );\n") +
                                        "gl_FragData[%d] = val;\n\n";
                const char* cc = outputTemplate.c_str();
                char buf[150];

                for( unsigned int i=0; i<inputs.size(); i++ )
                {
                    sprintf( buf, cc, inputs[i].c_str(), i );
                    outputString += buf;
                }

                QString str;
                readResource( "compactor.template", str );
                string fsBody = str.toStdString();

                //file_rw::read_file( glslPath + "compactor.template", fsBody );
                string fsSource = samplersDeclaration + fsBody + outputString + "}";
                return shader_set::create_from_source( simpleVSSource, fsSource, "" );
            }

        };
    }
}

#endif // COMPACTOR_H
