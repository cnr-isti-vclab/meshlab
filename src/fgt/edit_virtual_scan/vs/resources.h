#ifndef RESOURCES_H
#define RESOURCES_H

#include <GL/glew.h>
#include <QString>

#include <wrap/gl/shot.h>
#include <wrap/gl/trimesh.h>
#include <vcg/space/box2.h>
#include <map>

#include "utils.h"

#define STRINGFY(x) #x

using namespace std;

namespace vs
{
    typedef pair< vcg::Shot< float >, vcg::Box2i > Pov;

    /* Virtual Scanning parameters */
    class VSParameters
    {
    public:
        int povs;                           // point of views
        float coneAxis[3];                  // looking-cone axis
        float coneGap;                      // looking-cone gap

        int uniformResolution;              // uniform sampling textures resolution (n x n)
        int featureResolution;              // feature sampling textures resolution (n x n)
        int attributeMask;                  // indicates which attributes to extract
        float smallDepthJump;               // the maximum allowable depth jump to recognize mesh patches continuity
        float angleThreshold;               // the threshold angle (degrees) used in feature detection step
        float bigDepthJump;                 // used to detect big depth offset
        float frontFacingConeU;             // the angle (in degrees) used to determine whether a pixel "is facing" the observer (uniform sampling)
        float frontFacingConeF;             // the facing cone gap used in the feature sensitive sampling step

        vcg::GLW::DrawMode      cmDrawMode;     // the color-map draw mode
        vcg::GLW::ColorMode     cmColorMode;    // the color-map color mode
        vcg::GLW::TextureMode   cmTextureMode;  // the color-map texture mode

        vector< Pov > customPovs;           // custom point-of-views
        bool useCustomPovs;
    };

    /*
        This class manages framebuffer objects, textures and shaders used by
        the scanning process.
     */
    class Resources
    {

    public:
        Resources( VSParameters* params )
        {
            assert( params );
            this->params = params;
            attributes.push_back( "depth" );
            attributes.push_back( "position" );
            attributes.push_back( "normal" );
            attributes.push_back( "eye_normal" );
            attributes.push_back( "color" );

            simpleVertexProgram = "void main(){ gl_Position = ftransform(); }";
            offsets = "const ivec2 offset[4]={ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(1, 1)};";
            initialized = false;
        }

        ~Resources( void )
        {
            finalize();
        }

        void initialize( void )
        {
            initializeTextures();
            initializeShaders();
            fbo = new Fbo();
            initialized = true;
        }

        void changeResolution( void )
        {
            std::vector< QString > dataToChange;
            getResolutionDependentDataNames( dataToChange );
            PixelData* pData = 0;
            for( unsigned int i=0; i<dataToChange.size(); i++ )
            {
                pData = buffers[ dataToChange[i] ];
                pData->resize( params->featureResolution );
            }
            buffers[ "depth_buffer" ]->resize( params->featureResolution );
        }

        void finalize( void )
        {
            if( initialized )
            {
                delete fbo;
                destroyShaders();
                destroyTextures();
                initialized = false;
            }
        }

        VSParameters*               params;
        vector< QString >           attributes;
        map< QString, PixelData* >  buffers;
        map< QString, Program* >    shaders;
        Fbo*                        fbo;
        GLfloat                     mvMatrix[ 16 ];
        GLfloat                     projMatrix[ 16 ];

    private:
        bool                initialized;
        string              simpleVertexProgram;
        string              offsets;

        /* ----------------- textures ------------------------------------------------ */
        void getResolutionDependentDataNames( std::vector< QString >& target )
        {
            target.clear();
            QString str = "";
            for( unsigned int i=0; i<attributes.size(); i++ )
            {
                str = QString("input_") + attributes[i];
                target.push_back( str );
            }

            QString otherData[] = { "start_mask", "out_mask",
                                    "killer_map", "dead_map", "pov_alive_mask" };
            for( int i=0; i<5; i++ ) target.push_back( otherData[i] );
        }

