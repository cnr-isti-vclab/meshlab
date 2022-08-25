/* Synthesis step */

//#extension GL_ARB_draw_buffers : enable
#version 120

#extension GL_EXT_gpu_shader4 : enable

const float pi = 3.1416;

const float reduc_factor = 1.0;

uniform vec2 fbo_size;
uniform vec2 oo_fbo_size;
uniform vec2 oo_canvas_size;

//uniform float half_pixel_size;

// flag for depth test on/off
uniform bool depth_test;
uniform bool elliptical_weight;

uniform ivec2 displacement;
uniform int mask_size;

uniform int level;

uniform float quality_threshold;

uniform float reconstruction_filter_size;
uniform float prefilter_size;

uniform float minimum_size;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform sampler2D textureC;


// tests if a point is inside a circle.
// Circle is centered at origin, and point is
// displaced by param d.
float pointInCircle(in vec2 d, in float radius){
  float sqrt_len = d.x*d.x + d.y*d.y;

  radius *= 1.0;
  radius += prefilter_size;

  float dif = sqrt_len / (radius*radius);

  if (dif <= reconstruction_filter_size)
    return dif;
  else return -1.0;
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

  //  if (len == 0.0)
  //  if ((normal.y == 0.0) || (normal.x == 0.0))
  if (normal.z == 1.0)
    //return pointInCircle(d, radius);
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
/*   float a = radius*reconstruction_filter_size; */
/*   float b = a*normal.z; */

  float a = 2.0 * radius * reconstruction_filter_size;
  float b = a * max(pow(normal.z, prefilter_size), minimum_size);

  // include antialiasing filter (increase both axis)
/*   a += prefilter_size; */
/*   b += prefilter_size; */

  // inside ellipse test
  float test = ((rotated_pos.x*rotated_pos.x)/(a*a)) + ((rotated_pos.y*rotated_pos.y)/(b*b));

  if (test <= 1.0)
    return test;
  else return -1.0;
}

void splatEllipse(inout vec4 buffer0, inout vec4 buffer1,
				  in vec3 normal, in float r, in float ellipseZ, in float depth_interval, 
				  in vec4 color, in vec2 local_displacement) {
  // if pixel from displacement position is a projected surfel, check if current
  // pixel is inside its radius
  float dist_test = pointInEllipse(local_displacement.xy, r, normal);

  // Ellipse in range
  if (dist_test >= 0.0) {

    // weight is the gaussian exponential of the distance to the ellipse's center
    float weight;
    if (elliptical_weight)
      weight = (1.0 - dist_test)*(1.0 - dist_test);
    else
      weight = exp(-0.5*dist_test);

	weight *= (color.a*color.a);

	float accumulated_weight = length(buffer0.xyz);
    // current depth value of pixel (weighted depth / accumulated weight)
    float pixelZ = buffer1.x / accumulated_weight;

    // if the weight is zero this pixel is empty, attribute the ellipse values
    if (buffer0.w == 0.0) {
      buffer0 = vec4(normal * weight, color.a);
      buffer1 = vec4(ellipseZ * weight, color.rgb * weight);
    }
    else
      {
		// add ellipses values to current buffer if they belong to same surface		
		if ( (!depth_test) ||
			 ((abs(ellipseZ - pixelZ) <= (depth_interval))) ) {
		  /* buffer0 += vec4(normal * weight, 0.0); */
		  /* buffer1 += vec4(ellipseZ * weight, color.rgb * weight); */
		  if (abs(color.a - buffer0.w) < quality_threshold) {
			buffer0 += vec4(normal * weight, 0.0);
			buffer1 += vec4(ellipseZ * weight, color.rgb * weight);
		  }
		  else if (color.a > buffer0.w) {
			buffer0 = vec4(normal * weight, color.a);
			buffer1 = vec4(ellipseZ * weight, color.rgb * weight);
		  }
		}
		// overwrite pixel if ellipse is in front or if pixel is empty, otherwise keep current pixel
		else if ((ellipseZ < pixelZ) ) {
		  buffer0 = vec4(normal * weight, color.a);
		  buffer1 = vec4(ellipseZ * weight, color.rgb * weight);
		}
      }
  }
}

void main (void) {

  // retrieve actual pixel with current values
  vec4 buffer0 = texture2D (textureA, gl_TexCoord[0].st, 0).xyzw;
  vec4 buffer1 = texture2D (textureB, gl_TexCoord[0].st, 0).xyzw;
  vec4 ellipse0, ellipse1, ellipse2;
  vec2 local_displacement;

  // this is to make sure weight is not clamped if greater than 1.0
  //buffer0.w *= 10.0;

  if (level == 0) {
    buffer0 = vec4(0.0);
    buffer1 = vec4(0.0);
  }

  for (int j = -mask_size; j <= mask_size; ++j) {
    for (int i = -mask_size; i <= mask_size; ++i) {
      {
		{
		  local_displacement = vec2(i, j) * oo_fbo_size.st;
		  ellipse1 = texture2D (textureB, gl_TexCoord[3].st + local_displacement.xy).xyzw;

		  // if pixel is set as in current level and radius > 0
		  if ((ellipse1.y > 0.0) && (ellipse1.x > 0.0)) {
	    
			// retrieve candidate ellipse from displacement position
			ellipse0 = texture2D (textureA, gl_TexCoord[3].st + local_displacement.xy).xyzw;
			ellipse2 = texture2D (textureC, gl_TexCoord[3].st + local_displacement.xy).xyzw;
			{				
			  // displacement from current pixel and ellipse center in pixel dimension
			  vec2 local_pixel_displacement = (gl_TexCoord[0].st - ellipse1.zw) * fbo_size * oo_canvas_size;
		  
			  splatEllipse(buffer0, buffer1, ellipse0.xyz, ellipse1.y, 
						   ellipse0.w, ellipse1.x, ellipse2.rgba, local_pixel_displacement);
			}
		  }
		}
      }
    }
  }

  // to make sure weight is not clamped if greater than 1.0
  //buffer0.w *= 0.1;

  gl_FragData[0] = buffer0;
  gl_FragData[1] = buffer1;
}
