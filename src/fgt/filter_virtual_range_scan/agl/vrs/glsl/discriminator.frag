uniform sampler2D featureMap;
uniform sampler2D inputMask;
uniform float threshold;

void main()
{
    ivec2 coords = ivec2( gl_FragCoord.xy );

    vec4 val = texelFetch( inputMask, coords, 0 );
    if( val.x == 0.0 )
    {
        gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
    }
    else
    {
        val = texelFetch( featureMap, coords, 0 );

        if( val.x >= threshold )
        {
            gl_FragColor = vec4( 1.0, 0.0, 0.0, 0.0 );
        }
        else
        {
            gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
        }
    }
}
