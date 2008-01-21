#ifndef HANDLE_H
#define HANDLE_H

//#include <QObject>
#include <QGraphicsItem>

enum SHAPE 
{
	SQUARE_SHAPE = 0,
	NUMBER_OF_SHAPES
};

class Handle : public QGraphicsItem
{
	Q_OBJECT

public:
	Handle(int,SHAPE);
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
};

#endif // HANDLE_H
