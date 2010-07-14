varying vec3 eye_normal;

void main()
{
    eye_normal = gl_NormalMatrix * gl_Normal;
    gl_Position = ftransform();
}
