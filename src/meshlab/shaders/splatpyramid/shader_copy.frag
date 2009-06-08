/* Copy step */
#version 120

#extension GL_ARB_draw_buffers : enable

uniform sampler2D textureA;
uniform sampler2D textureB;

uniform int level;

void main (void) {

  vec2 tex_coord = gl_TexCoord[0].st;

  vec4 bufferA = texture2DLod (textureA, tex_coord.st, float(level));
  vec4 bufferB = texture2DLod (textureB, tex_coord.st, float(level));

  gl_FragData[0] = bufferA;
  gl_FragData[1] = bufferB;
}
