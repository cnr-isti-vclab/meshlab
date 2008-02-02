#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <QObject>
#include <QGraphicsItem>
#include <QtGui>
#include <limits>
#include "util.h"

/* Drag&Drop generic handle to be used in QGrahicsScenes */
class Handle : public QObject, public QGraphicsItem
{
	Q_OBJECT

public:
	Handle(void);
	~Handle(void);
	inline void setSize (int size) {_size = size;};
	inline void setColor (QColor color) {_color = color;};
	inline void setChartInfo (CHART_INFO* info) {_chartInfo = info;};	
	
	// Overriding QGraphicsItem methods
	QRectF boundingRect () const;
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) = 0; 

protected:
	void mousePressEvent  (QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event) = 0;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	QColor _color;
	int _size;
	CHART_INFO*		_chartInfo;

private:



private slots:
	

};

#endif // HANDLE_H
