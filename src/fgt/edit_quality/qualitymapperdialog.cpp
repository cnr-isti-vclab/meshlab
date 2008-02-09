#include "qualitymapperdialog.h"
#include <limits>
#include <QPen>
#include <QBrush>
#include <cmath>

#include <vcg/complex/trimesh/update/color.h>

using namespace vcg;

bool TfHandleCompare(TFHandle*h1, TFHandle*h2)
{
	return (h1->getRelativeX() < h2->getRelativeX());
}


QualityMapperDialog::QualityMapperDialog(QWidget *parent, MeshModel *m, GLArea *gla) : QDockWidget(parent), mesh(m)
{
	ui.setupUi(this);

	this->setWidget(ui.frame);
	/*this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);*/
	this->setFloating(true);
	// Setting dialog position in top right corner
	QPoint p=parent->mapToGlobal(QPoint(0,0));
	this->setGeometry(p.x()+(parent->width()-width()),p.y()+40,width(),height() );

	this->gla = gla;

	//building up histogram...
	int numberOfBins = 100;
	_equalizer_histogram = new Histogramf();
	Frange mmmq(tri::Stat<CMeshO>::ComputePerVertexQualityMinMax(mesh->cm));
	this->ComputePerVertexQualityHistogram(mesh->cm, mmmq, _equalizer_histogram, numberOfBins);
	//...histogram built

	_histogram_info = 0;

	_transferFunction = new TransferFunction( STARTUP_TF_TYPE );
	_isTransferFunctionInitialized = false;
	_transferFunction_info = 0;
	_currentTfHandle = 0;

	this->initTF();

	//connect(this, SIGNAL(closing()),gla,SLOT(endEdit()) );

	// toggle Trackball button (?)

	//connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );
	//emit suspendEditToggle();

	//gla->suspendEditToggle();
}

QualityMapperDialog::~QualityMapperDialog()
{
	this->clearItems(REMOVE_ALL_ITEMS | DELETE_REMOVED_ITEMS);

	if ( _histogram_info )
	{
		delete _histogram_info;
		_histogram_info = 0;
	}

	if ( _transferFunction )
	{
		delete _transferFunction;
		_transferFunction = 0;
	}

	if ( _transferFunction_info )
	{
		delete _transferFunction_info;
		_transferFunction_info = 0;
	}
//	this->disconnect();
}


void QualityMapperDialog::ComputePerVertexQualityHistogram( CMeshO &m, Frange range, Histogramf *h, int bins )    // V1.0
{
			h->Clear();
			h->SetRange( range.minV, range.maxV, bins);
			CMeshO::VertexIterator vi;
			for(vi = m.vert.begin(); vi != m.vert.end(); ++vi)
				if(!(*vi).IsD()) h->Add((*vi).Q());		
}

// ATTUALMENTE NON VIENE MAI UTILIZZATA (UCCIO)
GRAPHICS_ITEMS_LIST* QualityMapperDialog::clearScene(QGraphicsScene *scene, int cleanFlag)
{
//	_removed_items.clear();

	//deleting scene items
	GRAPHICS_ITEMS_LIST allItems = _transferFunctionScene.items();
	QGraphicsItem *item = 0;
	foreach (item, allItems)
	{
		scene->removeItem( item );
		_removed_items << item;
	}

	if ((cleanFlag & DELETE_REMOVED_ITEMS) == DELETE_REMOVED_ITEMS)
	{
		foreach (item, _removed_items)
		{
			if ((item != _equalizerHandles[0]) || (item != _equalizerHandles[1]) || (item != _equalizerHandles[2]))
			{
				delete item;
				item = 0;
			}
		}
	}

	return &_removed_items;
}


