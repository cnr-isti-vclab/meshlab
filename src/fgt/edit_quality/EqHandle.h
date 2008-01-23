#ifndef _EQHANDLE_H_
#define _EQHANDLE_H_

//#include <QObject>
//#include <QGraphicsItem>
#include "handle.h"
#include "util.h"

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

protected:
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);

private:
	qreal _barHeight;
	QVector<QLineF> _triangle;
	CHART_INFO* _histogramInfo;

signals:
	void positionChanged(qreal);

private slots:
	// changing equalizer spinboxes moves the connected handle
	void moveBy (double offset);
};

#endif // EQHANDLE_H