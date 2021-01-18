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

#ifndef CLONEVIEW_H_
#define CLONEVIEW_H_

#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMouseEvent>

class CloneView : public QGraphicsView
{
	Q_OBJECT
	
private :
	QPointF last_pos;
	QPointF start_pos;
	
	QGraphicsItem * crosshair;

public :
	QGraphicsItem * scenegroup;
	
	CloneView(QWidget * parent) : QGraphicsView(parent)
	{
		setDragMode(QGraphicsView::NoDrag); setMouseTracking(false);
	}
	
	virtual void mousePressEvent(QMouseEvent * event) {
		start_pos = QPointF(event->pos());
		last_pos.setX(event->pos().x()); last_pos.setY(event->pos().y());

    if (event->buttons() == Qt::RightButton)
      emit positionReset();
  }
	
	virtual void mouseMoveEvent ( QMouseEvent * event)
	{
		if (!(event->buttons() & Qt::LeftButton)) return;
		QGraphicsItem * item = itemAt(event->pos());
		if (item != NULL) 
			if (!crosshair->isAncestorOf(item)) item->moveBy(event->pos().x() - last_pos.x(), event->pos().y() - last_pos.y());
		last_pos.setX(event->pos().x()); last_pos.setY(event->pos().y());
	//	emit positionChanged(last_pos.x() - start_pos.x(), last_pos.y() - start_pos.y());
	}
	
	virtual void mouseReleaseEvent ( QMouseEvent *)
	{
		emit positionChanged(last_pos.x() - start_pos.x(), last_pos.y() - start_pos.y());
	}
	
	virtual void wheelEvent(QWheelEvent *)
	{
		//ignore or move..
	}
	
	virtual void setScene(QGraphicsScene * scene)
	{
		QGraphicsView::setScene(scene);
		QList<QGraphicsItem*> gil;
		scenegroup = scene->createItemGroup(gil);
		crosshair = new QGraphicsItemGroup(scenegroup);
		crosshair->setZValue(2);
		QPen pen;
		pen.setWidth(3);
		pen.setColor(QColor(qRgb(255, 255, 255)));
		QGraphicsItem * p = scene->addLine(0, 8, 0, -8, pen);
		p->setParentItem(crosshair);
		QGraphicsItem * c = scene->addLine(8, 0, -8, 0, pen);
		c->setParentItem(p); p = c;
		pen.setWidth(1);
		pen.setColor(QColor(qRgb(0, 0, 0)));
		c = scene->addLine(0, 8, 0, -8, pen);
		c->setParentItem(p); p = c;
		c = scene->addLine(8, 0, -8, 0, pen);
		c->setParentItem(p); p = c;
	}
	
signals:
	void positionChanged(double x, double y);
	void positionReset();

};

#endif /*CLONEVIEW_H_*/
