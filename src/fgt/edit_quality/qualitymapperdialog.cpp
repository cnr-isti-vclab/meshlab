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

#include "qualitymapperdialog.h"
#include <limits>
#include <QPen>
#include <QBrush>

#include <vcg/complex/trimesh/update/color.h>

using namespace vcg;

//returns true if relative x of h1 is < then x of h2.
//if x values of h1 and h2 are the same, true is returned if relative y of h1 is < then relative y of h2
bool TfHandleCompare(TFHandle*h1, TFHandle*h2)
{	return (h1->getRelativeX() <= h2->getRelativeX());	}

//callback to manage the double-click of TFDoubleClickCatcher object
void TFDoubleClickCatcher::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{	emit TFdoubleClicked(event->scenePos());	}


//class constructor
QualityMapperDialog::QualityMapperDialog(QWidget *parent, MeshModel& m, GLArea *gla) : QDockWidget(parent), mesh(m)
{
	ui.setupUi(this);

	this->setWidget(ui.frame);
	this->setFloating(true);
	// Setting dialog position in top right corner
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+40,width(),height() );

	this->gla = gla;

	_histogram_info = 0;
	_equalizer_histogram = 0;
	for (int i=0; i<NUMBER_OF_EQHANDLES; i++)
		_equalizerHandles[i] = 0;
	_signalDir = UNKNOWN_DIRECTION;

	//building default Transfer Function
	_transferFunction = new TransferFunction( STARTUP_TF_TYPE );
	_isTransferFunctionInitialized = false;
	_transferFunction_info = 0;
	_currentTfHandle = 0;
	//initializing Transfer Function
	this->initTF();

	//building a catcher for double click in empty areas of transfer function view
	//and adding it to the scene
	_tfCatcher = new TFDoubleClickCatcher(_transferFunction_info);
	_tfCatcher->setZValue(0);
	_transferFunctionScene.addItem(_tfCatcher);
	connect(_tfCatcher, SIGNAL(TFdoubleClicked(QPointF)), this, SLOT(on_TF_view_doubleClicked(QPointF)));

	// toggling Trackball button
	connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );

	suspendEditToggle();
}

//class destructor
QualityMapperDialog::~QualityMapperDialog()
{
	//removing from scene and deleting each graphical object
	this->clearItems(REMOVE_ALL_ITEMS | DELETE_REMOVED_ITEMS);

	//destroying info about histogram chart
	if ( _histogram_info )
	{
		delete _histogram_info;
		_histogram_info = 0;
	}

	//destroying transfer function object
	if ( _transferFunction )
	{
		delete _transferFunction;
		_transferFunction = 0;
	}

	//destroying info about histogram chart
	if ( _transferFunction_info )
	{
		delete _transferFunction_info;
		_transferFunction_info = 0;
	}

	//destroying transfer function double-click catcher
	if ( _tfCatcher )
	{
		delete _tfCatcher;
		_tfCatcher = 0;
	}
	
//	this->disconnect();

	//sending closing dialog, to call EndEdit of plugin
	emit closingDialog();
}

/* Resets histogram and builds a new one */
void QualityMapperDialog::ComputePerVertexQualityHistogram( CMeshO &m, Frange range, Histogramf *h, int bins )    // V1.0
{
	h->Clear();
	h->SetRange( range.minV, range.maxV, bins);
	CMeshO::VertexIterator vi;
	for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
		if(!(*vi).IsD()) h->Add((*vi).Q());		
}

//clears the whole scene by removing and\or deleting all its items
//returns a pointer to the list of removed items
GRAPHICS_ITEMS_LIST* QualityMapperDialog::clearScene(QGraphicsScene *scene, int cleanFlag)
{
	//deleting scene items
	GRAPHICS_ITEMS_LIST allItems = _transferFunctionScene.items();
	QGraphicsItem *item = 0;
	foreach (item, allItems)
	{
		scene->removeItem( item );
		_removed_items << item;
	}

	//deleting if necessary
	if ((cleanFlag & DELETE_REMOVED_ITEMS) == DELETE_REMOVED_ITEMS)
	{
		foreach (item, _removed_items)
		{
			delete item;
			item = 0;
		}
	}

	return &_removed_items;
}


/*
Clears some particular types of graphical items from dialog.
toClear value represents the logical OR of some macros that define what to clear
returns a list of pointers to Items removed (NOT DELETED!!)
If DELETE_REMOVED_ITEMS is passed in toClear flag, the items are destroyed too and the list returned is empty
*/
GRAPHICS_ITEMS_LIST* QualityMapperDialog::clearItems(int toClear)
{
	_removed_items.clear();
	QGraphicsItem *item = 0;

	if ((toClear & REMOVE_TF_HANDLE) == REMOVE_TF_HANDLE)
	{
		//removing TF Handles
		for (int i=0; i<NUMBER_OF_CHANNELS; i++)
		{
			foreach( item, _transferFunctionHandles[i] )
			{
				((TFHandle*)item)->disconnect();
				_transferFunctionScene.removeItem( item );
				_removed_items << item;
			}
			_transferFunctionHandles[i].clear();
		}
	}

	if ((toClear & REMOVE_EQ_HANDLE) == REMOVE_EQ_HANDLE)
	{
		//removing EQ Handles
		for (int i=0; i<NUMBER_OF_EQHANDLES; i++)
		{
			if ( _equalizerHandles[i] )
			{
				_equalizerHandles[i]->disconnect();
				_equalizerHistogramScene.removeItem( (QGraphicsItem*)_equalizerHandles[i] );
				_removed_items << _equalizerHandles[i];
			}
		}

		if ((toClear & DELETE_REMOVED_ITEMS) == DELETE_REMOVED_ITEMS)
		{
			for (int i=0; i<NUMBER_OF_EQHANDLES; i++)
			{
				if ( _equalizerHandles[i] )
				{
					delete _equalizerHandles[i];
					_equalizerHandles[i] = 0;
					_removed_items.removeLast();
				}
			}
		}
	}

	if ((toClear & REMOVE_TF_BG) == REMOVE_TF_BG)
	{
		//removing background Histogram bars of the TF
		foreach( item, _transferFunctionBg )
		{
			//disconnecting everything connected to TF handle before removing it
			_transferFunctionScene.removeItem( item );
			_removed_items << item;
		}
		_transferFunctionBg.clear();
	}

	if ((toClear & REMOVE_TF_LINES) == REMOVE_TF_LINES)
	{
		//removing TF lines
		foreach( item, _transferFunctionLines )
		{
			//disconnecting everything connected to TF handle before removing it
			_transferFunctionScene.removeItem( item );
			_removed_items << item;
		}
		_transferFunctionLines.clear();
	}

	if ((toClear & REMOVE_EQ_HISTOGRAM) == REMOVE_EQ_HISTOGRAM)
	{
		//removing Histogram
		foreach( item, _equalizerHistogramBars )
		{
			//disconnecting everything connected to TF handle before removing it
			_equalizerHistogramScene.removeItem( item );
			_removed_items << item;
		}
		_equalizerHistogramBars.clear();

		//_removed_items = *(this->clearScene( &_equalizerHistogramScene, toClear & DO_NOT_DELETE_REMOVED_ITEMS ));
	}

	if ((toClear & DELETE_REMOVED_ITEMS) == DELETE_REMOVED_ITEMS)
	{
		//deleting removed items
		foreach(item, _removed_items)
		{
			if (item != 0)
			{
				delete item;
				item = 0;
			}
		}
		_removed_items.clear();
	}

	return &_removed_items;
}

