/* Analysis step */

#extension GL_ARB_draw_buffers : enable
//#version 120

const float PI = 3.1416;

// flag for depth test on/off
uniform bool depth_test;

// one over 2 * fbo size
uniform vec2 oo_2fbo_size;

uniform vec2 fbo_size;
uniform vec2 oo_canvas_size;

uniform int mask_size;

// size of half a pixel
uniform float canvas_width;
uniform int level;

uniform bool quality_per_vertex;
uniform float quality_threshold;

uniform float reconstruction_filter_size;
uniform float prefilter_size;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform sampler2D textureC;

/* vec2 gather_pixel_desloc[4] = vec2[4](vec2(-half_pixel_size, -half_pixel_size), */
/* 				      vec2(half_pixel_size, -half_pixel_size), */
/* 				      vec2(-half_pixel_size, half_pixel_size), */
/* 				      vec2(half_pixel_size, half_pixel_size)); */

// tests if a point is inside a circle.
// Circle is centered at origin, and point is
// displaced by param d.
float pointInCircle(in vec2 d, in float radius){
  float sqrt_len = d.x*d.x + d.y*d.y;

  radius += prefilter_size;

  float dif = sqrt_len / (radius*radius);

  if (dif <= reconstruction_filter_size)
    //if (dif <= 1.0)
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
    return 1.0;

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
    return 1.0;

  // ellipse totally outside pixel without intersection
  return -1.0;
}

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
  vec4 bufferC = vec4(0.0, 0.0, 0.0, 0.0);

  float valid_pixels = 0.0;

  vec4 pixelA[4], pixelB[4], pixelC[4];

  //up-right
  tex_coord[0].st = gl_TexCoord[0].st + oo_2fbo_size.st;
  //up-left
  tex_coord[1].s = gl_TexCoord[0].s - oo_2fbo_size.s;
  tex_coord[1].t = gl_TexCoord[0].t + oo_2fbo_size.t;
  //down-right
  tex_coord[2].s = gl_TexCoord[0].s + oo_2fbo_size.s;
  tex_coord[2].t = gl_TexCoord[0].t - oo_2fbo_size.t;
  //down-left
  tex_coord[3].st = gl_TexCoord[0].st - oo_2fbo_size.st;

  // Gather pixels values
  for (int i = 0; i < 4; ++i) {
    pixelA[i] = texture2D (textureA, tex_coord[i].st).xyzw;
    pixelB[i] = texture2D (textureB, tex_coord[i].st).xyzw;
    pixelC[i] = texture2D (textureC, tex_coord[i].st).xyzw;
  }

  // Compute the front most pixel from lower level (minimum z
  // coordinate)
  float dist_test = 0.0;
  float zmin = 10000.0;
  float zmax = -10000.0;
  float obj_id = -1.0;
  for (int i = 0; i < 4; ++i) {
    // Valid pixel : radius > 0
    if (pixelB[i].y > 0.0) {

      // this ellipse has stop propagating, it lies on the finer level
      // the sign of the unprojected radius determines if it has reached the correct level (positive) or not (negative)
      if (pixelB[i].x < 0.0) {
		// test for minimum depth coordinate of valid ellipses
		if (pixelA[i].w <= zmin) {
		//if ( ((quality_per_vertex) && (pixelC[i].w > obj_id)) || ((!quality_per_vertex) && (pixelA[i].w <= zmin)) ) {
		  zmin = pixelA[i].w;
		  zmax = abs(pixelB[i].x);
		  obj_id = pixelC[i].w; //only necessary if using color buffer for quality
		}
      }
      else {
		// if ellipse not in correct level ignore it during average
		pixelB[i].y = -1.0;
      }
    }
  }

  // Gather pixels values
  for (int i = 0; i < 4; ++i)
    {
      // Check if valid gather pixel or unspecified (or ellipse out of reach set above)
      if (pixelB[i].y > 0.0) 
		{
		  if ((!quality_per_vertex) || (abs(pixelC[i].w - obj_id) <= quality_threshold) )
			{
			  // Depth test between valid in reach ellipses
			  // if ((!depth_test) || (pixelB[i].x - pixelC[i].y <= zmax))
			  if ((!depth_test) || (abs(zmin - pixelA[i].w) <= (zmax+pixelB[i].x)))
				{
				  //float w = abs(4.0 * PI * 4.0 * pixelA[i].w * pixelA[i].w * pixelA[i].z);
				  float w = 1.0;//pixelC[i].a;
				  bufferA.xyz += pixelA[i].xyz * w;

				  // radius computation
				  bufferB.x = max(abs(bufferB.x), abs(pixelB[i].x));
				  bufferB.y = max(bufferB.y, pixelB[i].y);

				  // average depth
				  bufferA.w += pixelA[i].w * w;

				  // average tex coords
				  bufferB.zw += pixelB[i].zw * w;

				  // average tex coords
				  bufferC += pixelC[i] * w;
	      
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
      bufferB.zw /= valid_pixels;
      bufferC.rgb /= valid_pixels;
      bufferC.w = obj_id;

      // If this ellipse is on the correct pyramid level stop its propagation
      // i.e., it will not be used in the average of the next coarser level	  

      float log_level = log2( ( 2.0 * bufferB.y * reconstruction_filter_size * canvas_width ) / float(mask_size*2 + 1) );
      if (level == int( floor(log_level) ))
		bufferB.x = abs(bufferB.x);
      else
		bufferB.x = abs(bufferB.x) * -1.0;

/* 	  if ((2.0*bufferB.y*canvas_width*reconstruction_filter_size) > float(mask_size*2 + 1)) */
/* 		bufferB.x = abs(bufferB.x); */
/*       else */
/* 		bufferB.x = abs(bufferB.x) * -1.0; */
    }

  // first buffer = (n.x, n.y, n.z, radius)
  gl_FragData[0] = bufferA;
  // second buffer = (depth, max_depth, dx, dy)
  gl_FragData[1] = bufferB;
  // color value = (r, g, b, obj_id)
  gl_FragData[2] = bufferC;
}
