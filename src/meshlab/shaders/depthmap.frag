//
// Depth Map
//
// by 
// Massimiliano Corsini
// Visual Computing Lab (2007)


varying vec4 vpos;

uniform float zmin;
uniform float zmax;

void main()
{
	vec4 v = vec4(vpos);
	v /= v.w;
	float gray = (v.z - zmin) / (zmax - zmin);
	gl_FragColor = vec4(gray);
}
