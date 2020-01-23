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
uniform sampler2D bump;
uniform float Kd;
uniform float Ks;
uniform float Ke;
varying vec4 baseColor;
varying vec3 ViewDirection;
varying vec3 LightDirection;

void main(void)
{

   vec3 bumpPert = normalize( -(0.5,0.5,0.5) + texture2D(bump, gl_TexCoord[0].st).rgb );

   vec3 LightDir = normalize(LightDirection);
   vec3 ViewDir = normalize(ViewDirection);
   vec3 nNormal =  normalize(gl_NormalMatrix * bumpPert);
   
   float diffuse = clamp(dot(LightDir, nNormal), 0.0, 1.0);
   float specular = pow(clamp(dot(reflect(ViewDir, nNormal),-LightDir),0.0, 1.0), Ke);
   gl_FragColor = (Kd * diffuse * baseColor) +  (Ks * specular)*baseColor;
}