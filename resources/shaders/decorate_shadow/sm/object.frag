uniform mat4 mvpl;
uniform sampler2DShadow shadowMap;
uniform float shadowIntensity; // 1.0 black, 0, transparent
varying vec4 pPos;

void main(){
    vec4 shadowCoord = mvpl * pPos;
    //shadow2DProj fa automaticamente la divisione prospettica..al momento siamo in ortografica e nn serve
    shadowCoord.xyz = shadowCoord.xyz * 0.5 + 0.5;
    float sh = shadow2D(shadowMap, shadowCoord.xyz).x;
    if (sh > 0.1) discard;

    gl_FragColor = vec4(vec3(sh), (shadowIntensity-sh));
}
