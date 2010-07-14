uniform sampler2D inputMask;
uniform sampler2D inputDepth;

void main()
{
    ivec2 myCoords = ivec2( gl_FragCoord.xy );

    float maskVal = texelFetch( inputMask, myCoords, 0 ).x;
    if( maskVal < 0.5 )
    {   // this is a background pixel
        gl_FragColor = vec4( 1.0, 1.0, 1.0, 0.0 );
    }
    else
    {   // this is a valid pixel
        float depthVal = texelFetch( inputDepth, myCoords, 0 ).x;
        gl_FragColor = vec4( depthVal, depthVal, depthVal, 0.0 );
    }
}
