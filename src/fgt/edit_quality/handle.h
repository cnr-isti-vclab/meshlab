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
	Handle(CHART_INFO *environment_info, QColor color, QPointF position, int zOrder=1, int size = DEFAULT_HANDLE_SIZE  );
	~Handle(void);
	inline void setSize (int size) {_size = size;};
	inline void setColor (QColor color) {_color = color;};
	inline float	getSize() {return _size;}
	
	// Overriding QGraphicsItem methods
	QRectF boundingRect () const;
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) = 0; 

protected:
	void mousePressEvent  (QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event) = 0;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	QColor _color;
	int _size;
	CHART_INFO*		_chartInfo;

private:
	

signals:
	void handleReleased();

private slots:
	

};

#endif // HANDLE_H
