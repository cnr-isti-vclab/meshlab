//
// Vertex shader for cartoon-style shading
//
// Author: Philip Rideout
//
// Copyright (c) 2004 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

varying vec3 Normal;

void main(void)
{
	Normal = normalize(gl_NormalMatrix * gl_Normal);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
