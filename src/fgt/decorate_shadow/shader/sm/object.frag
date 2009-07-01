uniform sampler2D shadowMap;
/*
uniform float Ka;
uniform float Kd;
uniform float Ks;*/
varying vec3 normalVec;
varying float lightDepth; //current frag distance from light
varying vec3 viewVec;
varying vec3 lightVec;


const float texel = 1.0/2048.0;
const float w = 0.5;
float pcf()
{
   vec4 depths; 
   vec4 ProjCoord = gl_TexCoord[1];
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
   
   // base texture color
   vec4 modelColor = texture2D(modelTexture, gl_TexCoord[0].st);
   
   // diffuse and specular contribution
   float diffuse = clamp(dot(nLight, nNormal), 0.0, 1.0);
   float specular = pow(clamp(dot(reflect(-nView, nNormal), nLight), 0.0, 1.0), 16.0);
   
   
   //apply viewport transform to current frag distance from light
   float depth = (lightDepth/2.0 + 0.5 );
   // retrieve stored depth from depthmap
   // this is the same as:
   //float sDepth = texture2D(shadowMap, gl_TexCoord[1].st/gl_TexCoord[1].q).r;
   float sDepth = texture2DProj(shadowMap, gl_TexCoord[1]).q ;
   
   // compare distances to determine if fragment should be
   // lit or shadowed 
   float shadow = pcf();
   
   float Ka = 0.5;
  float Kd = 0.7;
  float Ks = 0.4;

   // if fragment is shadowed, do not add specular and diffuse
   // contribution
  gl_FragColor =( Ka )  +
               (Kd * diffuse  + Ks * specular) * shadow;

 
 
}