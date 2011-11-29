uniform sampler2D u_Tex;


void main()
{
    ivec2 pos = ivec2( gl_FragCoord.xy );

    vec4 avg = vec4( 0.0 );
    for( int y=0; y<2; ++y )
        for( int x=0; x<2; ++x )
            avg += texelFetch( u_Tex, 2*pos+ivec2(x,y), 0 );

    if( avg.w < 0.5 )
        gl_FragColor = vec4( 0.0 );
    else
        gl_FragColor = vec4( avg.xyz/avg.w, 1.0 );
}
