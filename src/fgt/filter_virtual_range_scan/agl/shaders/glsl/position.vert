uniform vec4    bbox_center;
uniform float   bbox_diag;
uniform int     normalize_vertex;

varying vec4    transformed_vertex;

void main()
{
    if( normalize_vertex == 1 )
    {
        transformed_vertex = ( ( gl_Vertex - bbox_center ) / bbox_diag ) * 0.5 + 0.5;
    }
    else
    {
        transformed_vertex = gl_Vertex;
    }

    gl_Position = ftransform();
}
