#include "EqHandle.h"

EqHandle::EqHandle(CHART_INFO *environment_info, QColor color, QPointF position, 
				   EQUALIZER_HANDLE_TYPE type, EqHandle** handles, qreal* midHandlePercentilePosition, QDoubleSpinBox* spinbox,
				   int zOrder, int size):
	Handle (environment_info, color, position, zOrder, size)
{
	setSize(size);
	setBarHeight(environment_info->chartHeight);

	_type = type;
	_handlesPointer = handles;
	_midHandlePercentilePosition = midHandlePercentilePosition;
	_spinBoxPointer = spinbox;	

	_triangle[0] = QPointF(-_size/2,-_size);
	_triangle[1] = QPointF(_size/2, -_size);
	_triangle[2] = QPointF(0, -1.87f*_size);
}


EqHandle::~EqHandle(void)
{
}


void EqHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	//Q_UNUSED(option);
	//Q_UNUSED(widget);
	//QPen pen(Qt::black);
	//non sepen.setWidth(2);
	painter->setPen(_color);
	painter->setBrush(_color);
	painter->drawLine(0, -_size, 0, -_barHeight);
	
	painter->drawPolygon ( _triangle, 3);
	//painter->drawLines(_triangle);
	painter->drawRect(-_size/2, -_size, _size, _size);
}

QRectF EqHandle::boundingRect () const
{
	return QRectF(-_size/2, -_barHeight, _size, _barHeight);
}

void EqHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);


	QPointF newPos = event->scenePos();
	/*
	if ( (newPos.x() < _chartInfo->leftBorder) || (newPos.x() > _chartInfo->rightBorder) )
		return;
*/
	QPointF oldPos = pos();
	qreal handleOffset = abs(newPos.x()-oldPos.x());

	/* for testing only
	qreal leftx = _handlesPointer[LEFT_HANDLE]->pos().x();
	qreal midx =  _handlesPointer[MID_HANDLE]->pos().x();
	qreal rightx= _handlesPointer[RIGHT_HANDLE]->pos().x();
	*/

	if (handleOffset >= std::numeric_limits<float>::epsilon())
	{
		switch (_type)
		{
		case MID_HANDLE:
			if ( (newPos.x() > _handlesPointer[LEFT_HANDLE]->pos().x()) && (newPos.x() < _handlesPointer[RIGHT_HANDLE]->pos().x()) )
			{
				*_midHandlePercentilePosition = calculateMidHandlePercentilePosition(newPos.x());
				moveMidHandle();
				emit positionChanged(); // for gammaCorrectionLabel
			}
			break;
		case LEFT_HANDLE:
			/*if (newPos.x() < _handlesPointer[RIGHT_HANDLE]->pos().x()) 
			{
				setPos(newPos.x(), oldPos.y());
				qreal newSpinboxValue = positionToQuality(pos().x());
				_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->setMinimum(newSpinboxValue);
				_spinBoxPointer->blockSignals(true);
				emit positionChangedToSpinBox((double)newSpinboxValue);
				_spinBoxPointer->blockSignals(false);
				emit positionChanged();
			}*/
			{
				// calculating new spinbox value
				qreal newQualityValue = positionToQuality(newPos.x());
				if (newQualityValue < _spinBoxPointer->minimum()) 
					break;
				
				if (newPos.x() < _handlesPointer[RIGHT_HANDLE]->pos().x()) 
				{					
					// Changing minimum/maximum value of opposite spinbox
					_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->setMinimum(newQualityValue);
					// Emitting signals to spinbox and mid handle
					_spinBoxPointer->blockSignals(true);
					emit positionChangedToSpinBox((double)newQualityValue);
					_spinBoxPointer->blockSignals(false);

					if (  newQualityValue < _chartInfo->minX )
						emit invalidateHistogram();
					else
					{
						setPos(newPos.x(), oldPos.y());
						emit positionChanged();  // for redrawing transferFunctionScene and moving mid equalizerHistogram Handle
					}					
				}
			}
			break;
		case RIGHT_HANDLE:
			/*if (newPos.x() > _handlesPointer[LEFT_HANDLE]->pos().x()) 
			{
				setPos(newPos.x(), oldPos.y());
				qreal newSpinboxValue = positionToQuality(pos().x());
				_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->setMaximum(newSpinboxValue);
				_spinBoxPointer->blockSignals(true);
				emit positionChangedToSpinBox((double)newSpinboxValue);
				_spinBoxPointer->blockSignals(false);
				emit positionChanged();
			}*/
			{
				// calculating new spinbox value
				qreal newQualityValue = positionToQuality(newPos.x());
				if (newQualityValue > _spinBoxPointer->maximum()) 
					break;
				
				if (newPos.x() > _handlesPointer[LEFT_HANDLE]->pos().x()) 
				{					
					// Changing minimum/maximum value of opposite spinbox
					_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->setMaximum(newQualityValue);
					// Emitting signals to spinbox and mid handle
					_spinBoxPointer->blockSignals(true);
					emit positionChangedToSpinBox((double)newQualityValue);
					_spinBoxPointer->blockSignals(false);

					if (  newQualityValue > _chartInfo->maxX )
						emit invalidateHistogram();
					else
					{
						setPos(newPos.x(), oldPos.y());
						emit positionChanged();  // for redrawing transferFunctionScene and moving mid equalizerHistogram Handle
					}					
				}
			}

			break;
		}

		
	}
	
}

