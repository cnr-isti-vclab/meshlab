/*uniform float farPlane;
uniform float nearPlane;
uniform float diag;*/

varying vec3 normal;
varying vec4 point;
//varying float depth;

void main(){
    normal = normalize(gl_NormalMatrix * gl_Normal);
    
    gl_Position = ftransform();
    point = gl_Position;
    
    //depth = gl_Position.z;
    //depth = (gl_Position.z + nearPlane) / farPlane;
    //depth = (gl_Position.z/ farPlane) + nearPlane;
    //depth = (gl_Position.z * (nearPlane * farPlane/ nearPlane + farPlane))// + (nearPlane);// + nearPlane);
    //depth = gl_Position / diag;
}
