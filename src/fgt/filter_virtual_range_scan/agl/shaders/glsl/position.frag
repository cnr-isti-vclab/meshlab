varying vec4 transformed_vertex;

void main()
{
    gl_FragColor = vec4( transformed_vertex.xyz, 0.0 );
}
