#ifndef _TFHANDLE_H_
#define _TFHANDLE_H_

#include "handle.h"
#include "util.h"



/* Specific handle for transgerFunctionScene 
*/
class TFHandle : public Handle
{
	Q_OBJECT

public:
	TFHandle();
	//~EqHandle(void);
	QRectF boundingRect () const;
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 
	

protected:
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);

private:


signals:


private slots:

};

#endif // EQHANDLE_H