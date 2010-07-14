uniform sampler2D compactedDeadMap;

void main()
{
    ivec2 coords = ivec2( gl_Vertex.xy );
    vec4 deadCoords = texelFetch( compactedDeadMap, coords, 0 );
    vec4 myVertex = vec4( deadCoords.xy, -1.0, 1.0 );
    gl_Position = gl_ModelViewProjectionMatrix * myVertex;
}
