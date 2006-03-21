void main()
{
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
	vec3 LightVector = vec3(gl_LightSource[0].position);
	
	
	vec3 VertexToEye = normalize(vec3(2.0 *  gl_LightSource[0].halfVector -  gl_LightSource[0].position));
	
	float VdotN = dot(VertexToEye, normal);
	float LdotN = dot(LightVector, normal);
	float Irradiance = max(0.0, LdotN);
	
	float AngleViewNormal  = acos(VdotN);
	float AngleLightNormal = acos(LdotN);
	
	float AngleDifference = max (0.0, dot(normalize(VertexToEye - normal * VdotN), 
	                             normalize(LightVector - normal * LdotN)));
	
	float Alpha = max(AngleViewNormal, AngleLightNormal);
	float Beta  = min(AngleViewNormal, AngleLightNormal);
	
	vec4 DiffuseColor = gl_FrontMaterial.diffuse;
	
	float RoughnessSquared = 0.5;
	float A = 1.0 - (0.5 * RoughnessSquared) / (RoughnessSquared + 0.33);
	float B = (0.45 * RoughnessSquared) / (RoughnessSquared + 0.09);	
				
	gl_FrontColor  = DiffuseColor * (A + B * AngleDifference * sin(Alpha) * tan(Beta)) * Irradiance;
	
	gl_Position = ftransform();
}
