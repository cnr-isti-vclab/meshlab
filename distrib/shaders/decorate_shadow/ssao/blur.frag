uniform sampler2D scene;
uniform vec2 scale;
void main() {
    vec4 color = vec4(vec3(0.0), 0.0);

    color += texture2D( scene, gl_TexCoord[0].st + vec2( -3.0 * scale.x, -3.0 * scale.y ) ) * 0.015625;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( -2.0 * scale.x, -2.0 * scale.y ) )*0.09375;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( -1.0 * scale.x, -1.0 * scale.y) )*0.234375;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( 0.0 , 0.0) )*0.3125;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( 1.0 * scale.x,  1.0 * scale.y ) )*0.234375;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( 2.0 * scale.x,  2.0 * scale.y ) )*0.09375;
    color += texture2D( scene, gl_TexCoord[0].st + vec2( 3.0 * scale.x, 3.0 * scale.y ) ) * 0.015625;

    if(color.a < 0.5)
      discard;
    if(scale.y == 0.0)
      gl_FragColor = color;//vec4(color.xyz, 0.5);
    else
	gl_FragColor = gl_FragColor = vec4(vec3(0.0), (1.0 - color.x));
      //gl_FragColor = vec4(color.xyz, 0.5);//texture2D(scene, gl_TexCoord[0].st);

}