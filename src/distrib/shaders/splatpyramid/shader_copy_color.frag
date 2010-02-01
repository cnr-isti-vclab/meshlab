/* Copy step */

#extension GL_ARB_draw_buffers : enable

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform sampler2D textureC;

void main (void) {

  vec4 bufferA = texture2D (textureA, gl_TexCoord[0].st);
  vec4 bufferB = texture2D (textureB, gl_TexCoord[0].st);
  vec4 bufferC = texture2D (textureC, gl_TexCoord[0].st);

  gl_FragData[0] = bufferA;
  gl_FragData[1] = bufferB;
  gl_FragData[2] = bufferC;
}
