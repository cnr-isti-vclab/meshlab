varying vec3 normal;

void main(){
    normal = (gl_Normal + 1.0) / 2.0;
    gl_Position = ftransform();
}
