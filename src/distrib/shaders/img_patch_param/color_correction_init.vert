void main()
{
    gl_Position = ftransform();
    gl_TexCoord[0] = gl_Vertex;
    gl_TexCoord[1] = gl_MultiTexCoord0;
}
