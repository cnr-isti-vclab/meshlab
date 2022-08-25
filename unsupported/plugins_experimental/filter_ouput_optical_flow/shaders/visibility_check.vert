uniform ivec2   u_Viewport;
uniform vec3    u_Viewpoint;
uniform mat4    u_ShadowProj;

varying float   v_FrontFacingFactor;
varying vec4    v_ProjVert;


void main()
{
    // Transform the vertex Id in coordinates in clipping space so as to recover the result
    // of the computation into the framebuffer, with a pixel ordering corresponding to the
    // ordering of vertices in the mesh.
    int vertId = int(gl_MultiTexCoord0.x);

    gl_Position.x = 2.0*(vertId%u_Viewport.x) / float(u_Viewport.x)  -  1.0;
    gl_Position.y = 2.0*(vertId/u_Viewport.x) / float(u_Viewport.y)  -  1.0;
    gl_Position.z = 0.0;
    gl_Position.w = 1.0;


    // First visibility check: orientation of the normal with respect to the camera.
    // If the vertex is facing the camera, the projected vertex used for fetching
    // projective textures is computed.
    v_FrontFacingFactor = dot( u_Viewpoint-gl_Vertex, gl_Normal );

    if( v_FrontFacingFactor > 0.0 )
        v_ProjVert = u_ShadowProj * gl_Vertex;
}
