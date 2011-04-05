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

#include "handle.h"


Handle::Handle(CHART_INFO *environment_info, QColor color, QPointF position, int zOrder, int size ) : _chartInfo(environment_info)
{
	_color = color;
	this->setPos( position );
	this->setZValue( zOrder );
	_size = size;

	setCursor(Qt::OpenHandCursor);
	//setToolTip(QString("Drag me..."));
}

Handle::~Handle()
{
}


QRectF Handle::boundingRect () const
{
	return QRectF(((qreal)-_size)/2.0f, ((qreal)-_size)/2.0f, _size, _size);
}


void Handle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() != Qt::LeftButton) {
         event->ignore();
         return;
     }
     setCursor(Qt::ClosedHandCursor);
} 


void Handle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);
	setCursor(Qt::OpenHandCursor);
	emit handleReleased();
}
