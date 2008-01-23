#include "EqHandle.h"

EqHandle::EqHandle()
{
	_barHeight = 100;
}

/*
EqHandle::~EqHandle(void)
{
}*/

void EqHandle::setBarHeight(qreal height)
{
	_barHeight = height;
}

void EqHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	Q_UNUSED(option);
    Q_UNUSED(widget);
	painter->setPen(_color);
	painter->setBrush(_color);
	//painter->drawLine(0, -_size/2, 0, -_barHeight);
	painter->drawRect(-_size/2.0f, -_size/2.0f, _size, _size);
}

void EqHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QPointF itemPos = pos();
	setPos(event->scenePos().x(), itemPos.y());
	/*QMimeData *data = new QMimeData;
	
	data->setColorData(Qt::green);

	QDrag *drag = new QDrag(event->widget());
	drag->setMimeData(data);
	drag->start();*/
	setCursor(Qt::OpenHandCursor);
}

void EqHandle::moveBy(double offset)
{
	QPointF itemPos = pos();
	setPos(itemPos.x()+offset, itemPos.y());
}
