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
