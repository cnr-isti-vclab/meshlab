uniform vec3 color;
varying vec3 eye_normal;

void main()
{
    vec3 light_dir = vec3( 0.0, 0.0, -1.0 );
    vec3 light_color = color * dot( -light_dir, normalize( eye_normal ) );
    gl_FragColor = vec4( light_color, 0.0 );
}
