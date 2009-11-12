varying vec4 texCoord;
 
void main(void)
{
  texCoord = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_Position = ftransform();
}