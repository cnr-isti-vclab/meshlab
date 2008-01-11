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


#include <vcg/math/base.h>
#include <vector>
#include <assert.h>

/* Structure containing 3 float with values included between 0 and 1 */
struct Key
{
private:
	float position;
	float upper;
	float lower;

public:
	Key(float p, float u, float l);
}

/* Representation of a transfer function as a triple of vectors of Keys, 
one for each color (RGB)*/
class TransferFunction 
{
private:
	std::vector <Key> red;
	std::vector <Key> green;
	std::vector <Key> blue;

public:
	TransferFunction();
	~TransferFunction();
}


Key::Key(float p, float u, float l)
{
	assert (l<=u);
	position = p;
	upper = u;
	lower = l;
}


TransferFunction::TransferFunction()
{
}

TransferFunction::~TransferFunction()
{
}