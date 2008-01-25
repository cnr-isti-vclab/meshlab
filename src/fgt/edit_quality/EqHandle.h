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
	void setBarHeight (qreal);
	void setHistogramInfo (CHART_INFO*);
	void setType(EQUALIZER_HANDLE_TYPE);
	void setMidHandlePercentilePosition(qreal*);
	void setHandlesPointer(EqHandle*);
	void moveMidHandle();

protected:
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);

private:
	qreal			_barHeight;
	QVector<QLineF> _triangle;
	CHART_INFO*		_histogramInfo;
	EQUALIZER_HANDLE_TYPE _type;
	qreal*			_midHandlePercentilePosition;
	EqHandle*		_handlesPointer;

signals:
	void positionChangedToSpinBox(double);
	void positionChangedToMidHandle();

private slots:
	// changing equalizer spinboxes moves the connected handle
	void setXBySpinBoxValueChanged (double spinBoxValue);
};

#endif // EQHANDLE_H