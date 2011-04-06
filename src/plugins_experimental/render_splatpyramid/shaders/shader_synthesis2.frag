/* Synthesis step */
#version 120

#extension GL_ARB_draw_buffers : enable

// flag for depth test on/off
uniform bool depth_test;
uniform bool elliptical_weight;

uniform vec2 canvas_size;
uniform vec2 half_pixel_size;

uniform int level;

uniform float reconstruction_filter_size;
uniform float prefilter_size;

// textures on finer level
uniform sampler2D textureA;
uniform sampler2D textureB;

// tests if a point is inside a circle.
// Circle is centered at origin, and point is
// displaced by param d.
float pointInCircle(in vec2 d, in float radius){
  float sqrt_len = d.x*d.x + d.y*d.y;

  radius *= 2.0;
  radius += prefilter_size;

  float dif = sqrt_len / (radius*radius);

  if (dif <= reconstruction_filter_size)
    return dif;
  else return -1.0;
}

/**
 * Compute the intersection of an ellipse (axis oriented) and a line
 * segment.
 * Obtained from http://www.kevlindev.com/
 * @param p Center of ellipse
 * @param rx Major axis of ellipse
 * @param ry Minor axis of ellipse
 * @param a1 Point of line segment
 * @param a2 Point of line segment
 * @return 0 if no intersection, 1 if segment intersects ellipse, 2 if
 * segment is contained inside the ellipse
 **/
int intersectEllipseLine (in vec2 p, in float rx, in float ry, in vec2 a1, in vec2 a2) {
  vec2 origin = a1;
  vec2 dir = a2 - a1;
  vec2 center = p;
  vec2 diff = origin - center;
  vec2 mDir = vec2(dir.x/(rx*rx), dir.y/(ry*ry));
  vec2 mDiff = vec2(diff.x/(rx*rx), diff.y/(ry*ry));

  float a = dot(dir, mDir);
  float b = dot(dir, mDiff);
  float c = dot(diff, mDiff) - 1.0;
  float d = b*b - a*c;

  if (d < 0.0)
    return 0;

  if ( d > 0.0 ) {
    float root = sqrt(d);
    float t_a = (-b - root) / a;
    float t_b = (-b + root) / a;
    if ( ((t_a < 0.0) || (1.0 < t_a)) && ((t_b < 0.0) || (1.0 < t_b)) ) {
      if ( ((t_a < 0.0) && (t_b < 0.0)) || ((t_a > 1.0) && (t_b > 1.0)) )
		return 0;
      else
		return 2;
    }
    else
      return 1;
  } 
  else {
    float t = -b/a;
    if ( (0.0 <= t) && (t <= 1.0) )
      return 1;
    else
      return 0;
  }
}

/**
 * Intersection between a pixel's box and an ellipse.
 * @param pixel Given pixel.
 * @param point Center of pixel.
 * @param unit Half the size of a pixel, orthogonal distance from
 * @param center to boundaries of pixel.
 * @return 1 if ellipse intersects or is inside pixel, 0 otherwise.
 **/
