
void main(void)
{
  gl_TexCoord[0] = gl_MultiTexCoord0;
  //gl_TexCoord[1] = gl_MultiTexCoord1;
  //gl_TexCoord[3] = gl_MultiTexCoord2;
  gl_TexCoord[3] = gl_MultiTexCoord3;

  gl_Position = gl_ProjectionMatrix * gl_Vertex;
  //gl_Position = ftransform();
}
