//
// Minnaert reflection model (per-pixel)
//
// by
// Massimiliano Corsini 
// Visual Computing Lab (2006)
//

varying vec3 normal;
varying vec3 vpos;

uniform float m;

void main (void)
{
	vec3 N = normalize(normal);
	vec3 V = normalize(-vpos);
	vec3 L = normalize(gl_LightSource[0].position.xyz - vpos);

	// diffuse color
	vec4 kd = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse;

	// Minnaert reflection model
	float NdotV = dot(N,V);
	float NdotL = dot(N,L);

	gl_FragColor = pow(max(NdotV * NdotL, 0.0), m) * kd;
}
