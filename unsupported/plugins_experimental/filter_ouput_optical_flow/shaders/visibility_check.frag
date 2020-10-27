uniform sampler2DShadow u_ShadowMap;
uniform sampler2DShadow u_WeightMap;
uniform sampler2DShadow u_WeightMapSilh;
uniform bool            u_UseSilhouetteMask;

varying float           v_FrontFacingFactor;
varying vec4            v_ProjVert;


void main()
{
    float weight = -2.0;
    vec2 clipCoord = v_ProjVert.xy / v_ProjVert.w;

    if( v_FrontFacingFactor > 0.0 &&
        clipCoord.x>=0.0 && clipCoord.x<=1.0 &&
        clipCoord.y>=0.0 && clipCoord.y<=1.0 &&
        shadow2DProj(u_ShadowMap,v_ProjVert).r>0.001 )
    {
        weight = shadow2DProj( u_WeightMap, v_ProjVert ).x;
        if( u_UseSilhouetteMask )
            weight *= shadow2DProj( u_WeightMapSilh, v_ProjVert ).x;
    }

    gl_FragColor = vec4(weight);
}
