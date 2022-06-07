/*****************************************************************************
 * MeshLab                                                           o o     *
 * Visual and Computer Graphics Library                            o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2022                                           \/)\/    *
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

#ifndef MESHLAB_POSITION_WIDGET_H
#define MESHLAB_POSITION_WIDGET_H

#include "point3_widget.h"

class PositionWidget : public Point3Widget
{
	Q_OBJECT
public:
	PositionWidget(
		QWidget*            p,
		const RichPosition& param,
		const Point3Value&  defaultValue,
		QWidget*            gla);
	~PositionWidget();

public slots:
	void getPoint();

signals:
	void askViewPos(QString);
	void askSurfacePos(QString);
	void askCameraPos(QString);
	void askTrackballPos(QString);
};

#endif // MESHLAB_POSITION_WIDGET_H
