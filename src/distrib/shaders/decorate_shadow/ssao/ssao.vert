void main(void)
{
  gl_TexCoord[0] = gl_Vertex * 0.5 + 0.5;
  gl_Position = gl_Vertex;
}