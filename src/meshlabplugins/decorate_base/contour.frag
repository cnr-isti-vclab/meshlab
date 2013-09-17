uniform vec4 stripe_alpha;
uniform float stripe_width;   // between 0 constant and 1 binary.

varying float scaled_quality;

void main(void)
{
//  vec4 color = vec4(1.0,1.0,1.0,0.5);
  vec4 color = vec4(1.0,1.0,1.0,0.5);
  if(fract(scaled_quality)<stripe_width) gl_FragColor = color;
                    else discard;

}
