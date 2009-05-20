
//
// reflexion_lines.vert: Fragment shader for rendering with reflexion lines
//
// author: Gael Guennebaud
//

varying vec3 EyePos;
varying vec3 Normal;

uniform float ScaleFactor;

void main (void)
{
    vec4 color;

    vec3 reflexionVector;
    vec3 n = normalize(Normal);
    vec3 viewVec = normalize(EyePos);
    reflexionVector = viewVec.xyz - 2.0*n*dot(n,viewVec.xyz);
    reflexionVector.z += 1.0;
    reflexionVector.z = dot(reflexionVector,reflexionVector);
    reflexionVector.xy = (0.5*reflexionVector.xy/reflexionVector.z) + 0.5;
    reflexionVector *= 2.0;
    color = (fract(reflexionVector.x*ScaleFactor) < 0.5 ? vec4( 1.0) : vec4( 0.1));

//     color.xyz = viewVec;
    color.a = 1.0;
    gl_FragColor = min(color, vec4(1.0,1.0,1.0,1.0));
}
