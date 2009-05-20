
//
// reflexion_lines.vert: Vertex shader for rendering with reflexion lines
//
// author: Gael Guennebaud
//

varying vec3 EyePos;
varying vec3 Normal;
void main(void)
{
  //gl_Position = ftransform();
  gl_Position  =  gl_ModelViewProjectionMatrix * gl_Vertex;
  Normal = normalize(vec3(gl_NormalMatrix * gl_Normal));
  vec4 position = gl_ModelViewMatrix * gl_Vertex;
  EyePos = position.xyz;
}
