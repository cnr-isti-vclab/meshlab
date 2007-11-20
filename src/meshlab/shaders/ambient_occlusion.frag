uniform sampler2D vTexture;
uniform sampler2D nTexture;
uniform sampler2DShadow dTexture;
uniform vec3 viewDirection;
uniform mat4 mvMatrix;
uniform mat4 prMatrix;
uniform int texSize;
varying vec2 texCoord;

vec4 project(vec4 coords)
{
   coords = (prMatrix * mvMatrix) * coords;
   coords.xy = ( vec2(texSize) * (coords.xy + vec2(1.0)) ) / vec2(2.0);  //we assume viewport as (0,0)..(texSize,texSize)
   coords.z = (coords.z + 1.0) / 2.0;
   coords.a = 1.0;
   
   return coords;
}

void main(void)
{
   vec4 R = vec4(0.0, 0.0, 0.0, 1.0);
   vec4 N = texture2D(nTexture, floor(texCoord.xy));
   vec4 C = texture2D(vTexture, floor(texCoord.xy));
   if ( all(lessThanEqual(C.xyz, vec3(0.0))) )
      discard;
      
   C = project(C);
   
   /* C is the R component and dTexture is set
    * as GL_COMPARE_R_TO_TEXTURE and GL_LEQUAL so:
    *
    *       if (C.z <= dTexture.z)
    *          depthTest == vec4(1.0)
    */

   if ( all(equal(shadow2DProj(dTexture, C), vec4(1.0))) )
      R.r = dot(N.xyz,viewDirection);
   
   gl_FragColor = R;
}