//draws the charts basics (axis)
void QualityMapperDialog::drawChartBasics(QGraphicsScene& scene, CHART_INFO *chart_info)
{
	//a valid chart_info must be passed
	assert( chart_info != 0 );

	QPen p( Qt::black, 2 );
	QGraphicsItem *current_item = 0;

	//drawing axis
	//x axis
	current_item = scene.addLine( chart_info->leftBorder(), chart_info->lowerBorder(), chart_info->rightBorder(), chart_info->lowerBorder(), p );
	current_item->setZValue( 0 );
	if (chart_info == _transferFunction_info)
		_transferFunctionLines << current_item;
	else
		_equalizerHistogramBars << current_item;
	//y axis
	current_item = scene.addLine( chart_info->leftBorder(), chart_info->upperBorder(), chart_info->leftBorder(), chart_info->lowerBorder(), p );
	current_item->setZValue( 0 );
	if (chart_info == _transferFunction_info)
		_transferFunctionLines << current_item;
	else
		_equalizerHistogramBars << current_item;
}

// Initializes equalizerHistogramView
// (This method is called only once)
bool QualityMapperDialog::initEqualizerHistogram()
{
	if (_equalizer_histogram) //added by MAL 17\02\08
	{
//		_equalizer_histogram->disconnect();	//added by MAL 17\02\08
		delete _equalizer_histogram;		//added by MAL 17\02\08
		_equalizer_histogram = 0;
	}
	_leftHandleWasInsideHistogram = true;
	_rightHandleWasInsideHistogram = true;

	if ( !drawEqualizerHistogram(true, true) )
		return false;

	//DRAWING HANDLES
	QDoubleSpinBox* spinboxes[] = { ui.minSpinBox, ui.midSpinBox, ui.maxSpinBox };

	qreal xStart = _histogram_info->leftBorder();
	qreal xPos = 0.0f;
	qreal yPos = _histogram_info->lowerBorder();
	_equalizerMidHandlePercentilePosition = 0.5f;
	for (int i=0; i<NUMBER_OF_EQHANDLES; i++)
	{
		xPos = xStart + _histogram_info->chartWidth()/2.0f*i;
		_equalizerHandles[i] = new EqHandle(_histogram_info, Qt::black, QPointF(xPos, yPos), 
											(EQUALIZER_HANDLE_TYPE)i, _equalizerHandles, &_equalizerMidHandlePercentilePosition, spinboxes[i], 
											1, 5);
		_equalizerHistogramScene.addItem((QGraphicsItem*)_equalizerHandles[i]);
	}


	// SETTING SPNIBOX VALUES
	// (Se venissero inizializzati prima di impostare setHistogramInfo sulle handles darebbero errore nello SLOT setX delle handles.)
	initEqualizerSpinboxes();

	//SETTING UP CONNECTIONS
	// Connecting spinboxes to handles
	connect(ui.minSpinBox, SIGNAL(valueChanged(double)), _equalizerHandles[LEFT_HANDLE],  SLOT(setXBySpinBoxValueChanged(double)));
	connect(ui.midSpinBox, SIGNAL(valueChanged(double)), _equalizerHandles[MID_HANDLE],   SLOT(setXBySpinBoxValueChanged(double)));
	connect(ui.maxSpinBox, SIGNAL(valueChanged(double)), _equalizerHandles[RIGHT_HANDLE], SLOT(setXBySpinBoxValueChanged(double)));
	
	// Connecting handles to spinboxes
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(positionChangedToSpinBox(double)), ui.minSpinBox, SLOT(setValue(double)));
	connect(_equalizerHandles[MID_HANDLE],   SIGNAL(positionChangedToSpinBox(double)), ui.midSpinBox, SLOT(setValue(double)));
	connect(_equalizerHandles[MID_HANDLE],   SIGNAL(positionChangedToSpinBox(double)), this, SLOT(on_midSpinBox_valueChanged(double)));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(positionChangedToSpinBox(double)), ui.maxSpinBox, SLOT(setValue(double)));
	
	// Connecting left and right handles to mid handle
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(positionChanged()), _equalizerHandles[MID_HANDLE], SLOT(moveMidHandle()));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(positionChanged()), _equalizerHandles[MID_HANDLE], SLOT(moveMidHandle()));

	// Making spinboxes and handles changes redrawing transferFunctionScene
	// NotE: it is not necessary to connect spinboxes too
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(positionChanged()), this, SLOT(on_EQHandle_moved()));
	connect(_equalizerHandles[MID_HANDLE],   SIGNAL(positionChanged()), this, SLOT(on_EQHandle_moved()));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(positionChanged()), this, SLOT(on_EQHandle_moved()));

	// Connecting mid equalizerHistogram handle to gammaCorrectionLabel
	connect(_equalizerHandles[MID_HANDLE], SIGNAL(positionChanged()), this, SLOT(drawGammaCorrection()) );
	connect(ui.midSpinBox, SIGNAL(valueChanged(double)), this, SLOT(drawGammaCorrection()) );
	
	// Connecting eqHandles to histogram drawing
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(insideHistogram(EqHandle*,bool)), this, SLOT(on_EqHandle_crossing_histogram(EqHandle*,bool)) );
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(insideHistogram(EqHandle*,bool)), this, SLOT(on_EqHandle_crossing_histogram(EqHandle*,bool)) );

	// Connecting handles to preview method
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(handleReleased()), this, SLOT(meshColorPreview()));
	connect(_equalizerHandles[MID_HANDLE],   SIGNAL(handleReleased()), this, SLOT(meshColorPreview()));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(handleReleased()), this, SLOT(meshColorPreview()));
	connect(ui.brightnessSlider,             SIGNAL(sliderReleased()), this, SLOT(meshColorPreview()));
	connect(ui.brightessSpinBox,			 SIGNAL(valueChanged(double)), this, SLOT(meshColorPreview()));
	connect(ui.minSpinBox, SIGNAL(editingFinished()), this, SLOT(on_previewButton_clicked()));
	connect(ui.midSpinBox, SIGNAL(editingFinished()), this, SLOT(on_previewButton_clicked()));
	connect(ui.maxSpinBox, SIGNAL(editingFinished()), this, SLOT(on_previewButton_clicked()));

	// Connecting brightessSpinBox to brightnessSlider
	connect(ui.brightessSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_brightessSpinBox_valueChanged()));

	ui.equalizerGraphicsView->setScene(&_equalizerHistogramScene);

	drawGammaCorrection();
	drawTransferFunctionBG();

	return true;
}