//this method clears some particular types of items from dialog.
//toClear value represent the logical OR of some macros that define what to clear
//returns a list of poiters to Items removed (NOT DELETED!!)
GRAPHICS_ITEMS_LIST* QualityMapperDialog::clearItems(int toClear)
{
	_removed_items.clear();
	QGraphicsItem *item = 0;

	if ((toClear & REMOVE_TF_HANDLE) == REMOVE_TF_HANDLE)
	{
		//removing TF Handles
		for (int i=0; i<NUMBER_OF_CHANNELS; i++)
		{
/*			int a = _transferFunctionHandles[i].size();*/
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
		//just removing! NOT DELETING!!
		for (int i=0; i<NUMBER_OF_EQHANDLES; i++)
		{
			_equalizerHistogramScene.removeItem( _equalizerHandles[i] );
			_removed_items << _equalizerHandles[i];
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

	if ((toClear & REMOVE_HISTOGRAM) == REMOVE_HISTOGRAM)
	{
		//removing Histogram
		_removed_items = *(this->clearScene( &_equalizerHistogramScene, toClear & DO_NOT_DELETE_REMOVED_ITEMS ));
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

void QualityMapperDialog::drawChartBasics(QGraphicsScene& scene, CHART_INFO *chart_info)
{
	//a valid chart_info must be passed
	assert( chart_info != 0 );

	QPen p( Qt::black, 2 );
	QGraphicsItem *current_item = 0;

	//drawing axis
	//x axis
	current_item = scene.addLine( chart_info->leftBorder, chart_info->lowerBorder, chart_info->rightBorder, chart_info->lowerBorder, p );
	current_item->setZValue( 0 );
	_transferFunctionLines << current_item;
	//y axis
	current_item = scene.addLine( chart_info->leftBorder, chart_info->upperBorder, chart_info->leftBorder, chart_info->lowerBorder, p );
	current_item->setZValue( 0 );
	_transferFunctionLines << current_item;
}

// Add histogram bars to equalizerHistogram Scene
// (This method is called only once)
void QualityMapperDialog::drawEqualizerHistogram()
{
	//building histogram chart informations
	if ( _histogram_info == 0 )
	{
		//processing minY and maxY values for histogram
		int maxY = 0;
		int minY = std::numeric_limits<int>::max();
		for (int i=0; i<_equalizer_histogram->n; i++) 
		{
			if ( _equalizer_histogram->H[i] > maxY )
				maxY = _equalizer_histogram->H[i];

			if ( _equalizer_histogram->H[i] < minY )
				minY = _equalizer_histogram->H[i];
		}
		_histogram_info = new CHART_INFO( ui.equalizerGraphicsView->width(), ui.equalizerGraphicsView->height(), _equalizer_histogram->n, _equalizer_histogram->minv, _equalizer_histogram->maxv, minY, maxY );
		//_histogram_info->data = this;

	}

	//drawing axis and other basic items
	this->drawChartBasics( _equalizerHistogramScene, _histogram_info );

	//drawing histogram bars
	drawHistogramBars (_equalizerHistogramScene, _histogram_info, 0, _histogram_info->numOfItems, QColor(128,128,128));

	//drawing handles
	//QColor colors[] = { QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue) };
	QDoubleSpinBox* spinboxes[] = { ui.minSpinBox, ui.midSpinBox, ui.maxSpinBox };

	qreal xStart = _histogram_info->leftBorder;
	qreal xPos = 0.0;
	qreal yPos = _histogram_info->lowerBorder;
	_equalizerMidHandlePercentilePosition = 0.5f;
	for (int i=0; i<NUMBER_OF_EQHANDLES; i++)
	{
		xPos = xStart + _histogram_info->chartWidth/2.0*i;
//		_equalizerHandles[i].setColor(colors[i]);
		_equalizerHandles[i] = new EqHandle(_histogram_info, Qt::black, QPointF(xPos, yPos), 
											(EQUALIZER_HANDLE_TYPE)i, _equalizerHandles, &_equalizerMidHandlePercentilePosition, spinboxes[i], 
											1, 5);
		_equalizerHistogramScene.addItem(_equalizerHandles[i]);
	}


	// Setting spinbox values
	// (Se venissero inizializzati prima di impostare setHistogramInfo sulle handles darebbero errore nello SLOT setX delle handles.)
	double singleStep = (_histogram_info->maxX - _histogram_info->minX) / _histogram_info->chartWidth;
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

	ui.minSpinBox->setValue(_histogram_info->minX);
	ui.minSpinBox->setRange(_histogram_info->minX, _histogram_info->maxX);
	ui.minSpinBox->setSingleStep(singleStep);
	ui.minSpinBox->setDecimals(decimals);

	ui.midSpinBox->setValue((_histogram_info->maxX + _histogram_info->minX) / 2.0f);
	ui.midSpinBox->setRange(_histogram_info->minX, _histogram_info->maxX);
	ui.midSpinBox->setSingleStep(singleStep);
	ui.midSpinBox->setDecimals(decimals);

	ui.maxSpinBox->setValue(_histogram_info->maxX);
	ui.maxSpinBox->setRange(_histogram_info->minX, _histogram_info->maxX);
	ui.maxSpinBox->setSingleStep(singleStep);
	ui.maxSpinBox->setDecimals(decimals);

	//SETTING UP CONNECTIONS
	// Connecting spinboxes to handles
	connect(ui.minSpinBox, SIGNAL(valueChanged(double)), _equalizerHandles[LEFT_HANDLE],  SLOT(setXBySpinBoxValueChanged(double)));
	connect(ui.midSpinBox, SIGNAL(valueChanged(double)), _equalizerHandles[MID_HANDLE],   SLOT(setXBySpinBoxValueChanged(double)));
	connect(ui.maxSpinBox, SIGNAL(valueChanged(double)), _equalizerHandles[RIGHT_HANDLE], SLOT(setXBySpinBoxValueChanged(double)));
	
	// Connecting handles to spinboxes
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(positionChangedToSpinBox(double)), ui.minSpinBox, SLOT(setValue(double)));
	connect(_equalizerHandles[MID_HANDLE],   SIGNAL(positionChangedToSpinBox(double)), ui.midSpinBox, SLOT(setValue(double)));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(positionChangedToSpinBox(double)), ui.maxSpinBox, SLOT(setValue(double)));
	
	// Connecting left and right handles to mid handle
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(positionChanged()), _equalizerHandles[MID_HANDLE], SLOT(moveMidHandle()));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(positionChanged()), _equalizerHandles[MID_HANDLE], SLOT(moveMidHandle()));

	// Making spinboxes and handles changes redrawing transferFunctionScene
	// Nota: non è necessario connettere anche le spinbox (UCCIO) 
	//connect(ui.minSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_left_right_equalizerHistogram_handle_changed()));
	//connect(ui.maxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(on_left_right_equalizerHistogram_handle_changed()));
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(positionChanged()), this, SLOT(on_EQHandle_moved()));
	connect(_equalizerHandles[MID_HANDLE],  SIGNAL(positionChanged()), this, SLOT(on_EQHandle_moved()));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(positionChanged()), this, SLOT(on_EQHandle_moved()));

	// Connecting mid equalizerHistogram handle to gammaCorrectionLabel
	connect(_equalizerHandles[MID_HANDLE], SIGNAL(positionChanged()), this, SLOT(drawGammaCorrection()) );
	connect(ui.midSpinBox, SIGNAL(valueChanged(double)), this, SLOT(drawGammaCorrection()) );

	// Connecting handles to preview method
	connect(_equalizerHandles[LEFT_HANDLE], SIGNAL(handleReleased()), this, SLOT(on_handle_released()));
	connect(_equalizerHandles[MID_HANDLE], SIGNAL(handleReleased()), this, SLOT(on_handle_released()));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(handleReleased()), this, SLOT(on_handle_released()));
	connect(ui.brightnesslSlider, SIGNAL(sliderReleased()), this, SLOT(on_handle_released()));

	
	this->drawGammaCorrection();
	this->drawTransferFunctionBG();

	ui.equalizerGraphicsView->setScene(&_equalizerHistogramScene);
}


