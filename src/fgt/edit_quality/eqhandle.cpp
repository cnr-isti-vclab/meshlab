/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
History
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#include "eqhandle.h"

EqHandle::EqHandle(CHART_INFO *environment_info, QColor color, QPointF position, 
				   EQUALIZER_HANDLE_TYPE type, EqHandle** handles, qreal* midHandlePercentilePosition, QDoubleSpinBox* spinbox,
				   int zOrder, int size):
	Handle (environment_info, color, position, zOrder, size)
{
	setSize(size);
	_type = type;
	_barHeight = environment_info->chartHeight();

	_handlesPointer = handles;
	_midHandlePercentilePosition = midHandlePercentilePosition;
	_spinBoxPointer = spinbox;	

	// Setting triangle position above EqHandle
	_triangle[0] = QPointF(-_size/2,-_size);
	_triangle[1] = QPointF(_size/2, -_size);
	_triangle[2] = QPointF(0, -1.87f*_size);
}


EqHandle::~EqHandle(void)
{
}

void EqHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/ )
{
	painter->setPen(_color);
	painter->setBrush(_color);
	painter->drawLine(0, -_size, 0, -_barHeight);		// painting the vertical bar
	
	painter->drawPolygon ( _triangle, 3);				// painting the triangle above EqHandle
	painter->drawRect(-_size/2, -_size, _size, _size);	// painting the base rect of  EqHandle
}

QRectF EqHandle::boundingRect () const
{
	return QRectF(-_size/2, -_barHeight, _size, _barHeight);
}


/* Overload method implementing the movement of EqHandle
Note: Left [Right] handle can also move "outside" the histogram causing a stretching of it. 
Moving the handle again "inside" the histogram reset gradually its original size 
(the histogram is recalculated for every MouseMoveEvent call "outside" of it). 
The process of stretching is managed by QualityMapperDialog. 
The handle just notify its position relative to he histogram. 
*/
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
			// Mid handle can move only between left and right handle
			if ( (newPos.x() > _handlesPointer[LEFT_HANDLE]->pos().x()) && (newPos.x() < _handlesPointer[RIGHT_HANDLE]->pos().x()) )
			{
				*_midHandlePercentilePosition = calculateMidHandlePercentilePosition(newPos.x());
				moveMidHandle();
			}
			emit positionChanged(); // for gammaCorrectionLabel
			break;
		case LEFT_HANDLE:
			{
				// calculating new spinbox value associated to handle
				qreal newQualityValue = positionToQuality(newPos.x());
				if (newQualityValue < _spinBoxPointer->minimum()) 
					break;
				
				// Left handle can move only on the left of right handle
				if (newPos.x() < _handlesPointer[RIGHT_HANDLE]->pos().x()) 
				{					
					// Changing minimum/maximum value of others spinboxes
					_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->setMinimum(newQualityValue);
					_handlesPointer[MID_HANDLE]->_spinBoxPointer->setMinimum(newQualityValue);

					// Emitting new quality value of the handle to spinbox
					_spinBoxPointer->blockSignals(true);
					emit positionChangedToSpinBox((double)newQualityValue);
					_spinBoxPointer->blockSignals(false);

					// if left handle is "inside" histogram
					if ( newQualityValue >= _chartInfo->minX )
					{
						emit insideHistogram(this,true); // for redrawing equalizerHistogram bars, if needed
						setPos(newPos.x(), pos().y());
					}
					else
						emit insideHistogram(this,false);

					emit positionChanged();  // for redrawing transferFunctionScene and moving midHandle

				}
			}
			break;
		case RIGHT_HANDLE:
			{
				// calculating new spinbox value associated to handle
				qreal newQualityValue = positionToQuality(newPos.x());
				if (newQualityValue > _spinBoxPointer->maximum()) // Why this control??? It seems to be never reached!
					break;

				// Right handle can move only on the right of left handle
				if (newPos.x() > _handlesPointer[LEFT_HANDLE]->pos().x()) 
				{		
					// Changing minimum/maximum value of others spinboxes
					_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->setMaximum(newQualityValue);
					_handlesPointer[MID_HANDLE]->_spinBoxPointer->setMaximum(newQualityValue);

					// Emitting new quality value of the handle to spinbox
					_spinBoxPointer->blockSignals(true);
					emit positionChangedToSpinBox((double)newQualityValue);
					_spinBoxPointer->blockSignals(false);

					// if right handle is "inside" histogram
					if ( newQualityValue <= _chartInfo->maxX )
					{
						emit insideHistogram(this,true); // for redrawing equalizerHistogram bars, if needed
						setPos(newPos.x(), pos().y());
					}
					else
						emit insideHistogram(this,false);

					emit positionChanged();  // for redrawing transferFunctionScene and moving midHandle
				}
			}
			break;
		}
	}
}

