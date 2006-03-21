void main()
{
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 LightVector = vec3(gl_LightSource[0].position);
	vec3 VertexToEye = normalize(vec3(2.0 *  gl_LightSource[0].halfVector -  gl_LightSource[0].position));
	vec3 HalfVector  = vec3(gl_LightSource[0].halfVector);
	float  NdotH    = max(0.0, dot(normal, HalfVector));
	
	vec3 RoughnessParams = vec3(0.5, 0.5, 0.5);
	
	//Start the "D" term, use Blinn Gaussian
	float Alpha = acos(NdotH);
	float C     = RoughnessParams.x;
	float m     = RoughnessParams.y;
	float D    = C * exp(-(pow(Alpha / m, 2.0)));
	
	//Start the "G" term
	float NdotV = dot(normal, VertexToEye);
	float VdotH = dot(HalfVector, VertexToEye);
	float NdotL = dot(LightVector, normal);
	float G1 = 2.0 * NdotH * NdotV /  NdotH;
	float G2 = 2.0 * NdotH * NdotL /  NdotH;
	float G = min(1.0, max(0.0, min(G1, G2)));
	
	//Start the fresnel term. Use the approximation from 
	//http://developer.nvidia.com/docs/IO/3035/ATT/FresnelReflection.pdf
	float R0 = RoughnessParams.z;
	float F = R0 + (1.0 - R0) * pow(1.0 - NdotL, 5.0);
	
	vec4 DiffuseColor  = gl_FrontMaterial.diffuse;

    gl_FrontColor  = DiffuseColor * F * D * G / (NdotL * NdotV);
	gl_Position = ftransform();

} 
