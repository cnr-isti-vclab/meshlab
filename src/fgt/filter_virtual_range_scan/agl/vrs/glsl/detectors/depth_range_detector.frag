uniform sampler2D pyramid;
uniform sampler2D inputMask;
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
    float min = 1.0;
	float max = -1.0;
	vec4 inputMaskVal;
	vec4 pyramidVal;
	float currentVal;
	
    for( int i=0; i<4; i++ )
    {
		if( level == 0 )
		{
			inputMaskVal = texelFetch( inputMask, up_coords + offset[i], 0 );
			
			if( inputMaskVal.x > 0.5 )
			{	// inputMask equal to 1 => valid pixel => update bounds
				pyramidVal = texelFetch( pyramid, up_coords + offset[i], 0 );
				
				currentVal = pyramidVal.x;
				if( currentVal < min )
				{
					min = currentVal;
				}
				
				if( currentVal > max )
				{
					max = currentVal;
				}
			}
		}
		else
		{
			pyramidVal = texelFetch( pyramid, up_coords + offset[i], level-1 );
			
			if( pyramidVal.y > 0.0 )
			{	// valid pixel
				if( pyramidVal.x < min )
				{
					min = pyramidVal.x;
				}
				
				if( pyramidVal.y > max )
				{
					max = pyramidVal.y;
				}
			}
		}
    }

    gl_FragColor = vec4( min, max, 0.0, 0.0 );
}
