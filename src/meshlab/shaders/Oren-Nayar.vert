//
// Oren-Nayar reflectance model (per-pixel)
//
// by
// Massimiliano Corsini
// Visual Computing Lab (2006)
//

//uniform float roughnessSquared;

varying vec3 normal;
varying vec3 vpos;

void main()
{
	// vertex normal
	normal = gl_NormalMatrix * gl_Normal;

	// vertex position (in eye space)
	vpos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	gl_Position = ftransform();
}
