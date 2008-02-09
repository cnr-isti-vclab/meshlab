#version 110

uniform sampler3D vTexture;
uniform sampler3D nTexture;
uniform sampler2DShadow dTexture;

uniform vec3 viewDirection;
uniform mat4 mvprMatrix;
uniform int numTexPages;
uniform int viewpSize;
uniform int texSize;

vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5+0.5, coords.w);
}

void main(void)
{
   float pscale = float(viewpSize)/float(texSize),
         zcoord = 0.0,
         dTest  = 0.0;
   
   int potTexPages = 0,
       z3D = 0;

   vec4 N,P,R,depthTest;
   vec3 c3D;

   for (potTexPages=1; potTexPages<numTexPages; potTexPages*=2);

   for (z3D=0; z3D<numTexPages; z3D+=1)
   {
      R = vec4(0.0, 0.0, 0.0, 1.0);

      zcoord = float(z3D)/float(potTexPages);
      zcoord += 1.0/float(potTexPages*2);

      c3D = vec3(gl_FragCoord.xy/float(viewpSize), zcoord);

      N = texture3D(nTexture, c3D);
      P = project(texture3D(vTexture, c3D)) * pscale;

	  /*
      if (z3D+1 == numTexPages)
         if (any(lessThan(N.rgb,vec3(-1.1))))
            discard;
*/
      dTest = shadow2DProj(dTexture, P).r;
      if (dTest > 0.5)
         R.r = max(dot(N.xyz, viewDirection), 0.0);

      gl_FragData[z3D] = R;
   }

}