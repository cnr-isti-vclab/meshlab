//varying vec2  uv;
varying vec4 texCoord;
 
void main(void)
{
  //gl_TexCoord[1] = gl_MultiTexCoord1;
  //gl_TexCoord[2].xy = gl_MultiTexCoord2.xy;
  //texCoord = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_Position = ftransform();
  texCoord = gl_Position;
  texCoord = (texCoord / texCoord.w)* 0.5 + 0.5;
  

  
//  gl_Position = mvpl * gl_Vertex;
  //gl_Position = sign( gl_Position );
  
  // Texture coordinate for screen aligned (in correct range):
  //uv = (vec2( gl_Position.x, gl_Position.y ) + vec2( 1.0 ) ) * 0.5;
  //uv = vec2( gl_Position.x, gl_Position.y )+ vec2( 1.0 ) * 0.5;
}