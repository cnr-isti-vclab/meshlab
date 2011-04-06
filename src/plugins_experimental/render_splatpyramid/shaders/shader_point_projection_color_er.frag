/// GLSL CODE

/// 1st Fragment Shader

// Projects points to screen space and rotates normal
// stores output on texture
#extension GL_ARB_draw_buffers : enable

uniform vec2 oo_fbo_size;
uniform float scale;
//uniform vec2 canvas_border;
//uniform float min_size;

uniform float reconstruction_filter_size;
uniform float canvas_width;
uniform int mask_size;

varying vec3 normal_vec;
varying vec3 radius_depth_w;
varying float dist_to_eye;

void main(void)
{
  if (radius_depth_w.x <= 0.0)
	discard;

  vec2 texCoord = vec2( floor(gl_FragCoord.st)*oo_fbo_size );

  float radius = radius_depth_w.x * scale / dist_to_eye;
  float unprojected_radius = radius;

  int log_level = int(floor(log2( ( 2.0 * radius * reconstruction_filter_size * canvas_width ) / float(mask_size*2 + 1) )));

  //  if (radius > min_size)
  if (log_level > 0)
	unprojected_radius *= -1.0;	

/*   if ((2.0*radius*(1.0/canvas_width)*reconstruction_filter_size) > float(mask_size*2 + 1)) */
/* 	unprojected_radius *= -1.0; */

  gl_FragData[0] = vec4 ( normalize(normal_vec), radius_depth_w.y );
  //  gl_FragData[0] = vec4 ( normalize(normal_vec), gl_FragCoord.z);
  
  gl_FragData[1] = vec4 ( 4.0*unprojected_radius, radius, texCoord.st );
  gl_FragData[2] = gl_Color;

  // avoids sample with low quality overwritting sample with high quality in same pixel
  // during depth test
  //gl_FragDepth = gl_FragCoord.z - gl_Color.a*0.05;  
}
