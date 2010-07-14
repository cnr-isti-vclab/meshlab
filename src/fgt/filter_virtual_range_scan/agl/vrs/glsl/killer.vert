uniform sampler2D bestPosition;
uniform sampler2D bestNormal;

varying vec2    sample_coords;
varying vec3    sample_eye_normal;

void main()
{
    sample_coords = gl_Vertex.xy;
    ivec2 tex_coords = ivec2( sample_coords );
    vec3 pos = texelFetch( bestPosition, tex_coords, 0 );
    vec4 vert = vec4( pos, 1.0 );
    vec3 sample_normal = texelFetch( bestNormal, tex_coords, 0 );
    sample_eye_normal = gl_NormalMatrix * sample_normal;
    gl_Position = gl_ModelViewProjectionMatrix * vert;
}
