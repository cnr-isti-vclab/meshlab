uniform sampler2D negationMask;
uniform int elements;
uniform int texSide;

void main()
{
    ivec2 coords = ivec2( gl_FragCoord.xy );
    int key_index = coords.y * texSide + coords.x;

    if( key_index >= elements ) discard;

    float negVal = texelFetch( negationMask, coords, 0 ).x;
    if( negVal == 1.0 ) discard;

    gl_FragColor = vec4( 1.0, 0.0, 0.0, 0.0 );
}
