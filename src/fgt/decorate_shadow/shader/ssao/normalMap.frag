varying vec3 normal;

void main(){
  vec3 n = normalize(normal)* 0.5 + 0.5;
  //gl_FragColor = vec4(vec3(n), gl_FragCoord.z);
  gl_FragColor = vec4(vec3(n), 1.0);
}