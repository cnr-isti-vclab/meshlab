uniform sampler2D rnm;
uniform sampler2D normalMap;
uniform sampler2D depthMap;

varying vec4 texCoord;

const float rad = 0.005;
const float strength = 200.0;

#define SAMPLES 16 // Cambiare sempre anche come color viene calcolato

void main(void)
{
  // these are the random vectors inside a unit sphere
  vec3 pSphere[SAMPLES];
    pSphere[0] = vec3(-0.010735935, 0.01647018, 0.0062425877);
    pSphere[1] = vec3(-0.06533369, 0.3647007, -0.13746321);
    pSphere[2] = vec3(-0.6539235, -0.016726388, -0.53000957);
    pSphere[3] = vec3(0.40958285, 0.0052428036, -0.5591124);
    pSphere[4] = vec3(-0.1465366, 0.09899267, 0.15571679);
    pSphere[5] = vec3(-0.44122112, -0.5458797, 0.04912532);
    pSphere[6] = vec3(0.03755566, -0.10961345, -0.33040273);
    pSphere[7] = vec3(0.019100213, 0.29652783, 0.066237666);
    pSphere[8] = vec3(0.8765323, 0.011236004, 0.28265962);
    pSphere[9] = vec3(0.29264435, -0.40794238, 0.15964167);	
    pSphere[10] = vec3(0.50958289, 0.2424578036, -0.777556324);
    pSphere[11] = vec3(-0.5476366, 0.09554684, 0.157546479);
    pSphere[12] = vec3(0.445354129, -0.54557427, -0.02452532);
    pSphere[13] = vec3(0.274621116, -0.991025345, -0.25477073);
    pSphere[14] = vec3(0.757454103, 0.00575283, -0.2236553);
    pSphere[15] = vec3(0.8325432003, -0.011354804, 0.9564872);

  vec4 texCoordPostW = texCoord / texCoord.w;
  texCoordPostW = texCoordPostW * 0.5 + 0.5;

  // grab a normal for reflecting the sample rays later on
  vec3 fres = normalize((texture2D(rnm , (texCoordPostW).st).xyz * 2.0) - vec3(1.0));
  //vec3 fres = texture2D(rnm , texCoordPostW.st).xyz;
  //Depth del pixel corrente recuperata dalla texture di depth
  float currentPixelDepth = texture2D(depthMap, texCoordPostW.st).x;

  // current fragment coords in screen space
  vec3 ep = vec3(texCoordPostW.st,currentPixelDepth);

  // get the normal of current fragment
  vec3 normal = texture2D(normalMap,texCoordPostW.st).xyz;

  float bl = 0.0;

  // adjust for the depth ( not shure if this is good..)
  float radD = rad/currentPixelDepth;

  //vec3 ray, se, occNorm;
  float sampleDepth;
  vec3 sampleNormal, ray;
  vec2 sample;
  float depthDifference;

  for(int i=0; i < SAMPLES; ++i)
  {
    // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
    ray = radD*reflect(pSphere[i],fres);

    //se normale e raggio formano un angolo maggiore di 90 gradi il raggio viene flippato.
    ray = sign(dot(ray,normal)) * ray;

    //coordinate del campione
    sample = ep.xy + ray.xy;

    //depth del campione
    sampleDepth = texture2D(depthMap, sample).x;
    //normale del campione
    sampleNormal = texture2D(normalMap,sample).xyz;
    depthDifference = currentPixelDepth - sampleDepth;
    
    float zd = strength * (1.0 - dot(normal, sampleNormal)) * max((ep.z + ray.z) - sampleDepth, 0.0);
    bl += 1.0/(1.0+zd*zd);
  }
  
  //float color = bl/SAMPLES;
  float color = bl/16.0;
  /*if(color > 0.8)
    discard;*/
  gl_FragColor = vec4(vec3(color), 1.0);
  //gl_FragColor = vec4(fres, 1.0);
}