uniform sampler2D float_map;
uniform sampler2D inputMask;

const vec2 offset[9] =
    {
        vec2( -1, -1 ), vec2( 0, -1 ), vec2( 1, -1 ),
        vec2( -1,  0 ), vec2( 0,  0 ), vec2( 1,  0 ),
        vec2( -1,  1 ), vec2( 0,  1 ), vec2( 1,  1 )
    };

uniform ivec4 viewport;

void main()
{
    float val[9];
    vec2 dxy = vec2( 1.0/viewport[2], 1.0/viewport[3] );

    for( int i=0; i<9; i++ )
    {
        val[i] = texture2D( float_map,
                            ( gl_FragCoord.xy + offset[i] ) * dxy ).x;
        if( val[i] == 1.0 )
        {
            discard;
        }

    }

    float gx = val[0] + 2 * val[1] + val[2] - val[6] - 2 * val[7] - val[8];
    float gy = val[0] + 2 * val[3] + val[6] - val[2] - 2 * val[5] - val[8];
    float abs_grad = gx * gx + gy * gy;
    //float result = sqrt( abs_grad );
    float result = 1 - sqrt( abs_grad );

    gl_FragColor = vec4( result, result, result, 0.0 );
}
