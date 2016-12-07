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

#ifndef ML_SELECTION_BUFFERS
#define ML_SELECTION_BUFFERS

#include <QReadWriteLock>
#include <vector>
#include <common/meshmodel.h>

class MLSelectionBuffers
{
public:
	MLSelectionBuffers(MeshModel& m,unsigned int primitivebatch);
	~MLSelectionBuffers();

	enum ML_SELECTION_TYPE {ML_PERVERT_SEL = 0,ML_PERFACE_SEL = 1};

	void updateBuffer(ML_SELECTION_TYPE selbuf);
	void drawSelection(ML_SELECTION_TYPE selbuf) const;
	void deallocateBuffer(ML_SELECTION_TYPE selbuf);
	void setPointSize(float ptsz);
private:
	mutable QReadWriteLock _lock;

	MeshModel& _m;
	unsigned int _primitivebatch;
	typedef std::vector<GLuint> SelectionBufferNames;
	typedef std::vector< SelectionBufferNames > SelMap;
	SelMap _selmap;
	float _pointsize;
};

#endif