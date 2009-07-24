uniform mat4 mvpl;
uniform sampler2D shadowMap;
uniform float texSize;
varying vec4 shadowCoord; 


vec4 shadowCoordPostW;
float chebyshevUpperBound( float distance) {
  // We retrive the two moments previously stored (depth and depth*depth) 
  vec2 moments = texture2D(shadowMap,shadowCoordPostW.xy).rb; 
  // Surface is fully lit. as the current fragment is before the light occluder 
  if (distance <= moments.x) 
    return 1.0 ; 

  float variance = moments.y - (moments.x*moments.x); 
  //variance = max(variance,0.00002);
  variance = max(variance,0.000195);  
  float d = distance - moments.x; 
  float p_max = variance / (variance + d*d); 
  return p_max;
}

void main() {
  shadowCoordPostW = shadowCoord / shadowCoord.w;
  shadowCoordPostW = shadowCoordPostW * 0.5 + 0.5;
  float shadow = chebyshevUpperBound(shadowCoordPostW.z);

  vec4 kd = gl_LightSource[0].diffuse;
  if (shadow > 0.4)
    discard;

  gl_FragColor = vec4(vec3(0.0), 0.5 - shadow);
} 