//varying vec2  uv;
varying vec4 texCoord;
 
void main(void)
{
  gl_Position = ftransform();
  texCoord = gl_Position;
//  gl_Position = mvpl * gl_Vertex;
  //gl_Position = sign( gl_Position );
  
  // Texture coordinate for screen aligned (in correct range):
 // uv = (vec2( gl_Position.x, gl_Position.y ) + vec2( 1.0 ) ) * 0.5;
  //uv = vec2( gl_Position.x, gl_Position.y )+ vec2( 1.0 ) * 0.5;
}