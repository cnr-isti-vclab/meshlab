
#ifndef EXPE_DEPTH_INTERPOLATION
    #define EXPE_DEPTH_INTERPOLATION 0
#endif

#ifndef EXPE_OUTPUT_DEPTH
    #define EXPE_OUTPUT_DEPTH 0
#endif

#define EXPE_DEFERRED_SHADING

// avoid an annoying bug with the nvidia driver 87XX serie.
#define epsilon 0.000001

uniform vec4 viewport;

#ifndef EXPE_DEFERRED_SHADING

uniform sampler2DRect ColorWeight;
#ifdef EXPE_OUTPUT_DEPTH
uniform sampler2DRect Depth;
#endif

void Finalization(void)
{
    vec4 color = texture2DRect(ColorWeight, gl_FragCoord.st - viewport.xy + epsilon);
    #ifdef EXPE_OUTPUT_DEPTH
    //gl_FragDepth = texture2DRect(Depth, gl_FragCoord.st + epsilon).x;
    #endif
    discard(color.w<0.01);
    gl_FragColor = color/color.w;
    gl_FragColor.a = 1.;
}

#else

vec4 meshlabLighting(vec4 color, vec3 eyePos, vec3 normal)
{
	vec3 ldir = normalize(gl_LightSource[0].position.xyz);
	return color * 0.75 * clamp(dot(normal,ldir),0.0,1.0);
}

uniform vec2 unproj;

uniform sampler2DRect ColorWeight;
uniform sampler2DRect NormalWeight;

#if ( (EXPE_DEPTH_INTERPOLATION==0) || (EXPE_OUTPUT_DEPTH==1))
uniform sampler2DRect Depth;
#endif

void Finalization(void)
{
    vec4 color = texture2DRect(ColorWeight, gl_FragCoord.st - viewport.xy + epsilon);

    discard(color.w<0.01);

    if(color.w>0.01)
        color.xyz /= color.w;

    vec3 viewVec = normalize(gl_TexCoord[0].xyz);
    vec4 normaldepth = texture2DRect(NormalWeight, gl_FragCoord.st + epsilon);

    normaldepth.xyz = normaldepth.xyz/normaldepth.w;
//     gl_FragColor.rgb = meshlabLighting(color, vec3(0,0,0), normaldepth.xyz);
//     gl_FragColor.w = 1;
#if 1
//     #ifdef EXPE_OUTPUT_DEPTH
//     gl_FragDepth = texture2DRect(Depth, gl_FragCoord.st + epsilon).x;
//     #endif

//     #if EXPE_DEPTH_INTERPOLATION==2
//         float depth = -normaldepth.z;
//     #elif EXPE_DEPTH_INTERPOLATION==1
//         float depth = unproj.y/(2.0*normaldepth.z+unproj.x-1.0);
//     #else
//         float depth = texture2DRect(Depth, gl_FragCoord.st + epsilon).x;
//         depth = unproj.y/(2.0*depth+unproj.x-1.0);
//     #endif

    vec3 normal = normaldepth;
//     #if EXPE_DEPTH_INTERPOLATION!=0
//     normal.z = sqrt(1. - dot(vec3(normal.xy,0),vec3(normal.xy,0)));
//     #endif
    normal = normalize(normal);
    //vec3 eyePos = gl_TexCoord[0].xyz*depth;
    vec3 eyePos;

    vec4 shadedColor;
//     #if defined (EXPE_LIGHTING)
//         shadedColor = expeLighting(color, eyePos, normal);
//     #else
        shadedColor = meshlabLighting(color, eyePos, normal);
//     #endif

    #ifdef EXPE_REFLEXION
    vec3 reflexionVector = expeComputeReflexionVector(viewVec, normal);
    vec3 envColor = half3(expeComputeReflectedColor(reflexionVector));
    shadedColor.xyz *= envColor;
    #endif

    gl_FragColor = shadedColor;
    gl_FragColor.a = 1.0;
#endif
}

#endif



