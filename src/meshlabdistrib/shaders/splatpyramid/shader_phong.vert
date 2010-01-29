/// GLSL CODE

/// Vertex Shader -- Phong Shader
void main(void)
{
   
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ProjectionMatrix * gl_Vertex;
}
