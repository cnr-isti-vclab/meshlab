#ifndef FEATURE_DETECTOR_H
#define FEATURE_DETECTOR_H

#include "../../shaders/simple_shader_variables.h"
#include "../../shaders/shader_set.h"
#include "../vrs_parameters.h"
#include "../../agl_global.h"
#include "../../utils/file_rw.h"

using namespace agl::shaders;
using namespace agl::utils;

namespace agl
{
    namespace vrs
    {
        class FeatureDetector
        {
        public:
            FeatureDetector( string detectorShaderName )
            {
                string vrsPath = agl::global::vrs_path;
                string shaderFilename = vrsPath + "glsl/detectors/" + detectorShaderName + ".detector";
                string vsSource = "void main(){ gl_Position = ftransform(); }";
                string fsSource = "";
                file_rw::read_file( shaderFilename, fsSource );
                detectorShader = shader_set::create_from_source( vsSource, fsSource, "" );
                assert( detectorShader );
                detectorShader->set_variables( &ssv );
                parameters.push_back( "inputMap" );
                parameters.push_back( "inputMask" );
            }

            virtual ~FeatureDetector( void )
            {
                if( detectorShader )
                {
                    delete detectorShader;
                }
            }

            void load( void )
            {
                detectorShader->load();
            }

            void unload( void )
            {
                detectorShader->unload();
            }

            void set_texture( string name, GLint unit, texture2d* t )
            {
                assert( existsParameter( name ) );
                ssv.bind_texture( name, unit, t );
            }

            void set_param( string name, GLenum type, GLvoid* v,
                            GLint components = 1, GLint count = 1 )
            {
                assert( existsParameter( name ) );
                ssv.set_uniform_vector( name, type, v, components, count );
            }

            virtual int getInputAttribute( void )
            {
                return 0;
            }

            virtual int getAuxiliaryAttributes( void )
            {
                return 0;
            }

       protected:
            vector< string > parameters;
            shader* detectorShader;
            simple_shaders_variables ssv;

            bool existsParameter( string pName )
            {
                vector< string >::iterator vi =
                        find( parameters.begin(), parameters.end(), pName );
                return ( vi != parameters.end() );
            }

        };
    }
}

#endif // FEATURE_DETECTOR_H
