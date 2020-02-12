varying vec4 point;

void main(){
  gl_Position = ftransform();
  point = gl_Position;
}