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

//
// Minnaert reflection model (per-pixel)
//
// by
// Massimiliano Corsini 
// Visual Computing Lab (2006)
//

varying vec3 normal;
varying vec3 vpos;

uniform float m;

void main (void)
{
	vec3 N = normalize(normal);
	vec3 V = normalize(-vpos);
	vec3 L = normalize(gl_LightSource[0].position.xyz - vpos);

	// diffuse color
	vec4 kd = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse;

	// Minnaert reflection model
	float NdotV = dot(N,V);
	float NdotL = dot(N,L);

	gl_FragColor = pow(max(NdotV * NdotL, 0.0), m) * kd;
}