void QualityMapperDialog::initEqualizerSpinboxes()
{
	double singleStep = (_histogram_info->maxX - _histogram_info->minX) / _histogram_info->chartWidth();
	int decimals = 0;
	if (singleStep > std::numeric_limits<float>::epsilon())
	{	
		double temp = singleStep;
		while (temp < 0.1)
		{
			decimals++;
			temp *= 10;
		}
		
	}
	decimals+=2;

	ui.minSpinBox->setDecimals(decimals);
	ui.minSpinBox->setValue(_histogram_info->minX);
	ui.minSpinBox->setRange(numeric_limits<int>::min(), _histogram_info->maxX);
	//ui.minSpinBox->setRange(2*_histogram_info->minX - _histogram_info->maxX, 2*_histogram_info->maxX - _histogram_info->minX);
	ui.minSpinBox->setSingleStep(singleStep);

	ui.maxSpinBox->setDecimals(decimals);
	ui.maxSpinBox->setValue(_histogram_info->maxX);
	ui.maxSpinBox->setRange(_histogram_info->minX, numeric_limits<int>::max());
	//ui.maxSpinBox->setRange(2*_histogram_info->minX - _histogram_info->maxX, 2*_histogram_info->maxX - _histogram_info->minX);
	ui.maxSpinBox->setSingleStep(singleStep);

	ui.midSpinBox->setDecimals(decimals);
	ui.midSpinBox->setValue((_histogram_info->maxX + _histogram_info->minX) / 2.0f);
	ui.midSpinBox->setRange(_histogram_info->minX, _histogram_info->maxX);
	ui.midSpinBox->setSingleStep(singleStep);

}

// Add histogram bars to equalizerHistogram Scene
// Return false if mesh has no quality => dialog will not be built
bool QualityMapperDialog::drawEqualizerHistogram(bool leftHandleIsInsideHistogram, bool rightHandleIsInsideHistogram)
{
	if (_equalizer_histogram == 0)
	{
		// This block is called only the first time
		_equalizer_histogram = new Histogramf();
		Frange histogramRange(tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mesh.cm));
		this->ComputePerVertexQualityHistogram(mesh.cm, histogramRange, _equalizer_histogram, NUMBER_OF_HISTOGRAM_BINS);
		if (histogramRange.minV == histogramRange.maxV)
		{
			QMessageBox::warning(this, tr("Quality Mapper"), tr("The model has no vertex quality"), QMessageBox::Ok); 
			return false;
		}
		//building histogram chart informations
		_histogram_info = new CHART_INFO( ui.equalizerGraphicsView, _equalizer_histogram->MinV(), _equalizer_histogram->MaxV(), 0, computeEqualizerMaxY(_equalizer_histogram, _equalizer_histogram->MinV(), _equalizer_histogram->MaxV()) );
	}
	else
	{
		//added by MAL 23/04/08
		_transferFunction_info->updateChartInfo(_equalizer_histogram->MinV(), _equalizer_histogram->MaxV(), 0, computeEqualizerMaxY(_equalizer_histogram, _equalizer_histogram->MinV(), _equalizer_histogram->MaxV()) );

		// if histogram doesn't need to be redrawn, return
		if ( (leftHandleIsInsideHistogram && _leftHandleWasInsideHistogram) && (rightHandleIsInsideHistogram && _rightHandleWasInsideHistogram) )
			return true;
		
		// Updating flags _leftHandleWasInsideHistogram [right] with new values
		_leftHandleWasInsideHistogram = leftHandleIsInsideHistogram;
		_rightHandleWasInsideHistogram = rightHandleIsInsideHistogram;

		this->clearItems( REMOVE_EQ_HISTOGRAM | DELETE_REMOVED_ITEMS ); // removing and deleting histogram bars

		// Calculating new Histogram
		//float minX = (_histogram_info->minX<ui.minSpinBox->value())?_histogram_info->minX:ui.minSpinBox->value();
		//float maxX = (_histogram_info->maxX>ui.maxSpinBox->value())?_histogram_info->maxX:ui.maxSpinBox->value();
		//Frange histogramRange(minX, maxX);
		//this->ComputePerVertexQualityHistogram(mesh.cm, histogramRange, _equalizer_histogram, NUMBER_OF_HISTOGRAM_BINS);
		// Only minY and maxY are modified in _histogram_info
		//_histogram_info->maxY = computeEqualizerMaxY(_equalizer_histogram, minX, maxX);
	}

	float minX = (_histogram_info->minX<ui.minSpinBox->value())?_histogram_info->minX:ui.minSpinBox->value();
	float maxX = (_histogram_info->maxX>ui.maxSpinBox->value())?_histogram_info->maxX:ui.maxSpinBox->value();
	_histogram_info->maxY = computeEqualizerMaxY(_equalizer_histogram, minX, maxX);

	//drawing axis and other basic items
	this->drawChartBasics( _equalizerHistogramScene, _histogram_info );

	//drawing histogram bars
	this->drawHistogramBars (_equalizerHistogramScene, _histogram_info, minX, maxX, QColor(192,192,192));

//	this->drawGammaCorrection();
	this->drawTransferFunctionBG();

	return true;
}

int QualityMapperDialog::computeEqualizerMaxY (Histogramf *h, float minX, float maxX)
{
	float barRange = (maxX-minX)/(float)NUMBER_OF_HISTOGRAM_BARS;
	int maxY = 0;
	int currentY;
	for (int i=0; i<NUMBER_OF_HISTOGRAM_BARS; i++)
	{
		currentY = h->BinCount( minX+i*barRange, barRange );
		if (currentY>maxY)
			maxY = currentY;
	}
	return maxY;
}

// Add histogramBars to destinationScene with GAMMA-STRETCHING
void QualityMapperDialog::drawHistogramBars (QGraphicsScene& destinationScene, CHART_INFO *chartInfo, float minVal, float maxVal, QColor color)
{
	float barHeight = 0.0f;	//initializing height of the histogram bars
	float barWidth  = chartInfo->chartWidth() / (float)(NUMBER_OF_HISTOGRAM_BARS);	//processing width of the histogram bars 

	// exp is such that: _equalizerMidHandlePercentilePosition^exp = 0.5
	float exp = log10(0.5f) / log10((float)_equalizerMidHandlePercentilePosition);

	QPen drawingPen(color);
	QBrush drawingBrush (color);
	QPointF startBarPt;

	//drawing histogram bars
	QGraphicsItem *current_item = 0;
	float barRange = (maxVal-minVal)/(float)NUMBER_OF_HISTOGRAM_BARS;
	for (int i = 0; i < NUMBER_OF_HISTOGRAM_BARS; i++)
	{
		// Setting barHeight proportional to max height
		barHeight = (float)(chartInfo->chartHeight() * _equalizer_histogram->BinCount( minVal+i*barRange, barRange) ) / (float)chartInfo->maxY;
		
		startBarPt.setY( (float)chartInfo->lowerBorder() - barHeight );

		//drawing histogram bar
		if ( &destinationScene == &_transferFunctionScene )
		{
			// Histogram bars positions are calculted applying an exponential function: relIndex^exp
			startBarPt.setX( chartInfo->leftBorder() + relative2AbsoluteValf(pow( absolute2RelativeValf(i,NUMBER_OF_HISTOGRAM_BARS ), exp ), chartInfo->chartWidth()) );
			current_item = destinationScene.addLine(startBarPt.x(), startBarPt.y(), startBarPt.x(), (float)chartInfo->lowerBorder(), drawingPen);
			_transferFunctionBg << current_item;
		}
		else // ( &destinationScene == &_equalizerHistogramScene )
		{
			// histogram bars are added all at same distance
			startBarPt.setX( chartInfo->leftBorder() + ( barWidth * (i) ) );
			current_item = destinationScene.addRect(startBarPt.x(), startBarPt.y(), barWidth, barHeight, drawingPen, drawingBrush);
			_equalizerHistogramBars << current_item;
		}
		current_item->setZValue(-1);
	}
}


