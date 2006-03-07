uniform float frequency;
uniform float edgew;            // width of smooth step

varying vec3  ObjPos;               // object space coord (noisy)
varying float V;                    // generic varying
varying float LightIntensity;


void main()
{
    float dp       = length(vec2(dFdx(V), dFdy(V)));
    float logdp    = -log2(dp * 8.0);
    float ilogdp   = floor(logdp);
    float stripes  = exp2(ilogdp);

    float sawtooth = fract((V ) * frequency * stripes);
    float triangle = abs(2.0 * sawtooth - 1.0);

    // adjust line width
    float transition = logdp - ilogdp;

    // taper ends
    triangle = abs((1.0 + transition) * triangle - transition);


    float edge0  = clamp(LightIntensity - edgew, 0.0, 1.0);
    float edge1  = clamp(LightIntensity, 0.0, 1.0);
    float square = 1.0 - smoothstep(edge0, edge1, triangle);

// uncomment this line to get the original color instead of white
//   gl_FragColor = gl_Color * vec4(vec3(square), 1.0);

   gl_FragColor =  vec4(vec3(square), 1.0);
}
