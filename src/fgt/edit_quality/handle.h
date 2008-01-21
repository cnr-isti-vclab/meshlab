#ifndef _HANDLE_H_
#define _HANDLE_H_

//#include <QObject>
#include <QGraphicsItem>

enum SHAPE 
{
	SQUARE_SHAPE = 0,
	NUMBER_OF_SHAPES
};

class Handle : public QObject, QGraphicsItem
{
	Q_OBJECT

public:
	Handle(int size, SHAPE shape);
	~Handle();
	void setSize (int size);
	// Overriding QGraphicsItem methods
	QRectF boundingRect () const;
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 

protected:
	void mousePressEvent  (QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	QColor _color;
	SHAPE _shape;
	int _size;

private slots:

};

#endif // HANDLE_H
