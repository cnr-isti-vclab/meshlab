

uniform sampler2D textureLastDepth;
uniform float tolerance;
uniform vec2 oneOverBufSize;
varying vec3 N;

void main()
{
  vec2 tc =  gl_FragCoord.xy * oneOverBufSize ;
  
  float lastDepth = texture2D(textureLastDepth, tc ).x;
  if (gl_FragCoord.z<=lastDepth + tolerance) discard;
  
  gl_FragColor = vec4(normalize(N),1.0); 
    //
}