//Prepares the Transfer Function unit to work
void QualityMapperDialog::initTF()
{
	assert(_transferFunction != 0);

	//UPDATE OF PRESET COMBO BOX STATE

	//blocking comboBox signals
	ui.presetComboBox->blockSignals( true );

	//adding default TFs text to comboBox
	QString itemText;
	for (int i=0; i<NUMBER_OF_DEFAULT_TF; i++ )
	{
		//fetching default TF text
		itemText = TransferFunction::defaultTFs[(STARTUP_TF_TYPE + i)%NUMBER_OF_DEFAULT_TF];

		//items are added to the list only if they're not present yet
		if ( -1 == ui.presetComboBox->findText( itemText ) )
			ui.presetComboBox->addItem( itemText );
	}

	//adding to comboBox "Known External TFs", i.e. TFs previously loaded from external TF Files
	//for each external file TF the system stores the path of the file and its name (this one is added to comboBox)
	for (int i=0; i<_knownExternalTFs.size(); i++)
	{
		//fetching KETF from KETFs list
		itemText = _knownExternalTFs.at(i).name;

		//items are added to the list only if they're not present yet
		if ( -1 == ui.presetComboBox->findText( itemText ) )
			ui.presetComboBox->insertItem( 0, itemText );
	}
	
	//comboBox operations complete. Re-enabling signals for comboBox
	ui.presetComboBox->blockSignals( false );

	//building transfer function chart informations (if necessary)
	if ( _transferFunction_info == 0 )
		_transferFunction_info = new CHART_INFO( ui.transferFunctionView, /*_transferFunction->size(),*/ 0.0f, 1.0f, 0.0f, 1.0f );

	//removing and deleting any old TF graphics item (TF lines and handles)
	this->clearItems( REMOVE_TF_ALL | DELETE_REMOVED_ITEMS );

	//passing to TFHandles the pointer to the current Transfer Function
	assert(_transferFunction != 0);
	TFHandle::setTransferFunction(_transferFunction);

	//setting up handles
	TF_KEY *val = 0;
	QColor channelColor;
	qreal zValue = 0;
	for (int c=0; c<NUMBER_OF_CHANNELS; c++)
	{
		//processing z value
		zValue = (((*_transferFunction)[c].getType() + 1)*2.0f) + 1;
		TYPE_2_COLOR( c, channelColor );

		//adding to the TF view the TFHandles corresponding to the logical channel keys
		for (int i=0; i<_transferFunction->getChannel(c).size(); i++)
		{
			val = _transferFunction->getChannel(c)[i];
			this->addTfHandle( c,
							   QPointF(_transferFunction_info->leftBorder() + relative2AbsoluteValf( val->x, (float)_transferFunction_info->chartWidth() ), _transferFunction_info->lowerBorder() - relative2AbsoluteValf( val->y, (float)_transferFunction_info->chartHeight() )), 
							   val,
							   zValue );
		}
	}

	//adding to TF Scene TFHandles
	//if handles are not already in the transferFunctionScene, they're added to it
	if ( ! _transferFunctionScene.items().contains(_transferFunctionHandles[0][0]) )
		for (int i=0; i<NUMBER_OF_CHANNELS; i++)
			if ( _transferFunctionHandles[i].size() > 0 )
				for (int h=0; h<_transferFunctionHandles[i].size(); h++)
					_transferFunctionScene.addItem( _transferFunctionHandles[i][h] );

	//no TfHande selected yet
	_currentTfHandle = 0;

	//all done
	_isTransferFunctionInitialized = true;

	//refreshing TF BG
	this->drawTransferFunctionBG();

	//initializing forward channel to BLUE
	ui.blueButton->setChecked( true );
}

/*
Draws gammaCorrection only depending on equalizerMidHandlePercentilePosition and using a quadric spline
*/
void QualityMapperDialog::drawGammaCorrection()
{
	int width = ui.gammaCorrectionLabel->width();
	int height = ui.gammaCorrectionLabel->height();

	QPixmap pixmap(width, height);
	pixmap.fill();
	QPainter painter(&pixmap);
	painter.setOpacity(1.0);
	painter.setPen(Qt::black);
	painter.drawRect(0,0,width-1,height-1);

	painter.setPen(QColor(128,128,128));
	painter.drawLine(0,height-1,width-1,0);


	//// Painting the spline representing the exponential funcion: x^exp, 0<=x<=1
	int c = _equalizerMidHandlePercentilePosition*width;
	QPainterPath path;
	path.moveTo(0, height);
	path.quadTo(c, c, width, 0);
	painter.setPen(QColor(255,0,0));
	painter.drawPath(path);
	////painter.end();
	ui.gammaCorrectionLabel->setPixmap(pixmap);
	/*delete pixmap;*/
}

//draws the Transfer Function in the transfer function view
void QualityMapperDialog::drawTransferFunction()
{
	//before drawing, old TF lines are removed and destroyed
	this->clearItems( REMOVE_TF_LINES | DELETE_REMOVED_ITEMS );

	//TF must exist!
	assert(_transferFunction != 0);

	//building transfer function chart informations
	if ( _transferFunction_info == 0 )
		_transferFunction_info = new CHART_INFO( ui.transferFunctionView, /* _transferFunction->size(),*/ 0.0f, 1.0f, 0.0f, 1.0f );
	else
		// added by MAL 23/04/08
		_transferFunction_info->updateChartInfo( /* _transferFunction->size(),*/ 0.0f, 1.0f, 0.0f, 1.0f );

	//is necessary, initialize TF
	if ( !_isTransferFunctionInitialized )
		this->initTF();

	//drawing axis and other basic items
	this->drawChartBasics( _transferFunctionScene, _transferFunction_info );

	QColor channelColor;
	QPen drawingPen(Qt::black, 3);

	QGraphicsItem *item = 0;
	QGraphicsItem *handle1 = 0;
	QGraphicsItem *handle2 = 0;
	qreal zValue = 0;
	int channelType = 0;

	//for each channel
	for(int c=0; c<NUMBER_OF_CHANNELS; c++)
	{
		//processing correct channel type (the correct order is obtained through the [] operator of TransferFunction class)
		channelType = (*_transferFunction)[c].getType();
		//converting channel code in the proper color
		TYPE_2_COLOR(channelType, channelColor);
		//pen color is set to proper color
		drawingPen.setColor( channelColor );
		//z order for lines
		zValue = ((c + 1)*2.0f);

		QPointF pos1;
		QPointF pos2;
		//for each TfHandle of the current channel
		for (int i=0; i<_transferFunctionHandles[channelType].size(); i++)
		{
			//fetch the i-th handle
			handle1 = _transferFunctionHandles[channelType][i];
			//setting z-order in the scene
			handle1->setZValue( zValue);
			//if there's another handle after this in the sequence
			if ( (i+1)<_transferFunctionHandles[channelType].size() )
			{
				//fetching 2nd handle
				handle2 = _transferFunctionHandles[channelType][i+1];
				//setting z-order
				handle1->setZValue( zValue+1 );	//modified by MAL 15/02/08

				//fetching positions of handles
				pos1 = handle1->scenePos();
				pos2 = handle2->scenePos();
				//highlighting line if it touches the currently selected handle
				if (( handle1 == _currentTfHandle ) || (handle2 == _currentTfHandle) )
					drawingPen.setColor( channelColor.lighter() );
				else
					drawingPen.setColor( channelColor );
				//tracing line between the 2 handles
				item = _transferFunctionScene.addLine( handle1->scenePos().x(), handle1->scenePos().y(), handle2->scenePos().x(), handle2->scenePos().y(), drawingPen );
				//setting z-order of line
				item->setZValue( zValue );
				//adding line to lines list
				_transferFunctionLines << item;
			}
		}
	}


	
	// updating Color Band
	this->updateColorBand();

	//_transferFunctionScene.addRect(0.5,0.5,0.2,0.2);
	ui.transferFunctionView->setScene( &_transferFunctionScene );
	ui.transferFunctionView->update();
}

