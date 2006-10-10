//
// Bui Tuong Phong shading model (per-pixel) 
// 
// by 
// Massimiliano Corsini
// Visual Computing Lab (2006)
// 

varying vec3 normal;
varying vec3 vpos;

void main()
{	
	vec3 n, lightDir, rVector;
	vec4 diffuse = {0.0};
	vec4 specular = {0.0};

	// vertex normal
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	// vertex position
	vpos = vec3(gl_ModelViewMatrix * gl_Vertex);
	
	// vertex position
	gl_Position = ftransform();
}
