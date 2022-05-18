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

uniform vec3 DiffuseColor;
uniform vec3 PhongColor;
uniform float Edge;
uniform float Phong;
varying vec3 Normal;
varying vec3 LightDir;
uniform float Fuzz;

void main (void)
{
        vec3 color = DiffuseColor;
	float f = dot(LightDir,Normal);
	
	if (f < Edge+Fuzz) {
		float frac=clamp((f-Edge)/Fuzz, 0.0, 1.0);
		color = mix(vec3(0.0), DiffuseColor, frac);
	} else { 
		float frac=clamp((f-Phong)/Fuzz, 0.0, 1.0);
		color = mix(DiffuseColor, PhongColor, frac);
	}
	gl_FragColor = vec4(color, 1.0);
}
