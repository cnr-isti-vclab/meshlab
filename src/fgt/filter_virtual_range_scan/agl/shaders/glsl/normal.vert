varying vec3 norm;

uniform int eye_space_map;

void main()
{
    if( eye_space_map == 1 )
    {
        norm = gl_NormalMatrix * gl_Normal;
    }
    else
    {
        norm = gl_Normal;
    }

    gl_Position = ftransform();
}
