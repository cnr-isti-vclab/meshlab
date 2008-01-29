#ifndef _EQHANDLE_H_
#define _EQHANDLE_H_

//#include <QObject>
//#include <QGraphicsItem>
#include "handle.h"
#include "util.h"

enum EQUALIZER_HANDLE_TYPE
{
	LEFT_HANDLE = 0,
	MID_HANDLE,
	RIGHT_HANDLE,
	NUMBER_OF_POSITIONS
};

/* Specific handle for equalizerHistogramScene 
It can only be dragged horizontally */
class EqHandle : public Handle
{
	Q_OBJECT

public:
	EqHandle();
	//~EqHandle(void);
	QRectF boundingRect () const;
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 
	
	void setSize (int size) {(size%2==0) ? _size=size+1 : _size=size;};
	void setBarHeight (qreal height) {_barHeight = height;};
	void setHistogramInfo (CHART_INFO* info) {_histogramInfo = info;};	
	void setType (EQUALIZER_HANDLE_TYPE type) {_type = type;};	
	void setMidHandlePercentilePosition (qreal* pointer) {_midHandlePercentilePosition = pointer;};	
	void setHandlesPointer (EqHandle* pointer) {_handlesPointer = pointer;};	
	void setSpinBoxPointer (QDoubleSpinBox* pointer){_spinBoxPointer = pointer;};
	

protected:
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);

private:
	/*
	qreal leftBorder;
	qreal center = 0;
	qreal rightBorder; */
	qreal			_barHeight;
	QVector<QLineF> _triangle;
	CHART_INFO*		_histogramInfo;
	EQUALIZER_HANDLE_TYPE _type;
	qreal*			_midHandlePercentilePosition;
	EqHandle*		_handlesPointer;
	QDoubleSpinBox* _spinBoxPointer;

	qreal calculateMidHandlePercentilePosition(qreal newHandleX)
	{
		return (newHandleX - _handlesPointer[LEFT_HANDLE].pos().x()) / (_handlesPointer[RIGHT_HANDLE].pos().x() - _handlesPointer[LEFT_HANDLE].pos().x());
	};

	qreal calculateSpinBoxValueFromHandlePosition(qreal xPos)
	{
		qreal percentagePos = (xPos-_histogramInfo->leftBorder) / _histogramInfo->chartWidth;
		return percentagePos * (_histogramInfo->maxX - _histogramInfo->minX) + _histogramInfo->minX;
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