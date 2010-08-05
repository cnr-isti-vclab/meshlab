#ifndef SHADERS_H
#define SHADERS_H

#include "utils.h"

#define STRINGFY(X) #X

namespace vrs
{
    // This class holds the source code of all shaders that
    // virtual range scan uses
    class Shaders
    {
    public:

        Shaders( int attributeMask )
        {
            this->attributeMask = attributeMask;
            simpleVertexProgram = "void main(){ gl_Position = ftransform(); }";
            offsets = "const ivec2 offset[4]={ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(1, 1)};";

            shadersMap[ "start_shader" ]            =   createStartShader();
            shadersMap[ "depth_fixer_shader" ]      =   createDepthFixerShader();
            shadersMap[ "copier_shader" ]           =   createCopierShader();
            shadersMap[ "level_builder_shader" ]    =   createLevelBuilderShader();

            string inputs[] = { "inputDepth", "inputEyeNormal", "inputNormal", "inputPosition" };
            shadersMap[ "input_compactor" ]         = createCompactionShader( inputs, 4 );

            shadersMap[ "cone_filter_shader" ]      =   createConeFilterShader();
            shadersMap[ "killer_shader" ]           =   createKillerShader();
            shadersMap[ "mask_updater_shader" ]     =   createMaskUpdaterShader();

            inputs[0] = "deadMap";
            shadersMap[ "dead_compactor" ]          =   createCompactionShader( inputs, 1 );
            shadersMap[ "dm_shader" ]               =   createDeadMaskerShader();
            shadersMap[ "dmneg_shader" ]            =   createDeadMaskerNegationShader();

            string bests[] = { "bestDepth", "bestEyeNormal", "bestNormal", "bestPosition" };
            shadersMap[ "alive_compactor" ]     =   createCompactionShader( bests, 4 );

            string alive[] = { "aliveDepth", "aliveEyeNormal", "aliveNormal", "alivePosition" };
            string currentBests[] = { "currentBestDepth", "currentBestEyeNormal",
                                      "currentBestNormal", "currentBestPosition" };
            shadersMap[ "final_compactor_shader" ]  =   createFinalCompactorShader( alive, currentBests, 4 );
            shadersMap[ "detector_shader" ]         =   createFeatureDetectorShader();
            shadersMap[ "depth_range_builder" ]     =   createDepthRangeDetectorShader();
            shadersMap[ "best_normal_map_shader" ]  =   createBestNormalMapper();
        }

        ~Shaders( void )
        {
            for( map< string, Program* >::iterator i = shadersMap.begin();
            i != shadersMap.end(); ++i )
            {
                delete (*i).second;
            }
            shadersMap.clear();
        }

        Program* operator[]( const string& key ){ return shadersMap[key]; }

    private:
        int attributeMask;
        map< string, Program* > shadersMap;
        string simpleVertexProgram;
        string offsets;

        /* --------------- creates the shader for attributes extraction -------------- */
        /* This shader is used in the start stage to make the attributes screenshots   */
        Program* createStartShader( void )
        {
            const string vertexProgram = STRINGFY(

                    varying vec4 pos;
            varying vec3 norm;
            varying vec3 eyeNorm;

            void main()
            {
                pos = gl_Vertex;
                norm = gl_Normal;
                eyeNorm = gl_NormalMatrix * gl_Normal;
                gl_Position = ftransform();
            }
            );

            const string fragmentProgram = STRINGFY(

                    varying vec4 pos;
            varying vec3 norm;
            varying vec3 eyeNorm;

            void main()
            {
                float d = gl_FragCoord.z;
                vec3 the_norm = normalize( norm );
                vec3 the_eye_norm = normalize( eyeNorm );

                gl_FragData[0] = vec4( d, d, d, 0.0 );
                gl_FragData[1] = vec4( pos.xyz, 0.0 );
                gl_FragData[2] = vec4( the_norm, 0.0 );
                gl_FragData[3] = vec4( the_eye_norm, 0.0 );
                gl_FragData[4] = vec4( 1.0, 0.0, 0.0, 0.0 );
            }
            );

            return new Program( vertexProgram, fragmentProgram );
        }
        /* --------------------------------------------------------------------------- */

