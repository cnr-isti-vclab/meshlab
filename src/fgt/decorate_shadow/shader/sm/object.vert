//uniform mat4 mvpl;
varying vec4 pPos;
void main(void)
{

   pPos = gl_Vertex;
  
   //gl_Position = gl_ModelViewProjectionMatrix * pPos;
    gl_Position = ftransform();
}