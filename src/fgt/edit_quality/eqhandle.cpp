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


void EqHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/ )
{
	//QPen pen(Qt::black);
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
	qreal handleOffset = abs(newPos.x()-pos().x());

	if (handleOffset >= std::numeric_limits<float>::epsilon())
	{
		switch (_type)
		{
		case MID_HANDLE:
			if ( (newPos.x() > _handlesPointer[LEFT_HANDLE]->pos().x()) && (newPos.x() < _handlesPointer[RIGHT_HANDLE]->pos().x()) )
			{
				*_midHandlePercentilePosition = calculateMidHandlePercentilePosition(newPos.x());
				moveMidHandle();
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
					_handlesPointer[MID_HANDLE]->_spinBoxPointer->setMinimum(newQualityValue);

					// Emitting signals to spinbox and mid handle
					_spinBoxPointer->blockSignals(true);
					emit positionChangedToSpinBox((double)newQualityValue);
					_spinBoxPointer->blockSignals(false);

					if ( newQualityValue >= _chartInfo->minX )
					{
						emit insideHistogram(this,true); // for redrawing equalizerHistogram bars, if needed
						setPos(newPos.x(), pos().y());
					}
					else
						emit insideHistogram(this,false);

					emit positionChanged();  // for redrawing transferFunctionScene and moving  equalizerHistogram midHandle

				}
			}
			break;
		case RIGHT_HANDLE:
			{
				// calculating new spinbox value
				qreal newQualityValue = positionToQuality(newPos.x());
				if (newQualityValue > _spinBoxPointer->maximum()) // Why this control??? It seems to be never reached!
					break;
				
				if (newPos.x() > _handlesPointer[LEFT_HANDLE]->pos().x()) 
				{		
					// Changing minimum/maximum value of opposite spinbox
					_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->setMaximum(newQualityValue);
					_handlesPointer[MID_HANDLE]->_spinBoxPointer->setMaximum(newQualityValue);

					// Emitting signals to spinbox and mid handle
					_spinBoxPointer->blockSignals(true);
					emit positionChangedToSpinBox((double)newQualityValue);
					_spinBoxPointer->blockSignals(false);

					if ( newQualityValue <= _chartInfo->maxX )
					{
						emit insideHistogram(this,true); // for redrawing equalizerHistogram bars, if needed
						setPos(newPos.x(), pos().y());
					}
					else
						emit insideHistogram(this,false);

					emit positionChanged();  // for redrawing transferFunctionScene and moving  equalizerHistogram midHandle
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

	emit positionChanged(); // for gammaCorrectionLabel
}


void EqHandle::setXBySpinBoxValueChanged(double spinBoxValue)
{
	float maxQ = (_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->value() > _chartInfo->maxX)?_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->value():_chartInfo->maxX;
	float minQ = (_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->value()  < _chartInfo->minX)?_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->value() :_chartInfo->minX;
	qreal newHandleX;
	/*
	qreal handleOffset = abs(newHandleX-pos().x());
	// this control avoid counter invoking (?)
	if (handleOffset < std::numeric_limits<float>::epsilon())
		return;*/

	switch (_type)
	{
	case MID_HANDLE:
		newHandleX = qualityToPosition(spinBoxValue, minQ, maxQ);
		if ( (newHandleX > _handlesPointer[LEFT_HANDLE]->pos().x()) && (newHandleX < _handlesPointer[RIGHT_HANDLE]->pos().x()) )
		{
			*_midHandlePercentilePosition = calculateMidHandlePercentilePosition(newHandleX);
			moveMidHandle();
		}
		break;
	case LEFT_HANDLE:
		newHandleX = qualityToPosition(spinBoxValue, _chartInfo->minX, _chartInfo->maxX);
		if (newHandleX < _handlesPointer[RIGHT_HANDLE]->pos().x()) 
		{
			// Changing minimum/maximum value of other spinboxes
			_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->setMinimum(spinBoxValue);
			_handlesPointer[MID_HANDLE]->_spinBoxPointer->setMinimum(spinBoxValue);
			if ( newHandleX < _chartInfo->leftBorder )
				emit insideHistogram(this,false);
			else
			{
				emit insideHistogram(this,true); // invalidates equalizerHistogram scene
				setPos(newHandleX, pos().y());
				emit positionChanged(); // signal for drawGammaCorrection and minSpinBox
			}
			
		}
		break;
	case RIGHT_HANDLE:
		newHandleX = qualityToPosition(spinBoxValue, _chartInfo->minX, _chartInfo->maxX);
		if (newHandleX > _handlesPointer[LEFT_HANDLE]->pos().x()) 
		{
			// Changing minimum/maximum value of other spinboxes
			_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->setMaximum(spinBoxValue);
			_handlesPointer[MID_HANDLE]->_spinBoxPointer->setMaximum(spinBoxValue);
			if ( newHandleX > _chartInfo->rightBorder )
				emit insideHistogram(this,false);
			else
			{
				emit insideHistogram(this,true); // invalidates equalizerHistogram scene
				setPos(newHandleX, pos().y());
				emit positionChanged(); // signal for drawGammaCorrection and maxSpinBox
			}
		}
		break;
	}
}