        void initializeTextures( void )
        {
            std::vector< QString > dataNames;
            getResolutionDependentDataNames( dataNames );
            PixelData* newData = 0;
            for( unsigned int i=0; i<dataNames.size(); i++ )
            {
                newData = new PixelData( params->uniformResolution );
                buffers[ dataNames[i] ] = newData;
            }
            newData = new PixelData( params->uniformResolution, true );
            buffers[ "depth_buffer" ] = newData;

            newData = new PixelData( 1 );
            buffers[ "compacted_deads" ] = newData;
            newData = new PixelData( 1 );
            buffers[ "dead_mask" ] = newData;
            newData = new PixelData( 1 );
            buffers[ "alive_mask" ] = newData;

            createAttributeTextures( "current_best_" );
            createAttributeTextures( "best_" );
            createAttributeTextures( "alive_" );
        }

        void createAttributeTextures( QString prefix, int side = 1 )
        {
            QString name = "";
            PixelData* newData = 0;

            for( unsigned int i=0; i<attributes.size(); i++ )
            {
                name = prefix + attributes[i];
                newData = new PixelData( side, false );
                buffers[ name ] = newData;
            }
        }

        void destroyTextures( void )
        {
            map< QString, PixelData* >::iterator mi;
            for( mi = buffers.begin(); mi != buffers.end(); ++mi )
            {
                delete (*mi).second;
            }
            buffers.clear();
        }
        /* --------------------------------------------------------------------------- */

        /* -------------------- shaders ---------------------------------------------- */
        void initializeShaders( void )
        {
            shaders[ "start" ]                  = createStartShader();
            shaders[ "level_builder" ]          = createLevelBuilderShader();
            shaders[ "cone_filter" ]            = createConeFilterShader();
            shaders[ "killer" ]                 = createKillerShader();
            shaders[ "mask_updater" ]           = createMaskUpdaterShader();
            shaders[ "dead_masker" ]            = createDeadMaskerShader();
            shaders[ "alive_masker" ]           = createAliveMaskerShader();
            shaders[ "final_compactor" ]        = createFinalCompactorShader();
            shaders[ "depth_range_detector" ]   = createDepthRangeDetectorShader();
            shaders[ "feature_detector" ]       = createFeatureDetectorShader();
        }

        void destroyShaders( void )
        {
            map< QString, Program* >::iterator si;
            for( si = shaders.begin(); si != shaders.end(); ++si )
            {
                delete (*si).second;
            }
            shaders.clear();
        }

        // attributes extraction shader
        Program* createStartShader( void )
        {
            const string vertexProgram = STRINGFY(

                    varying vec4 pos;
            varying vec3 norm;
            varying vec3 eyeNorm;
            varying vec4 col;

            void main()
            {
                pos = gl_Vertex;
                norm = gl_Normal;
                eyeNorm = gl_NormalMatrix * gl_Normal;
                col = gl_Color;
                gl_Position = ftransform();
            }
            );

            const string fragmentProgram = STRINGFY(

                    varying vec4 pos;
            varying vec3 norm;
            varying vec3 eyeNorm;
            varying vec4 col;

            void main()
            {
                float d = gl_FragCoord.z;
                vec3 the_norm = normalize( norm );
                vec3 the_eye_norm = normalize( eyeNorm );

                gl_FragData[0] = vec4( d, d, 0.0f, 0.0f );
                gl_FragData[1] = vec4( pos.xyz, 0.0f );
                gl_FragData[2] = vec4( the_norm, 0.0f );
                gl_FragData[3] = vec4( the_eye_norm, 0.0f );
                gl_FragData[4] = col;
                gl_FragData[5] = vec4( 0.0f, 0.0f, 0.0f, 0.0f );
            }
            );

            return new Program( vertexProgram, fragmentProgram );
        }

