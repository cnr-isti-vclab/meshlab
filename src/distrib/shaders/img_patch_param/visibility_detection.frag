uniform sampler2D       u_VertexMap;
uniform sampler2D       u_NormalMap;
uniform sampler2DShadow u_SadowMap;
uniform mat4            u_ShadowProj;
uniform vec3            u_Viewpoint;

const float             V_UNDEFINED = 0.0;
const float             V_BACKFACE  = 1.0 / 255.0;
const float             V_VISIBLE   = 2.0 / 255.0;


void main()
{
    vec3 pos = texelFetch( u_VertexMap, ivec2(gl_FragCoord.xy), 0 ).xyz;
    vec3 nor = texelFetch( u_NormalMap, ivec2(gl_FragCoord.xy), 0 ).xyz;

    if( dot(u_Viewpoint-pos,nor) < 0.0 )
        gl_FragColor = vec4( V_BACKFACE );
    else
    {
        vec4 projVert = u_ShadowProj * vec4(pos,1.0);
        vec2 clipCoord = projVert.xy / projVert.w;

        if( clipCoord.x>=0.0 && clipCoord.x<=1.0 &&
            clipCoord.y>=0.0 && clipCoord.y<=1.0 &&
            shadow2DProj( u_SadowMap, projVert ).r > 0.5 )
            gl_FragColor = vec4( V_VISIBLE );
        else
            gl_FragColor = vec4( V_UNDEFINED );
    }
}
