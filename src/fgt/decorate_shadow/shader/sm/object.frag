uniform sampler2D shadowMap;
uniform float width;
varying vec3 normalVec;
varying float lightDepth; //current frag distance from light
varying vec3 viewVec;
varying vec3 lightVec;



const float w = 0.5;
float pcf()
{
   float texel = 1.0/width;
   vec4 depths; 
   vec4 ProjCoord = gl_TexCoord[0];
   depths[0] = texture2D(shadowMap, ProjCoord.st/ProjCoord.q + texel*vec2(-w, -w)).r; 
   depths[1] = texture2D(shadowMap, ProjCoord.st/ProjCoord.q + texel*vec2(+w, -w)).r;
   depths[2] = texture2D(shadowMap, ProjCoord.st/ProjCoord.q + texel*vec2(-w, +w)).r;
   depths[3] = texture2D(shadowMap, ProjCoord.st/ProjCoord.q + texel*vec2(+w, +w)).r;
   
   float r = ProjCoord.p / ProjCoord.q;
   vec4 inShadow = vec4(lessThanEqual(vec4(r,r,r,r), depths+0.011));
   return dot(inShadow, vec4(0.25, 0.25, 0.25, 0.25));
}


void main(void)
{
 

   // normalize vectors for lighting
   vec3 nNormal = normalize(normalVec);
   vec3 nLight = normalize(lightVec.xyz);
   vec3 nView = normalize(viewVec);
   
   
   // diffuse and specular contribution
   float diffuse = clamp(dot(nLight, nNormal), 0.0, 1.0);
   float specular = pow(clamp(dot(reflect(-nView, nNormal), nLight), 0.0, 1.0), 16.0);
   //apply viewport transform to current frag distance from light
   float depth = (lightDepth/2.0 + 0.5 );
   // retrieve stored depth from depthmap
   // this is the same as:
   //float sDepth = texture2D(shadowMap, gl_TexCoord[1].st/gl_TexCoord[1].q).r;
   float sDepth = texture2DProj(shadowMap, gl_TexCoord[0]).q ;
   
   // compare distances to determine if fragment should be
   // lit or shadowed
   float shadow = (lightDepth < sDepth)? 1.0 : 0.0;

  vec4 Ka = gl_LightSource[0].ambient;
  vec4 Kd = gl_LightSource[0].diffuse;
  vec4 Ks = gl_LightSource[0].specular;

   // if fragment is shadowed, do not add specular and diffuse
   // contribution
  if(shadow == 0.0)
    gl_FragColor = (Ka   + (Kd * diffuse  + Ks * specular));
  else
    gl_FragColor = vec4(vec3(0.0), 1.0);

 
 
}