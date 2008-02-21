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

#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <QObject>
#include <QGraphicsItem>
#include <QtGui>
#include <limits>
#include "common/util.h"

/* Drag&Drop generic handle to be used in QGrahicsScenes */
class Handle : public QObject, public QGraphicsItem
{
	Q_OBJECT

public:
	Handle(CHART_INFO *environment_info, QColor color, QPointF position, int zOrder=1, int size = DEFAULT_HANDLE_SIZE  );
	~Handle(void);
	inline void setSize (int size) {_size = size;};
	inline void setColor (QColor color) {_color = color;};
	inline float	getSize() {return _size;}
	
	// Overriding QGraphicsItem methods
	QRectF boundingRect () const;
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) = 0; 

protected:
	void mousePressEvent  (QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event) = 0;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	QColor _color;
	int _size;
	CHART_INFO*		_chartInfo;

private:
	

signals:
	void handleReleased();

private slots:
	

};

#endif // HANDLE_H