/*
Updates color band label
*/
void QualityMapperDialog::updateColorBand()
{
	// Calculating the vector of colors representing the transfer function
	QColor* colors = _transferFunction->buildColorBand();

	// Building the image that will represent to be inserted in the label
	QImage image(ui.colorbandLabel->width(), 1, QImage::Format_RGB32);
	float step = ((float)COLOR_BAND_SIZE) / ((float)ui.colorbandLabel->width());

	// Setting each pixel of the image
	for (int i=0; i<image.width(); i++)
		image.setPixel (i, 0, colors[(int)(i*step)].rgb());

	ui.colorbandLabel->setPixmap(QPixmap::fromImage(image));
}

/*
Draws the partial histogram as background of the transfer funtion scene
*/
void QualityMapperDialog::drawTransferFunctionBG ()
{
	this->clearItems( REMOVE_TF_BG | DELETE_REMOVED_ITEMS );
		
	if (_histogram_info !=0)
	{
	//	int minIndex = _equalizer_histogram->Interize((float)ui.minSpinBox->value());
	//	int maxIndex = _equalizer_histogram->Interize((float)ui.maxSpinBox->value());
	//	drawHistogramBars (_transferFunctionScene, _transferFunction_info, minIndex, maxIndex, QColor(192,192,192));

		_transferFunction_info->maxY = this->computeEqualizerMaxY(_equalizer_histogram, (float)ui.minSpinBox->value(), (float)ui.maxSpinBox->value());
		drawHistogramBars (_transferFunctionScene, _transferFunction_info, (float)ui.minSpinBox->value(), (float)ui.maxSpinBox->value(), QColor(192,192,192));
	}
}

//callback for save button click
void QualityMapperDialog::on_savePresetButton_clicked()
{
	//setting default save name
	QString tfName = ui.presetComboBox->currentText();
	//user chooses the file to save and saves it onto disk
	EQUALIZER_INFO eqInfo;
	eqInfo.minQualityVal = ui.minSpinBox->value();
	eqInfo.midQualityPercentage = _equalizerMidHandlePercentilePosition;
	eqInfo.maxQualityVal = ui.maxSpinBox->value();
	eqInfo.brightness = (1.0f - (float)(ui.brightnessSlider->value())/(float)(ui.brightnessSlider->maximum()) )*2.0;
	
	QString tfPath = _transferFunction->saveColorBand( tfName, eqInfo );

	//user didn't select anything. Nothing to do.
	if (tfPath.isNull())
		return ;

	//building file info
	QFileInfo fi(tfPath);
	tfName = fi.fileName();
	QString ext = CSV_FILE_EXSTENSION;
	if ( tfName.endsWith( ext ) )
	tfName.remove( tfName.size() - ext.size(), ext.size() );

	//adding external file to the list of known ones
	KNOWN_EXTERNAL_TFS newTF( tfPath, tfName );
	_knownExternalTFs << newTF;

	this->clearItems( REMOVE_TF_ALL | DELETE_REMOVED_ITEMS );

	//preparing TF to work
	_isTransferFunctionInitialized = false;
	this->initTF();
	ui.presetComboBox->setCurrentIndex( 0 );
}

//callback for loading a new TF from an external CSV file
void QualityMapperDialog::on_loadPresetButton_clicked()
{
	//user chooses the file to load
	QString csvFileName = QFileDialog::getOpenFileName(0, "Open Transfer Function File", QDir::currentPath(), QString("Quality Mapper File (*") + QString(CSV_FILE_EXSTENSION) + QString(")") );

	//user didn't select anything. Nothing to do.
	if (csvFileName.isNull())
		return ;

	//deleting any previous TF object
	if ( _transferFunction )
		delete _transferFunction;

	//building new TF object from external file
	_transferFunction = new TransferFunction( csvFileName );

	//building file info
	QFileInfo fi(csvFileName);
	QString tfName = fi.fileName();
	QString ext = CSV_FILE_EXSTENSION;
	if ( tfName.endsWith( ext ) )
		tfName.remove( tfName.size() - ext.size(), ext.size() );

	//adding external file to the list of known ones
	KNOWN_EXTERNAL_TFS newTF( csvFileName, tfName );
	_knownExternalTFs << newTF;

	//preparing TF to work
	_isTransferFunctionInitialized = false;
	this->initTF();

	//setting combo box to TF just built
	ui.presetComboBox->setCurrentIndex( 0 );

	//setting equalizer values
	EQUALIZER_INFO eqData;
	loadEqualizerInfo(csvFileName, &eqData);
	eqData.brightness = ui.brightnessSlider->maximum() * (1.0f - (eqData.brightness / 2.0f));
	this->setEqualizerParameters(eqData);
/*	on_resetButton_clicked(csvFileName, &eqData);*/

	//drawing new TF
	this->drawTransferFunction();

	//applying preview if necessary
	if (ui.previewButton->isChecked()) //added by FB 07\02\08
		on_applyButton_clicked();
}


//callback to manage the user selection of a TF from combo box
//builds a new TF joined to newValue. It searches first among the default ones, then among the external file ones
void QualityMapperDialog::on_presetComboBox_textChanged(const QString &newValue)
{
	//searching newValue among default TFs
	for (int i=0; i<NUMBER_OF_DEFAULT_TF; i++)
	{
		//found it!
		if ( TransferFunction::defaultTFs[i] == newValue )
		{
			//deleting any previous TF object
			if ( _transferFunction )
				delete _transferFunction;

			//building a new one
			_transferFunction = new TransferFunction( (DEFAULT_TRANSFER_FUNCTIONS)i );

			//preparing TF to work
			this->initTF(); //added by MAL 04\02\08

			//drawing new TF
			this->drawTransferFunction();

			//applying preview if necessary
			if (ui.previewButton->isChecked()) //added by FB 07\02\08
				on_applyButton_clicked();
			return ;
		}
	}

	//TF selected is not a default one. Maybe it's a loaded or saved one. Searching among known external TFs
	KNOWN_EXTERNAL_TFS external_tf;
	for (int i=0; i<_knownExternalTFs.size(); i++)
	{
		external_tf = _knownExternalTFs.at(i);

		//found it!
		if ( newValue == external_tf.name )
		{
			//deleting any previous TF object
			if ( _transferFunction )
				delete _transferFunction;

			//building new TF object from external file
			_transferFunction = new TransferFunction( external_tf.path );

			//preparing TF to work
			this->initTF(); //added by MAL 04\02\08

			//drawing new TF
			this->drawTransferFunction();

			//applying preview if necessary
			if (ui.previewButton->isChecked()) //added by FB 07\02\08
				on_applyButton_clicked();
			return ;
		}
	}
}

