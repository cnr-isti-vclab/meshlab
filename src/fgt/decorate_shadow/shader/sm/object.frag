uniform mat4 mvpl;
uniform sampler2DShadow shadowMap;
varying vec4 pPos;

void main(){
    vec4 shadowCoord = mvpl * pPos;
    //shadow2DProj fa automaticamente la divisione prospettica..al momento siamo in ortografica e nn serve
    shadowCoord.xyz = shadowCoord.xyz * 0.5 + 0.5;
    float sh = shadow2D(shadowMap, shadowCoord.xyz).x;
    if (sh > 0.4)
      discard;
    //else
    gl_FragColor = vec4(vec3(sh), (0.5-sh));
}