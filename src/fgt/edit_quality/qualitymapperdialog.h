#ifndef QUALITYMAPPERDIALOG_H
#define QUALITYMAPPERDIALOG_H

#include <QDockWidget>
#include <QGraphicsItem>
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
#include "tfhandle.h"

using namespace vcg;


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

class TFDoubleClickCatcher : public QObject, public QGraphicsItem
{
	Q_OBJECT

private:
	CHART_INFO		*_environmentInfo;
	QGraphicsView	*_myView;
	QRectF			_boundingRect;

protected:
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public:
	TFDoubleClickCatcher(/*QGraphicsView *view*/CHART_INFO *environmentInfo) : _environmentInfo(environmentInfo)
	{
		assert(environmentInfo);
		_boundingRect.setX(_environmentInfo->leftBorder);
		_boundingRect.setY(_environmentInfo->upperBorder);
		_boundingRect.setWidth(_environmentInfo->chartWidth);
		_boundingRect.setHeight(_environmentInfo->chartHeight);
	}
	~TFDoubleClickCatcher(){_environmentInfo = 0;}
	// Overriding QGraphicsItem methods
	QRectF boundingRect () const
	{
		return _boundingRect;
	}
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
// 		painter->setPen(QColor(100,212,136));
// 		painter->setBrush(QColor(32,64,128));
// 		painter->drawRect(_boundingRect);
	}

 signals:
 	void TFdoubleClicked(QPointF);
};


#define GRAPHICS_ITEMS_LIST		QList<QGraphicsItem *>
#define TF_HANDLES_LIST			QList<TFHandle*>

class QualityMapperDialog : public QDockWidget
{
	Q_OBJECT

public:
	QualityMapperDialog(QWidget *parent=0, MeshModel *m=0, GLArea *gla=0);
	~QualityMapperDialog();
	
//	inline void setMesh(MeshModel *m){ mesh=m; }
	void ComputePerVertexQualityHistogram( CMeshO& m, Frange range, Histogramf *h, int bins=10000);

	void clearScene(QGraphicsScene *scene);
	void drawChartBasics(QGraphicsScene& scene, CHART_INFO *current_chart_info );	//controllare il puntatore alla vista (!!) MAL
	void initEqualizerHistogram();
	void drawTransferFunction();


	static pair<int,int> computeHistogramMinMaxY (Histogramf*);
private:
	Ui::QualityMapperDialogClass ui;
//	QualityMapperSettings _settings;

	Histogramf		*_equalizer_histogram;
	CHART_INFO		*_histogram_info;
	QGraphicsScene	_equalizerHistogramScene;	//questo equivale a graphics di .NET. O ne conserviamo una sola e la utilizziamo per disegnare tutto, o ne creiamo una ogni volta che dobbiamo disegnare qualcosa. forse sbaglio in pieno(??) indagare MAL
	EqHandle*		_equalizerHandles[NUMBER_OF_EQHANDLES];
	qreal			_equalizerMidHandlePercentilePosition;
	bool			_leftHandleWasInsideHistogram;
	bool			_rightHandleWasInsideHistogram;
	GRAPHICS_ITEMS_LIST _equalizerHistogramBars;

	TransferFunction *_transferFunction;
	CHART_INFO		*_transferFunction_info;
	QGraphicsScene	_transferFunctionScene;
	TFDoubleClickCatcher *_tfCatcher;
	TF_HANDLES_LIST	_transferFunctionHandles[NUMBER_OF_CHANNELS];
	TFHandle		*_currentTfHandle;
	GRAPHICS_ITEMS_LIST _transferFunctionLines;
	GRAPHICS_ITEMS_LIST _transferFunctionBg;
	bool			_isTransferFunctionInitialized;
	QList<KNOWN_EXTERNAL_TFS>		_knownExternalTFs;


	GRAPHICS_ITEMS_LIST _removed_items;

	MeshModel		*mesh;
	GLArea			*gla;

	void initTF();
	void updateColorBand();
	void drawTransferFunctionBG ();
	void drawEqualizerHistogram(bool leftHandleInsideHistogram, bool rightHandleInsideHistogram);
	void drawHistogramBars (QGraphicsScene&, CHART_INFO*, int minIndex, int maxIndex, QColor color = QColor(Qt::black));

	GRAPHICS_ITEMS_LIST	*clearScene(QGraphicsScene *scene, int toClean=0);
	GRAPHICS_ITEMS_LIST	*clearItems(int itemsToClear);
	void				deleteRemoveItems();
	void				moveAheadChannel( TF_CHANNELS channelCode );
	void				manageBorderTfHandles(TFHandle *sender);
	void				updateTfHandlesOrder(int channelCode);
	TFHandle			*addTfHandle(int channelCode, QPointF handlePos, TF_KEY *key, int zOrder );
	TFHandle			*addTfHandle(TFHandle *handle);
	TFHandle			*removeTfHandle(TFHandle *handle);

signals:
	void suspendEditToggle();
	void closingDialog();

private slots:
	void on_removePointButton_clicked();
	void on_ySpinBox_valueChanged(double);
	void on_xSpinBox_valueChanged(double);
	
	void on_previewButton_clicked();
	void on_applyButton_clicked();
	void on_resetButton_clicked();

	void on_blueButton_toggled(bool checked);
	void on_greenButton_toggled(bool checked);
	void on_redButton_toggled(bool checked);

	void on_presetComboBox_textChanged(const QString &);
	void on_loadPresetButton_clicked();
	void on_savePresetButton_clicked();

	void on_addPointButton_clicked();

	void on_Handle_released();

	void on_EQHandle_moved();
	void on_EqHandle_crossing_histogram(EqHandle*,bool);

	void on_TfHandle_moved(TFHandle *sender);
	void on_TfHandle_clicked(TFHandle *sender);
	void on_TfHandle_doubleClicked(TFHandle *sender);
	void on_TF_view_doubleClicked(QPointF pos);
	
	void drawGammaCorrection();
};

bool TfHandleCompare(TFHandle*h1, TFHandle*h2);

#endif // QUALITYMAPPERDIALOG_H
