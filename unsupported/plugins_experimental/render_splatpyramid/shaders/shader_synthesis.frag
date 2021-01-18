/* Synthesis step */
#version 120

#extension GL_ARB_draw_buffers : enable

// canvas_height / canvas_width
uniform float canvas_ratio;

// 0.5* size of current level / size of one level up
uniform vec2 level_ratio;

// half the pixel size of current level
uniform vec2 half_pixel_size;

uniform int level;

uniform sampler2D textureA;
uniform sampler2D textureB;

uniform float reconstruction_filter_size;
uniform float prefilter_size;
uniform float minimum_size;

uniform bool depth_test;

// tests if a point is inside an ellipse.
// Ellipse is centered at origin and point displaced by d.
// Radius is the half the ellipse's major axis.
// Minor axis is computed by normal direction.
// @param d Difference vector from center of ellipse to point.
// @param radius Ellipse major axis length * 0.5.
// @param normal Normal vector.
float pointInEllipse(in vec2 d, in float radius, in vec3 normal){
 	float len = length(normal.xy);

	if (len == 0.0)
	normal.y = 0.0;
	else
	normal.y /= len;

	// angle between normal and z direction
	float angle = acos(normal.y);
	if (normal.x > 0.0)
	angle *= -1.0;

	// scale pixel distance according to screen dimensions
	d.x *= canvas_ratio;

  	// rotate point to ellipse coordinate system
	vec2 rotated_pos = vec2(d.x*cos(angle) + d.y*sin(angle),
						  -d.x*sin(angle) + d.y*cos(angle));
	// major and minor axis
	float a = 2.0*radius;

	//  float b = a*normal.z;
	float b = a * max(pow(normal.z, prefilter_size), minimum_size);

	// include antialiasing filter (increase both axis)
	/*   a += prefilter_size; */
	/*   b += prefilter_size; */

	// inside ellipse test
	float test = ((rotated_pos.x*rotated_pos.x)/(a*a)) + ((rotated_pos.y*rotated_pos.y)/(b*b));

	if (test <= reconstruction_filter_size)
	return test;
	else return -1.0;
}


void main (void) {

	// first buffer = (n.x, n.y, n.z, weight)
	vec4 bufferA = vec4(0.0, 0.0, 0.0, 0.0);
	// second buffer = (depth, dx, dy, radius)
	vec4 bufferB = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 pixelA[4], pixelB[4];

	// retrieve pixel from analysis pyramid
	bufferA = texture2DLod (textureA, gl_TexCoord[0].st, level).xyzw;
	bufferB = texture2DLod (textureB, gl_TexCoord[0].st, level).xyzw;  

	// Occlusion test - if this pixel is far behind this position
	// one level up in the pyramid, it is synthesized since it is
	// occluded
	// (test the pixel z value with the depth range of the above pixel)
	bool occluded = false;

	if (depth_test) {
		if  (bufferA.w != 0.0) {
			vec4 up_pixelA = texture2DLod (textureA, gl_TexCoord[0].st, float(level+1)).xyzw;
			vec4 up_pixelB = texture2DLod (textureB, gl_TexCoord[0].st, float(level+1)).xyzw;

			if ( (up_pixelA.w != 0.0) && (bufferB.x > up_pixelB.x + up_pixelB.y) ) {
				occluded = true;
			}
		}
	}

  // unspecified pixel (weight == 0.0) or occluded pixel
  // synthesize pixel
  if ((bufferA.w == 0.0) || occluded)
	{		
	// first find coordinates for center of the four pixels in lower resolution level
	// the level_ratio already compensates for non power of two mipmapping with floor strategy
	vec2 center_coord = gl_TexCoord[0].st * level_ratio;

	vec2 tex_coord[4];
	//up-right
	tex_coord[0].st = center_coord + half_pixel_size.st;
	//up-left
	tex_coord[1].st = center_coord + vec2(-half_pixel_size.s, half_pixel_size.t);
	//down-right
	tex_coord[2].st = center_coord + vec2( half_pixel_size.s, -half_pixel_size.t);
	//down-left
	tex_coord[3].st = center_coord - half_pixel_size;

	vec2 dist_to_pixel;
	vec2 curr_coords = gl_TexCoord[0].st;
	float dist_test;
	float total_weight = 0.0;
	vec4 weights = vec4(0.0);
	for (int i = 0; i < 4; ++i) {
	  pixelA[i] = texture2DLod(textureA, tex_coord[i], float(level+1));

	  if (pixelA[i].w > 0.0) {
		pixelB[i] = texture2DLod(textureB, tex_coord[i], float(level+1));
	  
		dist_to_pixel = pixelB[i].zw - curr_coords;

		dist_test = pointInEllipse(dist_to_pixel, pixelA[i].w, pixelA[i].xyz);
		if (dist_test == -1)
		  pixelA[i].w = 0;
		else {
		  //		  weights[i] = 1.0 - dist_test;
		  weights[i] = exp(-0.5*dist_test);
		  total_weight += 1.0;
		}
	  }
	}

	// If the pixel was set as occluded but there is an ellipse
	// in range that does not occlude it, do not synthesize
	// Usually means that pixel is in a back surface near an internal silhouette
	if (occluded) {
	  for (int i = 0; i < 4; ++i)
		if ((bufferB.x <= pixelB[i].x + pixelB[i].y) && (weights[i] != 0.0))
		  occluded = false;
	}

	// If the pixel was set as occluded but there are no valid
	// pixels in range to synthesize, leave as it is
	if (occluded && (total_weight == 0.0))
	  occluded = false;


	if ((bufferA.w == 0.0) || occluded) 
	  {
	  bufferA = vec4(0.0);
	  bufferB = vec4(0.0);
	  total_weight = 0;
	  for (int i = 0; i < 4; ++i) {
		if (pixelA[i].w > 0.0)
		  {
			total_weight += weights[i];
			bufferA += pixelA[i] * weights[i];
			bufferB += pixelB[i] * weights[i];
		  }
	  }

	  if (total_weight > 0.0) {
		bufferA /= total_weight;
		bufferA.xyz = normalize(bufferA.xyz);
		bufferB /= total_weight;
	  }
	}
  }

  // first buffer = (n.x, n.y, n.z, radius)
  gl_FragData[0] = bufferA;
  // second buffer = (depth min, depth range, dx, dy)
  gl_FragData[1] = bufferB;
}
