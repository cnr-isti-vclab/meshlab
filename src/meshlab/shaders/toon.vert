varying vec3 Normal;
varying vec3 LightDir;

void main(void)
{
	Normal = normalize(gl_NormalMatrix * gl_Normal);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	LightDir=vec3(gl_LightSource[0].position);
}