//callback for RED radio button
void QualityMapperDialog::on_redButton_toggled(bool checked)
{
	//if checked moves ahead the RED CHANNEL TF items
	if (checked)
		this->moveAheadChannel( RED_CHANNEL );
}

//callback for GREEN radio button
void QualityMapperDialog::on_greenButton_toggled(bool checked)
{
	//if checked moves ahead the GREEN CHANNEL TF items
	if (checked)
		this->moveAheadChannel( GREEN_CHANNEL );
}

//callback for BLUE radio button
void QualityMapperDialog::on_blueButton_toggled(bool checked)
{
	//if checked moves ahead the BLUE CHANNEL TF items
	if (checked)	
		this->moveAheadChannel( BLUE_CHANNEL );
}

//bring in first plane the TfHandles and Tf lines of the channel passed to it
void QualityMapperDialog::moveAheadChannel( TF_CHANNELS channelCode )
{
	//if Transfer Function object instanced
	if ( _transferFunction )
	{
		//changing drawing order for channel lines
		_transferFunction->moveChannelAhead( channelCode );

		//changing z order of TF handles
		QGraphicsItem *item = 0;
		for (int i=0; i<NUMBER_OF_CHANNELS; i++)
			foreach( item, _transferFunctionHandles[i] )
				item->setZValue( ((i + 1)*2.0f) + 1 );

		//all done. Drawing updated TF
		this->drawTransferFunction();
	}
}

/*
Redraws histogram background of transfer function scene and updates x-quality Label
*/
void QualityMapperDialog::on_EQHandle_moved()
{
	if ( _transferFunction )
		// Redrawing histogram background of transfer function scene
		this->drawTransferFunctionBG();

	if ( _currentTfHandle )
		//updating x-quality Label
		this->updateXQualityLabel(_currentTfHandle->getRelativeX());
}

//callback to manage move of a TfHandle object
//updates the object position in the position spinboxes, restores the correct order among the TfHandle objects and refreshes the TF scene
void QualityMapperDialog::on_TfHandle_moved(TFHandle *sender)
{
	//suspending signals from sender
	sender->blockSignals( true );

	//setting position spinboxes to Handle position
	ui.xSpinBox->blockSignals( true );
	ui.xSpinBox->setValue(sender->getRelativeX());
	ui.xSpinBox->blockSignals( false );

	ui.ySpinBox->blockSignals( true );
	ui.ySpinBox->setValue(sender->getRelativeY());
	ui.ySpinBox->blockSignals( false );

	this->manageBorderTfHandles(sender);

	//updating correct order among TF Handle objects
	this->updateTfHandlesOrder(sender->getChannel());
	//refreshing TF scene
	this->drawTransferFunction();

	//all done. Unlocking sender signals
	sender->blockSignals( false );

	this->updateXQualityLabel(sender->getRelativeX());
}

void QualityMapperDialog::manageBorderTfHandles(TFHandle *handle)
{
	TF_KEY *firstKey = 0;
	TF_KEY *newKey = 0;
	if ( _transferFunction->getChannel(handle->getChannel()).size() > 0 )
	{
		firstKey = _transferFunction->getChannel(handle->getChannel())[0];
		if ( handle->getMyKey() == firstKey )
		{
			if ( ! _transferFunction->getChannel(handle->getChannel()).isHead(/*firstKey*/handle->getMyKey()) )
			{
				newKey = new TF_KEY(0.0f, handle->getRelativeY());
				_transferFunction->getChannel(handle->getChannel()).addKey(newKey);
				this->addTfHandle( handle->getChannel(),
					QPointF(_transferFunction_info->leftBorder() + relative2AbsoluteValf( 0.0f, (float)_transferFunction_info->chartWidth() ), _transferFunction_info->lowerBorder() - relative2AbsoluteValf( handle->getRelativeY(), (float)_transferFunction_info->chartHeight() )), 
					newKey,
					((handle->getChannel() + 1)*2.0f) + 1 );
			}
		}
	}

	TF_KEY *lastKey = 0;
	if ( _transferFunction->getChannel(handle->getChannel()).size() > 0 )
	{
		lastKey = _transferFunction->getChannel(handle->getChannel())[_transferFunction->getChannel(handle->getChannel()).size()-1];
		if ( handle->getMyKey() == lastKey )
		{
			if ( ! _transferFunction->getChannel(handle->getChannel()).isTail(/*lastKey*/handle->getMyKey()) )
			{
				newKey = new TF_KEY(1.0f, handle->getRelativeY());
				_transferFunction->getChannel(handle->getChannel()).addKey(newKey);
				this->addTfHandle( handle->getChannel(),
					QPointF(_transferFunction_info->leftBorder() + relative2AbsoluteValf( 1.0f, (float)_transferFunction_info->chartWidth() ), _transferFunction_info->lowerBorder() - relative2AbsoluteValf( handle->getRelativeY(), (float)_transferFunction_info->chartHeight() )), 
					newKey,
					((handle->getChannel() + 1)*2.0f) + 1);
			}
		}
	}
}

//callback to manage click on a TfHandle object
//updates the currenttfHandle attribute and refresh the position spinboxes
void QualityMapperDialog::on_TfHandle_clicked(TFHandle *sender)
{
	if (_currentTfHandle)
		_currentTfHandle->setCurrentlSelected( false );

	//updating currentTfHandle to sender
	_currentTfHandle = sender;
	_currentTfHandle->setCurrentlSelected( true );

	//setting position spinboxes to Handle position
	ui.xSpinBox->setValue(_currentTfHandle->getRelativeX());
	ui.ySpinBox->setValue(_currentTfHandle->getRelativeY());

	//moving handle channel in foreground
	this->moveAheadChannel((TF_CHANNELS)sender->getChannel());

	this->updateXQualityLabel(_currentTfHandle->getRelativeX());
	//applying preview if necessary
	if (ui.previewButton->isChecked()) //added by FB 07\02\08
		on_applyButton_clicked();
}

//callback to manage double-click on a TfHandle object
//updates the currenttfHandle attribute and refresh the position spinboxes
void QualityMapperDialog::on_TfHandle_doubleClicked(TFHandle *sender)
{
	//updating currentTfHandle to sender
	_currentTfHandle = sender;

	//removing sender
	_currentTfHandle = this->removeTfHandle(_currentTfHandle);
	//applying preview if necessary
	if (ui.previewButton->isChecked()) //added by FB 07\02\08
		on_applyButton_clicked();
}




