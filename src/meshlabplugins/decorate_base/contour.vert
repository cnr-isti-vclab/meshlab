uniform float quality_min;
uniform float quality_max;
uniform float stripe_num;  // between -1 and 1
uniform float stripe_width;   // between 0 constant and 1 binary.
uniform float colormap;
attribute float vert_quality;

varying float scaled_quality;
varying vec4 basecolor;

vec3 cJET0 = vec3(1,0,0);
vec3 cJET1 = vec3(1,1,0);
vec3 cJET2 = vec3(0,1,0);
vec3 cJET3 = vec3(0,1,1);
vec3 cJET4 = vec3(0,0,1);
vec3 cPAR0 = vec3(0.502, 0.086, 0.153);
vec3 cPAR1 = vec3(0.882, 0.388, 0.012);
vec3 cPAR2 = vec3(0.831, 0.522, 0.078);
vec3 cPAR3 = vec3(0.776, 0.655, 0.024);
vec3 cPAR4 = vec3(0.62, 0.725, 0.22);
vec3 cPAR5 = vec3(0.451, 0.749, 0.573);
vec3 cPAR6 = vec3(0.337, 0.729, 0.851);
vec3 cPAR7 = vec3(0.18, 0.808, 0.988);
vec3 cPAR8 = vec3(0.039, 0.98, 1.);

// not sure the best curve to use for mixing. linear looks nice to me though; better
// than smoothstep and smootherstep.
// smoothstep is of course the fastest though.
float gradientStep(float edge0, float edge1, float x)
{
    return smoothstep(edge0, edge1, x);

    // smootherstep
    //x = clamp((x - edge0)/(edge1 - edge0), 0.0, 1.0);
    //return x*x*x*(x*(x*6. - 15.) + 10.);
    
    // linear
    //x = clamp((x - edge0)/(edge1 - edge0), 0.0, 1.0);
    //return x;
}


vec3 gradient9(float t, vec3 c0, vec3 c1, vec3 c2, vec3 c3, vec3 c4, vec3 c5, vec3 c6, vec3 c7, vec3 c8)
{
    const float colCount = 9.;
    const float bandSize = 1./(colCount-1.);
    const float plateauSize = 0. * (bandSize/2.);
    float i = 0.;
	return
        + (c0 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c1 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c2 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c3 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c4 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c5 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c6 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c7 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c8 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        ;
}

vec3 gradient5(float t, vec3 c0, vec3 c1, vec3 c2, vec3 c3, vec3 c4)
{
    const float colCount = 5.;
    const float bandSize = 1./(colCount-1.);
    const float plateauSize = 0. * (bandSize/2.);
    float i = 0.;
	return
        + (c0 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c1 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c2 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c3 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        + (c4 * (1.-gradientStep(plateauSize,bandSize-plateauSize, abs(t-bandSize*i++))))
        ;
}


vec4 colormap_jet(float x) {
  return vec4(gradient5(x,cJET0,cJET1,cJET2,cJET3,cJET4),1.);
}


vec4 colormap_parula(float x) {
  return vec4(gradient9(x,cPAR0,cPAR1,cPAR2,cPAR3,cPAR4,cPAR5,cPAR6,cPAR7,cPAR8),1.);
}

void main(void)
{
   gl_Position = ftransform();
   scaled_quality = ( (vert_quality - quality_min)/(quality_max - quality_min) ) * stripe_num;
   if(colormap==0.)
   {
     basecolor = vec4(1.,1.,1.,1.);
   }else if(colormap==1.)
   {
     basecolor = colormap_jet((vert_quality-quality_min)/(quality_max-quality_min));
   } else
   {
     basecolor = colormap_parula((vert_quality-quality_min)/(quality_max-quality_min));
   }
}
