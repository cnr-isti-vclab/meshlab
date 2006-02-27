varying vec3 pos;
uniform vec3 StripeDir;
uniform float Fuzz=.1;
uniform float Width=0.5;
uniform float Scale=1.0;
void main (void) 
{
        vec4 color;
        float scalar_pos=dot(Scale*StripeDir,pos);
        float scaled_pos=fract(scalar_pos*10.0)	;
        color = gl_Color;
        	
        float frac1 = clamp(scaled_pos / Fuzz, 0.0, 1.0);
        float frac2 = clamp((scaled_pos - Width) / Fuzz, 0.0, 1.0);

		frac1 = frac1 * (1.0 - frac2);
    	frac1 = frac1 * frac1 * (3.0 - (2.0 * frac1));
           
        color.a=mix(0,1,frac1);
        gl_FragColor = color;
}
