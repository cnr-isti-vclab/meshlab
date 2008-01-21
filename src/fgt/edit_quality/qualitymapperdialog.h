#ifndef QUALITYMAPPERDIALOG_H
#define QUALITYMAPPERDIALOG_H

#include <QDialog>
/*
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/face/topology.h>
//#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <vcg/complex/trimesh/allocate.h>
*/

#include <vcg/complex/trimesh/base.h>
#include "ui_qualitymapperdialog.h"

#include <vcg/math/histogram.h>

#include "transferfunction.h"

using namespace vcg;


//questa clsse l'hai creata tu? (??) MAL
//l'ho ripresa da meshcolorize perché mi serviva per utilizzare altro codice copiato. ;-) Dobbiamo valutare se mantenerla UCCIO
// QualityMapperSettings stores all the settings that should be inserted in QualityMapperDialog fields
class QualityMapperSettings
{
public:
	//int percentile;
	int range;
	float meshMaxQ;
	float meshMidQ;
	float meshMinQ;
	float manualMaxQ;
	float manualMidQ;
	float manualMinQ;
	//float histoMaxQ;
	//float histoMinQ;
	//bool useManual;

	QualityMapperSettings()
	{
		//percentile=20;
		//range=10000;
		range=500;
		meshMaxQ=meshMinQ=meshMidQ=manualMaxQ=manualMinQ=manualMidQ=0.0f;
		//useManual=false;
	}
};


//questa dovrebbe essere trasferita in util.h
struct CHART_INFO
{
	int leftBorder;
	int rightBorder;
	int upperBorder;
	int lowerBorder;
	float chartWidth;
	float chartHeight;
	int numOfItems;
	int	  yScaleStep;
	int maxRoundedY;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float dX;
	float dY;
	float variance;						//variance of y values

	CHART_INFO(int view_width, int view_height, int num_of_items=1, float min_X=0.0f, float max_X=0.0f, float min_Y=0.0f, float max_Y=0.0f )
	{
		assert(num_of_items != 0);

		leftBorder	= CANVAS_BORDER_DISTANCE;
		rightBorder	= view_width - CANVAS_BORDER_DISTANCE;
		upperBorder	= CANVAS_BORDER_DISTANCE;
		lowerBorder	= view_height - CANVAS_BORDER_DISTANCE;
		chartWidth = rightBorder - leftBorder;
		chartHeight = lowerBorder - upperBorder;
		numOfItems = num_of_items;
		yScaleStep = Y_SCALE_STEP;
		this->updateMinMax( min_X, max_X, min_Y, max_Y );

		dX = chartWidth / (float)numOfItems;
		dY = chartHeight / (float)numOfItems;
	}
	inline void updateMinMax( float min_X=0.0f, float max_X=0.0f, float min_Y=0.0f, float max_Y=0.0f )
	{
		minX = min_X;
		maxX = max_X;
		minY = min_Y;
		maxY = max_Y;
		maxRoundedY = (int)(maxY + Y_SCALE_STEP - (relative2AbsoluteVali(maxY, maxY) % Y_SCALE_STEP));    //the highest value represented in the y values scale
		variance = maxY - minY;          //variance of y values
	}
};




class QualityMapperDialog : public QDialog
{
	Q_OBJECT

public:
	QualityMapperDialog(QWidget *parent = 0);
	~QualityMapperDialog();
	
	void setValues(const QualityMapperSettings& qms);
	QualityMapperSettings getValues();

	void drawChartBasics(QGraphicsScene& scene, QGraphicsView *view, CHART_INFO *current_chart_info );	//controllare il puntatore alla vista (!!) MAL
	void initEqualizerHistogram( vcg::Histogramf& h );
	void drawTransferFunction( TransferFunction& tf);

private:
	Ui::QualityMapperDialogClass ui;
	QualityMapperSettings _settings;

	CHART_INFO		*_histogram_info;
	QGraphicsScene	_equalizerScene;	//questo equivale a graphics di .NET. O ne conserviamo una sola e la utilizziamo per disegnare tutto, o ne creiamo una ogni volta che dobbiamo disegnare qualcosa. forse sbaglio in pieno(??) indagare MAL

	CHART_INFO		*_transferFunction_info;
	QGraphicsScene	_transferFunctionScene;


	//questi parametri variano a seconda del grafico che si sta disegnando MAL
	//in effetti si, a parte forse border, leftBorder, rightBorder, upperBorder che potrebbero essere gli stessi UCCIO



	private slots:

};

#endif // QUALITYMAPPERDIALOG_H
