void main()
{
    float d = gl_FragCoord.z;
    gl_FragColor = vec4( d, d, d, 0.0 );
}
