#ifndef SHADER_BUILDER_H
#define SHADER_BUILDER_H

#define STRINGFY(S) #S

#include <string>

using namespace std;

namespace agl
{
    namespace shaders
    {
        class ShaderBuilder
        {
        public:
            virtual ~ShaderBuilder( void ){}

            virtual void buildVSVariables   ( void ){}
            virtual void buildVSFunctions   ( void ){}
            virtual void buildVSBody        ( void ){}

            virtual void buildFSVariables   ( void ){}
            virtual void buildFSFunctions   ( void ){}
            virtual void buildFSBody        ( void ){}

            virtual void getShaderSource( string& vs, string& fs )
            {
                vs = "void main(){\n\tgl_Position = ftransform();\n}";
                fs = "void main(){\n\tgl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n}";
            }

        };
    }
}

#endif // SHADER_BUILDER_H
