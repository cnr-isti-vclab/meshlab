uniform sampler2D u_TexColor;
uniform sampler2D u_TexCorrection;


void main()
{
    ivec2 pos = ivec2( gl_FragCoord.xy );

    vec4 color = texelFetch( u_TexColor, pos, 0 );
    color += texelFetch( u_TexCorrection, pos, 0 );
    
    gl_FragColor = vec4( clamp(color.xyz,0.0,1.0), 1.0 );
}
