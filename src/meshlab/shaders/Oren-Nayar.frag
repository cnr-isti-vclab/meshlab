//
// Oren-Nayar reflectance model (per-pixel)
//
// by 
// Massimiliano Corsini
// Visual Computing Laboratory (2006)
//

varying vec3 normal;
varying vec3 vpos;

// surface roughness
uniform float roughnessSquared;

void main()
{
	// normalize interpolated normal
	vec3 N = normalize(normal);

	// light vector
	vec3 L = normalize(gl_LightSource[0].position.xyz - vpos);
	
	// view vector
	vec3 V = normalize(-vpos);
	
	// ambient term
	vec4 ambient = gl_FrontMaterial.ambient * gl_LightModel.ambient;
	
	// diffuse color
	vec4 kd = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;	
	
	// Oren-Nayar model
	float A = 1.0 - (0.5 * roughnessSquared) / (roughnessSquared + 0.33);
	float B = (0.45 * roughnessSquared) / (roughnessSquared + 0.09);	
	
	float VdotN = dot(V, N);
	float LdotN = dot(L, N);
	float irradiance = max(0.0, LdotN);
	
	float angleViewNormal  = acos(VdotN);
	float angleLightNormal = acos(LdotN);
	
	// max( 0.0 , cos(phi_incident, phi_reflected) )
	float angleDiff = max(0.0, dot(normalize(V - N * VdotN),
		normalize(L - N * LdotN)));
	
	float alpha = max(angleViewNormal, angleLightNormal);
	float beta  = min(angleViewNormal, angleLightNormal);
				
	// final color
	gl_FragColor  = kd * irradiance * 
		(A + B * angleDiff * sin(alpha) * tan(beta));
}
