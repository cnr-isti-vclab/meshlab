varying vec4    v_ClipSpaceVert;
varying vec3    v_Nor;
varying vec3    v_View;


void main()
{
    gl_Position = v_ClipSpaceVert = ftransform();
    v_Nor = gl_NormalMatrix * gl_Normal;
    v_View = -(gl_ModelViewMatrix * gl_Vertex).xyz;
}
