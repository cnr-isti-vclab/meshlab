
//
// glass.frag: Fragment shader for doing a glass-like effect
//
// author: John Kessenich
//
// Copyright (c) 2002: 3Dlabs, Inc.
//

varying vec3 LightDir;
varying vec3 EyeDir;
varying vec3 Normal;

uniform vec4 GlassColor;
uniform vec4 SpecularColor1;
uniform vec4 SpecularColor2;
uniform float SpecularFactor1;
uniform float SpecularFactor2;

void main (void)
{
    vec4 color;

    vec3 reflectDir = -reflect(LightDir, Normal);
    
    //
    // Uncomment this line for a cool solid look.
    // Comment it out for light shining off of flat glass.
    //
 reflectDir = reflectDir;
    
    float spec = max(dot(EyeDir, reflectDir), 0.0);

    //
    // Compute two rings of specular effect.
    //
    spec = spec * spec;

    //
    // Basic glass color + first specular effect
    //
    color = GlassColor + SpecularFactor1 * spec * SpecularColor1;

    //
    // Second specular effect
    //
    spec = pow(spec, 8.0) * SpecularFactor2;
    color += spec * SpecularColor2;
    
    gl_FragColor = min(color, vec4(1.0));
}
