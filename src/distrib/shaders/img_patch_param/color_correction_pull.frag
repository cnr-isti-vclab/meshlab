uniform sampler2D u_TexLower;
uniform sampler2D u_TexUpper;


void main()
{
    vec4 color = texelFetch( u_TexUpper, ivec2(gl_FragCoord.xy), 0 );
    if( color.w < 0.5 )
    {
        vec2 pos = gl_FragCoord.xy / vec2(textureSize(u_TexUpper,0).xy);
        color = texture2D( u_TexLower, pos );
    }

    gl_FragColor = color;
}