        // this shader is used to compute the hystogram pyramid levels
        Program* createLevelBuilderShader( void )
        {
            const string fragmentTemplate = STRINGFY(

                    uniform sampler2D pyramid;
            uniform int level;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                ivec2 upCoords = 2 * coords;
                float val = 0.0;

                for( int i=0; i<4; i++ )
                {
                    vec3 upVal = texelFetch( pyramid, upCoords + offset[i], level-1 );
                    if( level == 1 )
                    {
                        val += ( 1.0f - upVal.x );
                    }
                    else
                    {
                        val += upVal.x;
                    }
                }

                gl_FragColor = vec4( val, 0.0f, 0.0f, 0.0f );
            }

            );

            string fragmentProgram = offsets + fragmentTemplate;
            return new Program( simpleVertexProgram, fragmentProgram );
        }

        /* this shader takes a normal map and an input mask, and produces another mask
                   with 1's where the normal is facing the observer within a given cone of
                   directions */
        Program* createConeFilterShader( void )
        {
            const string fragmentProgram = STRINGFY(

                    uniform sampler2D start_mask;
            uniform sampler2D input_eye_normal;
            uniform float thresholdCosine;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                float maskVal = texelFetch( start_mask, coords, 0 ).x;
                if( maskVal > 0.5 )
                {
                    gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
                }
                else
                {
                    vec3 n = texelFetch( input_eye_normal, coords, 0 ).xyz;
                    vec3 viewDir = vec3( 0.0, 0.0, -1.0 );

                    float viewCosine = dot( n, -viewDir );
                    if( viewCosine > thresholdCosine )
                    {
                        gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
                    }
                    else
                    {
                        gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
                    }
                }
            }
            );

