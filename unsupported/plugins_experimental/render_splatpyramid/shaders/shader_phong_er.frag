#version 120

uniform sampler2D textureA;
uniform sampler2D textureB;

uniform vec4 color_ambient;
uniform vec4 color_diffuse;
uniform vec4 color_specular;
uniform float shininess;

void main (void) {

  vec4 normal = texture2D (textureA, gl_TexCoord[0].st).xyzw;
  vec4 color = texture2D (textureB, gl_TexCoord[0].st).yzwx;
  //  gl_FragDepth = abs(color.a / normal.a);
  //  gl_FragDepth = 0.0;
  float depth = color.a / length(normal.xyz);
  if (depth <= 0.0)
	discard;

  if (normal.a != 0.0) {	
	
	color.a = normal.a;
	normal.a = length(normal.xyz);

	color.rgb /= normal.a;
 	normal.rgb = normalize(normal.rgb);	

	// Quality map
	if (shininess == 99.0) {
	  color.a *= normal.b;
	  color.rgb = vec3(color.a, 1.0-color.a, 0.0);
	}
	// Normal map
	else if (shininess == 98.0) {
	  color.rgb = normal.rgb; 
	}
	else if (shininess != 90.0) {
	  vec3 lightVec = normalize(gl_LightSource[0].position.xyz);
	  vec3 halfVec = gl_LightSource[0].halfVector.xyz; // normalize( lightVec - normalize(eyePos) );
	  float aux_dot = dot(normal.xyz, lightVec);
	  float diffuseCoeff = clamp(aux_dot, 0.0, 1.0);
	  float specularCoeff = aux_dot>0.0 ? clamp(pow(clamp(dot(halfVec, normal.xyz),0.0,1.0), gl_FrontMaterial.shininess), 0.0, 1.0) : 0.0;
	  color = vec4(color.rgb * ( gl_FrontLightProduct[0].ambient.rgb + diffuseCoeff * gl_FrontLightProduct[0].diffuse.rgb) + specularCoeff * gl_FrontLightProduct[0].specular.rgb, 1.0);


/* 	  vec3 lightDir = normalize(vec3(gl_LightSource[0].position)); */
	  
/* 	  color = color * (gl_LightSource[0].ambient + gl_LightModel.ambient); */

/* 	  float NdotL = max(dot(normal.xyz, lightDir.xyz), 0.0); */

/* 	  if (NdotL > 0.0) { */
/* 		color += color * gl_LightSource[0].diffuse * NdotL; */
/* 		float NdotHV = max(dot(normal.xyz, gl_LightSource[0].halfVector.xyz), 0.0); */
/* 		color += color * gl_LightSource[0].specular * pow(NdotHV, 1.0); */
/* 	  } */
	}
	else {
	  vec3 lightDir = normalize(vec3(gl_LightSource[0].position));
	  
	  color += color_ambient * (gl_LightSource[0].ambient + gl_LightModel.ambient);

	  float NdotL = max(dot(normal.xyz, lightDir.xyz), 0.0);

	  if (NdotL > 0.0) {
		color += color_diffuse * gl_LightSource[0].diffuse * NdotL;
		float NdotHV = max(dot(normal.xyz, gl_LightSource[0].halfVector.xyz), 0.0);
		color += color_specular * gl_LightSource[0].specular * pow(NdotHV, shininess);
	  }
	}
    color.a = 1.0;
  }
  else
	discard;
  //    color = vec4(1.0, 1.0, 1.0, 0.0);
  
  gl_FragColor = color;
  //gl_FragDepth = depth;
}
