#include "qualitymapperdialog.h"
#include <limits>
#include <QPen>
#include <QBrush>


QualityMapperDialog::QualityMapperDialog(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	_histogram_info = 0;
	_transferFunction_info = 0;
}

QualityMapperDialog::~QualityMapperDialog()
{
	if ( _histogram_info )
	{
		delete _histogram_info;
		_histogram_info = 0;
	}

	if ( _transferFunction_info )
	{
		delete _transferFunction_info;
		_transferFunction_info = 0;
	}
}

void QualityMapperDialog::setValues(const QualityMapperSettings& qms)
{
	_settings=qms;
	QString qnum="%1";
	ui.minSpinBox->setValue(_settings.meshMinQ);
	ui.midSpinBox->setValue(_settings.meshMidQ);
	ui.maxSpinBox->setValue(_settings.meshMaxQ);
}

QualityMapperSettings QualityMapperDialog::getValues()
{
	_settings.manualMinQ=ui.minSpinBox->value();
	_settings.manualMidQ=ui.midSpinBox->value();
	_settings.manualMaxQ=ui.maxSpinBox->value();

	return _settings;
}


void QualityMapperDialog::drawChartBasics(QGraphicsScene& scene, QGraphicsView *view, CHART_INFO *chart_info)
{

	//a valid view must be passed!
	assert(view != 0);

	assert( chart_info != 0 );

	QPen p( Qt::black, 2 );

	//drawing axis
	//x axis
	scene.addLine( chart_info->leftBorder, chart_info->lowerBorder, chart_info->rightBorder, chart_info->lowerBorder, p );
	//y axis
	scene.addLine( chart_info->leftBorder, chart_info->upperBorder, chart_info->leftBorder, chart_info->lowerBorder, p );
}

//_equalizerScene dovrebbe chiamarsi in realtà histogramScene
//l'histogram e la transfer function potrebbero diventare attributi di questa classe? valutare l'impatto.
//in generale il codice di questo metodo va ripulito un po'...
void QualityMapperDialog::drawEqualizerHistogram( Histogramf& h )
{
	//building histogram chart informations
	if ( _histogram_info == 0 )
	{
		//processing minY and maxY values for histogram
		int maxY = 0;
		int minY = std::numeric_limits<int>::max();
		for (int i=0; i<h.n; i++) 
		{
			if ( h.H[i] > maxY )
				maxY = h.H[i];

			if ( h.H[i] < minY )
				minY = h.H[i];
		}
		_histogram_info = new CHART_INFO( ui.equalizerGraphicsView->width(), ui.equalizerGraphicsView->height(), h.n, h.minv, h.maxv, minY, maxY );
	}

	//drawing axis and other basic items
	this->drawChartBasics( _equalizerScene, ui.equalizerGraphicsView, _histogram_info );

	float barHeight = 0.0f;					//initializing height of the histogram bars
	float barWidth = _histogram_info->dX;	//processing width of the histogram bars (4\5 of dX)
//	float barSeparator = dX - barWidth;        //processing space between consecutive bars of the histogram bars (1\5 of dX)

	QPen drawingPen(Qt::black);
	QBrush drawingBrush (QColor(32, 32, 32),Qt::SolidPattern);

	QPointF startBarPt;
	QSizeF barSize;
	
	//drawing histogram bars
	for (int i = 0; i < _histogram_info->numOfItems; i++)
	{
		barHeight = (float)(_histogram_info->chartHeight * h.H[i]) / (float)_histogram_info->maxRoundedY;
		startBarPt.setX( _histogram_info->leftBorder + ( barWidth * i ) );
		startBarPt.setY( (float)_histogram_info->lowerBorder - barHeight );

		barSize.setWidth(barWidth);
		barSize.setHeight(barHeight);

		//drawing histogram bar
		_equalizerScene.addRect(startBarPt.x(), startBarPt.y(), barSize.width(), barSize.height(), drawingPen, drawingBrush);
	}

	//drawing handles
	QColor colors[] = { QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue) };

	qreal xStart = _histogram_info->leftBorder;
	qreal xPos = 0.0;
	qreal yPos = _histogram_info->lowerBorder;
	for (int i=0; i<3; i++)
	{
		xPos = xStart + _histogram_info->chartWidth/2*i;
		_equalizerScene.addItem(&_equalizerHandles[i]);
		_equalizerHandles[i].setColor(colors[i]);
		_equalizerHandles[i].setPos(xPos, yPos);
	}

	ui.equalizerGraphicsView->setScene(&_equalizerScene);
}


void QualityMapperDialog::drawTransferFunction(TransferFunction& tf)
{
	//building transfer function chart informations
	if ( _transferFunction_info == 0 )
		_transferFunction_info = new CHART_INFO( ui.equalizerGraphicsView->width(), ui.equalizerGraphicsView->height(), tf.size(), 0.0f, 1.0f, 0.0f, 1.0f );

	//drawing axis and other basic items
	this->drawChartBasics( _transferFunctionScene, ui.transferFunctionView, _transferFunction_info );

//	_transferFunctionScene.addLine(0, 0, 100, 430, QPen(Qt::green, 3));

	//questo per il momento è fisso e definito quì, ma dovrà essere gestito nella classe handle
	int pointMarkerRadius = 1;
	QPointF pointToRepresent;
	QPointF previousPoint;
	QRectF pointRect(0, 0, 2.0*pointMarkerRadius, 2.0*pointMarkerRadius );

	QColor channelColor;
	QPen drawingPen(Qt::black);
	QBrush drawingBrush ( QColor(32, 32, 32), Qt::SolidPattern );


	//drawing chart points
//	TfChannel *tf_c = 0;
	TF_KEY *key = 0;
	for(int i=0; i<NUMBER_OF_CHANNELS; i++)
	{
//		tf_c = tf[i];
		for (int j=0; j<tf[i].size(); j++)
		{
			key = tf[i][j];
		}
	}
/*
		for (int i = 0; i < values.Length; i++)
		{
			pointToRepresent.setX(_transferFunction_info->lowerBorder - (float)_transferFunction_info->chartHeight * values[i].yValue / _transferFunction_info->maxRoundedY);
			pointToRepresent.setY(_transferFunction_info->leftBorder + (_transferFunction_info->dX * i));
	
			//drawing single current point
			pointRect.setX(pointToRepresent.x() - pointMarkerRadius );
			pointRect.setY(pointToRepresent.y() - pointMarkerRadius );
			_transferFunctionScene.addEllipse( pointRect, drawingPen, drawingBrush );
	//		_transferFunctionScene.FillEllipse( drawingBrush, pointRect );
	
			//linear interpolation between current point and previous one
			//interpolation will not be executed if the current point is the first of the list
			if (i > 0)
				_transferFunctionScene.addLine(drawingPen, previousPoint, pointToRepresent);
	
			//refresh of previous point.
			//So, it's possible to interpolate linearly the current point with the previous one
			previousPoint = pointToRepresent;
	
	// 		//if the point is too low...
	// 		if (lowerBorderForCartesians - pointToRepresent.Y < (valuesStringSize.Height + chartRectangleThickness))
	// 			pointToRepresent.Y -= (valuesStringSize.Height + chartRectangleThickness);
		}*/
	

	ui.transferFunctionView->setScene( &_transferFunctionScene );
}