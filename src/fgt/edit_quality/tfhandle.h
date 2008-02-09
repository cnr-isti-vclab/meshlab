#ifndef _TFHANDLE_H_
#define _TFHANDLE_H_

#include "handle.h"
#include "util.h"
#include "transferfunction.h"


/* Specific handle for TransferFunctionScene*/
class TFHandle : public Handle
{
	Q_OBJECT

public:
	TFHandle(CHART_INFO *environment_info, QColor color, QPointF position, TF_KEY *myKey, int zOrder=1, int size = DEFAULT_HANDLE_SIZE  );
	~TFHandle(void);

	QRectF boundingRect (void) const;
	void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget); 
	inline void setChannel(int ch_code)	{_channelCode = ch_code;}
	inline int getChannel(void) {return _channelCode;}
	inline TF_KEY *getMyKey(void)	{return _myKey;}
	inline float getRelativeX(void)	{return _myKey->x;}
	inline float getRelativeY(void)	{return _myKey->y;}
//	inline int getToSwapIndex(void)	{return _toSwapIndex;}
/*	inline bool toSwap(void)		{return _toSwap;}*/
	static void setTransferFunction(TransferFunction *tf)	{_tf = tf;}
	void updateTfHandlesState(QPointF newTfHanldePos);
	

protected:
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mousePressEvent(QGraphicsSceneMouseEvent * event);

private:
	int		_channelCode;
	TF_KEY	*_myKey;
// 	int		_toSwapIndex;
// 	bool	_toSwap;
	static TransferFunction *_tf;

signals:
	void clicked(TFHandle*);
	void positionChanged(TFHandle*);


private slots:

};

#endif // EQHANDLE_H