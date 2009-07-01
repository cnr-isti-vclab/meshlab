uniform float far;
uniform float near;
uniform float width;


void main(void)
{

  //vec3 lightPosition = vec3(-1.0,0.0,0.0);
   vec4 lightPosition = gl_ModelViewMatrix * gl_LightSource[0].position;
//Reproduce GL_LookAt behaviour
   //create ortonormal basis for lightspace
  vec3 newZ = normalize(lightPosition .xyz);
   vec3 newX = normalize(cross(newZ, vec3(0.0, 0.80001, 0.0)));
   vec3 newY = normalize(cross(newX, newZ));
  

    mat4 toLightSpaceRot = mat4(
                        newX.x, newY.x, newZ.x,  0.0, 
                        newX.y, newY.y, newZ.y,  0.0,
                        newX.z, newY.z, newZ.z,  0.0,
                        0.0, 0.0, 0.0,  1.0
                       );
   
     
               
//Perform parallel Ortographic projection
   float F = -near; //distance of Front clipping plane from VRP measured along VPN (a.k.a. lightDir) 
   float B = far;//distance of Back clipping plane from VRP measured along VPN (a.k.a. lightDir) 
   float umin = -width/2.0;
   float umax = width/2.0;
   float vmin = -width/2.0;
   float vmax = width/2.0;
      
                  
   mat4 parallelViewProject = mat4(
                        2.0/(umax-umin), 0.0, 0.0, 0.0,
                        0.0, 2.0/(vmax-vmin), 0.0, 0.0,
                        0.0, 0.0, 2.0/(B - F), 0.0,
                       -(umax+ umin)/2.0, -(vmax+vmin)/2.0, -(F+B)/2.0, 1.0
                       );
                     
  
   gl_Position =  parallelViewProject
                  * toLightSpaceRot  
                  * gl_Vertex;
                    
}