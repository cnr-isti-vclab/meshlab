// Bui Tuong Phong shading model (per-vertex) 
// 
// by 
// Massimiliano Corsini
// Visual Computing Lab (2006)
// 

void main()
{	
	vec3 n, lightDir, rVector;
	vec4 ambient = {0.0};
	vec4 diffuse = {0.0};
	vec4 specular = {0.0};

	// vertex normal
	n = normalize(gl_NormalMatrix * gl_Normal);
	
	// ambient term
	ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;

	// diffuse term
	vec3 vpos = vec3(gl_ModelViewMatrix * gl_Vertex);
	lightDir = normalize(gl_LightSource[0].position.xyz - vpos);
	vec4 kd = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	float NdotL = dot(n, lightDir);
	
	if (NdotL > 0.0)
		diffuse = kd * NdotL;
	
	// specular term
	rVector = normalize(2.0 * n * dot(n, lightDir) - lightDir);
	vec4 ks = gl_FrontMaterial.specular * gl_LightSource[0].specular;
	vec3 viewVector = normalize(-vpos);
	float RdotV = dot(rVector, viewVector);
	
	if (RdotV > 0.0)
		specular = ks * pow(RdotV, gl_FrontMaterial.shininess);
	
	// final color
	gl_FrontColor = ambient + diffuse + specular;
	
	// vertex position
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
