// GLSL CODE

/// 1st Vertex Shader

// Projects points to screen space and rotates normal
// stores output on texture

uniform vec3 eye;
uniform int back_face_culling;

varying vec3 normal_vec;
varying vec3 radius_depth_w;
varying float dist_to_eye;

//varying vec2 pos;

void main(void)
{  

  float dot = (dot(normalize(eye - gl_Vertex.xyz), gl_Normal));

  if ( (back_face_culling == 1) && ((dot < -0.0 ))) {

    radius_depth_w.x = 0.0;
    
    // for some reason seting the vector to vec4(0.0) drops
    // the performance significantly, at least on the GeForce8800 -- RM 2007-10-19
    gl_Position = vec4(1.0);
  }
  else {
	// only rotate point and normal if not culled
	vec4 v = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);           

	normal_vec = normalize(gl_NormalMatrix * gl_Normal);
	float radius = gl_Vertex.w;

	dist_to_eye = length(eye - gl_Vertex.xyz);

	// compute depth value without projection matrix, only modelview
	radius_depth_w = vec3(radius, -(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0)).z, v.w);
      
	//~ pos = vec2 (v.x, v.y) / v.w;
	//~ pos = (pos * 0.5) + vec2(0.5);

	gl_Position = v;
  }
}
