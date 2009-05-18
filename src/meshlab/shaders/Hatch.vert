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