            return new Program( simpleVertexProgram, fragmentProgram );
        }

        /* this shader is used to determine which samples of the current view must be
                   killed, and which best samples must be killed */
        Program* createKillerShader( void )
        {
            const string vertexProgram = STRINGFY(

                    uniform sampler2D bestPosition;
            uniform sampler2D bestNormal;

            varying vec2    sample_coords;
            varying vec3    sample_eye_normal;

            void main()
            {
                sample_coords = gl_Vertex.xy;
                ivec2 tex_coords = ivec2( sample_coords );
                vec3 pos = texelFetch( bestPosition, tex_coords, 0 );
                vec4 vert = vec4( pos, 1.0f );
                vec3 sample_normal = texelFetch( bestNormal, tex_coords, 0 );
                sample_eye_normal = gl_NormalMatrix * sample_normal;
                gl_Position = gl_ModelViewProjectionMatrix * vert;
            }

            );

            const string fragmentProgram = STRINGFY(

                    uniform sampler2D outMask;
            uniform sampler2D inputEyeNormal;
            uniform sampler2D inputDepth;

            varying vec2    sample_coords;
            varying vec3    sample_eye_normal;

            void main()
            {
                ivec2 frag_coords = ivec2( gl_FragCoord.xy );
                float mask_val = texelFetch( outMask, frag_coords, 0 );

                if( mask_val < 0.5f )
                {   // possible conflict

                    float arrivingDepth = gl_FragCoord.z;
                    float povDepth = texelFetch( inputDepth, frag_coords, 0 ).x;
                    float gap = abs( arrivingDepth - povDepth );

                    if( gap < 0.03f )
                    {   // conflict
                        vec3 current_eye_normal = normalize( texelFetch( inputEyeNormal, frag_coords, 0 ) );
                        vec3 norm_sample_eye_normal = normalize( sample_eye_normal );
                        vec3 perfect_view = vec3( 0.0f, 0.0f, -1.0f );

                        float look_at_me_1 = dot( -perfect_view, current_eye_normal );
                        float look_at_me_2 = dot( -perfect_view, norm_sample_eye_normal );

                        if( look_at_me_1 > look_at_me_2 )
                        {   // the current view is the best
                            gl_FragData[0] = vec4( 0.0f, 1.0f, 1.0f, 1.0f );
                            gl_FragData[1] = vec4( sample_coords, 1.0f, 1.0f );
                        }
                        else
                        {   // the arriving sample is the best
                            gl_FragData[0] = vec4( 1.0f, -1.0f, 1.0f, 1.0f );
                            gl_FragData[1] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                        }
                    }
                    else
                    {   // no conflict
                        gl_FragData[0] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                        gl_FragData[1] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                    }
                }
                else
                {   // no conflict
                    gl_FragData[0] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                    gl_FragData[1] = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                }


            }

            );

            return new Program( vertexProgram, fragmentProgram );
        }

        Program* createMaskUpdaterShader( void )
        {
            const string fragmentProgram = STRINGFY(

                    uniform sampler2D out_mask;
            uniform sampler2D killer_map;

            void main()
            {
                ivec2 my_coords = ivec2( gl_FragCoord.xy );

                float val = texelFetch( out_mask, my_coords, 0 ).x;
                if( val > 0.5f )
                {   // background pixel
                    gl_FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                }
                else
                {
                    float val2 = texelFetch( killer_map, my_coords, 0 ).y;
                    if( val2 > -0.5f )
                    {   // alive pixel
                        gl_FragColor = vec4( 0.0f, 0.0f, 0.0f, 0.0f );
                    }
                    else
                    {   // killed pixel -> background pixel
                        gl_FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                    }
                }
            }

            );

            return new Program( simpleVertexProgram, fragmentProgram );
        }

        Program* createDeadMaskerShader( void )
        {
            const string vertexProgram = STRINGFY(

                    uniform sampler2D compactedDeadMap;

            void main()
            {
                ivec2 coords = ivec2( gl_Vertex.xy );
                vec4 deadCoords = texelFetch( compactedDeadMap, coords, 0 );
                vec4 myVertex = vec4( deadCoords.xy, -1.0f, 1.0f );
                gl_Position = gl_ModelViewProjectionMatrix * myVertex;
            }

            );

            const string fragmentProgram = STRINGFY(

                    void main()
                    {
                gl_FragColor = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
            }

            );

            return new Program( vertexProgram, fragmentProgram );
        }

        Program* createAliveMaskerShader( void )
        {
            const string fragmentProgram = STRINGFY(

                    uniform sampler2D deadMask;
            uniform int elements;
            uniform int texSide;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                int key_index = coords.y * texSide + coords.x;

                if( key_index >= elements )
                {
                    gl_FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                }
                else
                {
                    float negVal = texelFetch( deadMask, coords, 0 ).x;
                    if( negVal < 0.5f )
                    {
                        gl_FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                    }
                    else
                    {
                        gl_FragColor = vec4( 0.0f, 0.0f, 0.0f, 0.0f );
                    }
                }
            }

            );

            return new Program( simpleVertexProgram, fragmentProgram );
        }

        string getFinalCompactorTemplate( void )
        {
            const string fsTemplate = STRINGFY(

                    uniform int totalElements;
            uniform int survivors;
            uniform int currentBests;
            uniform int targetSide;
            uniform int survivorsSide;
            uniform int currentBestsSide;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                int myPos = coords.y * targetSide + coords.x;
                int x = 0;
                int y = 0;
                int fromSurvivors = 0;

                if( myPos >= totalElements )
                {
                    discard;
                }

                if( myPos < survivors )
                {   // copy the survivor
                    y = myPos / survivorsSide;
                    x = myPos - y * survivorsSide;
                    fromSurvivors = 1;
                }
                else
                {   // append the sample of current view
                    myPos = myPos - survivors;
                    y = myPos / currentBestsSide;
                    x = myPos - y * currentBestsSide;
                }

                ivec2 texelCoords = ivec2( x, y );
                vec4 val = vec4( 0.0f, 0.0f, 0.0f, 0.0f );
            });

            return fsTemplate.substr( 0, fsTemplate.length() - 1 );
        }

        Program* createFinalCompactorShader( void )
        {
            int count = attributes.size();
            string* alive = new string[ count ];
            string* currentBests = new string[ count ];

            for( int i=0; i<count; i++ )
            {
                alive[i] = "alive_" + attributes[i].toStdString();
                currentBests[i] = "current_best_" + attributes[i].toStdString();
            }

            string shaderTemplate = getFinalCompactorTemplate();
            const char decTemplate[] = "uniform sampler2D %s;\n";

            string output1 = "", output2 = "";
            string outputTemplate = string("val = texelFetch( %s, texelCoords, 0 );\n") +
                                    "gl_FragData[%d] = vec4( val.xyz, 0.0 );\n";
            const char* templateCC = outputTemplate.c_str();

            string samplersDeclarations = "";
            char buf[300];
            for( int i=0; i<count; i++ )
            {
                sprintf( buf, decTemplate, alive[i].c_str() );
                samplersDeclarations += buf;
                sprintf( buf, decTemplate, currentBests[i].c_str() );
                samplersDeclarations += buf;
                sprintf( buf, templateCC, alive[i].c_str(), i );
                output1 += buf;
                sprintf( buf, templateCC, currentBests[i].c_str() );
                output2 += buf;
            }

            string ifStatement = string("if( fromSurvivors == 1 ){\n") +
                                 output1 + "\n}else{\n" +
                                 output2 + "}\n";

            string fragmentProgram = samplersDeclarations + shaderTemplate +
                                     ifStatement + "}";

            delete[] alive;
            delete[] currentBests;

            return new Program( simpleVertexProgram, fragmentProgram );
        }

        Program* createDepthRangeDetectorShader( void )
        {

            const string fragmentTemplate =
                    STRINGFY
                    (

                            uniform sampler2D input_depth;
            uniform int level;

            void main()
            {
                ivec2   my_coords = ivec2( gl_FragCoord.xy );
                ivec2   up_coords = 2 * my_coords;
                vec4    up_val = vec4( 0.0f, 0.0f, 0.0f, 0.0f );

                float   min_depth = 1.0f;
                float   max_depth = 0.0f;
                bool    ok  = false;

                for( int i=0; i<4; i++ )
                {
                    up_val = texelFetch( input_depth, up_coords + offset[i], level - 1 );

                    if( up_val.z < 0.5f )
                    {
                        if( up_val.x < min_depth ) min_depth = up_val.x;
                        if( up_val.y > max_depth ) max_depth = up_val.y;
                        ok = true;
                    }
                }

                if( ok )
                {
                    gl_FragColor = vec4( min_depth, max_depth, 0.0f, 0.0f );
                }
                else
                {
                    gl_FragColor = vec4( 1.0f, 1.0f, 1.0f, 0.0f );
                }
            }

            );

            string fragmentProgram = offsets + fragmentTemplate;
            return new Program( simpleVertexProgram, fragmentProgram );
        }

        Program* createFeatureDetectorShader( void )
        {
            string nearOffsets =
                    string( "const ivec2 near[8] = {\n" ) +
                    "ivec2( -1, 0 ), ivec2( -1, 1 ), ivec2( 0, 1 ),\n" +
                    "ivec2( 1, 1 ), ivec2( 1, 0 ), ivec2( 1, -1 ),\n" +
                    "ivec2( 0, -1 ), ivec2( -1, -1 )};\n";

            string neighborOffsets =
                    string( "const ivec2 neighbours[4][2] = { \n" ) +
                    "{ ivec2( -1, 0 ), ivec2(  1,  0 ) },\n" +
                    "{ ivec2( -1, 1 ), ivec2(  1, -1 ) }," +
                    "{ ivec2(  0, 1 ), ivec2(  0, -1 ) }," +
                    "{ ivec2(  1, 1 ), ivec2( -1, -1 ) } };";


            const string shaderBody = STRINGFY(

                    uniform sampler2D startMask;
            uniform sampler2D inputDepth;
            uniform sampler2D inputNormal;
            uniform sampler2D inputEyeNormal;
            uniform float smallDepthJump;
            uniform float bigDepthJump;
            uniform float thresholdCosine;
            uniform float frontFacingCosine;

            float depths[8];
            float myDepth;
            ivec2 my_coords;

            bool neighboursTest( int pairIndex )
            {
                // compute the neighbours texel coordinates
                ivec2 n1 = my_coords + neighbours[ pairIndex ][ 0 ];
                ivec2 n2 = my_coords + neighbours[ pairIndex ][ 1 ];

                // validity test
                float maskVal = texelFetch( startMask, n1, 0 ).x;
                if( maskVal > 0.5f ){ return false; }   // background pixel
                maskVal = texelFetch( startMask, n2, 0 ).x;
                if( maskVal > 0.5f ){ return false; }   // background pixel

                // depth test

                // retrieve depth values
                float bufferedDepth = depths[ pairIndex ];
                float firstDepth = bufferedDepth;
                if( bufferedDepth < -0.5f  )
                {
                    firstDepth = texelFetch( inputDepth, n1, 0 ).x;
                }

                bufferedDepth = depths[ pairIndex + 4 ];
                float secondDepth = bufferedDepth;
                if( bufferedDepth < -0.5f  )
                {
                    secondDepth = texelFetch( inputDepth, n2, 0 ).x;
                }

                float centralDepth = myDepth;
                if( myDepth < -0.5f )
                {
                    myDepth = texelFetch( inputDepth, my_coords, 0 ).x;
                }

                bool ok = false;
                if( firstDepth < centralDepth )
                {
                    ok = ( secondDepth < centralDepth );
                }
                else
                {
                    ok = ( secondDepth >= centralDepth );
                }

                if( !ok )
                {
                    return false;
                }

                float jump = max( abs( firstDepth - centralDepth ), abs( secondDepth - centralDepth ) );
                if( !( jump <= smallDepthJump ) ){ return false; }  // depth test failed

                // normals test
                vec4 firstAtt = texelFetch( inputNormal, n1, 0 );
                vec4 secondAtt = texelFetch( inputNormal, n2, 0 );
                float angleCosine = dot( firstAtt.xyz, secondAtt.xyz );
                return ( angleCosine < thresholdCosine );
            }

            bool bigDepthJumpTest( void )
            {
                vec4 eyeNormal = texelFetch( inputEyeNormal, my_coords, 0 );
                vec3 myView = vec3( 0.0f, 0.0f, -1.0f );

                float angleCos = dot( -myView, eyeNormal.xyz );
                if( angleCos < frontFacingCosine )
                {
                    return false;
                }

                myDepth = texelFetch( inputDepth, my_coords, 0 ).x;
                bool ok = false;
                int i=0;

                while( !ok && i<8 )
                {
                    depths[i] = texelFetch( inputDepth, my_coords + near[i], 0 ).x;
                    ok = ( depths[i] - myDepth > bigDepthJump );
                    i++;
                }

                return ok;
            }

            void main()
            {
                my_coords = ivec2( gl_FragCoord.xy );
                float maskVal = texelFetch( startMask, my_coords, 0 ).x;
                if( maskVal > 0.5f )
                {   // this is a background pixel
                    gl_FragColor = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
                    return;
                }

                // initialize depth values
                for( int i=0; i<8; i++ ){ depths[i] = -1.0f; }
                myDepth = -1.0f;

                // perform feature-detection
                bool detected = bigDepthJumpTest();
                int i = 0;
                while( !detected && i < 4 )
                {
                    detected = neighboursTest( i );
                    i++;
                }

                // save the mask value
                float on = detected? 0.0f : 1.0f;
                gl_FragColor = vec4( on, on, on, 1.0f );
            }

            );

            string fragmentProgram = neighborOffsets + nearOffsets + shaderBody;
            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* --------------------------------------------------------------------------- */
    };
}

#endif // RESOURCES_H
