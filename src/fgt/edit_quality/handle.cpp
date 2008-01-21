#include <QtGui>
#include "handle.h"


Handle::Handle(int size=10, SHAPE shape=SQUARE_SHAPE)
{
	_size = size;
	_shape = shape;
	_color = QColor(Qt::black);

	setCursor(Qt::OpenHandCursor);
	setToolTip(QString("Drag me..."));
}

Handle::~Handle()
{

}

void Handle::setSize(int size)
{
	_size = size;

}

QRectF Handle::boundingRect () const
{
	return QRectF(-_size/2, -_size/2, _size, _size);
}

void Handle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	Q_UNUSED(option);
     Q_UNUSED(widget);
	painter->drawRect(-_size/2, -_size/2, _size, _size);

}


void Handle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() != Qt::LeftButton) {
         event->ignore();
         return;
     }

     setCursor(Qt::ClosedHandCursor);

	
} 

void Handle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QMimeData *data = new QMimeData;
	
	data->setColorData(Qt::green);

	QDrag *drag = new QDrag(event->widget());
	drag->setMimeData(data);
	drag->start();
	//setCursor(Qt::OpenHandCursor);
}

void Handle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);
}
