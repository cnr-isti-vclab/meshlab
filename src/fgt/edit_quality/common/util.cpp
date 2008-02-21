/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
History
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#include "util.h"
#include <cmath>

//returns a relative-absolute x value conversion rounded to closer integer value
float relative2AbsoluteValf(float relative_val, float max_val)
{	return (relative_val * max_val);	}

//returns a relative-absolute x value conversion rounded to closer integer value
int relative2AbsoluteVali(float relative_val, float max_val)
{	return (int)(relative2AbsoluteValf(relative_val, max_val)+0.5f);	}

//returns an absolute-relative x value conversion
float absolute2RelativeValf(float absolute_val, float max_val)
{	assert(max_val!=0);	return (absolute_val / max_val);	}

//returns an absolute-relative x value conversion
int absolute2RelativeVali(float absolute_val, float max_val)
{	return (int)(absolute2RelativeValf(absolute_val, max_val)+0.5f);	}

float relative2QualityValf(float relative_val, float min_q, float max_q, float exp)
{ 
	assert( (relative_val>=0.0f) && (relative_val<=1.0f) );
	assert(min_q<=max_q);
	relative_val = pow( relative_val, exp ); // applying exponential function to relative val
	return ( relative_val * (max_q - min_q) ) + min_q;// Converting relative val in absolute quality
}


