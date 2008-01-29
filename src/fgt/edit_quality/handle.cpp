
#include "handle.h"


Handle::Handle()
{
	_size = 10.0f;
	_color = QColor(Qt::black);

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
}
