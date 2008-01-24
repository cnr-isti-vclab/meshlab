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
#include "eqhandle.h"

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








class QualityMapperDialog : public QDialog
{
	Q_OBJECT

public:
	QualityMapperDialog(QWidget *parent = 0);
	~QualityMapperDialog();
	
	void setValues(const QualityMapperSettings& qms);
	QualityMapperSettings getValues();

	void drawChartBasics(QGraphicsScene& scene, CHART_INFO *current_chart_info );	//controllare il puntatore alla vista (!!) MAL
	void drawEqualizerHistogram( vcg::Histogramf& h );
	void drawTransferFunction( TransferFunction& tf);

private:
	Ui::QualityMapperDialogClass ui;
	QualityMapperSettings _settings;

	CHART_INFO		*_histogram_info;
	QGraphicsScene	_equalizerScene;	//questo equivale a graphics di .NET. O ne conserviamo una sola e la utilizziamo per disegnare tutto, o ne creiamo una ogni volta che dobbiamo disegnare qualcosa. forse sbaglio in pieno(??) indagare MAL
	EqHandle		_equalizerHandles[3];

	CHART_INFO		*_transferFunction_info;
	QGraphicsScene	_transferFunctionScene;



	private slots:
	void on_savePresetButton_clicked();
	void on_addPointButton_clicked();
};

#endif // QUALITYMAPPERDIALOG_H
