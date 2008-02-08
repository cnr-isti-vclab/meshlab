#include "handle.h"


Handle::Handle(CHART_INFO *environment_info, QColor color, QPointF position, int zOrder, int size ) : _chartInfo(environment_info)
{
	_color = color;
	this->setPos( position );
	this->setZValue( zOrder );
	_size = size;

	setCursor(Qt::OpenHandCursor);
	setToolTip(QString("Drag me..."));
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
	setCursor(Qt::OpenHandCursor);
	emit handleReleased();
}
