uniform sampler2D inputMask;
uniform sampler2D inputDepth;
uniform sampler2D inputNormal;
uniform sampler2D inputEyeNormal;
uniform float smallDepthJump;
uniform float bigDepthJump;
uniform float thresholdCosine;
uniform float frontFacingCosine;

const ivec2 neighbours[4][2] = 
	{
		{ ivec2( -1, 0 ), ivec2(  1,  0 ) },
		{ ivec2(  0, 1 ), ivec2(  0, -1 ) },
		{ ivec2( -1, 1 ), ivec2(  1, -1 ) },
		{ ivec2( -1, -1), ivec2(  1,  1 ) }
	};

const ivec2 near[8] = 
	{
		ivec2( -1, 0 ), ivec2( -1, 1 ), ivec2( 0, 1 ),
		ivec2( 1, 1 ), ivec2( 1, 0 ), ivec2( 1, -1 ),
		ivec2( 0, -1 ), ivec2( -1, -1 )
	};

bool isValid( ivec2 pixel )
{
	float maskVal = texelFetch( inputMask, pixel, 0 ).x;
	return ( maskVal > 0.5 );
}

bool validTest( ivec2 first, ivec2 second )
{
	return ( isValid( first ) && isValid( second ) );
}

bool smallDepthJumpTest( ivec2 first, ivec2 second )
{
	float firstDepth = texelFetch( inputDepth, first, 0 ).x;
	float secondDepth = texelFetch( inputDepth, second, 0 ).x;
	float diff = abs( firstDepth - secondDepth );
	return ( diff < smallDepthJump );
}

bool attributeTest( ivec2 first, ivec2 second )
{
	vec4 firstAtt = texelFetch( inputNormal, first, 0 );
	vec4 secondAtt = texelFetch( inputNormal, second, 0 );
	float angleCosine = dot( firstAtt.xyz, secondAtt.xyz );
	return ( angleCosine < thresholdCosine );
}

bool neighboursTest( ivec2 my_coords, int pairIndex )
{
    ivec2 n1 = my_coords + neighbours[ pairIndex ][ 0 ];
	ivec2 n2 = my_coords + neighbours[ pairIndex ][ 1 ];
	
	if( !validTest( n1, n2 ) ) return false;
	if( !smallDepthJumpTest( n1, n2 ) ) return false;
	return ( attributeTest( n1, n2 ) );
}

bool bigDepthJumpTest( ivec2 my_coords )
{
	vec4 eyeNormal = texelFetch( inputEyeNormal, my_coords, 0 );
	vec3 myView = vec3( 0.0, 0.0, -1.0 );
	
	float angleCos = dot( -myView, eyeNormal.xyz );
	if( angleCos < frontFacingCosine )
	{
		return false;
	}
	
	float myDepth = texelFetch( inputDepth, my_coords, 0 ).x;
	bool ok = false;
	int i=0;
	float neighborDepth;
	
	while( !ok && i<8 )
	{
		neighborDepth = texelFetch( inputDepth, my_coords + near[i], 0 ).x;
		ok = ( neighborDepth - myDepth > bigDepthJump );
		i++;
	}
	
	return ok;
}

bool isFeature( ivec2 my_coords )
{
	bool detected = bigDepthJumpTest( my_coords );

	int i = 0;
	while( !detected && i<4 )
	{
		detected = neighboursTest( my_coords, i );
		i++;
	}
	
	return detected;
}

void main()
{
	ivec2 coords = ivec2( gl_FragCoord.xy );
	float maskVal = texelFetch( inputMask, coords, 0 ).x;
	if( maskVal < 0.5 ) discard; 	// this is not a valid pixel
	
	float on;
	
	if( isFeature( coords ) )
	{
		on = 1.0;
	}
	else
	{
		on = 0.0;
	}
	
	gl_FragColor = vec4( on, 0.0, 0.0, 0.0 );
}