/*
// Add histogramBars to destinationScene
void QualityMapperDialog::drawHistogramBars (QGraphicsScene& destinationScene, CHART_INFO *chartInfo, int minIndex, int maxIndex, QColor color)
{
	if (&destinationScene == &(this->_transferFunctionScene))
	{
		this->drawHistogramBarsSTRETCHED(destinationScene, chartInfo, minIndex, maxIndex, color);
		return;
	}
	// Questro controllo è necessario perché se si cerca in Histogram il valore minimo viene restituito l'indice -1. Forse deve essere corretto? (UCCIO)
	if (minIndex<0)
		minIndex = 0;
	float barHeight = 0.0f;	//initializing height of the histogram bars
	float barWidth = chartInfo->chartWidth / (float)(maxIndex-minIndex);	//processing width of the histogram bars (4\5 of dX)
	//	float barSeparator = dX - barWidth; //processing space between consecutive bars of the histogram bars (1\5 of dX)

	QPen drawingPen(color);
	QBrush drawingBrush (color);

	QPointF startBarPt;

	//drawing histogram bars
	QGraphicsItem *current_item = 0;
	for (int i = minIndex; i < maxIndex; i++)
	{
		barHeight = (float)(chartInfo->chartHeight * _equalizer_histogram->H[i]) / (float)_histogram_info->maxRoundedY;
		startBarPt.setX( chartInfo->leftBorder + ( barWidth * (i-minIndex) ) );
		startBarPt.setY( (float)chartInfo->lowerBorder - barHeight );

		//drawing histogram bar
		current_item = destinationScene.addRect(startBarPt.x(), startBarPt.y(), barWidth, barHeight, drawingPen, drawingBrush);
		current_item->setZValue(-1);
		if ( &destinationScene == &_transferFunctionScene )
			_transferFunctionBg << current_item;
	}
}*/

