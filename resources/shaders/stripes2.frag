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

varying vec3 pos;
uniform vec3 StripeDir;
uniform float Fuzz;
uniform float Width;//=0.5;
uniform float Scale;//=1.0;
void main (void) 
{
        vec4 color;
        float scalar_pos=dot(Scale*StripeDir,pos);
        float scaled_pos=fract(scalar_pos*10.0);
        color = gl_Color;
        	
        float frac1 = clamp(scaled_pos / Fuzz, 0.0, 1.0);
        float frac2 = clamp((scaled_pos - Width) / Fuzz, 0.0, 1.0);

	  frac1 = frac1 * (1.0 - frac2);
    	  frac1 = frac1 * frac1 * (3.0 - (2.0 * frac1));
           
        color.a=mix(0.0,1.0,frac1);
        gl_FragColor = color;
}
