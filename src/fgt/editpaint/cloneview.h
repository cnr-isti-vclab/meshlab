#ifndef CLONEVIEW_H_
#define CLONEVIEW_H_

#include <QtGui>

class CloneView : public QGraphicsView
{
	Q_OBJECT
	
private :
	QPointF last_pos;
	QPointF start_pos;
	
public :
	CloneView(QWidget * parent) : QGraphicsView(parent){setDragMode(QGraphicsView::NoDrag); setMouseTracking(false);}
	
	virtual void mousePressEvent(QMouseEvent * event) {
		start_pos = event->posF();
		last_pos.setX(event->posF().x()); last_pos.setY(event->posF().y());}
	
	virtual void mouseMoveEvent ( QMouseEvent * event)
	{
		if (!(event->buttons() & Qt::LeftButton)) return;
		QGraphicsItem * item = itemAt(event->pos());
		if (item != NULL) item->moveBy(event->posF().x() - last_pos.x(), event->posF().y() - last_pos.y());
		last_pos.setX(event->posF().x()); last_pos.setY(event->posF().y());
	//	emit positionChanged(last_pos.x() - start_pos.x(), last_pos.y() - start_pos.y());
	}
	
	virtual void mouseReleaseEvent ( QMouseEvent *)
	{
		emit positionChanged(last_pos.x() - start_pos.x(), last_pos.y() - start_pos.y());
	}
	
	virtual void wheelEvent(QWheelEvent *)
	{
		
	}
	
signals:
	void positionChanged(double x, double y);
		
};

#endif /*CLONEVIEW_H_*/