// Add histogramBars to destinationScene with GAMMA-STRETCHING
void QualityMapperDialog::drawHistogramBars (QGraphicsScene& destinationScene, CHART_INFO *chartInfo, int minIndex, int maxIndex, QColor color)
{
	// Questro controllo è necessario perché se si cerca in Histogram il valore minimo viene restituito l'indice -1. Forse deve essere corretto? (UCCIO)
	if (minIndex<0)
		minIndex = 0;
	float barHeight = 0.0f;	//initializing height of the histogram bars
	float barWidth  = chartInfo->chartWidth / (float)(maxIndex-minIndex);	//processing width of the histogram bars (4\5 of dX)
	//	float barSeparator = dX - barWidth; //processing space between consecutive bars of the histogram bars (1\5 of dX)

	int numberOfItems = maxIndex - minIndex;
	float exp = log10(0.5f) / log10((float)_equalizerMidHandlePercentilePosition);

	QPen drawingPen(color);
	QBrush drawingBrush (color);

	QPointF startBarPt;

	//drawing histogram bars
	QGraphicsItem *current_item = 0;

	for (int i = minIndex; i < maxIndex; i++)
	{
		barHeight = (float)(chartInfo->chartHeight * _equalizer_histogram->H[i]) / (float)_histogram_info->maxRoundedY;
		
		startBarPt.setY( (float)chartInfo->lowerBorder - barHeight );

		//drawing histogram bar
		if ( &destinationScene == &_transferFunctionScene )
		{
			startBarPt.setX( chartInfo->leftBorder + relative2AbsoluteValf(pow( absolute2RelativeValf(i-minIndex,numberOfItems ), exp ), chartInfo->chartWidth) );
			current_item = destinationScene.addLine(startBarPt.x(), startBarPt.y(), startBarPt.x(), (float)chartInfo->lowerBorder, drawingPen);
			_transferFunctionBg << current_item;
		}
		else
		{
			startBarPt.setX( chartInfo->leftBorder + ( barWidth * (i-minIndex) ) );
			current_item = destinationScene.addRect(startBarPt.x(), startBarPt.y(), barWidth, barHeight, drawingPen, drawingBrush);
		}
		
		current_item->setZValue(-1);
			
	}
}


