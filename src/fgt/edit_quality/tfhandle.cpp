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

#include "tfhandle.h"

//declaration of static member of TFHandle class
TransferFunction* TFHandle::_tf = 0;

//TFHandle::TFHandle(int channel_code, int junction, CHART_INFO *environment_info) : _channelCode(channel_code), _junction_side(junction)
TFHandle::TFHandle(CHART_INFO *environment_info, QColor color, QPointF position, TF_KEY *myKey, int zOrder, int size  )
	: Handle(environment_info, color, position, zOrder, size  )
{
	COLOR_2_TYPE(color, _channelCode);
	_myKey = myKey;
	_currentlySelected = false;
	this->setZValue( zOrder );

	//updating the position at logical state of the handle
	this->updateTfHandlesState(position);
}

TFHandle::~TFHandle(void)
{
}


//paint callback
void TFHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	//if the handle is the currently selected one a darker color is used to draw it
	if (_currentlySelected)
	{
		painter->setPen(_color.darker());
		painter->setBrush(_color.darker());
	}
	else
	{
		//else, simply use normal channel color
		painter->setPen(_color);
		painter->setBrush(_color);
	}

	painter->drawRect(((qreal)-_size)/2.0f, -((qreal)_size)/2.0f, _size, _size);
}

QRectF TFHandle::boundingRect () const
{
	return QRectF(((qreal)-_size)/2.0f, ((qreal)-_size)/2.0f, _size, _size);
}

//callback to manage the mouse move
void TFHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);

	//fetching new position in the scene
	QPointF newPos = event->scenePos();
	newPos.setX(newPos.x()-(_size/2.0f));
	newPos.setY(newPos.y()-(_size/2.0f));

	//the handle can be moved only INSIDE the TF scene
	if (( newPos.x() >= _chartInfo->leftBorder() ) && ( newPos.x() <= _chartInfo->rightBorder() ) &&
		( newPos.y() >= _chartInfo->upperBorder() ) && ( newPos.y() <= _chartInfo->lowerBorder() ))
	{
		//updating new position in the scene
		this->setPos(newPos);

		//updating the position at logical state of the handle
		this->updateTfHandlesState(newPos);

		emit positionChanged(this);	
	}
}

//updates the position at logical state of the handle
void TFHandle::updateTfHandlesState(QPointF newPos)
{
	assert(_tf != 0);
	//updating the position at logical state
	_myKey->x = absolute2RelativeValf( newPos.x()-_chartInfo->leftBorder(), _chartInfo->chartWidth() );
	_myKey->y = 1.0f-absolute2RelativeValf( newPos.y()-_chartInfo->upperBorder(), _chartInfo->chartHeight() );

	//key changed... restoring correct order
	(*_tf)[this->getChannel()].updateKeysOrder();
}


void TFHandle::mousePressEvent(QGraphicsSceneMouseEvent *)
{
	emit clicked(this);
}

void TFHandle::mouseDoubleClickEvent (QGraphicsSceneMouseEvent *event)
{
	Q_UNUSED(event);
	emit doubleClicked(this);
}