/* 
Method for moving midHandle and setting its associated spinbox
*/
void EqHandle::moveMidHandle()
{
	assert(_type==MID_HANDLE);
	// Calculating new absolute position of the mid handle depending on its relative pos to the other handles
	qreal newPosX = _handlesPointer[LEFT_HANDLE]->pos().x() + *_midHandlePercentilePosition * (_handlesPointer[RIGHT_HANDLE]->pos().x() - _handlesPointer[LEFT_HANDLE]->pos().x());
	setPos(newPosX, pos().y());
	qreal newSpinboxValue = positionToQuality(newPosX);

	// Emitting new quality value of the handle to spinbox
	_spinBoxPointer->blockSignals(true);
	emit positionChangedToSpinBox((double)newSpinboxValue);
	_spinBoxPointer->blockSignals(false);

	//emit positionChanged(); // for gammaCorrectionLabel
}

/* 
Method invoked by spinbox value changing. Updates the position of the associated handle.
*/
void EqHandle::setXBySpinBoxValueChanged(double spinBoxValue)
{
	// Min and Max quality values are normally the values of the min e max spinboxes, but they are different if an handle is "outside" the histogram
	float maxQ = (_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->value() > _chartInfo->maxX)?_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->value():_chartInfo->maxX;
	float minQ = (_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->value()  < _chartInfo->minX)?_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->value() :_chartInfo->minX;
	qreal newHandleX;

	switch (_type)
	{
	case MID_HANDLE:
		newHandleX = qualityToPosition(spinBoxValue, minQ, maxQ);
		if ( (newHandleX > _handlesPointer[LEFT_HANDLE]->pos().x()) && (newHandleX < _handlesPointer[RIGHT_HANDLE]->pos().x()) )
		{
			*_midHandlePercentilePosition = calculateMidHandlePercentilePosition(newHandleX);
			moveMidHandle();
			emit positionChanged();  // for redrawing transferFunctionScene and moving midHandle
		}
		break;
	case LEFT_HANDLE:
		newHandleX = qualityToPosition(spinBoxValue, _chartInfo->minX, _chartInfo->maxX);
		if (newHandleX < _handlesPointer[RIGHT_HANDLE]->pos().x()) 
		{
			// Changing minimum/maximum value of others spinboxes
			_handlesPointer[RIGHT_HANDLE]->_spinBoxPointer->setMinimum(spinBoxValue);
			_handlesPointer[MID_HANDLE]->_spinBoxPointer->setMinimum(spinBoxValue);

			// if left handle is "inside" histogram
			if ( newHandleX >= _chartInfo->leftBorder() )
			{
				setPos(newHandleX, pos().y());
				emit insideHistogram(this,true); // for redrawing equalizerHistogram bars, if needed
			}
			else
			{
				setPos(_chartInfo->leftBorder(), pos().y());
				emit insideHistogram(this,false);
			}


			emit positionChanged();  // for redrawing transferFunctionScene and moving midHandle
		}
		break;
	case RIGHT_HANDLE:
		newHandleX = qualityToPosition(spinBoxValue, _chartInfo->minX, _chartInfo->maxX);
		if (newHandleX > _handlesPointer[LEFT_HANDLE]->pos().x()) 
		{
			// Changing minimum/maximum value of other spinboxes
			_handlesPointer[LEFT_HANDLE]->_spinBoxPointer->setMaximum(spinBoxValue);
			_handlesPointer[MID_HANDLE]->_spinBoxPointer->setMaximum(spinBoxValue);

			// if right handle is "inside" histogram
			if ( newHandleX <= _chartInfo->rightBorder() )
			{
				setPos(newHandleX, pos().y());
				emit insideHistogram(this,true); // invalidates equalizerHistogram scene
			}
			else
			{
				setPos(_chartInfo->rightBorder(), pos().y());
				emit insideHistogram(this,false);
			}



			emit positionChanged();  // for redrawing transferFunctionScene and moving midHandle
		}
		break;
	}
}