        /* ------------------ depth fixer shader ------------------------------------- */
        /* This shader is used in the start stage to invert the clear color of the
           depth-map. Due to the OpenGL version used in Meshlab, the glClearBuffers()
           call is not available, so we have to clear the depth-map to 1.0 in a second pass. */

        Program* createDepthFixerShader( void )
        {
            const string fragmentProgram = STRINGFY(

                    uniform sampler2D startMask;
            uniform sampler2D depthBlack;

            void main()
            {
                ivec2 myCoords = ivec2( gl_FragCoord.xy );

                float maskVal = texelFetch( startMask, myCoords, 0 ).x;
                if( maskVal < 0.5 )
                {   // this is a background pixel
                    gl_FragColor = vec4( 1.0, 1.0, 1.0, 0.0 );
                }
                else
                {   // this is a valid pixel
                    float depthVal = texelFetch( depthBlack, myCoords, 0 ).x;
                    gl_FragColor = vec4( depthVal, depthVal, depthVal, 0.0 );
                }
            }
            );

            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* ---------------------------------------------------------------------------- */

        /* ------------------ copier shader ------------------------------------------- */
        // copies a texture

        Program* createCopierShader( void )
        {
            const string fragmentProgram = STRINGFY(

                    uniform sampler2D inputMap;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                vec4 texel = texelFetch( inputMap, coords, 0 );
                gl_FragColor = texel;
            }
            );

            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* ---------------------------------------------------------------------------- */

        /* -------------- level builder shader ---------------------------------------- */
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
                    val += upVal.x;
                }

