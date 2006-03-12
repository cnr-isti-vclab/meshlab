void main()
{	
	vec3 normal, lightDir, viewVector, rVector;
	vec4 diffuse, ambient, globalAmbient, specular = vec4(0.0);
	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	lightDir = normalize(vec3(gl_LightSource[0].position));
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	rVector = normalize(2.0 * normal * dot(normal, lightDir) - lightDir);
	specular = gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(dot(rVector, viewVector), gl_FrontMaterial.shininess);

	gl_FrontColor = globalAmbient + dot(normal, lightDir) * diffuse + ambient + specular;
	
	gl_Position = ftransform();
} 
