// 
// Bui Tuong Phong shading model (per-pixel) 
// 
// by 
// Massimiliano Corsini
// Visual Computing Lab (2006)
//

varying vec3 normal;
varying vec3 vpos;

uniform float shininess;

void main()
{
	vec3 n = normalize(normal);
	vec4 diffuse = vec4(0.0);
	vec4 specular = vec4(0.0);
	
	// the material properties are embedded in the shader (for now)
	vec4 mat_ambient = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 mat_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 mat_specular = vec4(1.0, 1.0, 1.0, 1.0);
	
	// ambient term
	vec4 ambient = mat_ambient * gl_LightSource[0].ambient;
	
	// diffuse color
	vec4 kd = mat_diffuse * gl_LightSource[0].diffuse;
	
	// specular color
	vec4 ks = mat_specular * gl_LightSource[0].specular;
	
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
		specular = ks * pow(RdotV, shininess);

	gl_FragColor = ambient + diffuse + specular;
} 
