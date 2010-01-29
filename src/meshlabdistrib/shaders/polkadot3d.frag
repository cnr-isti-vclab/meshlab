//
// Fragment shader for 3 dimensional polka dot shader.
//
// Author:  Joshua Doss
//   
// Copyright (C) 2002-2004  3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//
varying float LightIntensity;
varying vec3 MCPosition;

//Create uniform variables so dots can be spaced and scaled by user
uniform vec3 Spacing;
uniform float DotSize;

//Create colors as uniform variables so they can be easily changed
uniform vec3 ModelColor, PolkaDotColor;

void main(void)
{
   float insidesphere, sphereradius, scaledpointlength;
   vec3 scaledpoint, finalcolor;

   // Scale the coordinate system
   // The following line of code is not yet implemented in current drivers:
   // mcpos = mod(Spacing, MCposition);
   // We will use a workaround found below for now
   scaledpoint       = MCPosition - (Spacing * floor(MCPosition/Spacing));

   // Bring the scaledpoint vector into the center of the scaled coordinate system
   scaledpoint       = scaledpoint - Spacing/2.0;

   // Find the length of the scaledpoint vector and compare it to the dotsize
   scaledpointlength = length(scaledpoint);
   insidesphere      = step(scaledpointlength,DotSize);
   
   // Determine final output color before lighting
   finalcolor        = vec3(mix(ModelColor, PolkaDotColor, insidesphere));

   // Output final color and factor in lighting
   gl_FragColor      = clamp((vec4( finalcolor, 1.0 ) * LightIntensity), vec4(0.0), vec4(1.0));
}

