uniform float stripe_alpha;
uniform float stripe_width;   // between 0 and 1 .
uniform bool stripe_ramp;
varying float scaled_quality;
varying vec4 basecolor;



void main(void)
{
//  vec4 color = vec4(1.0,1.0,1.0,0.5);
//  vec4 color = vec4(1.0,1.0,1.0,stripe_alpha);
  if(fract(scaled_quality) > stripe_width) discard;

  float alpha = 1.0;
//  vec4 color = vec4(1.0,1.0,1.0,0.5*alpha);
  if(stripe_ramp) alpha = fract(scaled_quality)/stripe_width;

  vec4 color = vec4(1.0,1.0,1.0,stripe_alpha*alpha);
  color = vec4(basecolor.rgb,stripe_alpha*alpha);
  gl_FragColor = color;
}
