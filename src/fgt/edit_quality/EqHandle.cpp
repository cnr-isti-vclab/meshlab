#include "EqHandle.h"

EqHandle::EqHandle()
{
	_barHeight = 100;
	_size = 4;

	QPointF a(-_size/2.0f,-_size);
	QPointF b(_size/2.0f, -_size);
	QPointF c(0,          -1.87f*_size);

	_triangle.append(QLineF(a,b));
	_triangle.append(QLineF(b,c));
	_triangle.append(QLineF(c,a));
}

/*
EqHandle::~EqHandle(void)
{
}*/

void EqHandle::setBarHeight(qreal height)
{
	_barHeight = height;
}

void EqHandle::setHistogramInfo (CHART_INFO *info)
{
	_histogramInfo = info;
}

void EqHandle::paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget /*= 0*/ )
{
	Q_UNUSED(option);
    Q_UNUSED(widget);
	QPen pen(Qt::black);
	pen.setWidth(2);
	painter->setPen(_color);
	painter->setBrush(_color);
	painter->drawLine(0, -_size, 0, -_barHeight);
	
	painter->drawLines(_triangle);
	painter->drawRect(-_size/2.0f, -_size, _size, _size);
}

QRectF EqHandle::boundingRect () const
{
	return QRectF(-_size/2.0f, -_barHeight, _size, _barHeight);
}

void EqHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	setCursor(Qt::OpenHandCursor);

	QPointF newPos = event->scenePos();
	if ( (newPos.x() < _histogramInfo->leftBorder) || (newPos.x() > _histogramInfo->rightBorder) )
		return;
	
	/*QMimeData *data = new QMimeData;
	
	data->setColorData(Qt::green);

	QDrag *drag = new QDrag(event->widget());
	drag->setMimeData(data);
	drag->start();*/
	
	QPointF oldPos = pos();
	qreal handleOffset = newPos.x()-oldPos.x();
	if (handleOffset<0)
		handleOffset = -handleOffset;
	
	if (handleOffset >= std::numeric_limits<float>::epsilon())
	{
		setPos(newPos.x(), oldPos.y());
		qreal percentagePos = (pos().x()-_histogramInfo->leftBorder) / _histogramInfo->chartWidth;
		qreal newSpinboxValue = percentagePos * (_histogramInfo->maxX - _histogramInfo->minX) + _histogramInfo->minX;
		emit positionChanged((double)newSpinboxValue);
	}
}

void EqHandle::setX(double spinBoxValue)
{
	qreal percentageValue = (spinBoxValue -  _histogramInfo->minX) / (_histogramInfo->maxX - _histogramInfo->minX);
	qreal newHandleX = percentageValue * _histogramInfo->chartWidth + _histogramInfo->leftBorder;
	setPos(newHandleX, pos().y());
}
