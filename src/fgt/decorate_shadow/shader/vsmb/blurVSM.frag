uniform sampler2D scene;
uniform vec2 scale;
void main() {
    vec4 color = vec4(vec3(0.0), 1.0);

    color += texture2D( scene, gl_TexCoord[0].st + vec2( -3.0 * scale.x, -3.0 * scale.y ) ) * 0.015625;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( -2.0 * scale.x, -2.0 * scale.y ) )*0.09375;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( -1.0 * scale.x, -1.0 * scale.y) )*0.234375;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( 0.0 , 0.0) )*0.3125;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( 1.0 * scale.x,  1.0 * scale.y ) )*0.234375;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( 2.0 * scale.x,  2.0 * scale.y ) )*0.09375;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( 3.0 * scale.x, -3.0 * scale.y ) ) * 0.015625;
    
    if(color.x == 1.0 && color.y == 1.0 && color.z == 1.0)
      discard;
    gl_FragColor = color;
}