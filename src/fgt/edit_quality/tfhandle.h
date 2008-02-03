#ifndef _TFHANDLE_H_
#define _TFHANDLE_H_

#include "handle.h"
#include "util.h"
#include "transferfunction.h"



/* Specific handle for transgerFunctionScene 
*/
class TFHandle : public Handle
{
	Q_OBJECT

public:
	enum { LEFT_JUNCTION_HANDLE = 0, RIGHT_JUNCTION_HANDLE	};

	TFHandle(CHART_INFO *environment_info, QColor color, QPointF position, int junction, int zOrder=1, int size = DEFAULT_HANDLE_SIZE  );
	~TFHandle(void);

	QRectF boundingRect () const;
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 
	inline void setChannel(int ch_code)	{_channelCode = ch_code;}
	inline int getChannel(void) {return _channelCode;}
	inline float getXPosition(void)	{return _xPosition;}
	inline float getYPosition(void)	{return _yPosition;}
	

protected:
	void mouseMoveEvent   (QGraphicsSceneMouseEvent *event);

private:
	int		_channelCode;
	float	_xPosition;
	float	_yPosition;
	int		_junction_side;

signals:
	void positionChanged();


private slots:

};

#endif // EQHANDLE_H