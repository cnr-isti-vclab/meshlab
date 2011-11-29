varying vec4 m_Vert;


void main()
{
    gl_Position = ftransform();
    m_Vert = gl_Vertex;
}
