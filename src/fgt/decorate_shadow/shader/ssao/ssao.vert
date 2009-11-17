varying vec4 texCoord;
 
void main(void)
{
  texCoord = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_Position = ftransform();
}