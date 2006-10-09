// Bui Tuong Phong shading model (per-vertex) 
// 
// by 
// Massimiliano Corsini
// Visual Computing Lab (2006)
// 

void main()
{	
	vec3 n, lightDir, rVector;
	vec4 ambient, diffuse, specular;

	// vertex normal
	n = normalize(gl_NormalMatrix * gl_Normal);
	
	// ambient term
	ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	// diffuse term
	vec3 vpos = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = normalize(gl_LightSource[0].position.xyz - vpos);
	vec4 kd = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	diffuse = max(dot(lightDir, n), 0.0) * kd;
	
	// specular term
	vec3 viewVector = -vpos;
	rVector = reflect(lightDir, n);
	specular = gl_FrontMaterial.specular * max(pow(dot(rVector, viewVector), gl_FrontMaterial.shininess), 0.0); 
	
	gl_FrontColor = ambient + diffuse + specular;
	
	// vertex position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