void QualityMapperDialog::initTF()
{
	assert(_transferFunction != 0);

//	ui.presetComboBox->disconnect(SIGNAL(textChanged()));
//	connect(&_equalizerHandles[0], SIGNAL(positionChanged()), &_equalizerHandles[1], SLOT(moveMidHandle()));
	ui.presetComboBox->blockSignals( true );

	QString itemText;
	for ( int i=0; i<NUMBER_OF_DEFAULT_TF; i++ )
	{
		itemText = TransferFunction::defaultTFs[(STARTUP_TF_TYPE + i)%NUMBER_OF_DEFAULT_TF];

		//items are added to the list only if they're not present yet
		if ( -1 == ui.presetComboBox->findText( itemText ) )
			ui.presetComboBox->addItem( itemText );
	}

	for (int i=0; i<_knownExternalTFs.size(); i++)
	{
		itemText = _knownExternalTFs.at(i).name;

		//items are added to the list only if they're not present yet
		if ( -1 == ui.presetComboBox->findText( itemText ) )
			ui.presetComboBox->insertItem( 0, itemText );
	}
	
	ui.presetComboBox->blockSignals( false );

	//building transfer function chart informations
	if ( _transferFunction_info == 0 )
		_transferFunction_info = new CHART_INFO( ui.transferFunctionView->width(), ui.transferFunctionView->height(), _transferFunction->size(), 0.0f, 1.0f, 0.0f, 1.0f );

	//removing old TF graphics items
	this->clearItems( REMOVE_TF_HANDLE | DELETE_REMOVED_ITEMS );

	//passing to TFHandles the pointer to the current Transfer Function
	assert(_transferFunction != 0);
	TFHandle::setTransferFunction(_transferFunction);

	//setting up handles
	TF_KEY *val = 0;
	TFHandle *handle1 = 0;
	TFHandle *handle2 = 0;
	QColor channelColor;
	qreal xPos = 0;
	qreal yLeftPos = 0;
	qreal yRightPos = 0;
	qreal zValue = 0;
	int channelType = 0;
	for (int c=0; c<NUMBER_OF_CHANNELS; c++)
	{
		zValue = ((c + 1)*2.0f) + 1;
		channelType = (*_transferFunction)[c].getType();
		TYPE_2_COLOR( channelType, channelColor );

		for (int i=0; i<(*_transferFunction)[c].size(); i++)
		{
			val = (*_transferFunction)[channelType][i];

			xPos = _transferFunction_info->leftBorder + relative2AbsoluteValf( val->x, (float)_transferFunction_info->chartWidth );
			yLeftPos = _transferFunction_info->lowerBorder - relative2AbsoluteValf( val->y, (float)_transferFunction_info->chartHeight );
			yRightPos = _transferFunction_info->lowerBorder - relative2AbsoluteValf( val->y, (float)_transferFunction_info->chartHeight );
			handle1 = new TFHandle( _transferFunction_info, channelColor, QPointF(xPos, yLeftPos), val, zValue );
 			handle1->setZValue( zValue );
			_transferFunctionHandles[channelType] << handle1;
			connect(handle1, SIGNAL(positionChanged(TFHandle*)), this, SLOT(on_TfHandle_moved(TFHandle*)));
			connect(handle1, SIGNAL(clicked(TFHandle*)), this, SLOT(on_TfHandle_clicked(TFHandle*)));
			_transferFunctionScene.addItem(handle1);
			if ( yLeftPos != yRightPos )
			{
				handle2 = new TFHandle( _transferFunction_info, channelColor, QPointF(xPos, yRightPos), val, zValue );
 				handle2->setZValue( zValue );
				_transferFunctionHandles[channelType] << handle2;
				connect(handle2, SIGNAL(positionChanged(TFHandle*)), this, SLOT(on_TfHandle_moved(TFHandle*)));
				_transferFunctionScene.addItem(handle1);
			}
		}
	}
//	aggiungere le TFHandles quì controllando che nn siano già presenti nella lista di Handles per quel canale

	//adding to TF Scene TFlines and TFHandles
	if ( ! _transferFunctionScene.items().contains(_transferFunctionHandles[0][0]) )
		for (int i=0; i<NUMBER_OF_CHANNELS; i++)
			if ( _transferFunctionHandles[i].size() > 0 )
				for (int h=0; h<_transferFunctionHandles[i].size(); h++)
					_transferFunctionScene.addItem( _transferFunctionHandles[i][h] );

	if ( _transferFunctionLines.size() > 0 )
		if (!_transferFunctionScene.items().contains( _transferFunctionLines[0] ) )
			for ( int l=0; l<_transferFunctionLines.size(); l++ )
				_transferFunctionScene.addItem( _transferFunctionLines[l] );

	_isTransferFunctionInitialized = true;

	this->drawTransferFunctionBG();

	ui.blueButton->setChecked( true );
}

