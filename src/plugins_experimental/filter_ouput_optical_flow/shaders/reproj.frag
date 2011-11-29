uniform sampler2DShadow u_ShadowMap;
uniform sampler2DShadow u_ColorMap;
uniform mat4            u_ShadowProj;
uniform vec3            u_Viewpoint;

varying vec4            m_Vert;


void main()
{
    vec4 projVert = u_ShadowProj * m_Vert;
    vec2 clipCoord = projVert.xy / projVert.w;

    vec3 color;
    
    if( clipCoord.x<0.0 || clipCoord.x>1.0 ||
        clipCoord.y<0.0 || clipCoord.y>1.0 ||
        shadow2DProj(u_ShadowMap,projVert).r < 0.5 )
        color = vec3( 1.0, 0.0, 1.0 );
    else
        color = shadow2DProj( u_ColorMap, projVert ).xyz;

    gl_FragColor = vec4( color, 1.0 );
}
