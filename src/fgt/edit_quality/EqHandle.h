#ifndef _EQHANDLE_H_
#define _EQHANDLE_H_

//#include <QObject>
//#include <QGraphicsItem>
#include "handle.h"

/* Specific handle for equalizerHistogramScene 
It can only be dragged horizontally */
class EqHandle : public Handle
{
	Q_OBJECT

public:
	EqHandle();
	//~EqHandle(void);
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 
	void setBarHeight (qreal);

protected:
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);

private:
	qreal _barHeight;

private slots:
	// changing equalizer spinboxes moves the connected handle
	void moveBy (double offset);
};

#endif // EQHANDLE_H