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
	vec3 n = normalize(normal);
	vec4 diffuse = {0.0};
	vec4 specular = {0.0};
	
	// ambient term
	vec4 ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	// diffuse color
	vec4 kd = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	
	// specular color
	vec4 ks = gl_FrontMaterial.specular * gl_LightSource[0].specular;
	
	// diffuse term
	vec3 lightDir = normalize(gl_LightSource[0].position.xyz - vpos);
	float NdotL = dot(n, lightDir);
	
	if (NdotL > 0.0)
		diffuse = kd * NdotL;
	
	// specular term
	vec3 rVector = normalize(2.0 * n * dot(n, lightDir) - lightDir);
	vec3 viewVector = normalize(-vpos);
	float RdotV = dot(rVector, viewVector);
	
	if (RdotV > 0.0)
		specular = ks * pow(RdotV, gl_FrontMaterial.shininess);

	gl_FragColor = ambient + diffuse + specular;
} 
