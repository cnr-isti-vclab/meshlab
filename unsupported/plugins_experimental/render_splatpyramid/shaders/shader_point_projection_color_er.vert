// GLSL CODE

/// 1st Vertex Shader

// Projects points to screen space and rotates normal
// stores output on texture

uniform vec3 eye;
uniform int back_face_culling;

varying vec3 normal_vec;
varying vec3 radius_depth_w;
varying float dist_to_eye;

void main(void)
{
  
  //vec3 e = (gl_ModelViewProjectionMatrixInverse * vec4(eye, 1.0)).xyz;

  //if ( (back_face_culling == 1) && (dot(normalize(gl_Vertex.xyz - eye), gl_Normal) < 0.0 )) {
  if ( (back_face_culling == 1) && (dot(normalize(eye - gl_Vertex.xyz), gl_Normal) < -0.1 )) {

    radius_depth_w.x = 0.0;
    
    // for some reason seting the vector to vec4(0.0) drops
    // the performance significantly, at least on the GeForce8800 -- RM 2007-10-19
    gl_Position = vec4(1.0);
  }
  else
    {
      // only rotate point and normal if not culled
      vec4 v = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
            
      normal_vec = normalize(gl_NormalMatrix * gl_Normal);
      float radius = gl_Vertex.w;

	  //	  dist_to_eye = length(eye - gl_Vertex.xyz);
	  dist_to_eye = -(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0)).z;

	  radius_depth_w = vec3(radius, -(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0)).z, v.w);
      //radius_depth_w = vec3(radius, v.z, v.w);
      
      gl_Position = v;
    }
  gl_FrontColor = gl_Color;
}
