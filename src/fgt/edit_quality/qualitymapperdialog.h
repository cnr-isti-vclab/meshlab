/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
History
Revision 1.0 2008/02/20 Alessandro Maione, Federico Bellucci
FIRST RELEASE

****************************************************************************/

#ifndef _QUALITYMAPPER_DIALOG_H_
#define _QUALITYMAPPER_DIALOG_H_

#include <QDockWidget>
#include <QGraphicsItem>
#include <common/meshmodel.h>
#include <vcg/math/histogram.h>
#include <vcg/complex/trimesh/stat.h> // for ComputePerVertexQualityMinMax
#include <meshlab/glarea.h>
#include "ui_qualitymapperdialog.h"
#include "common/transferfunction.h"
#include "common/meshmethods.h"
#include "eqhandle.h"
#include "tfhandle.h"

using namespace vcg;

  class Frange
  {
  public:
    Frange(){}
    Frange(std::pair<float,float> minmax):minV(minmax.first),maxV(minmax.second){}
    Frange(float _min,float _max):minV(_min),maxV(_max){}

    float minV;
    float maxV;
  };


//struct used to store info about external TF files
//when a user saves and\or loads an external CSV file, the info about it (path and name) are saved and stored in a list
//So, if the info contained into the file are needed again, it's possible to access to it at any time
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

//this class defines an invisible graphics item that lies on the BG of TF view
//Its porpoise is to catch the double-clicks on the view and send a massage to dialog that will manage it
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
	TFDoubleClickCatcher(CHART_INFO *environmentInfo) : _environmentInfo(environmentInfo)
	{
		assert(environmentInfo);
		//setting graphics
		_boundingRect.setX(_environmentInfo->leftBorder());
		_boundingRect.setY(_environmentInfo->upperBorder());
		_boundingRect.setWidth(_environmentInfo->chartWidth());
		_boundingRect.setHeight(_environmentInfo->chartHeight());
	}
	~TFDoubleClickCatcher(){_environmentInfo = 0;}
	// Overriding QGraphicsItem methods
	QRectF boundingRect () const
	{	return _boundingRect;	}
	//nothing to paint... I'm invisible!!
	void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)	{}

 signals:
 	void TFdoubleClicked(QPointF);
};

//macros to make slier code below! :-)
#define GRAPHICS_ITEMS_LIST		QList<QGraphicsItem *>
#define TF_HANDLES_LIST			QList<TFHandle*>

enum MID_HANDLE_SIGNAL_DIRECTION
{
	UNKNOWN_DIRECTION = 0,
	SPINBOX2LABEL,
	LABEL2SPINBOX
};

//this class define the dialog of the plugin
class QualityMapperDialog : public QDockWidget
{
	Q_OBJECT

public:
	QualityMapperDialog(QWidget *parent, MeshModel& m, GLArea *gla=0);
	~QualityMapperDialog();
	
	void ComputePerVertexQualityHistogram( CMeshO& m, Frange range, Histogramf *h, int bins=50000);
	void clearScene(QGraphicsScene *scene);
	void drawChartBasics(QGraphicsScene& scene, CHART_INFO *current_chart_info );
	bool initEqualizerHistogram(void);
	void drawTransferFunction(void);

	float minQuality() {return ui.minSpinBox->value();};
	float maxQuality() {return ui.maxSpinBox->value();};
	float relativeMidQuality() {return _equalizerMidHandlePercentilePosition;};
	float brightness() {return (1.0f - (float)(ui.brightnessSlider->value())/(float)(ui.brightnessSlider->maximum()) )*2.0;};

private:
	Ui::QualityMapperDialogClass ui;

	//Equalizer-Histogram items
	Histogramf		*_equalizer_histogram;	// It changes if handles go "outside" histogram
	CHART_INFO		*_histogram_info;		// Its values minX and maxX never change
	QGraphicsScene	_equalizerHistogramScene;
	EqHandle*		_equalizerHandles[NUMBER_OF_EQHANDLES];
	qreal			_equalizerMidHandlePercentilePosition;
	bool			_leftHandleWasInsideHistogram;
	bool			_rightHandleWasInsideHistogram;
	GRAPHICS_ITEMS_LIST _equalizerHistogramBars;
	MID_HANDLE_SIGNAL_DIRECTION	_signalDir;

	//Transfer Function items
public:
	TransferFunction *_transferFunction;
private:
	CHART_INFO		*_transferFunction_info;
	QGraphicsScene	_transferFunctionScene;
	TFDoubleClickCatcher *_tfCatcher;
	TF_HANDLES_LIST	_transferFunctionHandles[NUMBER_OF_CHANNELS];
	TFHandle		*_currentTfHandle;
	QString			_currentTfHandleQualityValue;
	GRAPHICS_ITEMS_LIST _transferFunctionLines;
	GRAPHICS_ITEMS_LIST _transferFunctionBg;
	bool			_isTransferFunctionInitialized;
	
	QList<KNOWN_EXTERNAL_TFS>		_knownExternalTFs; //list of external CSV files info
	GRAPHICS_ITEMS_LIST _removed_items; //list of removed graphics items

	MeshModel&		mesh; //reference to current mesh
	GLArea			*gla; //pointer to GL Area object

	void initTF(void);
	void updateColorBand(void);
	void drawTransferFunctionBG(void);
	bool drawEqualizerHistogram(bool leftHandleInsideHistogram, bool rightHandleInsideHistogram);
	int  computeEqualizerMaxY (Histogramf *h, float minX, float maxX);
	void initEqualizerSpinboxes();
	void drawHistogramBars (QGraphicsScene&, CHART_INFO*, float minIndex, float maxIndex, QColor color = QColor(Qt::black));

	GRAPHICS_ITEMS_LIST	*clearScene(QGraphicsScene *scene, int toClean=0);
	GRAPHICS_ITEMS_LIST	*clearItems(int itemsToClear);
	void				deleteRemoveItems();
	void				moveAheadChannel( TF_CHANNELS channelCode );
	void				manageBorderTfHandles(TFHandle *sender);
	void				updateTfHandlesOrder(int channelCode);
	TFHandle			*addTfHandle(int channelCode, QPointF handlePos, TF_KEY *key, int zOrder );
	TFHandle			*addTfHandle(TFHandle *handle);
	TFHandle			*removeTfHandle(TFHandle *handle);
	void				updateXQualityLabel(float xRelativeTFPosition);
	void				setEqualizerParameters(EQUALIZER_INFO data);

signals:
	void suspendEditToggle();
	void closingDialog();

private slots:
	void on_midPercentageLine_editingFinished();
	void on_midSpinBox_valueChanged(double);
	void on_brightnessSlider_valueChanged(int);
	void on_brightessSpinBox_valueChanged(double);
	void on_clampButton_clicked();
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

	void meshColorPreview();

	void on_EQHandle_moved();
	void on_EqHandle_crossing_histogram(EqHandle*,bool);

	void on_TfHandle_moved(TFHandle *sender);
	void on_TfHandle_clicked(TFHandle *sender);
	void on_TfHandle_doubleClicked(TFHandle *sender);
	void on_TF_view_doubleClicked(QPointF pos);
	
	void drawGammaCorrection(void);
};


bool TfHandleCompare(TFHandle*h1, TFHandle*h2);

#endif // QUALITYMAPPERDIALOG_H
