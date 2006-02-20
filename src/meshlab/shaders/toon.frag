//
// Fragment shader for cartoon-style shading
//
// Author: Philip Rideout
//
// Copyright (c) 2004 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

uniform vec3 DiffuseColor;
uniform vec3 PhongColor;
uniform float Edge;
uniform float Phong;
varying vec3 Normal;
varying vec3 LightDir;
uniform float Fuzz=.1;

void main (void)
{
	vec3 color = DiffuseColor;
	float f = dot(LightDir,Normal);
	
	if (f < Edge+Fuzz)
	{
		float frac=clamp((f-Edge)/Fuzz,0,1);
		color=mix(vec3(0),DiffuseColor,frac);
	}
	else 
	{ 
		float frac=clamp((f-Phong)/Fuzz,0,1);
		color=mix(DiffuseColor,PhongColor,frac);
	}
	gl_FragColor = vec4(color, 1);
}
