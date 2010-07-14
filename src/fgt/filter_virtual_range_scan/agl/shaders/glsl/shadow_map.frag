uniform sampler2D light_depth_map;
uniform vec3 light_color;

varying vec4 light_clip_position;

void main()
{
    vec3 ndc_pos = light_clip_position.xyz / light_clip_position.w;
    ndc_pos = ndc_pos * 0.5 + 0.5;
    float light_depth = texture2D( light_depth_map, ndc_pos.xy );

    if( light_depth < ndc_pos.z )
    {   // shadowed
        gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
    }
    else
    {   // not shadowed
        gl_FragColor = vec4( light_color.xyz, 0.0 );
    }
}
