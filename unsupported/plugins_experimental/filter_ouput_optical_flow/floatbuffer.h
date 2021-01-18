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
#ifndef FLOATBUFFER_H
#define FLOATBUFFER_H

#include<vcg/math/shot.h>
#include<vcg/math/histogram.h>

#include<QString>
#include<QImage>
#include<queue>

using namespace std;
using namespace vcg;


class floatbuffer
{
 public:

	float* data;

	int sx,sy;
    float sobelMin, sobelMax;

	int loaded;	// -1 not created
				      //  1 loaded
				      //  0 unloaded

	QString filename; // when unloaded, offcore filename

	floatbuffer(void);
	floatbuffer(floatbuffer *from);
	~floatbuffer(void);

	int init(int sizex, int sizey);
	int destroy();

	float getval(int xx, int yy);

	void applysobel(floatbuffer *from);
	void initborder(floatbuffer* zerofrom);
	void distancefield();
};


#endif // FLOATBUFFER_H
