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
// Cook-Torrance reflectance model (per-pixel)
//
// by
// Massimiliano Corsini
// Visual Computing Lab (2006)
//

varying vec3 normal;
varying vec3 vpos;

// gaussian coefficient
uniform float C;

// index of refraction
uniform float ni;

void main()
{
	// the material propertise are embedded in the shader (for now)
	vec4 mat_ambient = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 mat_diffuse = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 mat_specular= vec4(1.0, 1.0, 1.0, 1.0);

	// normalize interpolated normal
	vec3 N = normalize(normal);

	// light vector
	vec3 L = normalize(gl_LightSource[0].position.xyz - vpos);
	
	// vertex-to-eye space (view vector)
	vec3 V = normalize(-vpos);
	
	// half-vector
	vec3 H = normalize(vec3(gl_LightSource[0].halfVector));
	
	
	float NdotH = max(0.0, dot(N, H));
	float VdotH = dot(V, H);
	float NdotV = dot(N, V);
	float NdotL = dot(N, L);
	
	// D term (gaussian)
	float alpha = acos(NdotH);
	float D = C * exp(- alpha * alpha * C);
	
	// Geometric factor (G)
	float G1 = 2.0 * NdotH * NdotV / VdotH;
	float G2 = 2.0 * NdotH * NdotL / VdotH;
	float G = min(1.0, min(G1, G2));
	
	// Fresnel Refraction (F)
	float k = pow(1.0 - NdotV, 5.0);
	float F = 1.0 - k + ni * k;
	
	// ambient
	vec4 ambient = mat_ambient * gl_LightSource[0].ambient;
	
	// diffuse color
	vec4 kd  = mat_diffuse * gl_LightSource[0].diffuse;

    gl_FragColor  = ambient + kd * (F * D * G) / NdotV;
}
