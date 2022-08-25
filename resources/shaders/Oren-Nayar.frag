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
// Oren-Nayar reflectance model (per-pixel)
//
// by 
// Massimiliano Corsini
// Visual Computing Laboratory (2006)
//

varying vec3 normal;
varying vec3 vpos;

// surface roughness
uniform float roughnessSquared;

void main()
{
	// material properties are embedded in the shader (for now)
	vec4 mat_ambient = vec4(1.0, 1.0, 1.0, 1.0);
	vec4 mat_diffuse = vec4(1.0, 1.0, 1.0, 1.0);

	// normalize interpolated normal
	vec3 N = normalize(normal);

	// light vector
	vec3 L = normalize(gl_LightSource[0].position.xyz - vpos);
	
	// view vector
	vec3 V = normalize(-vpos);
	
	// ambient term
	vec4 ambient = mat_ambient * gl_LightModel.ambient;
	
	// diffuse color
	vec4 kd = mat_diffuse * gl_LightSource[0].diffuse;	
	
	// Oren-Nayar model
	float A = 1.0 - (0.5 * roughnessSquared) / (roughnessSquared + 0.33);
	float B = (0.45 * roughnessSquared) / (roughnessSquared + 0.09);	
	
	float VdotN = dot(V, N);
	float LdotN = dot(L, N);
	float irradiance = max(0.0, LdotN);
	
	float angleViewNormal  = acos(VdotN);
	float angleLightNormal = acos(LdotN);
	
	// max( 0.0 , cos(phi_incident, phi_reflected) )
	float angleDiff = max(0.0, dot(normalize(V - N * VdotN),
		normalize(L - N * LdotN)));
	
	float alpha = max(angleViewNormal, angleLightNormal);
	float beta  = min(angleViewNormal, angleLightNormal);
				
	// final color
	gl_FragColor  = ambient + kd * irradiance * 
		(A + B * angleDiff * sin(alpha) * tan(beta));
}
