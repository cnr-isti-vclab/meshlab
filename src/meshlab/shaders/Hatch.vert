uniform vec3  HatchDirection;
uniform float Lightness;

varying vec3  ObjPos;
varying float V;
varying float LightIntensity;

void main()
{
    ObjPos          = vec3(gl_Vertex) * 0.2;

    vec3 pos        = vec3(gl_ModelViewMatrix * gl_Vertex);
    vec3 tnorm      = normalize(gl_NormalMatrix * gl_Normal);
    //vec3 lightVec   = normalize(LightPosition - pos);
	vec3 lightVec =  vec3(gl_LightSource[0].position);

    float grey = Lightness*dot (vec4(.333,.333,.333,0),gl_Color);
	LightIntensity  = max(grey * dot(lightVec, tnorm), 0.0);
	//LightIntensity  = max(dot(lightVec, tnorm), 0.0);
    
    //V = gl_MultiTexCoord0.t;  // try .s for vertical stripes
	V =dot(vec3(gl_Vertex),HatchDirection);

    gl_Position = ftransform();
//    gl_FrontColor=gl_Color;
}
