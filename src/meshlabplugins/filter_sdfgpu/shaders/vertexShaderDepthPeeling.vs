varying vec3 N;


void main(void)
{

    
   N = gl_Normal;

   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;


}
