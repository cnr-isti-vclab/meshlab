varying vec3 norm;

void main()
{
    vec3 the_normal = normalize( norm ) * 0.5 + 0.5;
    gl_FragColor = vec4( the_normal, 0.0 );
}
