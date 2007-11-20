uniform sampler2D vTexture;
uniform sampler2D nTexture;
uniform sampler2DShadow dTexture;
uniform vec3 viewDirection;
uniform mat4 mvMatrix;
uniform mat4 prMatrix;
uniform int texSize;

vec4 project(vec4 coords)
{
   coords = prMatrix * (mvMatrix * coords); // clip space [-1 .. 1]
   coords.xyz = coords.xyz * 0.5 + 0.5;
   
   return coords;
}

void main(void)
{
   vec4 R = vec4(0.0, 0.0, 0.0, 1.0);
   vec4 N = texture2D(nTexture, floor(gl_FragCoord.xy) / float(texSize-1));
   vec4 C = texture2D(vTexture, floor(gl_FragCoord.xy) / float(texSize-1));
   
   /*
   if ( all(lessThanEqual(C.xyz, vec3(0.0))) )
      discard;
   /**/
      
   C = project(C);
   
   /* C is the R component and dTexture is set
    * as GL_COMPARE_R_TO_TEXTURE and GL_LEQUAL so:
    *
    *       if (C.z <= dTexture.z)
    *          depthTest == vec4(1.0)
    */

   vec4 depthTest = shadow2DProj(dTexture, C);
   //if (C.z <= depthTest.r)
   if (depthTest.r > 0.5)
   {
      N = normalize(N);
      vec3 vd = normalize(viewDirection);
      R.r = max(dot(N.xyz,vd), 0.0);
   }
    
   gl_FragColor = R;
}