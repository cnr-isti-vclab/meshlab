#include "TFHandle.h"

TFHandle::TFHandle()
{
	_size = 4;
}

/*
TFHandle::~TFHandle(void)
{
}*/



void TFHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setPen(_color);
	painter->setBrush(_color);
	painter->drawEllipse(-_size/2.0f, -_size/2.0f, _size, _size);
}

QRectF TFHandle::boundingRect () const
{
	return QRectF(-_size/2.0f, -_size/2.0f, _size, _size);
}

void TFHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);

	QPointF newPos = event->scenePos();
	//QPointF oldPos = pos();
	setPos(newPos);
}
