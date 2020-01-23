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

// Application to vertex shader
varying vec3 N;
varying vec3 I;
varying vec4 Cs;
varying vec3 LightDir;

void main()
{
    vec4 P = gl_ModelViewMatrix * gl_Vertex;
    float nDotVP;         // normal . light direction

    N=gl_NormalMatrix * gl_Normal;    
    LightDir=vec3(gl_LightSource[0].position);

//    I  = P.xyz - vec3 (0);
    I  = P.xyz;
    
    Cs = gl_Color;
    
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; 
} 
