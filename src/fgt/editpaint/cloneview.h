#ifndef CLONEVIEW_H_
#define CLONEVIEW_H_

#include <QtGui>

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
		start_pos = event->posF();
		last_pos.setX(event->posF().x()); last_pos.setY(event->posF().y());}
	
	virtual void mouseMoveEvent ( QMouseEvent * event)
	{
		if (!(event->buttons() & Qt::LeftButton)) return;
		QGraphicsItem * item = itemAt(event->pos());
		if (item != NULL) 
			if (!crosshair->isAncestorOf(item)) item->moveBy(event->posF().x() - last_pos.x(), event->posF().y() - last_pos.y());
		last_pos.setX(event->posF().x()); last_pos.setY(event->posF().y());
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
		
		scenegroup = new QGraphicsItemGroup(NULL, scene);
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
		
};

#endif /*CLONEVIEW_H_*/