void QualityMapperDialog::drawGammaCorrection()
{
	int width = ui.gammaCorrectionLabel->width();
	int height = ui.gammaCorrectionLabel->height();

	QPixmap *pixmap = new QPixmap(width, height);
	QPainter painter(pixmap);

	painter.setPen(QColor(128,128,128));
	painter.drawLine(0,height-1,width-1,0);

	painter.setPen(Qt::black);
	painter.drawRect(0,0,width-1,height-1);
	int c = _equalizerMidHandlePercentilePosition*width;
	QPainterPath path;
	path.moveTo(0, height);
	path.quadTo(c, c, width, 0);
	//path.cubicTo(c,c,c,c,width,0);

	painter.drawPath(path);

	
	//painter.drawArc(0, 0, pixmap->width(), pixmap->height(), 0, -90*16);
	
	ui.gammaCorrectionLabel->setPixmap(*pixmap);
	painter.end();
	delete pixmap;
}


void QualityMapperDialog::drawTransferFunction()
{
	this->clearItems( REMOVE_TF_LINES | DELETE_REMOVED_ITEMS );

	assert(_transferFunction != 0);

	//building transfer function chart informations
	if ( _transferFunction_info == 0 )
		_transferFunction_info = new CHART_INFO( ui.transferFunctionView->width(), ui.transferFunctionView->height(), _transferFunction->size(), 0.0f, 1.0f, 0.0f, 1.0f );

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

	for(int c=0; c<NUMBER_OF_CHANNELS; c++)
	{
		channelType = (*_transferFunction)[c].getType();
		TYPE_2_COLOR(channelType, channelColor);
		drawingPen.setColor( channelColor );
		zValue = ((c + 1)*2.0f);

		QPointF pos1;
		QPointF pos2;
		for (int i=0; i<_transferFunctionHandles[channelType].size(); i++)
		{
			handle1 = _transferFunctionHandles[channelType][i];
			handle1->setZValue( zValue);
			if ( (i+1)<_transferFunctionHandles[channelType].size() )
			{
				handle2 = _transferFunctionHandles[channelType][i+1];
				handle1->setZValue( zValue);

				pos1 = handle1->scenePos();
				pos2 = handle2->scenePos();
				item = _transferFunctionScene.addLine( handle1->scenePos().x(), handle1->scenePos().y(), handle2->scenePos().x(), handle2->scenePos().y(), drawingPen );
				item->setZValue( zValue );
				_transferFunctionLines << item;
			}
		}
	}

	// updating colorBand
	this->updateColorBand();

	ui.transferFunctionView->setScene( &_transferFunctionScene );
}


void QualityMapperDialog::updateColorBand()
{
	QColor* colors = _transferFunction->buildColorBand();
	QImage image(ui.colorbandLabel->width(), 1, QImage::Format_RGB32);
	float step = ((float)COLOR_BAND_SIZE) / ((float)ui.colorbandLabel->width());

	for (int i=0; i<image.width(); i++)
		image.setPixel (i, 0, colors[(int)(i*step)].rgb());

	ui.colorbandLabel->setPixmap(QPixmap::fromImage(image));
}

void QualityMapperDialog::drawTransferFunctionBG ()
{
	this->clearItems( REMOVE_TF_BG | DELETE_REMOVED_ITEMS );

	if (_histogram_info !=0)
	{
		int minIndex = _equalizer_histogram->Interize((float)ui.minSpinBox->value());
		int maxIndex = _equalizer_histogram->Interize((float)ui.maxSpinBox->value());
		drawHistogramBars (_transferFunctionScene, _transferFunction_info, minIndex, maxIndex, QColor(192,192,192));
	}
}


void QualityMapperDialog::on_addPointButton_clicked()
{

}

