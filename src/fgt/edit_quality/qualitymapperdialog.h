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
	};
};

class QualityMapperDialog : public QDialog
{
	Q_OBJECT

public:
	QualityMapperDialog(QWidget *parent = 0);
	~QualityMapperDialog();
	
	void setValues(const QualityMapperSettings& qms);
	QualityMapperSettings getValues();

	void drawCartesianChartBasics(QGraphicsScene& scene, QGraphicsView *view /*, float maxRoundedY*/);	//controllare il puntatore alla vista (!!) MAL
	void initEqualizerHistogram(vcg::Histogramf& h);
	void drawTransferFunction( TransferFunction& tf);
	
	

private:
	Ui::QualityMapperDialogClass ui;
	QualityMapperSettings _settings;
	QGraphicsScene	_equalizerScene;	//questo equivale a graphics di .NET. O ne conserviamo una sola e la utilizziamo per disegnare tutto, o ne creiamo una ogni volta che dobbiamo disegnare qualcosa. forse sbaglio in pieno(??) indagare MAL
	QGraphicsScene	_transferFunctionScene;

	//questi parametri variano a seconda del grafico che si sta disegnando
	int border;
	int chartRectangleThickness;
	int leftBorder;
    int rightBorder;
    int upperBorder;
    int lowerBorderForCartesians;


	private slots:

};

#endif // QUALITYMAPPERDIALOG_H
