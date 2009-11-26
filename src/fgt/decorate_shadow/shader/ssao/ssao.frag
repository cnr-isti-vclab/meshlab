uniform sampler2D rnm;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

uniform mat4 proj;
uniform mat4 invProj;

uniform float rad;

const float totStrength = 4.0;
const float strength = 0.0005;
const float offset = 18.0;
const float falloff = 0.002;

#define SAMPLES 16 

//restituisce la depth presa dalla texture depth map
float getDepth(in vec2 texCoord)
{
    return texture2D(depthMap, texCoord).x;
}

vec3 getViewPosition(in vec2 texCoord)
{
    vec4 pos = vec4(texCoord.x, texCoord.y, getDepth(texCoord), 1.0);
    pos.xyz=(pos.xyz*2.0)-1.0;
    pos = invProj * pos;
    return pos.xyz/pos.w;
}

void main(void)
{
  vec2 texCoord = gl_TexCoord[0].st;  
  //Depth del pixel corrente recuperata dalla texture di depth
  float currentPixelDepth = getDepth(texCoord);

  if(currentPixelDepth == 1.0)
    discard;

  // these are the random vectors inside a unit sphere
  vec3 pSphere[SAMPLES];
    pSphere[0] = vec3(0.355512, -0.709318, -0.102371);
    pSphere[1] = vec3(0.534186, 0.71511, -0.115167);
    pSphere[2] = vec3(-0.87866, 0.157139, -0.115167);
    pSphere[3] = vec3(0.140679, -0.475516, -0.0639818);
    pSphere[4] = vec3(-0.0796121, 0.158842, -0.677075);
    pSphere[5] = vec3(-0.0759516, -0.101676, -0.483625);
    pSphere[6] = vec3(0.12493, -0.0223423, -0.483625);
    pSphere[7] = vec3(-0.0720074, 0.243395, -0.967251);
    pSphere[8] = vec3(-0.207641, 0.414286, 0.187755);
    pSphere[9] = vec3(-0.277332, -0.371262, 0.187755);
    pSphere[10] = vec3(0.63864, -0.114214, 0.262857);
    pSphere[11] = vec3(-0.184051, 0.622119, 0.262857);
    pSphere[12] = vec3(0.110007, -0.219486, 0.435574);
    pSphere[13] = vec3(0.235085, 0.314707, 0.696918);
    pSphere[14] = vec3(-0.290012, 0.0518654, 0.522688);
    pSphere[15] = vec3(0.0975089, -0.329594, 0.609803);

  
  // grab a normal for reflecting the sample rays later on
  vec3 fres = (normalize(texture2D(rnm , (texCoord.st * offset)).xyz)-vec3(0.5,0.5,0.5) )*2.0;
  //vec3 fres = vec3(1.0,0.0,0.0);
  
  //view space position del corrente frammento
  vec3 viewPos = getViewPosition(texCoord);

  // normale del frammento corrente
  vec3 normal = normalize(texture2D(normalMap,texCoord.st).xyz*2.0-1.0);

  float bl = 0.0;

  vec2 modifiers = texture2D(rnm , texCoord.st).xy;

  //vec3 ray, se, occNorm;
  float sampleDepth, depthDifference;
  vec3 sampleNormal, ray;
  vec4 sample, sampleView, viewRayPos;

  for(int i=0; i < SAMPLES; ++i)
  {
    // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
    ray = rad*reflect(pSphere[i],fres);

    //se normale e raggio formano un angolo maggiore di 90 gradi il raggio viene flippato.
    ray = sign(dot(ray,normal)) * ray;

    //viewRayPos = invProj * vec4(ray, 1.0);
	viewRayPos = vec4(ray, 1.0);

    //coordinate del campione
    sample = vec4(vec3(viewPos.xyz + viewRayPos.xyz), 0.0);
    
    //riproietto il campione
    sampleView = ( proj * sample );
    sampleView.xyz = (sampleView.xyz / sampleView.w)*0.5 + 0.5;

    //recupero una nuova normale dalla texture di noise
    fres = normalize((texture2D(rnm, (ray.st * offset)).xyz));

    //depth del campione
    sampleDepth = getDepth(sampleView.st);
    //normale del campione
    sampleNormal = normalize(texture2D(normalMap,sampleView.st).xyz*2.0-1.0);
  
    depthDifference =  currentPixelDepth - sampleDepth;
  
    // depthDifference  > 0 -> sono sotto (e.g. il sample ha una z minore quindi e' piu' vicino) 
    // depthDifference  < 0 -> sono sopra (e.g. il sample ha una z maggiore quindi e' piu' lontano) 
    // depthDifference << 0 -> sono molto sopra (e.g. il sample ha una z molto maggiore quindi e' molto piu' lontano) 
    
    float lightContrib = float(depthDifference<0.001); // se non e' proprio molto sotto  lo considero in piena luce
    
    // se era un po' sotto ma (non tanto), non sarebbe illuminato e allora ci metto un po' di luce con un test delle normali.
    if(lightContrib<0.5 && depthDifference>-0.1) lightContrib = dot(normal, sampleNormal);

    bl += lightContrib;
  }
  // remember: ao is skylight
  float ao = bl/float(SAMPLES);
  ao = ao*ao;

  gl_FragColor = vec4(vec3(ao), 1.0);
}