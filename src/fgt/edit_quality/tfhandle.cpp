#include "TFHandle.h"

//TFHandle::TFHandle(int channel_code, int junction, CHART_INFO *environment_info) : _channelCode(channel_code), _junction_side(junction)
TFHandle::TFHandle(CHART_INFO *environment_info, QColor color, QPointF position, TF_KEY *myKey, int zOrder, int size  )
	: Handle(environment_info, color, position, zOrder, size  )
{
	COLOR_2_TYPE(color, _channelCode);

	if ( _chartInfo != 0)
	{
		_xPosition = absolute2RelativeValf( this->x(), _chartInfo->leftBorder + _chartInfo->rightBorder );
		_yPosition = absolute2RelativeValf( this->y(), _chartInfo->upperBorder + _chartInfo->lowerBorder );
	}
	_myKey = myKey;
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
	//painter->drawEllipse(((qreal)-_size)/2.0f, -((qreal)_size)/2.0f, _size, _size);
	painter->drawRect(((qreal)-_size)/2.0f, -((qreal)_size)/2.0f, _size, _size);
}

QRectF TFHandle::boundingRect () const
{
	return QRectF(((qreal)-_size)/2.0f, ((qreal)-_size)/2.0f, _size, _size);
}

void TFHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);

	QPointF newPos = event->scenePos();
	newPos.setX(newPos.x()-(_size/2.0f));
	newPos.setY(newPos.y()-(_size/2.0f));

	//the handle can be moved only INSIDE the TF scene
	if (( newPos.x() >= _chartInfo->leftBorder ) && ( newPos.x() <= _chartInfo->rightBorder ) &&
		( newPos.y() >= _chartInfo->upperBorder ) && ( newPos.y() <= _chartInfo->lowerBorder ))
	{
		this->setPos(newPos);

		_myKey->x = absolute2RelativeValf( newPos.x(), _chartInfo->chartWidth );
		_myKey->y = absolute2RelativeValf( _chartInfo->chartHeight - newPos.y(), _chartInfo->chartHeight );


		emit positionChanged(this);
	}
}
