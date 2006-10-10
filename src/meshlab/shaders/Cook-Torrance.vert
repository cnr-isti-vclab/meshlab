//
// Cook-Torrance reflectance model (per-pixel)
//
// by
// Massimiliano Corsini
// Visual Computing Lab (2006)
//

varying vec3 normal;
varying vec3 vpos;

void main()
{
	// vertex normal 
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	// vertex position (in eye space)
	vpos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	gl_Position = ftransform();

} 
