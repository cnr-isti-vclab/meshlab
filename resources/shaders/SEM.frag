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

// vertex to fragment shader io
varying vec3 N;
varying vec3 I;
varying vec4 Cs;
varying vec3 LightDir;

// globals
uniform float edgefalloff;

// entry point
void main()
{
    float opac = dot(normalize(N), normalize(-LightDir));
    //float opac = dot(normalize(N), normalize(-I));
    opac = abs(opac);
    opac = 1.0-pow(opac, edgefalloff);
    //opac = 1.0 - opac;
    
    gl_FragColor =  opac * Cs;
 //   gl_FragColor.a = opac;
}
