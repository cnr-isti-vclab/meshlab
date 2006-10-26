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
uniform float C;

// index of refraction
uniform float ni;

void main()
{
	// the material propertise are embedded in the shader (for now)
	vec4 mat_ambient = {1.0, 1.0, 1.0, 1.0};
	vec4 mat_diffuse = {1.0, 1.0, 1.0, 1.0};
	vec4 mat_specular = {1.0, 1.0, 1.0, 1.0};

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
	
	// ambient
	vec4 ambient = mat_ambient * gl_LightSource[0].ambient;
	
	// diffuse color
	vec4 kd  = mat_diffuse * gl_LightSource[0].diffuse;

    gl_FragColor  = ambient + kd * (F * D * G) / NdotV;
}
