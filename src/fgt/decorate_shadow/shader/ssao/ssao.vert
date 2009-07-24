uniform mat4 mvpl;

varying vec2  uv;
 
void main(void)
{
  //gl_Position = ftransform();
  gl_Position = mvpl * gl_Vertex;
  gl_Position = sign( gl_Position );
  
  // Texture coordinate for screen aligned (in correct range):
  uv = (vec2( gl_Position.x, gl_Position.y ) + vec2( 1.0 ) ) * 0.5;
}