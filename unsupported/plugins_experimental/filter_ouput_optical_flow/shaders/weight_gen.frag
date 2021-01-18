uniform bool    u_UseImageBorderMask;
uniform bool    u_UseViewDirectionMask;
uniform bool    u_UseDistanceMask;

uniform float   u_DepthMin;
uniform float   u_DepthMax;

varying vec4    v_ClipSpaceVert;
varying vec3    v_Nor;
varying vec3    v_View;


void main()
{
    v_ClipSpaceVert /= v_ClipSpaceVert.w;

    float weight = 1.0;

    if( u_UseImageBorderMask )
        weight *= 1.0 - max( abs(v_ClipSpaceVert.x), abs(v_ClipSpaceVert.y) );

    if( u_UseViewDirectionMask && weight>0.0 )
        weight *= dot( normalize(v_View), normalize(v_Nor) );

    if( u_UseDistanceMask && weight>0.0 )
        weight *= (u_DepthMax-length(v_View)) / (u_DepthMax-u_DepthMin);

    gl_FragColor = vec4( max(weight,0.0) );
}
