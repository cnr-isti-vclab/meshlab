uniform sampler2D pyramid;
uniform int level;

const ivec2 offset[4] =
    {
        ivec2( 0, 0 ), ivec2( 1, 0 ),
        ivec2( 1, 1 ), ivec2( 0, 1 )
    };

void main()
{
    ivec2 coords = ivec2( gl_FragCoord.xy );

    ivec2 up_coords = 2 * coords;

    float val = 0.0;

    for( int i=0; i<4; i++ )
    {
        vec3 up_val = texelFetch( pyramid, up_coords + offset[i], level-1 );
        val += up_val.x;
    }

    gl_FragColor = vec4( val, 0.0, 0.0, 0.0 );
}