void QualityMapperDialog::on_savePresetButton_clicked()
{
	//setting default save name
	QString tfName = ui.presetComboBox->currentText();
	//user chooses the file to load
	QString tfPath = _transferFunction->saveColorBand( tfName );

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

	// FORSE QUANDO SI SALVA IL PRESET NON C'E' BISOGNO DI CANCELLARE TUTTO (UCCIO)
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
	QString csvFileName = QFileDialog::getOpenFileName(0, "Open Transfer Function File", QDir::currentPath(), "CSV File (*.csv)");

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


void QualityMapperDialog::on_EQHandle_moved()
{

	if ( _transferFunction )
	{
		this->drawTransferFunctionBG();
	}
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

	//updating correct order among Tf Handle objets
	this->updateTfHandlesOrder(sender->getChannel());
	//refreshing Tf scene
	this->drawTransferFunction();

	//all done. Unlocking sender signals
	sender->blockSignals( false );
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
}


void QualityMapperDialog::on_applyButton_clicked()
{
	// Colorazione della mesh
	float rangeMin = ui.minSpinBox->value();	
	float rangeMax = ui.maxSpinBox->value();	
    //tri::UpdateColor<CMeshO>::VertexQuality(mesh->cm,RangeMin,RangeMax);

	CMeshO::VertexIterator vi;

	float percentageQuality;
	// brightness value between 0 and 2
	float brightness = (1.0f - (float)(ui.brightnesslSlider->value())/(float)(ui.brightnesslSlider->maximum()) )*2.0;
	Color4b currentColor;
	for(vi=mesh->cm.vert.begin(); vi!=mesh->cm.vert.end(); ++vi)		
		if(!(*vi).IsD()) 
		{
			//(*vi).C().ColorRamp(minq,maxq,(*vi).Q());
			float vertexQuality = (*vi).Q();
			if (vertexQuality < rangeMin)
				percentageQuality = 0.0;
			else
				if (vertexQuality > rangeMax)
					percentageQuality = 1.0;
				else
					percentageQuality = pow( ((*vi).Q() - rangeMin) / (rangeMax - rangeMin) , (float)(2.0*_equalizerMidHandlePercentilePosition));

			currentColor = _transferFunction->getColorByQuality(percentageQuality);
			
			if (brightness!=1.0f) //Applying brightness to each color channel
				if (brightness<1.0f)
					for (int i=0; i<3; i++) 
						currentColor[i] = relative2AbsoluteVali(pow(absolute2RelativeValf(currentColor[i],255.0f),brightness), 255.0f);
				else
					for (int i=0; i<3; i++) 
						currentColor[i] = relative2AbsoluteVali(1.0f-pow(1.0f-absolute2RelativeValf(currentColor[i],255.0f),2-brightness), 255.0f);

			(*vi).C() = currentColor;
		}

	gla->update();
}

void QualityMapperDialog::on_handle_released()
{
	if (ui.previewButton->isChecked())
		on_applyButton_clicked();
}

void QualityMapperDialog::on_previewButton_clicked()
{
	on_applyButton_clicked();
}

//callback that manages the value change of current Handle y position
void QualityMapperDialog::on_xSpinBox_valueChanged(double newX)
{
	// if any handle is selected
	if (_currentTfHandle)
	{
		//updating handle position in the scene
		_currentTfHandle->setPos(_transferFunction_info->leftBorder+relative2AbsoluteValf(newX,_transferFunction_info->chartWidth), _currentTfHandle->scenePos().y());
		//updating the Tf Handle position at logical level (update of joined TF_KEY)
		_currentTfHandle->updateTfHandlesState(_currentTfHandle->scenePos());
		//restoring the correct order for TfHandles (they're drawn in the same order as they're stored)
		this->updateTfHandlesOrder(_currentTfHandle->getChannel());
		//refresh of TF
		this->drawTransferFunction();
	}
}

//callback that manages the value change of current Handle y position
void QualityMapperDialog::on_ySpinBox_valueChanged(double newY)
{
	// if any handle is selected
	if (_currentTfHandle)
	{
		//updating handle position in the scene
		_currentTfHandle->setPos(_currentTfHandle->scenePos().x(), _transferFunction_info->chartHeight+_transferFunction_info->upperBorder-relative2AbsoluteValf(newY,_transferFunction_info->chartHeight));
		//updating the Tf Handle position at logical level (update of joined TF_KEY)
		_currentTfHandle->updateTfHandlesState(_currentTfHandle->scenePos());
		//restoring the correct order for TfHandles (they're drawn in the same order as they're stored)
		this->updateTfHandlesOrder(_currentTfHandle->getChannel());
		//refresh of TF
		this->drawTransferFunction();
	}
}

//orders the TfHandles by the x value of the joined TF_KEY
void QualityMapperDialog::updateTfHandlesOrder(int channelCode)
{
	//ordering TfHandles list (sort is used because of the tiny number of elements to manage)
	qSort(_transferFunctionHandles[channelCode].begin(), _transferFunctionHandles[channelCode].end(), TfHandleCompare);
}