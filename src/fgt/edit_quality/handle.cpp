
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

void Handle::setSize(qreal size)
{
	_size = size;
}

void Handle::setColor(QColor color)
{
	_color = color;
}

QRectF Handle::boundingRect () const
{
	return QRectF(-_size/2, -_size/2, _size, _size);
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
