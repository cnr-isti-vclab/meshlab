#include "EqHandle.h"

EqHandle::EqHandle()
{
	_barHeight = 100;
	_size = 4;

	QPointF a(-_size/2.0f,-_size);
	QPointF b(_size/2.0f, -_size);
	QPointF c(0,          -1.87f*_size);

	_triangle.append(QLineF(a,b));
	_triangle.append(QLineF(b,c));
	_triangle.append(QLineF(c,a));
}

/*
EqHandle::~EqHandle(void)
{
}*/

void EqHandle::setBarHeight(qreal height)
{
	_barHeight = height;
}

void EqHandle::setHistogramInfo (CHART_INFO *info)
{
	_histogramInfo = info;
}

void EqHandle::setType (EQUALIZER_HANDLE_TYPE type)
{
	_type = type;
}

void EqHandle::setHandlesPointer(EqHandle* pointer)
{
	_handlesPointer = pointer;
}

void EqHandle::setMidHandlePercentilePosition(qreal* pointer)
{
	_midHandlePercentilePosition = pointer;
}

void EqHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	QPen pen(Qt::black);
	pen.setWidth(2);
	painter->setPen(_color);
	painter->setBrush(_color);
	painter->drawLine(0, -_size, 0, -_barHeight);

	painter->drawLines(_triangle);
	painter->drawRect(-_size/2.0f, -_size, _size, _size);
}

QRectF EqHandle::boundingRect () const
{
	return QRectF(-_size/2.0f, -_barHeight, _size, _barHeight);
}

void EqHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);


	QPointF newPos = event->scenePos();
	if ( (newPos.x() < _histogramInfo->leftBorder) || (newPos.x() > _histogramInfo->rightBorder) )
		return;

	QPointF oldPos = pos();
	qreal handleOffset = newPos.x()-oldPos.x();
	if (handleOffset<0)
		handleOffset = -handleOffset;

	qreal leftx = _handlesPointer[LEFT_HANDLE].pos().x();
	qreal midx =  _handlesPointer[MID_HANDLE].pos().x();
	qreal rightx= _handlesPointer[RIGHT_HANDLE].pos().x();

	if (handleOffset >= std::numeric_limits<float>::epsilon())
	{
		switch (_type)
		{
		case MID_HANDLE:
			if ( (newPos.x() > _handlesPointer[LEFT_HANDLE].pos().x()) && (newPos.x() < _handlesPointer[RIGHT_HANDLE].pos().x()) )
			{
				*_midHandlePercentilePosition = calculateMidHandlePercentilePosition(newPos.x());
				moveMidHandle();
			}
			break;
		case LEFT_HANDLE:
			if (newPos.x() < _handlesPointer[RIGHT_HANDLE].pos().x()) 
			{
				setPos(newPos.x(), oldPos.y());
				qreal percentagePos = (pos().x()-_histogramInfo->leftBorder) / _histogramInfo->chartWidth;
				qreal newSpinboxValue = percentagePos * (_histogramInfo->maxX - _histogramInfo->minX) + _histogramInfo->minX;
				emit positionChangedToSpinBox((double)newSpinboxValue);
				emit positionChangedToMidHandle();
			}
			break;
		case RIGHT_HANDLE:
			if (newPos.x() > _handlesPointer[LEFT_HANDLE].pos().x()) 
			{
				setPos(newPos.x(), oldPos.y());
				qreal percentagePos = (pos().x()-_histogramInfo->leftBorder) / _histogramInfo->chartWidth;
				qreal newSpinboxValue = percentagePos * (_histogramInfo->maxX - _histogramInfo->minX) + _histogramInfo->minX;
				emit positionChangedToSpinBox((double)newSpinboxValue);
				emit positionChangedToMidHandle();
			}
			break;
		}
	}
}

void EqHandle::moveMidHandle()
{
	assert(_type==MID_HANDLE);
	qreal newPosX = _handlesPointer[LEFT_HANDLE].pos().x() + *_midHandlePercentilePosition * (_handlesPointer[RIGHT_HANDLE].pos().x() - _handlesPointer[LEFT_HANDLE].pos().x());

	setPos(newPosX, pos().y());

	qreal percentagePos = (newPosX - _histogramInfo->leftBorder) / _histogramInfo->chartWidth;
	qreal newSpinboxValue = percentagePos * (_histogramInfo->maxX - _histogramInfo->minX) + _histogramInfo->minX;
	// Rischio di ricorsione?
	emit positionChangedToSpinBox((double)newSpinboxValue);
}

void EqHandle::setXBySpinBoxValueChanged(double spinBoxValue)
{
	qreal percentageValue = (spinBoxValue -  _histogramInfo->minX) / (_histogramInfo->maxX - _histogramInfo->minX);
	qreal newHandleX = percentageValue * _histogramInfo->chartWidth + _histogramInfo->leftBorder;

	qreal handleOffset = newHandleX-pos().x();
	if (handleOffset<0)
		handleOffset = -handleOffset;
	// this control avoid counter invoking (?)
	if (handleOffset < std::numeric_limits<float>::epsilon())
		return;

	switch (_type)
	{
	case MID_HANDLE:
		if ( (newHandleX > _handlesPointer[LEFT_HANDLE].pos().x()) && (newHandleX < _handlesPointer[RIGHT_HANDLE].pos().x()) )
		{
			*_midHandlePercentilePosition = calculateMidHandlePercentilePosition(newHandleX);
			moveMidHandle();
		}
		break;
	case LEFT_HANDLE:
		if (newHandleX < _handlesPointer[RIGHT_HANDLE].pos().x()) 
		{
			setPos(newHandleX, pos().y());
		}
		break;
	case RIGHT_HANDLE:
		if (newHandleX > _handlesPointer[LEFT_HANDLE].pos().x()) 
		{
			setPos(newHandleX, pos().y());
		}		
		break;
	}
}