float intersectEllipsePixel (in vec2 d, in float radius, in vec3 normal, in float unit){

  vec2 center = vec2(0.0, 0.0);

  // rotate point to ellipse's coordinate system
  vec2 desloc_point = d;
			
  // check if ellipse center is inside box
  if (((center[0] >= desloc_point[0] - unit) && (center[0] <= desloc_point[0] + unit)) &&
      ((center[1] >= desloc_point[1] - unit) && (center[1] <= desloc_point[1] + unit)))
    return length(d);

  // projected normal length
  float len = length(normal.xy);
  normal.y /= len;

  // ellipse rotation angle
  float angle = acos(normal.y);
  if (normal.x > 0.0)
    angle *= -1.0;

  // major and minor axis
  float a = 2.0*radius*reconstruction_filter_size;
  float b = a*normal.z;

  // include antialiasing filter
  a += prefilter_size;
  b += prefilter_size;

  // rotated pixel box to match ellipse coordinate system
  // box order = | 2  3 |
  //             | 0  1 |
  float cos_angle = cos(angle);
  float sin_angle = sin(angle);

  vec2 rot_box[4]; 
  rot_box[0] = vec2((desloc_point[0] - unit)*cos_angle + (desloc_point[1] - unit)*sin_angle,
					-(desloc_point[0] - unit)*sin_angle + (desloc_point[1] - unit)*cos_angle);

  rot_box[1] = vec2((desloc_point[0] + unit)*cos_angle + (desloc_point[1] - unit)*sin_angle,
					-(desloc_point[0] + unit)*sin_angle + (desloc_point[1] - unit)*cos_angle);

  rot_box[2] = vec2((desloc_point[0] - unit)*cos_angle + (desloc_point[1] + unit)*sin_angle,
					-(desloc_point[0] - unit)*sin_angle + (desloc_point[1] + unit)*cos_angle);
  
  rot_box[3] = vec2((desloc_point[0] + unit)*cos_angle + (desloc_point[1] + unit)*sin_angle,
					-(desloc_point[0] + unit)*sin_angle + (desloc_point[1] + unit)*cos_angle);

  // ellipse intersects the pixels box
  if (((intersectEllipseLine(center, a, b, rot_box[0], rot_box[1]) > 0) ||
       (intersectEllipseLine(center, a, b, rot_box[2], rot_box[0]) > 0) ||
       (intersectEllipseLine(center, a, b, rot_box[3], rot_box[1]) > 0) ||
       (intersectEllipseLine(center, a, b, rot_box[3], rot_box[2]) > 0)))
    return length(d);

  // ellipse totally outside pixel without intersection
  return -1.0;
}

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

  // rotate point to ellipse coordinate system
  vec2 rotated_pos = vec2(d.x*cos(angle) + d.y*sin(angle),
						  -d.x*sin(angle) + d.y*cos(angle));

  // major and minor axis
  float a = 2.0*radius;
  float b = a*normal.z;

  // include antialiasing filter (increase both axis)
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

  // first buffer = (n.x, n.y, n.z, weight)
  vec4 bufferA = vec4(0.0, 0.0, 0.0, 0.0);
  // second buffer = (depth, dx, dy, radius)
  vec4 bufferB = vec4(0.0, 0.0, 0.0, 0.0);
  vec4 pixelA[4], pixelB[4];

  // retrieve pixel from analysis pyramid
  bufferA = texture2DLod (textureA, gl_TexCoord[0].st, level).xyzw;
  bufferB = texture2DLod (textureB, gl_TexCoord[0].st, level).xyzw;  

  // Occlusion test - if this pixel is far behind this position
  // one level up in the pyramid it is synthesized since it is
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

	  vec2 curr_coords = gl_TexCoord[0].st * canvas_size;

	  // size of current level in pixels
	  vec2 s = canvas_size / pow(2.0, float(level));
	  
	  // first find coordinates for center of the four pixels in lower resolution level
	  vec2 center_coord = (ceil((gl_TexCoord[0].st * s) - vec2(0.5))) * (2.0/s);	 

      //up-right
      tex_coord[0].st = center_coord + half_pixel_size.st;
      //up-left
      tex_coord[1].st = center_coord + vec2(-half_pixel_size.s, half_pixel_size.t);
      //down-right
      tex_coord[2].st = center_coord + vec2( half_pixel_size.s, -half_pixel_size.t);
      //down-left
      tex_coord[3].st = center_coord - half_pixel_size;

      // lookup four pixels on coarser level (color attachment 0)
      pixelA[0] = texture2DLod(textureA, tex_coord[0], float(level+1)); // up-right
      pixelA[1] = texture2DLod(textureA, tex_coord[1], float(level+1)); // up-left
      pixelA[2] = texture2DLod(textureA, tex_coord[2], float(level+1)); // down-right
      pixelA[3] = texture2DLod(textureA, tex_coord[3], float(level+1)); // down-left

      // lookup four pixels on coarser level (color attachment 2)
      pixelB[0] = texture2DLod(textureB, tex_coord[0], float(level+1)); // up-right
      pixelB[1] = texture2DLod(textureB, tex_coord[1], float(level+1)); // up-left
      pixelB[2] = texture2DLod(textureB, tex_coord[2], float(level+1)); // down-right
	  pixelB[3] = texture2DLod(textureB, tex_coord[3], float(level+1)); // down-left

      float total_weight = 0.0;
      float dist_test;
      float zmin = 10000.0;
      float zmax = -zmin;

	  vec4 weights = vec4(0.0);

      // For each scatter pixel
      for (int i = 0; i < 4; ++i) {

      	// distance from synthesized pixel to center of scatter pixel
/* 		vec2 dist_to_pixel = vec2(float(pixel_config[i*2]), float(pixel_config[i*2 + 1])) * half_pixel_size; */

/* 		// Add distance to center of ellipse */
/* 		pixelB[i].zw += dist_to_pixel; */

		vec2 dist_to_pixel = pixelB[i].zw - curr_coords;

		// if specified scatter pixel test distance to center of ellipse
		if (pixelA[i].w > 0.0)
		  dist_test = pointInEllipse(pixelB[i].zw, pixelA[i].w, pixelA[i].xyz);
		//dist_test = intersectEllipsePixel (pixelB[i].zw, pixelA[i].w, pixelA[i].xyz, half_pixel_size);
		//dist_test = pointInCircle(pixelB[i].zw, pixelA[i].w);
		else
		  dist_test = 1.0;
 	
		// if not specified or out of range dont use it
		if ((pixelA[i].w == 0.0) || (dist_test == -1.0)) {
		  weights[i] = 0.0;
		}
		else {
		  weights[i] = 1.0;
/* 		  if (elliptical_weight) */
/* 			weights[i] = 1.0 - dist_test; */
/* 		  else */
/* 			weights[i] = exp(-0.5*dist_test); */

		  total_weight += 1.0;

		  // depth test only for ellises in range
/* 		  if (pixelB[i].x < zmin) { */
/* 			zmin = pixelB[i].x; */
/* 			zmax = pixelB[i].y; */
/* 		  } */
		}
      }

      // If the pixel was set as occluded but there is an ellipse
      // in range that does not occlude it, do not synthesize
	  // Usually means that pixel is in a back surface near an internal silhouette
/*       if (occluded) { */
/* 		for (int i = 0; i < 4; ++i) */
/* 		  if ((bufferB.x <= pixelB[i].x + pixelB[i].y) && (weights[i] != 0.0)) */
/* 			occluded = false; */
/*       } */

/*       // If the pixel was set as occluded but there are no valid */
/*       // pixels in range to synthesize, leave as it is */
/*       if (occluded && (total_weight == 0.0)) */
/* 		occluded = false; */

      // synthesize pixel
      if ((bufferA.w == 0.0) || occluded) {
		bufferA = vec4(0.0);
		bufferB = vec4(0.0);
		total_weight = 0.0;
	
		for (int i = 0; i < 4; ++i) {

		  // Ellipse in range
		  if (weights[i] > 0.0)
			{
			  // Depth test between ellipses in range
			  //if ((!depth_test) || (pixelB[i].x <= zmin + zmax))
			  if ((!depth_test) || (pixelB[i].x - pixelB[i].y <= zmin + zmax))
				{
				  total_weight += weights[i];
				  bufferA += weights[i] * pixelA[i];
				  bufferB += weights[i] * pixelB[i];
				}
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
