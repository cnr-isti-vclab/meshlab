// Depth Map
//
// by 
// Massimiliano Corsini
// Visual Computing Lab (2007)

varying vec4 vpos;

void main()
{
	vpos = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_Position = ftransform();
} 
