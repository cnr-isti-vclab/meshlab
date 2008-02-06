#ifndef _EQHANDLE_H_
#define _EQHANDLE_H_

//#include <QObject>
//#include <QGraphicsItem>
#include "handle.h"
//#include "util.h"

enum EQUALIZER_HANDLE_TYPE
{
	LEFT_HANDLE = 0,
	MID_HANDLE,
	RIGHT_HANDLE,
	NUMBER_OF_EQHANDLES
};

/* Specific handle for equalizerHistogramScene 
It can only be dragged horizontally */
class EqHandle : public Handle
{
	Q_OBJECT

public:
	EqHandle(CHART_INFO *environment_info, QColor color, QPointF position,   
		EQUALIZER_HANDLE_TYPE type, EqHandle** handles, qreal* midHandlePercentilePosition, QDoubleSpinBox* spinbox,
		int zOrder, int size);
	~EqHandle(void);
	QRectF boundingRect () const;
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 
	
	void setSize						(int size)					{(size%2==0) ? _size=size+1 : _size=size;};
	void setBarHeight					(qreal height)				{_barHeight = height;};
	void setType						(EQUALIZER_HANDLE_TYPE type){_type = type;};	
	void setMidHandlePercentilePosition (qreal* pointer)			{_midHandlePercentilePosition = pointer;};	
	void setHandlesPointer				(EqHandle** pointer)			{_handlesPointer = pointer;};	
	void setSpinBoxPointer				(QDoubleSpinBox* pointer)	{_spinBoxPointer = pointer;};
	

protected:
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);

private:
	EQUALIZER_HANDLE_TYPE _type;
	qreal			_barHeight;
	QPointF			_triangle[NUMBER_OF_EQHANDLES];
	qreal*			_midHandlePercentilePosition;
	EqHandle**		_handlesPointer;
	QDoubleSpinBox* _spinBoxPointer;

	qreal calculateMidHandlePercentilePosition(qreal newHandleX)
	{
		return (newHandleX - _handlesPointer[LEFT_HANDLE]->pos().x()) / (_handlesPointer[RIGHT_HANDLE]->pos().x() - _handlesPointer[LEFT_HANDLE]->pos().x());
	};

	qreal calculateSpinBoxValueFromHandlePosition(qreal newHandleX)
	{
		qreal percentagePos = (newHandleX-_chartInfo->leftBorder) / _chartInfo->chartWidth;
		return percentagePos * (_chartInfo->maxX - _chartInfo->minX) + _chartInfo->minX;
	};

signals:
	void positionChangedToSpinBox(double);
	void positionChanged();

private slots:
	// changing equalizer spinboxes moves the connected handle
	void setXBySpinBoxValueChanged (double spinBoxValue);
	void moveMidHandle();
};

#endif // EQHANDLE_H