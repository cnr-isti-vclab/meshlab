/// GLSL CODE

/// Vertex Shader -- Synthesis Shader

void main(void)
{
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_TexCoord[3] = gl_MultiTexCoord3;
  gl_Position = gl_ProjectionMatrix * gl_Vertex;
}
