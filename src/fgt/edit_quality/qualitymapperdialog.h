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
#include <meshlab/meshmodel.h>
#include "ui_qualitymapperdialog.h"

#include <vcg/math/histogram.h>
#include "../../meshlabplugins/meshcolorize/curvature.h"		//<--contains Frange
#include <vcg/complex/trimesh/stat.h>
#include <meshlab/glarea.h>
#include "transferfunction.h"
#include "eqhandle.h"

using namespace vcg;

/*
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
*/

struct KNOWN_EXTERNAL_TFS
{
	QString path;
	QString name;
	KNOWN_EXTERNAL_TFS( QString p="", QString n="" )
	{
		path = p;
		name = n;
	}
};
#define KNOWN_EXTERNAL_TFSsize	sizeof(KNOWN_EXTERNAL_TFS)


class QualityMapperDialog : public QDialog
{
	Q_OBJECT

public:
	QualityMapperDialog(QWidget *parent=0, MeshModel *m=0);
	~QualityMapperDialog();
	
	//void setValues(const QualityMapperSettings& qms);
	//QualityMapperSettings getValues();

//	inline void setMesh(MeshModel *m){ mesh=m; }
	void ComputePerVertexQualityHistogram( CMeshO& m, Frange range, Histogramf *h, int bins=10000);

	void clearScene(QGraphicsScene *scene);
	void drawChartBasics(QGraphicsScene& scene, CHART_INFO *current_chart_info );	//controllare il puntatore alla vista (!!) MAL
	void drawEqualizerHistogram();
	void drawTransferFunction();


private:
	Ui::QualityMapperDialogClass ui;
//	QualityMapperSettings _settings;

	Histogramf		*_equalizer_histogram;
	CHART_INFO		*_histogram_info;
	QGraphicsScene	_equalizerHistogramScene;	//questo equivale a graphics di .NET. O ne conserviamo una sola e la utilizziamo per disegnare tutto, o ne creiamo una ogni volta che dobbiamo disegnare qualcosa. forse sbaglio in pieno(??) indagare MAL

	TransferFunction *_transferFunction;
	CHART_INFO		*_transferFunction_info;
	QGraphicsScene	_transferFunctionScene;
	bool			_isTransferFunctionInitialized;
	QList<KNOWN_EXTERNAL_TFS>		_knownExternalTFs;

	EqHandle		_equalizerHandles[3];
	qreal			_equalizerMidHandlePercentilePosition;

	MeshModel		*mesh;

	void initTF();
	//void drawPartialHistogram(float minValue, float maxValue);
	void drawHistogramBars (QGraphicsScene&, CHART_INFO*, int minIndex, int maxIndex, QColor color = QColor(Qt::black));



private slots:
	void on_blueButton_toggled(bool checked);
	void on_greenButton_toggled(bool checked);
	void on_redButton_toggled(bool checked);
	void on_presetComboBox_textChanged(const QString &);
	void on_loadPresetButton_clicked();
	void on_savePresetButton_clicked();
	void on_addPointButton_clicked();
	void on_equalizerHistogram_changed();
};

#endif // QUALITYMAPPERDIALOG_H
