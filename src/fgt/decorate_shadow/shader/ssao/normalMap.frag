varying vec3 normal;
varying vec4 point;
//varying float depth;

void main(){
  float depth = point.z;// / point.w;
  depth = depth * 0.5 + 0.5;

  //gl_FragColor = vec4(normal, 1.0);
  //gl_FragColor = vec4(vec3(depth), 1.0);

  gl_FragColor = vec4(vec3(normal), depth);
}