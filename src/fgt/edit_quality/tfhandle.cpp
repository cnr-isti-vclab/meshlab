#include "TFHandle.h"

TFHandle::TFHandle(int channel_code, int junction, CHART_INFO *environment_info) : _channelCode(channel_code), _junction_side(junction)
{
	_size = 4;

	_chartInfo = environment_info;

	if ( _chartInfo != 0)
	{
		_xPosition = absolute2RelativeValf( this->x(), _chartInfo->leftBorder + _chartInfo->rightBorder );
		_yPosition = absolute2RelativeValf( this->y(), _chartInfo->upperBorder + _chartInfo->lowerBorder );
	}
}

TFHandle::~TFHandle(void)
{
}



void TFHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	painter->setPen(_color);
	painter->setBrush(_color);
	painter->drawEllipse(((qreal)-_size)/2.0f, -((qreal)_size)/2.0f, _size, _size);
}

QRectF TFHandle::boundingRect () const
{
	return QRectF(((qreal)-_size)/2.0f, ((qreal)-_size)/2.0f, _size, _size);
}

void TFHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);

	QPointF newPos = event->scenePos();
	setPos(newPos);

	_xPosition = absolute2RelativeValf( newPos.x(), _chartInfo->leftBorder + _chartInfo->rightBorder );
	_yPosition = absolute2RelativeValf( newPos.y(), _chartInfo->upperBorder + _chartInfo->lowerBorder );

	emit positionChanged();
}
