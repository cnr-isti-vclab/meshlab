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
	TFHandle(CHART_INFO *environment_info, QColor color, QPointF position, int myKeyIdx, int zOrder=1, int size = DEFAULT_HANDLE_SIZE  );
	~TFHandle(void);

	QRectF boundingRect (void) const;
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 
	inline void setChannel(int ch_code)	{_channelCode = ch_code;}
	inline int getChannel(void) {return _channelCode;}
	inline float getXKey(void)	{return _xPosition;}
	inline float getYKey(void)	{return _yPosition;}
	inline int getMyKeyIndex()	{return _myKeyIndex;}
	inline int getToSwapIndex()	{return _toSwapIndex;}
	inline bool toSwap()		{return _toSwap;}
	inline void updateKeyCoord( float x, float y)	{_xPosition=x; _yPosition=y;}
	static void setTransferFunction(TransferFunction *tf)	{_tf = tf;}
	

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent * event);

private:
	int		_channelCode;
	float	_xPosition;
	float	_yPosition;
	int		_myKeyIndex;
	int		_toSwapIndex;
	bool	_toSwap;
	static TransferFunction *_tf;

signals:
	void clicked(TFHandle*);
	void positionChanged(TFHandle*);


private slots:

};

#endif // EQHANDLE_H