#include "TFHandle.h"

//declaratio of static member of TFHandle class
TransferFunction* TFHandle::_tf = 0;

//TFHandle::TFHandle(int channel_code, int junction, CHART_INFO *environment_info) : _channelCode(channel_code), _junction_side(junction)
TFHandle::TFHandle(CHART_INFO *environment_info, QColor color, QPointF position, int myKeyIdx, int zOrder, int size  )
	: Handle(environment_info, color, position, zOrder, size  )
{
	COLOR_2_TYPE(color, _channelCode);
	_myKeyIndex = myKeyIdx;
	_toSwapIndex = _myKeyIndex;
	_toSwap = false;
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

		this->updateTfHandlesState(newPos);

		emit positionChanged(this);	
	}
}

void TFHandle::updateTfHandlesState(QPointF newPos)
{
	assert(_tf != 0);
	//updating the value of the key represented by this handle and updating the whole keys vector too
	_toSwapIndex = (*_tf)[this->getChannel()].updateKey( _myKeyIndex, absolute2RelativeValf( newPos.x()-_chartInfo->leftBorder, _chartInfo->chartWidth ), 1.0f-absolute2RelativeValf( newPos.y()-_chartInfo->upperBorder, _chartInfo->chartHeight ) );
	_toSwap = (_myKeyIndex != _toSwapIndex);
}


void TFHandle::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
	emit clicked(this);
}