void QualityMapperDialog::on_applyButton_clicked()
{
	float minQuality = ui.minSpinBox->value();	
	float maxQuality = ui.maxSpinBox->value();	

	// brightness value between 0 and 2
	float brightness = (1.0f - (float)(ui.brightnessSlider->value())/(float)(ui.brightnessSlider->maximum()) )*2.0;

	applyColorByVertexQuality((MeshModel&)mesh, _transferFunction, minQuality, maxQuality, (float)_equalizerMidHandlePercentilePosition, brightness);

	gla->update();
}

/* 
If preview button is toggled, apply color to mesh when a generic handle is released
*/
void QualityMapperDialog::meshColorPreview()
{
	//applying preview if necessary
	if (ui.previewButton->isChecked())
		on_applyButton_clicked();
}

void QualityMapperDialog::on_previewButton_clicked()
{
	if (ui.previewButton->isChecked())
		on_applyButton_clicked();
}

//callback that manages the value change of current Handle y position
void QualityMapperDialog::on_xSpinBox_valueChanged(double newX)
{
	// if any handle is selected
	if (_currentTfHandle)
	{
		//updating handle position in the scene
		_currentTfHandle->setPos(_transferFunction_info->leftBorder()+relative2AbsoluteValf(newX,_transferFunction_info->chartWidth()), _currentTfHandle->scenePos().y());
		//updating the Tf Handle position at logical level (update of joined TF_KEY)
		_currentTfHandle->updateTfHandlesState(_currentTfHandle->scenePos());
		this->manageBorderTfHandles(_currentTfHandle);
		//restoring the correct order for TfHandles (they're drawn in the same order as they're stored)
		this->updateTfHandlesOrder(_currentTfHandle->getChannel());
		//updating x-quality Label
		this->updateXQualityLabel(_currentTfHandle->getRelativeX());
		//refresh of TF
		this->drawTransferFunction();
		//applying preview if necessary
		if (ui.previewButton->isChecked()) //added by FB 07\02\08
			on_applyButton_clicked();
	}
}

//callback that manages the value change of current Handle y position
void QualityMapperDialog::on_ySpinBox_valueChanged(double newY)
{
	// if any handle is selected
	if (_currentTfHandle)
	{
		//updating handle position in the scene
		_currentTfHandle->setPos(_currentTfHandle->scenePos().x(), _transferFunction_info->chartHeight()+_transferFunction_info->upperBorder()-relative2AbsoluteValf(newY,_transferFunction_info->chartHeight()));
		//updating the Tf Handle position at logical level (update of joined TF_KEY)
		_currentTfHandle->updateTfHandlesState(_currentTfHandle->scenePos());
		//restoring the correct order for TfHandles (they're drawn in the same order as they're stored)
		this->manageBorderTfHandles(_currentTfHandle);
		this->updateTfHandlesOrder(_currentTfHandle->getChannel());
		//refresh of TF
		this->drawTransferFunction();
		//applying preview if necessary
		if (ui.previewButton->isChecked()) //added by FB 07\02\08
			on_applyButton_clicked();
	}
}

//orders the TfHandles by the x value of the joined TF_KEY
void QualityMapperDialog::updateTfHandlesOrder(int channelCode)
{
	//ordering TfHandles list (sort is used because of the tiny number of elements to manage)
	qSort(_transferFunctionHandles[channelCode].begin(), _transferFunctionHandles[channelCode].end(), TfHandleCompare);
}


//Resets original positions and values of equalizer handles, spinboxes and brightness slider
void QualityMapperDialog::on_resetButton_clicked()
{
	assert(_histogram_info != 0);
	EQUALIZER_INFO data;
	data.brightness = 50;
	data.minQualityVal = _histogram_info->minX;
	data.maxQualityVal = _histogram_info->maxX;
	data.midQualityPercentage = 0.5f;
	this->setEqualizerParameters(data);
}

// Method invoked when moving left/right EqHandles. It calls drawEqualizerHistogram with the correct parameters
void QualityMapperDialog::on_EqHandle_crossing_histogram(EqHandle* sender, bool insideHistogram)
{
	if (sender = _equalizerHandles[LEFT_HANDLE])
		drawEqualizerHistogram(insideHistogram, _rightHandleWasInsideHistogram);
	else
		drawEqualizerHistogram(_leftHandleWasInsideHistogram, insideHistogram);
}

//adds a TFHandle to the TF scene with the properties defined by passed parameters
//returns a pointer to the handle just added
TFHandle* QualityMapperDialog::addTfHandle(int channelCode, QPointF handlePos, TF_KEY *key, int zOrder )
{
	QColor channelColor;

	//converting channel code into proper color
	TYPE_2_COLOR(channelCode, channelColor);

	return this->addTfHandle( new TFHandle( _transferFunction_info, channelColor, handlePos, key, zOrder ) );
}

//adds a TFHandle to the TF scene with the properties defined by passed parameters
//returns a pointer to the handle just added
TFHandle* QualityMapperDialog::addTfHandle(TFHandle *handle)
{
	//adding new handle to list of TF Handless of the belonging channel
	_transferFunctionHandles[handle->getChannel()] << handle;

	//preparing handle to manage events
	connect(handle, SIGNAL(positionChanged(TFHandle*)), this, SLOT(on_TfHandle_moved(TFHandle*)));
	connect(handle, SIGNAL(clicked(TFHandle*)), this, SLOT(on_TfHandle_clicked(TFHandle*)));
	connect(handle, SIGNAL(doubleClicked(TFHandle*)), this, SLOT(on_TfHandle_doubleClicked(TFHandle*)));
	connect(handle, SIGNAL(handleReleased()), this, SLOT(meshColorPreview()));

	//adding handle to scene
	_transferFunctionScene.addItem((QGraphicsItem*)handle);

	return handle;
}

//remove a TFHandle from the scene (and at logical level too)
TFHandle* QualityMapperDialog::removeTfHandle(TFHandle *handle)
{
	//no handle. Nothing to do.
	if ( handle == 0)
		return handle;

	//removing TF Handle item from scene
	_transferFunctionScene.removeItem( handle );

	//removing it from TfHandles list
	for (int i=0; i<_transferFunctionHandles[handle->getChannel()].size(); i++)
	{
		if ( _transferFunctionHandles[handle->getChannel()][i] == handle )
		{
			_transferFunctionHandles[handle->getChannel()].removeAt(i);
			break;
		}
	}

	//destroying joined logical key
	_transferFunction->getChannel(_currentTfHandle->getChannel()).removeKey(handle->getMyKey());

	//disconnecting and destroying handle
	handle->disconnect();
	delete handle;
	handle = 0;

	//no more traces of handle...
	//refreshing TF
	this->drawTransferFunction();

	return handle;
}


