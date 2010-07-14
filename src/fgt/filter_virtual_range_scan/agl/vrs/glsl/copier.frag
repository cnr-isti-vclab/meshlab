uniform sampler2D inputMap;

void main()
{
	ivec2 coords = ivec2( gl_FragCoord.xy );
	vec4 val = texelFetch( inputMap, coords, 0 );
	gl_FragColor = val;
}