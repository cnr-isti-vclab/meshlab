varying vec4    v_ProjVert;
varying vec3    v_Normal;
varying vec3    v_RasterView;
varying vec3    v_Light;

uniform mat4    u_ProjMat;
uniform vec3    u_Viewpoint;
uniform mat4    u_LightToObj;
uniform mat4    u_ModelXf;


void main()
{
    gl_Position  = ftransform();
    v_ProjVert = u_ProjMat * u_ModelXf * gl_Vertex;
    v_Normal = (u_ModelXf*vec4(gl_Normal,1.0)).xyz;
    v_RasterView = u_Viewpoint - (u_ModelXf*gl_Vertex).xyz;
    v_Light = u_LightToObj[2].xyz;
    
    float d = length( gl_ModelViewMatrix * gl_Vertex );
    float distAtten = 1.0 / (gl_Point.distanceConstantAttenuation      +
                             gl_Point.distanceLinearAttenuation*d      +
                             gl_Point.distanceQuadraticAttenuation*d*d );
    gl_PointSize = clamp( gl_Point.size*sqrt(distAtten) + 0.5, gl_Point.sizeMin, gl_Point.sizeMax );
}