void EqHandle::moveMidHandle()
{
	assert(_type==MID_HANDLE);
	qreal newPosX = _handlesPointer[LEFT_HANDLE]->pos().x() + *_midHandlePercentilePosition * (_handlesPointer[RIGHT_HANDLE]->pos().x() - _handlesPointer[LEFT_HANDLE]->pos().x());
	setPos(newPosX, pos().y());
	qreal newSpinboxValue = positionToQuality(newPosX);

	_spinBoxPointer->blockSignals(true);
	emit positionChangedToSpinBox((double)newSpinboxValue);
	_spinBoxPointer->blockSignals(false);

}


void EqHandle::setXBySpinBoxValueChanged(double spinBoxValue)
{
	qreal percentageValue = (spinBoxValue -  _chartInfo->minX) / (_chartInfo->maxX - _chartInfo->minX);
	qreal newHandleX = percentageValue * _chartInfo->chartWidth + _chartInfo->leftBorder;

	qreal handleOffset = abs(newHandleX-pos().x());
	// this control avoid counter invoking (?)
	if (handleOffset < std::numeric_limits<float>::epsilon())
		return;

	switch (_type)
	{
	case MID_HANDLE:
		if ( (newHandleX > _handlesPointer[LEFT_HANDLE]->pos().x()) && (newHandleX < _handlesPointer[RIGHT_HANDLE]->pos().x()) )
		{
			*_midHandlePercentilePosition = calculateMidHandlePercentilePosition(newHandleX);
			moveMidHandle();
		}
		break;
	case LEFT_HANDLE:
		/*
		if (newHandleX < _handlesPointer[RIGHT_HANDLE]->pos().x()) 
		{
			setPos(newHandleX, pos().y());
			_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->setMinimum(spinBoxValue);
			emit positionChanged();
		}*/
		if (newHandleX < _handlesPointer[RIGHT_HANDLE]->pos().x()) 
		{
			if ( /*(pos().x() < _chartInfo->leftBorder) ||*/ newHandleX < _chartInfo->leftBorder )
				emit invalidateHistogram();
			else
			{
				setPos(newHandleX, pos().y());
				_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->setMinimum(spinBoxValue);
				emit positionChanged();
			}
		}
		break;
	case RIGHT_HANDLE:
		/*if (newHandleX > _handlesPointer[LEFT_HANDLE]->pos().x()) 
		{

			setPos(newHandleX, pos().y());
			_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->setMaximum(spinBoxValue);
			emit positionChanged();
		}*/
		if (newHandleX > _handlesPointer[LEFT_HANDLE]->pos().x()) 
		{
			if ( /*(pos().x() < _chartInfo->leftBorder) ||*/ newHandleX > _chartInfo->rightBorder )
				emit invalidateHistogram();
			else
			{
				setPos(newHandleX, pos().y());
				_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->setMaximum(spinBoxValue);
				emit positionChanged();
			}
		}
		break;
	}
}

