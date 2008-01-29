#ifndef _HANDLE_H_
#define _HANDLE_H_

#include <QObject>
#include <QGraphicsItem>
#include <QtGui>
#include <limits>

/* Drag&Drop generic handle to be used in QGrahicsScenes */
class Handle : public QObject, public QGraphicsItem
{
	Q_OBJECT

public:
	Handle(void);
	~Handle(void);
	void setSize (int size) {_size = size;};
	void setColor (QColor color) {_color = color;};
	
	// Overriding QGraphicsItem methods
	QRectF boundingRect () const;
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget) = 0; 

protected:
	void mousePressEvent  (QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event) = 0;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	QColor _color;
	int _size;

private:



private slots:
	

};

#endif // HANDLE_H
