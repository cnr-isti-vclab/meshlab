varying vec4            v_ProjVert;
varying vec3            v_Normal;
varying vec3            v_RasterView;
varying vec3            v_Light;

uniform sampler2DShadow u_ColorMap;
uniform sampler2DShadow u_DepthMap;
uniform bool            u_IsLightActivated;
uniform float           u_AlphaValue;


void main()
{
    if( dot(v_Normal,v_RasterView) <= 0.0 )
        discard;

    vec2 clipCoord = v_ProjVert.xy / v_ProjVert.w;
    if( clipCoord.x<0.0 || clipCoord.x>1.0 ||
        clipCoord.y<0.0 || clipCoord.y>1.0 )
        discard;
        
    float visibility = shadow2DProj( u_DepthMap, v_ProjVert ).r;
    if( visibility <= 0.001 )
        discard;

    vec4 color = shadow2DProj( u_ColorMap, v_ProjVert );

    if( u_IsLightActivated )
    {
        vec4 Ka = gl_LightModel.ambient * gl_FrontLightProduct[0].ambient;

        vec3 L = normalize( v_Light );
        vec3 N = normalize( v_Normal );
        float Kd = max( dot(L,N), 0.0 );
        
        color = Ka + gl_FrontMaterial.emission + Kd*gl_FrontLightProduct[0].diffuse*color;
    }

    gl_FragColor = vec4( color.xyz, u_AlphaValue );
}
