uniform sampler2D   u_Tex;
uniform int         u_Radius;
vec2                c_PixelSize = 1.0 / vec2(textureSize(u_Tex,0));


vec3 fetch( int texUnit, int x, int y )
{
    return texture2D( u_Tex, gl_TexCoord[texUnit].xy + c_PixelSize.x * vec2(x,y) ).xyz;
}


void main()
{
    vec3 finalColor = vec3(0.0);

    for( int y=-u_Radius; y<=u_Radius; ++y )
        for( int x=-u_Radius; x<=u_Radius; ++x )
        {
            vec3 c1 = fetch( 0, x, y );
            vec3 c2 = fetch( 1, x, y );
            finalColor += 0.5*( c2 - c1 );
        }
    
    int diameter = 2*u_Radius + 1;
    finalColor /= diameter * diameter;

    gl_FragColor = vec4( finalColor, 1.0 );
}
