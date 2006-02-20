// Application to vertex shader
varying vec3 N;
varying vec3 I;
varying vec4 Cs;
varying vec3 LightDir;

void main()
{
    vec4 P = gl_ModelViewMatrix * gl_Vertex;
    float nDotVP;         // normal . light direction

    N=gl_NormalMatrix * gl_Normal;    
    LightDir=vec3(gl_LightSource[0].position);

//    I  = P.xyz - vec3 (0);
    I  = P.xyz;
    
    Cs = gl_Color;
    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
} 
