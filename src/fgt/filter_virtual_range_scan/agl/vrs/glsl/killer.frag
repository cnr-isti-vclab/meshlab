uniform sampler2D outMask;
uniform sampler2D inputEyeNormal;

varying vec2    sample_coords;
varying vec3    sample_eye_normal;

void main()
{
    ivec2 frag_coords = ivec2( gl_FragCoord.xy );
    float mask_val = texelFetch( outMask, frag_coords, 0 );

    if( mask_val == 1.0 )
    {
        vec3 current_eye_normal = normalize( texelFetch( inputEyeNormal, frag_coords, 0 ) );
        vec3 norm_sample_eye_normal = normalize( sample_eye_normal );
        vec3 perfect_view = vec3( 0.0, 0.0, -1.0 );

        float look_at_me_1 = dot( -perfect_view, current_eye_normal );
        float look_at_me_2 = dot( -perfect_view, norm_sample_eye_normal );

        if( look_at_me_1 > look_at_me_2 )
        {   // the current view is the best
            gl_FragData[0] = vec4( 1.0, 0.0, 0.0, 0.0 );
            gl_FragData[1] = vec4( sample_coords, 0.0, 0.0 );
        }
        else
        {   // the arriving sample is the best
            gl_FragData[0] = vec4( 0.0, -1.0, 0.0, 0.0 );
            gl_FragData[1] = vec4( 0.0, 0.0, 0.0, 0.0 );
        }
    }
    else
    {
        gl_FragData[0] = vec4( 0.0, 0.0, 0.0, 0.0 );
        gl_FragData[1] = vec4( 0.0, 0.0, 0.0, 0.0 );
    }
}
