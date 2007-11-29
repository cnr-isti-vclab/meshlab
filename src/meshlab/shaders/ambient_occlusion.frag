uniform sampler2D vTexture;
uniform sampler2D nTexture;
uniform sampler2DShadow dTexture;
uniform vec3 viewDirection;
uniform mat4 mvprMatrix;
uniform int texSize;

vec4 project(vec4 coords)
{
   coords = mvprMatrix * coords; // clip space [-1 .. 1]   
   return vec4(coords.xyz * 0.5 + 0.5, coords.w);
}

void main(void)
{
   vec4 R = vec4(0.0, 0.0, 0.0, 1.0);
   vec4 N = texture2D(nTexture, floor(gl_FragCoord.xy) / float(texSize-1));
   vec4 P = project(texture2D(vTexture, floor(gl_FragCoord.xy) / float(texSize-1)));
   
   vec4 depthTest = shadow2DProj(dTexture, P);
   if (depthTest.r > 0.5)
      R.r = max(dot(N.xyz, viewDirection), 0.0);

   gl_FragColor = R;
}