//
// Fragment shader for environment mapping with an
// equirectangular 2D texture
//
// Authors: John Kessenich, Randi Rost
//
// Copyright (c) 2002-2004 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

const vec3 Xunitvec = vec3 (1.0, 0.0, 0.0);
const vec3 Yunitvec = vec3 (0.0, 1.0, 0.0);

uniform vec3  BaseColor;
uniform float MixRatio;

uniform sampler2D EnvMap;

varying vec3  Normal;
varying vec3  EyeDir;
//varying float LightIntensity;

void main (void)
{
    // Compute reflection vector   
    vec3 reflectDir = reflect(EyeDir, Normal);

//---- this is the original code (buggy):
//     // Compute altitude and azimuth angles
// 
//     index.y = dot(normalize(reflectDir), Yunitvec);
//     reflectDir.y = 0.0;
//     index.x = dot(normalize(reflectDir), Xunitvec) * 0.5;
// 
//     // Translate index values into proper range
// 
//     if (reflectDir.z >= 0.0)
//         index = (index + 1.0) * 0.5;
//     else
//     {
//         index.t = (index.t + 1.0) * 0.5;
//         index.s = (-index.s) * 0.5 + 1.0;
//     }
//     
//     // if reflectDir.z >= 0.0, s will go from 0.25 to 0.75
//     // if reflectDir.z <  0.0, s will go from 0.75 to 1.25, and
//     // that's OK, because we've set the texture to wrap.
//---- end of the original code

    // this is the version implemented from the OpenGL-1.2 programming guide:
    reflectDir.z += 1.0;
    float inv_m = 0.5/sqrt(dot(reflectDir,reflectDir));
    vec2 index = reflectDir.xy * inv_m + 0.5;
  
  
    // Do a lookup into the environment map.

    vec4 envColor = vec4 (texture2D(EnvMap, index));

    // Add lighting to base color and mix

    //vec3 base = LightIntensity * BaseColor;
    envColor = mix(envColor, gl_Color, MixRatio);

    gl_FragColor =envColor;
}