//callback that manages the value change of current Handle y position
void QualityMapperDialog::on_TF_view_doubleClicked(QPointF pos)
{
	//getting channel for new handle:
	int channelCode = -1;

//	if ( _currentTfHandle != 0)
//		//if an handle was already selected let's use the same channel of the selected one
//		channelCode = _currentTfHandle->getChannel();
//	else
//changed by MAL 28\02\08

	//let's use the more ahead channel in TF
		channelCode = _transferFunction->getFirstPlaneChanel();

	float xPos = pos.x() - _transferFunction_info->leftBorder();
	float yPos = pos.y() - _transferFunction_info->upperBorder();
	TF_KEY *val = new TF_KEY(absolute2RelativeValf(xPos, _transferFunction_info->chartWidth()),
							 absolute2RelativeValf(yPos, _transferFunction_info->chartHeight()));
	_transferFunction->getChannel(channelCode).addKey(val);
	TFHandle *newHandle = this->addTfHandle(channelCode, pos, val, ((channelCode + 1)*2.0f) + 1 );

	if (_currentTfHandle)
		_currentTfHandle->setCurrentlSelected( false );

	//updating currentTfHandle to sender
	_currentTfHandle = newHandle;
	_currentTfHandle->setCurrentlSelected( true );

	//updating correct order among TF Handle objects
	this->updateTfHandlesOrder(newHandle->getChannel());
	//refreshing TF scene
	this->drawTransferFunction();

	this->updateXQualityLabel(_currentTfHandle->getRelativeX());

	//applying preview if necessary
	if (ui.previewButton->isChecked()) //added by FB 07\02\08
		on_applyButton_clicked();
}

// Cut unuseful tails from equalizer histogram
void QualityMapperDialog::on_clampButton_clicked()
{
	_leftHandleWasInsideHistogram = false;
	_rightHandleWasInsideHistogram = false;

	// Calculate new Min e Max values
	_histogram_info->minX = _equalizer_histogram->Percentile((float)ui.clampHistogramSpinBox->value()/100.0f);
	_histogram_info->maxX = _equalizer_histogram->Percentile(1.0f-(float)ui.clampHistogramSpinBox->value()/100.0f);

	initEqualizerSpinboxes(); // reset Spinboxes and handles
	drawEqualizerHistogram(true, true); // redrawing histogram

	//applying preview if necessary
	if (ui.previewButton->isChecked()) //added by FB 21\03\08
		on_applyButton_clicked();
}

//writes in the x-quality label the quality value corresponding to the position of the currently selected TF handle
void QualityMapperDialog::updateXQualityLabel(float xPos)
{
	// exp = log_0.5 (_equalizerMidHandlePercentilePosition)
	float exp = log10((float)_equalizerMidHandlePercentilePosition) / log10(0.5f);
	string precision;

	_currentTfHandleQualityValue.setNum(relative2QualityValf(xPos, ui.minSpinBox->value(), ui.maxSpinBox->value(), exp));
	if (_currentTfHandleQualityValue.size() < QUALITY_LABEL_DIGITS_NUM)
	{
		QChar paddedZeros[] = {'0','0','0','0','0','0'};
		if(_currentTfHandleQualityValue.indexOf('.') == -1)
			_currentTfHandleQualityValue.append('.');
		if ((QUALITY_LABEL_DIGITS_NUM - _currentTfHandleQualityValue.size()) > 0)
			_currentTfHandleQualityValue.insert(_currentTfHandleQualityValue.size(), paddedZeros, QUALITY_LABEL_DIGITS_NUM-_currentTfHandleQualityValue.size());
	}
	ui.xQualityLabel->setText(_currentTfHandleQualityValue);
}

// Resets original positions and values of equalizer handles, spinboxes and brightness slider
void QualityMapperDialog::setEqualizerParameters(EQUALIZER_INFO data)
{
	// Resetting brightnessSlider position
	ui.brightnessSlider->setSliderPosition(data.brightness);

	// Resetting equalizerHistogram spinboxes values
	ui.minSpinBox->setValue(data.minQualityVal);
	ui.minSpinBox->setRange(numeric_limits<int>::min(), data.maxQualityVal);
	//ui.minSpinBox->setRange(2*data.minQualityVal - data.maxQualityVal, 2*data.maxQualityVal - data.minQualityVal);

	ui.maxSpinBox->setValue(data.maxQualityVal);
	ui.maxSpinBox->setRange(data.minQualityVal, numeric_limits<int>::max());
	//ui.maxSpinBox->setRange(2*data.minQualityVal - data.maxQualityVal, 2*data.maxQualityVal - data.minQualityVal);

	ui.midSpinBox->setValue(((ui.maxSpinBox->value() - ui.minSpinBox->value()) * data.midQualityPercentage) + ui.minSpinBox->value());
	ui.midSpinBox->setRange(ui.minSpinBox->value(), ui.maxSpinBox->value());

	drawGammaCorrection();
	drawTransferFunctionBG();
	if (ui.previewButton->isChecked())
		on_applyButton_clicked();
}





void QualityMapperDialog::on_brightnessSlider_valueChanged(int value)
{
	ui.brightessSpinBox->blockSignals(true);
	ui.brightessSpinBox->setValue((double)value/50.0);
	ui.brightessSpinBox->blockSignals(false);
}

void QualityMapperDialog::on_brightessSpinBox_valueChanged(double value)
{
	ui.brightnessSlider->blockSignals(true);
	ui.brightnessSlider->setValue(value*50);
	ui.brightnessSlider->blockSignals(false);
}

void QualityMapperDialog::on_midSpinBox_valueChanged(double)
{
	_signalDir = SPINBOX2LABEL;

	if ( _signalDir != LABEL2SPINBOX )
		ui.midPercentageLine->blockSignals( true );
	QString val;
	if ((ui.maxSpinBox->value() - ui.minSpinBox->value()) > 0)
		val.setNum( 100.0f * absolute2RelativeValf( ui.midSpinBox->value() - ui.minSpinBox->value(), ui.maxSpinBox->value() - ui.minSpinBox->value() ), 'g', 4 );
	else
		val.setNum( 50.0f );

	ui.midPercentageLine->setText( val );
	if ( _signalDir != LABEL2SPINBOX )
		ui.midPercentageLine->blockSignals( false );
/*

	if (ui.previewButton->isChecked())
		on_applyButton_clicked();
*/

	_signalDir = UNKNOWN_DIRECTION;
}

void QualityMapperDialog::on_midPercentageLine_editingFinished()
{
	bool conversionPossible;
	float numericValue = ui.midPercentageLine->text().toFloat(&conversionPossible);

	_signalDir = LABEL2SPINBOX;


	if ( _signalDir != SPINBOX2LABEL )
		ui.midSpinBox->blockSignals( true );
	if ((conversionPossible) && (numericValue>=0) && (numericValue<=100) )
	{
		ui.midSpinBox->setValue( ui.minSpinBox->value() + relative2AbsoluteValf(numericValue, ui.maxSpinBox->value() - ui.minSpinBox->value()) / 100.0f );
		_equalizerHandles[MID_HANDLE]->setXBySpinBoxValueChanged(ui.midSpinBox->value());
	}
	else
		QMessageBox::warning(this, tr("Wrong text value"), tr("The value inserted in the text area has a bad range value or is of a not compatible type"), QMessageBox::Ok); 
	if ( _signalDir != SPINBOX2LABEL )
		ui.midSpinBox->blockSignals( false );

	/*
		if (ui.previewButton->isChecked())
				on_applyButton_clicked();*/
		

	_signalDir = UNKNOWN_DIRECTION;
}
