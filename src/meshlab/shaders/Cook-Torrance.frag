//
// Cook-Torrance reflectance model (per-pixel)
//
// by
// Massimiliano Corsini
// Visual Computing Lab (2006)
//

varying vec3 normal;
varying vec3 vpos;

// gaussian coefficient
uniform float C = 0.8;

// index of refraction
uniform float ni = 5.0;

void main()
{
	// normalize interpolated normal
	vec3 N = normalize(normal);

	// light vector
	vec3 L = normalize(gl_LightSource[0].position.xyz - vpos);
	
	// vertex-to-eye space (view vector)
	vec3 V = normalize(-vpos);
	
	// half-vector
	vec3 H = normalize(vec3(gl_LightSource[0].halfVector));
	
	
	float NdotH = max(0.0, dot(N, H));
	float VdotH = dot(V, H);
	float NdotV = dot(N, V);
	float NdotL = dot(N, L);
	
	// D term (gaussian)
	float alpha = acos(NdotH);
	float D = C * exp(- alpha * alpha * C);
	
	// Geometric factor (G)
	float G1 = 2.0 * NdotH * NdotV / VdotH;
	float G2 = 2.0 * NdotH * NdotL / VdotH;
	float G = min(1.0, min(G1, G2));
	
	// Fresnel Refraction (F)
	float k = pow(1.0 - NdotV, 5.0);
	float F = 1.0 - k + ni * k;
	
	// diffuse color
	vec4 kd  = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;

    gl_FragColor  = kd * (F * D * G) / NdotV;
}
