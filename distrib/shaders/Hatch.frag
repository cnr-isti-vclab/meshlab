/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

uniform float frequency;
uniform float edgew;            // width of smooth step

varying vec3  ObjPos;               // object space coord (noisy)
varying float V;                    // generic varying
varying float LightIntensity;


void main()
{
    float dp       = length(vec2(dFdx(V), dFdy(V)));
    float logdp    = -log2(dp * 8.0);
    float ilogdp   = floor(logdp);
    float stripes  = exp2(ilogdp);

    float sawtooth = fract((V ) * frequency * stripes);
    float triangle = abs(2.0 * sawtooth - 1.0);

    // adjust line width
    float transition = logdp - ilogdp;

    // taper ends
    triangle = abs((1.0 + transition) * triangle - transition);


    float edge0  = clamp(LightIntensity - edgew, 0.0, 1.0);
    float edge1  = clamp(LightIntensity, 0.0, 1.0);
    float square = 1.0 - smoothstep(edge0, edge1, triangle);

// uncomment this line to get the original color instead of white
//   gl_FragColor = gl_Color * vec4(vec3(square), 1.0);

   gl_FragColor =  vec4(vec3(square), 1.0);
}