                gl_FragColor = vec4( val, 0.0, 0.0, 0.0 );
            }

            );

            string fragmentProgram = offsets + fragmentTemplate;
            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* ---------------------------------------------------------------------------- */

        /* ----------------- compaction shader template ------------------------------- */
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
                        pyramid_val = texelFetch( pyramid, map_coords, i ).x;
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
        /* ---------------------------------------------------------------------------- */

        /* --------------------------- compaction shader builder ---------------------- */
        /* this method is used to istantiate a compaction shader given a string matrix:
           the input samplers */
        Program* createCompactionShader( string* inputs, int count )
        {
            string shaderTemplate = getCompactionTemplate();
            string samplersDeclarations = "", outputString = "";
            string outputTemplate = string("val = texelFetch( %s, org_map_coords, 0 );\n") +
                                    "gl_FragData[%d] = val;\n\n";
            const char* cc = outputTemplate.c_str();
            char buf[250];

            for( int i=0; i<count; i++ )
            {
                sprintf( buf, "uniform sampler2D %s;\n", inputs[i].c_str() );
                samplersDeclarations += buf;
                sprintf( buf, cc, inputs[i].c_str(), i );
                outputString += buf;
            }

            string fragmentProgram = offsets + samplersDeclarations
                                     + shaderTemplate + outputString + "}";

            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* ---------------------------------------------------------------------------- */

        /* ---------------- cone_filter_shader ---------------------------------------- */
        /* this shader takes a normal map and an input mask, and produces another mask
           with 1's where the normal is facing the observer within a given cone of
           directions */

        Program* createConeFilterShader( void )
        {
            const string fragmentProgram = STRINGFY(

                    uniform sampler2D startMask;
            uniform sampler2D inputEyeNormal;
            uniform float thresholdCosine;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                float maskVal = texelFetch( startMask, coords, 0 ).x;
                if( maskVal < 0.5 )
                {
                    gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
                }
                else
                {
                    vec3 n = texelFetch( inputEyeNormal, coords, 0 ).xyz;
                    vec3 viewDir = vec3( 0.0, 0.0, -1.0 );

                    float viewCosine = dot( n, -viewDir );
                    if( viewCosine > thresholdCosine )
                    {
                        gl_FragColor = vec4( 1.0, 0.0, 0.0, 0.0 );
                    }
                    else
                    {
                        gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
                    }
                }
            }
            );

            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* ---------------------------------------------------------------------------- */

        /* --------------------- killer shader ---------------------------------------- */
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
                vec4 vert = vec4( pos, 1.0 );
                vec3 sample_normal = texelFetch( bestNormal, tex_coords, 0 );
                sample_eye_normal = gl_NormalMatrix * sample_normal;
                gl_Position = gl_ModelViewProjectionMatrix * vert;
            }

            );

            const string fragmentProgram = STRINGFY(

                    uniform sampler2D outMask;
            uniform sampler2D eyeNormalBest;

            varying vec2    sample_coords;
            varying vec3    sample_eye_normal;

            void main()
            {
                ivec2 frag_coords = ivec2( gl_FragCoord.xy );
                float mask_val = texelFetch( outMask, frag_coords, 0 );

                if( mask_val > 0.5 )
                {
                    vec3 current_eye_normal = normalize( texelFetch( eyeNormalBest, frag_coords, 0 ) );
                    vec3 norm_sample_eye_normal = normalize( sample_eye_normal );
                    vec3 perfect_view = vec3( 0.0, 0.0, -1.0 );

                    float look_at_me_1 = dot( -perfect_view, current_eye_normal );
                    float look_at_me_2 = dot( -perfect_view, norm_sample_eye_normal );

                    if( look_at_me_1 > look_at_me_2 )
                    {   // the current view is the best
                        gl_FragData[0] = vec4( 1.0, 0.0, 0.0, 0.0 );
                        gl_FragData[1] = vec4( sample_coords, 0.0, 0.0 );
                    }
                    else
                    {   // the arriving sample is the best
                        gl_FragData[0] = vec4( 0.0, -1.0, 0.0, 0.0 );
                        gl_FragData[1] = vec4( 0.0, 0.0, 0.0, 0.0 );
                    }
                }
                else
                {
                    gl_FragData[0] = vec4( 0.0, 0.0, 0.0, 0.0 );
                    gl_FragData[1] = vec4( 0.0, 0.0, 0.0, 0.0 );
                }
            }

            );

            return new Program( vertexProgram, fragmentProgram );
        }
        /* ---------------------------------------------------------------------------- */

        /* ------------------- mask updater shader ------------------------------------ */
        Program* createMaskUpdaterShader( void )
        {
            const string fragmentProgram = STRINGFY(

                    uniform sampler2D outMask;
            uniform sampler2D killerMap;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                float val = texelFetch( outMask, coords, 0 ).x;
                if( val == 0.0 )
                {
                    gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
                }
                else
                {
                    float val2 = texelFetch( killerMap, coords, 0 ).y;
                    if( val2 == -1.0 )
                    {
                        gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
                    }
                    else
                    {
                        gl_FragColor = vec4( 1.0, 0.0, 0.0, 0.0 );
                    }
                }
            }

            );

            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* ---------------------------------------------------------------------------- */

        /* --------------- dead masker negation shader -------------------------------- */
        Program* createDeadMaskerShader( void )
        {
            const string vertexProgram = STRINGFY(

                    uniform sampler2D compactedDeadMap;

            void main()
            {
                ivec2 coords = ivec2( gl_Vertex.xy );
                vec4 deadCoords = texelFetch( compactedDeadMap, coords, 0 );
                vec4 myVertex = vec4( deadCoords.xy, -1.0, 1.0 );
                gl_Position = gl_ModelViewProjectionMatrix * myVertex;
            }

            );

            const string fragmentProgram = STRINGFY(

                    void main()
                    {
                gl_FragColor = vec4( 1.0, 0.0, 0.0, 0.0 );
            }

            );

            return new Program( vertexProgram, fragmentProgram );
        }
        /* --------------------------------------------------------------------------- */

        /* -------------- dead mask negation shader ---------------------------------- */
        Program* createDeadMaskerNegationShader( void )
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
                    gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
                }
                else
                {
                    float negVal = texelFetch( deadMask, coords, 0 ).x;
                    if( negVal > 0.5 )
                    {
                        gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
                    }
                    else
                    {
                        gl_FragColor = vec4( 1.0, 0.0, 0.0, 0.0 );
                    }
                }
            }

            );

            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* --------------------------------------------------------------------------- */

        /* ---------------- final compactor template --------------------------------- */
        string getFinalCompactorTemplate( void )
        {
            // step1: loads the fragment shader template
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
                vec4 val = vec4( 0.0, 0.0, 0.0, 0.0 );
            });

            return fsTemplate.substr( 0, fsTemplate.length() - 1 );
        }
        /* --------------------------------------------------------------------------- */

        /* ---------------- final compactor shader ----------------------------------- */
        Program* createFinalCompactorShader( string* alive, string* currentBests, int count )
        {
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

            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* --------------------------------------------------------------------------- */

        /* --------------------- feature detector shader ----------------------------- */
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
                    "{ ivec2(  0, 1 ), ivec2(  0, -1 ) }," +
                    "{ ivec2( -1, 1 ), ivec2(  1, -1 ) }," +
                    "{ ivec2( -1, -1), ivec2(  1,  1 ) } };";


            const string shaderBody = STRINGFY(

                    uniform sampler2D startMask;
            uniform sampler2D inputDepth;
            uniform sampler2D inputNormal;
            uniform sampler2D inputEyeNormal;
            uniform float smallDepthJump;
            uniform float bigDepthJump;
            uniform float thresholdCosine;
            uniform float frontFacingCosine;

            bool isValid( ivec2 pixel )
            {
                float maskVal = texelFetch( startMask, pixel, 0 ).x;
                return ( maskVal > 0.5 );
            }

            bool validTest( ivec2 first, ivec2 second )
            {
                return ( isValid( first ) && isValid( second ) );
            }

            bool smallDepthJumpTest( ivec2 first, ivec2 second )
            {
                float firstDepth = texelFetch( inputDepth, first, 0 ).x;
                float secondDepth = texelFetch( inputDepth, second, 0 ).x;
                float diff = abs( firstDepth - secondDepth );
                return ( diff < smallDepthJump );
            }

            bool attributeTest( ivec2 first, ivec2 second )
            {
                vec4 firstAtt = texelFetch( inputNormal, first, 0 );
                vec4 secondAtt = texelFetch( inputNormal, second, 0 );
                float angleCosine = dot( firstAtt.xyz, secondAtt.xyz );
                return ( angleCosine < thresholdCosine );
            }

            bool neighboursTest( ivec2 my_coords, int pairIndex )
            {
                ivec2 n1 = my_coords + neighbours[ pairIndex ][ 0 ];
                ivec2 n2 = my_coords + neighbours[ pairIndex ][ 1 ];

                if( !validTest( n1, n2 ) ) return false;
                if( !smallDepthJumpTest( n1, n2 ) ) return false;
                return ( attributeTest( n1, n2 ) );
            }

            bool bigDepthJumpTest( ivec2 my_coords )
            {
                vec4 eyeNormal = texelFetch( inputEyeNormal, my_coords, 0 );
                vec3 myView = vec3( 0.0, 0.0, -1.0 );

                float angleCos = dot( -myView, eyeNormal.xyz );
                if( angleCos < frontFacingCosine )
                {
                    return false;
                }

                float myDepth = texelFetch( inputDepth, my_coords, 0 ).x;
                bool ok = false;
                int i=0;
                float neighborDepth;

                while( !ok && i<8 )
                {
                    neighborDepth = texelFetch( inputDepth, my_coords + near[i], 0 ).x;
                    ok = ( neighborDepth - myDepth > bigDepthJump );
                    i++;
                }

                return ok;
            }

            bool isFeature( ivec2 my_coords )
            {
                bool detected = bigDepthJumpTest( my_coords );

                int i = 0;
                while( !detected && i<4 )
                {
                    detected = neighboursTest( my_coords, i );
                    i++;
                }

                return detected;
            }

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                float maskVal = texelFetch( startMask, coords, 0 ).x;
                if( maskVal < 0.5 ) discard; 	// this is not a valid pixel

                float on;

                if( isFeature( coords ) )
                {
                    on = 1.0;
                }
                else
                {
                    on = 0.0;
                }

                gl_FragColor = vec4( on, 0.0, 0.0, 0.0 );
            }

            );

            //string fragmentProgram = nearOffsets + shaderBody;
            string fragmentProgram = neighborOffsets + nearOffsets + shaderBody;
            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* --------------------------------------------------------------------------- */

        /* ---------------- depth range detector ------------------------------------- */
        Program* createDepthRangeDetectorShader( void )
        {
            const string shaderBody = STRINGFY(

                    uniform sampler2D pyramid;
            uniform sampler2D startMask;
            uniform int level;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                ivec2 up_coords = 2 * coords;
                float min = 1.0;
                float max = -1.0;
                vec4 inputMaskVal;
                vec4 pyramidVal;
                float currentVal;

                for( int i=0; i<4; i++ )
                {
                    if( level == 0 )
                    {
                        inputMaskVal = texelFetch( startMask, up_coords + offset[i], 0 );

                        if( inputMaskVal.x > 0.5 )
                        {	// inputMask equal to 1 => valid pixel => update bounds
                            pyramidVal = texelFetch( pyramid, up_coords + offset[i], 0 );

                            currentVal = pyramidVal.x;
                            if( currentVal < min )
                            {
                                min = currentVal;
                            }

                            if( currentVal > max && pyramidVal.y < 1.0 )
                            {
                                max = currentVal;
                            }
                        }
                    }
                    else
                    {
                        pyramidVal = texelFetch( pyramid, up_coords + offset[i], level-1 );

                        if( pyramidVal.y > 0.0 )
                        {	// valid pixel
                            if( pyramidVal.x < min )
                            {
                                min = pyramidVal.x;
                            }

                            if( pyramidVal.y > max && pyramidVal.y < 1.0 )
                            {
                                max = pyramidVal.y;
                            }
                        }
                    }
                }

                gl_FragColor = vec4( min, max, 0.0, 0.0 );
            }
            );

            string fragmentProgram = offsets + shaderBody;
            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* --------------------------------------------------------------------------- */

        /* ------------------ "best" normal-map -------------------------------------- */
        Program* createBestNormalMapper( void )
        {
            string nearString = string( "const ivec2 kernel[9] = {" ) +
                                "ivec2(0, 0), ivec2(-1, 0), ivec2(-1, 1)," +
                                "ivec2(0, 1), ivec2(1, 1), ivec2(1, 0)," +
                                "ivec2(1, -1), ivec2(0, -1), ivec2(-1, -1)};";

            const string shaderBody = STRINGFY(

                    uniform sampler2D inputEyeNormal;
            uniform sampler2D startMask;

            void main()
            {
                ivec2 coords = ivec2( gl_FragCoord.xy );
                vec4 val = vec4( 0.0, 0.0, 0.0, 0.0 );
                float bestCosine = -2.0;
                vec3 viewDir = vec3( 0.0, 0.0, -1.0 );
                float currentCosine = 0.0;
                vec3 bestNormal = vec3( 0.0, 0.0, 0.0 );

                for( int i=0; i<9; i++ )
                {
                    val = texelFetch( startMask, coords + kernel[i], 0 );
                    if( val.x > 0.5 )
                    {
                        val = texelFetch( inputEyeNormal, coords + kernel[i], 0 );
                        currentCosine = dot( -viewDir, val.xyz );
                        if( currentCosine > bestCosine )
                        {
                            bestCosine = currentCosine;
                            bestNormal = val.xyz;
                        }
                    }
                }

                if( bestCosine < -1.0 )
                {
                    gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
                }
                else
                {
                    gl_FragColor = vec4( bestNormal, 0.0 );
                }
            }

            );

            string fragmentProgram = nearString + shaderBody;
            return new Program( simpleVertexProgram, fragmentProgram );
        }
        /* --------------------------------------------------------------------------- */

    };
}

#endif // SHADERS_H
