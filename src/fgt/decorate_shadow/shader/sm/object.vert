uniform float far;
uniform float near;
uniform float width;

varying vec3 normalVec;
varying float lightDepth;
varying vec3 viewVec;
varying vec3 lightVec;
void main(void)
{

   vec4 pPos = gl_Vertex;
  
   gl_Position = gl_ModelViewProjectionMatrix * pPos;
   gl_TexCoord[0] = gl_MultiTexCoord0;
   vec4 lightPosition = gl_ModelViewMatrix * gl_LightSource[0].position;
   // lighting is performed in eye space
   vec4 ePos = gl_ModelViewMatrix * gl_Vertex;
   vec4 eLight = gl_ModelViewMatrix * lightPosition; //lightPosition;
   vec3 eNorm = gl_NormalMatrix * gl_Normal;
   
   normalVec = eNorm;
   //lightVec = eLight.xyz - ePos.xyz;
   viewVec = /* (0,0,0) - we're in eye space */ - ePos.xyz;
    lightVec = eLight.xyz - ePos.xyz;
  //vec3 lightPosition = vec3(-1.0,0.0,0.0);
   
//Reproduce GL_LookAt behaviour
   //Reproduce GL_LookAt behaviour
   //create ortonormal basis for lightspace
  vec3 newZ = normalize(lightPosition.xyz);
   vec3 newX = normalize(cross(newZ, vec3(0.0, 1.0, 0.0)));
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
                     
  
   vec4 lightPos =  parallelViewProject
                  * toLightSpaceRot  
                  * gl_Vertex;
   lightDepth = lightPos.z;
   // glTexGen basically do:
   // T = S * Pp * Vp * M * vertex
   //
   // vertex from local to world space (M)
   // then to Light (projector) space (Vp)
   // then projected (Pp)
   // and at this point adjust to get texcoord from [-1, 1] to [0, 1] range (S)
   mat4 texAdjTrSc = mat4(
                          0.5, 0.0, 0.0, 0.0,
                          0.0, 0.5, 0.0, 0.0,
                          0.0, 0.0, 0.5, 0.0,
                          0.5, 0.5, 0.5, 1.0
                         );

   gl_TexCoord[1] = texAdjTrSc * lightPos;
   
   // we also need world space light vector
   //lightVecDist = (lightDirection.xyz - gl_Vertex.xyz) * (1.0 / zFar);
 
}