//uniform float far;
//uniform float near;
uniform float width;
uniform vec3 meshCenter;

/*uniform mat4 modelViewMX;
uniform mat4 prjMX;
*/
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
   float nearP = - (width / 2.0);//meshCenter.z - (width / 2.0); //distance of Front clipping plane from VRP measured along VPN (a.k.a. lightDir) 
   float farP = (width / 2.0);//meshCenter.z + (width / 2.0);//distance of Back clipping plane from VRP measured along VPN (a.k.a. lightDir) 
   float leftP = meshCenter.x - width/2.0;
   float rightP = meshCenter.x + width/2.0;
   float bottomP = meshCenter.y - width/2.0;
   float topP = meshCenter.y + width/2.0;
      
                  
   mat4 parallelViewProject = mat4(
                        2.0/(rightP-leftP), 0.0, 0.0, 0.0,
                        0.0, 2.0/(topP-bottomP), 0.0, 0.0,
                        0.0, 0.0, 2.0/(farP - nearP), 0.0,
                       -(rightP + leftP)/(rightP - leftP), -(topP + bottomP)/(topP-bottomP), -(farP + nearP)/(farP - nearP), 1.0
                       );
                     

 gl_Position =  parallelViewProject
                  * toLightSpaceRot 
                  * gl_Vertex;
                    
}

/*void main(){
  gl_Position = ftransform();
}*/