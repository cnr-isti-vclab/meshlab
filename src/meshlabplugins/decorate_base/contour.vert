uniform float quality_min;
uniform float quality_max;
uniform float stripe_num;  // between -1 and 1
uniform float stripe_width;   // between 0 constant and 1 binary.

attribute float vert_quality;

varying float scaled_quality;

void main(void)
{
   gl_Position = ftransform();
   scaled_quality = ( (vert_quality - quality_min)/(quality_max - quality_min) ) * stripe_num;
}
