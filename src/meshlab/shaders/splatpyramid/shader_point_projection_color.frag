/// GLSL CODE

/// 1st Fragment Shader

// Projects points to screen space and rotates normal
// stores output on texture

#extension GL_ARB_draw_buffers : enable

uniform float scale;
uniform vec2 canvas_size;

varying vec3 normal_vec;
varying vec3 radius_depth_w;
varying float dist_to_eye;

void main(void)
{ 
	if (radius_depth_w.x <= 0.0)
	discard;

	float proj_radius = radius_depth_w.x * scale / dist_to_eye;
	float depth_interval = proj_radius;
	
	vec2 screen_pos = (vec2(gl_FragCoord.xy) - vec2(0.5)) / canvas_size.xy;

	// First buffer  : normal.x, normal.y, normal.z, radius
	// Second buffer : minimum depth, depth interval, center.x, center.y
	// Third buffer  : color
	gl_FragData[0] = vec4 (normalize(normal_vec), proj_radius );
	gl_FragData[1] = vec4 (radius_depth_w.y, 2.0*depth_interval, screen_pos);
	gl_FragData[2] = gl_Color;

	// avoids point with low quality overwritting point with high quality
	// during depth test
	//gl_FragDepth = gl_FragCoord.z - gl_Color.a*0.05;
}
