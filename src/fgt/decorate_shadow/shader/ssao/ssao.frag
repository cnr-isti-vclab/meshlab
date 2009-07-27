uniform sampler2D rnm;
uniform sampler2D normalMap;

varying vec4 texCoord;

const float totStrength = 5.38;
const float strength = 0.7;
const float offset = 18.0;
const float falloff = 0.000002;
const float rad = 0.005;
#define SAMPLES 16 // 10 is good
const float invSamples = -1.38/2.0;

void main(void)
{
  vec4 texCoordPostW = texCoord / texCoord.w;
  texCoordPostW = texCoordPostW * 0.5 + 0.5;
  vec2 uv = texCoordPostW.xy;
  // these are the random vectors inside a unit sphere
  vec3 pSphere[16] = vec3[](
			vec3(-0.010735935, 0.01647018, 0.0062425877),
			vec3(-0.06533369, 0.3647007, -0.13746321),
			vec3(-0.6539235, -0.016726388, -0.53000957),
			vec3(0.40958285, 0.0052428036, -0.5591124),
			vec3(-0.1465366, 0.09899267, 0.15571679),
			vec3(-0.44122112, -0.5458797, 0.04912532),
			vec3(0.03755566, -0.10961345, -0.33040273),
			vec3(0.019100213, 0.29652783, 0.066237666),
			vec3(0.8765323, 0.011236004, 0.28265962),
			vec3(0.29264435, -0.40794238, 0.15964167),
			vec3(0.50958289, 0.2424578036, -0.777556324),
			vec3(-0.5476366, 0.09554684, 0.157546479),
			vec3(0.445354129, -0.54557427, -0.02452532),
			vec3(0.274621116, -0.991025345, -0.25477073),
			vec3(0.757454103, 0.00575283, -0.2236553),
			vec3(0.8325432003, -0.011354804, 0.9564872)
			);

  // grab a normal for reflecting the sample rays later on
  vec3 fres = normalize((texture2D(rnm , uv * offset).xyz * 2.0) - vec3(1.0));

  vec4 currentPixelSample = texture2D(normalMap,uv);

  float currentPixelDepth = currentPixelSample.a;

  // current fragment coords in screen space
  vec3 ep = vec3(uv.xy,currentPixelDepth);
  // get the normal of current fragment
  vec3 norm = currentPixelSample.xyz;

  float bl = 0.0;
  // adjust for the depth ( not shure if this is good..)
  float radD = rad/currentPixelDepth;

  //vec3 ray, se, occNorm;
  float occluderDepth, depthDifference;
  vec4 occluderFragment;
  vec3 ray;
  for(int i=0; i < SAMPLES; ++i)
  {
    // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
    ray = radD*reflect(pSphere[i],fres);

    // get the depth of the occluder fragment
    occluderFragment = texture2D(normalMap,ep.xy + sign(dot(ray,norm) )*ray.xy);
    // if depthDifference is negative = occluder is behind current fragment
    depthDifference = currentPixelDepth-occluderFragment.a;

    // calculate the difference between the normals as a weight
    // the falloff equation, starts at falloff and is kind of 1/x^2 falling
    bl += step(falloff,depthDifference)*(1.0-dot(occluderFragment.xyz,norm))*(1.0-smoothstep(falloff,strength,depthDifference));
  }

  float ao = 1.0 + totStrength * bl * invSamples;
  /*if(ao > 0.7)
    discard;
    gl_FragColor = vec4(vec3(0.0), 0.7 - ao);*/

  float alpha = 1.0;
  if((norm.x + norm.y + norm.z) == 0.0)
    alpha = 0.0;
    
  gl_FragColor = vec4(vec3(ao), alpha);

}