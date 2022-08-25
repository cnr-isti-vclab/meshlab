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

#ifndef _TFHANDLE_H_
#define _TFHANDLE_H_

#include "handle.h"
#include "common/transferfunction.h"


//this class defines a specialized type of handle used to build and modify a Transfer Function
class TFHandle : public Handle
{
	Q_OBJECT

public:
	TFHandle(CHART_INFO *environment_info, QColor color, QPointF position, TF_KEY *myKey, int zOrder=1, int size = DEFAULT_HANDLE_SIZE  );
	~TFHandle(void);

	QRectF	boundingRect (void) const;
	void	paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
	inline void		setChannel(int ch_code)	{_channelCode = ch_code;}
	inline int		getChannel(void)		{return _channelCode;}
	inline TF_KEY	*getMyKey(void)			{return _myKey;}
	inline float	getRelativeX(void)		{return _myKey->x;}
	inline float	getRelativeY(void)		{return _myKey->y;}
	inline void		setCurrentlSelected(bool selected){_currentlySelected=selected;}
	inline bool		isCurrentlySelected()	{return _currentlySelected;}
	static void		setTransferFunction(TransferFunction *tf)	{_tf = tf;}
	void			updateTfHandlesState(QPointF newTfHanldePos);
	

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseDoubleClickEvent (QGraphicsSceneMouseEvent *event);

private:
	//identifier of the code belonging to Handle
	int		_channelCode;
	//pointer to referenced TF_KEY
	TF_KEY	*_myKey;
	//pointer to current Transfer Function object
	static TransferFunction *_tf;
	//tells if this handle is the currently selected one
	bool	_currentlySelected;

signals:
	void clicked(TFHandle*);
	void doubleClicked(TFHandle*);
	void positionChanged(TFHandle*);

private slots:

};

#endif // EQHANDLE_H