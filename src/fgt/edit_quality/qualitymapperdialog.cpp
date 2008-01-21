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


//il nome non mi sembra giusto. Questo dovrebbe essere piuttosto drawHistogram
//_equalizerScene dovrebbe chiamarsi in realtà histogramScene
//l'histogram e la transfer function potrebbero diventare attributi di questa classe? valutare l'impatto.
//in generale il codice di questo metodo va ripulito un po'...
void QualityMapperDialog::initEqualizerHistogram( Histogramf& h )
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

//	_equalizerScene.addText("Hello World!");
	this->drawChartBasics( _equalizerScene, ui.equalizerGraphicsView, _histogram_info );

// 	assert(_histogram_info != 0);
// 
// 	int maxY = 0;
// 	int minY = std::numeric_limits<int>::max();
// 
// 	//processing minX, maxX, minY and maxY values
// 	for (int i=0; i<_histogram_info->numOfItems; i++) 
// 	{
// 		if (h.H[i] > maxY)
// 			maxY = h.H[i];
// 
// 		if (h.H[i] < minY)
// 			minY = h.H[i];
// 	}

//	_histogram_info->updateMinMax( h.minv, h.maxv, minY, maxY );

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

	ui.equalizerGraphicsView->setScene(&_equalizerScene);
}


void QualityMapperDialog::drawTransferFunction(TransferFunction& tf)
{
	//building transfer function chart informations
	if ( _transferFunction_info == 0 )
		_transferFunction_info = new CHART_INFO( ui.equalizerGraphicsView->width(), ui.equalizerGraphicsView->height(), tf.size(), 0.0f, 1.0f, 0.0f, 1.0f );

	this->drawChartBasics( _transferFunctionScene, ui.transferFunctionView, _transferFunction_info );
	_transferFunctionScene.addLine(0, 0, 100, 430, QPen(Qt::green, 3));
	ui.transferFunctionView->setScene( &_transferFunctionScene );

/*

	float dX = Convert.ToSingle(chartWidth) / values.Length;
	float dY = Convert.ToSingle(chartHeightForCartesians) / values.Length;

	//Single maxX = Single.MinValue;
	float maxY = Single.MinValue;
	float minY = Single.MaxValue;

	//processing minX, maxX, minY and maxY values
	for (int i = 0; i < values.Length; i++)
	{
		/ *
		if (Convert.ToSingle(values[i].xValue) > maxX)
		maxX = Convert.ToSingle(values[i].xValue);* /

		if (values[i].yValue > maxY)
			maxY = values[i].yValue;

		if (values[i].yValue < minY)
			minY = values[i].yValue;
	}

	int maxRoundedY = Convert.ToInt32(Math.Floor(maxY + yScaleStep - (maxY % yScaleStep)));    //the highest value represented in the y values scale
	float variance = maxY - minY;                                               //variance of y values
	Point3f pointToRepresent = new PointF();
	Point3f previousPoint = new PointF();

	QPen drawingPen = new Pen(Color.Navy);
	QBrush drawingBrush = new SolidBrush(Color.Navy);

	Color4f valuesColor;
	int pointMarkerRadius = 3;

	//drawing chart basics
	this->drawCartesianChartBasics( _transferFunctionScene, ui.transferFunctionView );
	SizeF valuesStringSize;
	Font f = new Font("Verdana", valuesLabelSize);
	RectangleF pointRect = new RectangleF();

	//drawing chart points
	for (int i = 0; i < values.Length; i++)
	{
		pointToRepresent.Y = lowerBorderForCartesians - Convert.ToSingle(chartHeightForCartesians) * values[i].yValue / maxRoundedY;
		pointToRepresent.X = leftBorder + (dX * i);

		//drawing single current point
		pointRect.X = pointToRepresent.X - pointMarkerRadius;
		pointRect.Y = pointToRepresent.Y - pointMarkerRadius;
		pointRect.Width = 2 * pointMarkerRadius;
		pointRect.Height = 2 * pointMarkerRadius;
		g.DrawEllipse(drawingPen, pointRect);
		g.FillEllipse(drawingBrush, pointRect);

		//linear interpolation between current point and previous one
		//interpolation will not be executed if the current point is the first of the list
		if (i > 0)
			g.DrawLine(drawingPen, previousPoint, pointToRepresent);

		//refresh of previous point.
		//So, it's possible to interpolate linearly the current point with the previous one
		previousPoint = pointToRepresent;

		//DRAWING POINT (NUMERIC) VALUE
		valuesStringSize = g.MeasureString(values[i].yValue.ToString(), f);

		//choosing text color (red for min and max value, white else)
		if ((values[i].yValue == maxY) || (values[i].yValue == minY))
			valuesColor = Color.Red;
		else
			valuesColor = Color.Gray;

		//if the point is too low...
		if (lowerBorderForCartesians - pointToRepresent.Y < (valuesStringSize.Height + chartRectangleThickness))
			pointToRepresent.Y -= (valuesStringSize.Height + chartRectangleThickness);

		//if there's space enough between consecutive points to contain a numeric value...
		//the value is written only if there's space enough between the bars or if the value to represent is the min or the max value
		if ((dX >= (valuesStringSize.Width * 0.9F)) || (valuesColor == Color.Red))
		{
			this.writeString(g, Convert.ToString(values[i].yValue), valuesLabelSize, valuesColor, Positions.Custom, pointToRepresent);
		}
	}

*/








}