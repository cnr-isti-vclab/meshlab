/* Analysis step */
#version 120

#extension GL_ARB_draw_buffers : enable

// flag for depth test on/off
uniform bool depth_test;

// 2.0*size of current level / size of one level down
uniform vec2 level_ratio;

// current read level
uniform int level;

uniform vec2 offset;

uniform float reconstruction_filter_size;
uniform float prefilter_size;

uniform sampler2D textureA;
uniform sampler2D textureB;


// tests if a point is inside an ellipse.
// Ellipse is centered at origin and point displaced by d.
// Radius is the half the ellipse's major axis.
// Minor axis is computed by normal direction.
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

  float cos_angle = normal.y;
  float sin_angle = sin(angle);

  // rotate point to ellipse coordinate system
  vec2 rotated_pos = vec2(d.x*cos_angle + d.y*sin_angle,
						  -d.x*sin_angle + d.y*cos_angle);

  // major and minor axis
  float a = 1.0*radius;
  float b = a*normal.z;

  // include antialiasing filter
  a += prefilter_size;
  b += prefilter_size;

  // inside ellipse test
  float test = ((rotated_pos.x*rotated_pos.x)/(a*a)) + ((rotated_pos.y*rotated_pos.y)/(b*b));

  if (test <= reconstruction_filter_size)
    return test;
  else return -1.0;
}

void main (void) {

	vec2 tex_coord[4];

	vec4 bufferA = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 bufferB = vec4(0.0, 0.0, 0.0, 0.0);

	float valid_pixels = 0.0;

	vec4 pixelA[4], pixelB[4];

	vec2 center_coord = gl_TexCoord[0].st * level_ratio;

	//up-right
	tex_coord[0].st = center_coord.st + offset.st;
	//up-left
	tex_coord[1].s = center_coord.s - offset.s;
	tex_coord[1].t = center_coord.t + offset.t;
	//down-right
	tex_coord[2].s = center_coord.s + offset.s;
	tex_coord[2].t = center_coord.t - offset.t;
	//down-left
	tex_coord[3].st = center_coord.st - offset.st;

	// Compute the front most pixel from lower level (minimum z
	// coordinate)
	float dist_test = 0.0;
	float zmin = 10000.0;
	float zmax = -10000.0;
	float obj_id = -1.0;
	for (int i = 0; i < 4; ++i) {
    pixelA[i] = texture2DLod (textureA, tex_coord[i].st, float(level-1)).xyzw;
    if (pixelA[i].w > 0.0) {
	  pixelB[i] = texture2DLod (textureB, tex_coord[i].st, float(level-1)).xyzw;	
      dist_test = pointInEllipse(pixelB[i].zw, pixelA[i].w, pixelA[i].xyz);

	  if  (dist_test != -10.0)
		{
		  // test for minimum depth coordinate of valid ellipses
		  if (pixelB[i].x <= zmin) {
			zmin = pixelB[i].x;
			zmax = zmin + pixelB[i].y;
		  }
		}
      else {
		// if the ellipse does not reach the center ignore it in the averaging
		pixelA[i].w = -1.0;
      }
    }
  }

  float new_zmax = zmax;

  // Gather pixels values
  for (int i = 0; i < 4; ++i)
    {
      // Check if valid gather pixel or unspecified (or ellipse out of reach set above)
      if (pixelA[i].w > 0.0) 
		{
	  	
		  //if (abs(pixelC[i].w - obj_id) < 0.1 )
		  {
			// Depth test between valid in reach ellipses
			if ((!depth_test) || (pixelB[i].x - pixelB[i].y <= zmax))
			  {
				float w = 1.0;
				bufferA += pixelA[i] * w;

				// Increment ellipse total path with distance from gather pixel to center
				//				bufferB.zw += (pixelB[i].zw + gather_pixel_desloc[i].xy) * w;
				bufferB.zw += pixelB[i].zw * w;
	      
				// Take maximum depth range
				new_zmax = max(pixelB[i].x + pixelB[i].y, new_zmax);
	      
				valid_pixels += w;
			  }
		  }
		}
    }

  // average values if there are any valid ellipses
  // otherwise the pixel will be writen as unspecified
  
  if (valid_pixels > 0.0)
    {
      bufferA /= valid_pixels;
      bufferA.xyz = normalize(bufferA.xyz);
      bufferB.x = zmin;
      bufferB.y = new_zmax - zmin;
      bufferB.zw /= valid_pixels;
    }

  // first buffer = (n.x, n.y, n.z, radius)
  gl_FragData[0] = bufferA;
  // second buffer = (depth, max_depth, dx, dy)
  gl_FragData[1] = bufferB;
}
