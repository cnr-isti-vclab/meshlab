#include "qualitymapperdialog.h"
#include <limits>
#include <QPen>
#include <QBrush>
#include <cmath>

using namespace vcg;


QualityMapperDialog::QualityMapperDialog(QWidget *parent, MeshModel *m) : QDialog(parent), mesh(m)
{
	ui.setupUi(this);

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
				//((TFHandle*)item)->disconnect();
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
		//questo dovrebbe implementarlo UCCIO!
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
	//y axis
	current_item = scene.addLine( chart_info->leftBorder, chart_info->upperBorder, chart_info->leftBorder, chart_info->lowerBorder, p );
	current_item->setZValue( 0 );
}

//_equalizerHistogramScene dovrebbe chiamarsi in realtà histogramScene
//l'histogram e la transfer function potrebbero diventare attributi di questa classe? valutare l'impatto.
//in generale il codice di questo metodo va ripulito un po'...
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
	connect(_equalizerHandles[LEFT_HANDLE],  SIGNAL(positionChanged()), this, SLOT(on_left_right_equalizerHistogram_handle_changed()));
	connect(_equalizerHandles[RIGHT_HANDLE], SIGNAL(positionChanged()), this, SLOT(on_left_right_equalizerHistogram_handle_changed()));

	// Connecting mid equalizerHistogram handle to gammaCorrectionLabel
	connect(_equalizerHandles[MID_HANDLE], SIGNAL(positionChanged()), this, SLOT(drawGammaCorrection()) );


	ui.equalizerGraphicsView->setScene(&_equalizerHistogramScene);
}



// Add histogramBars to destinationScene
void QualityMapperDialog::drawHistogramBars (QGraphicsScene& destinationScene, CHART_INFO *chartInfo, int minIndex, int maxIndex, QColor color)
{
	// Questro controllo è necessario perché se si cerca in Histogram il valore minimo viene restituito l'indice -1. Forse deve essere corretto? (UCCIO)
	if (minIndex<0)
		minIndex = 0;
	float barHeight = 0.0f;	//initializing height of the histogram bars
	float barWidth = chartInfo->chartWidth / (float)(maxIndex-minIndex);	//processing width of the histogram bars (4\5 of dX)
	//	float barSeparator = dX - barWidth; //processing space between consecutive bars of the histogram bars (1\5 of dX)

	QPen drawingPen(color);
	QBrush drawingBrush (color);

	QPointF startBarPt;
	QSizeF barSize;

	//drawing histogram bars
	QGraphicsItem *current_item = 0;
	for (int i = minIndex; i < maxIndex; i++)
	{
		barHeight = (float)(chartInfo->chartHeight * _equalizer_histogram->H[i]) / (float)_histogram_info->maxRoundedY;
		startBarPt.setX( chartInfo->leftBorder + ( barWidth * (i-minIndex) ) );
		startBarPt.setY( (float)chartInfo->lowerBorder - barHeight );

		barSize.setWidth(barWidth);
		barSize.setHeight(barHeight);

		//drawing histogram bar
		current_item = destinationScene.addRect(startBarPt.x(), startBarPt.y(), barSize.width(), barSize.height(), drawingPen, drawingBrush);
		current_item->setZValue(-1);
		if ( &destinationScene == &_transferFunctionScene )
			_transferFunctionBg << current_item;
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

	//setting up handles
	TF_CHANNEL_VALUE val;
	TFHandle *handle1 = 0;
	TFHandle *handle2 = 0;
	QColor channelColor;
	qreal xPos = 0;
	qreal yLeftPos = 0;
	qreal yRightPos = 0;
	qreal zValue = 0;
	for (int c=0; c<NUMBER_OF_CHANNELS; c++)
	{
		zValue = ((c + 1)*2.0f) + 1;
		/*TYPE_2_COLOR( c, channelColor );*/
		channelColor = Qt::black;
		for (int i=0; i<(*_transferFunction)[c].size(); i++)
		{
			val = (*_transferFunction)[c][i];

			xPos = _transferFunction_info->leftBorder + relative2AbsoluteValf( (* val.x), (float)_transferFunction_info->chartWidth );
			yLeftPos = _transferFunction_info->lowerBorder - relative2AbsoluteValf( val.y->getLeftJunctionPoint(), (float)_transferFunction_info->chartHeight );
			yRightPos = _transferFunction_info->lowerBorder - relative2AbsoluteValf( val.y->getRightJunctionPoint(), (float)_transferFunction_info->chartHeight );
			handle1 = new TFHandle( _transferFunction_info, channelColor, QPointF(xPos, yLeftPos), TFHandle::LEFT_JUNCTION_HANDLE, zValue );
// 			handle1->setColor(channelColor);
// 			handle1->setPos( xPos, yLeftPos );
// 			handle1->setZValue( zValue );
// 			handle1->setChannel(c);
			_transferFunctionHandles[c] << handle1;
			connect(handle1, SIGNAL(positionChanged()), this, SLOT(on_TfHandle_moved()));
//			int a = _transferFunctionHandles[c].size();
			_transferFunctionScene.addItem(handle1);
			if ( yLeftPos != yRightPos )
			{
				handle2 = new TFHandle( _transferFunction_info, channelColor, QPointF(xPos, yRightPos), TFHandle::RIGHT_JUNCTION_HANDLE, zValue );
// 				handle2->setColor(channelColor);
// 				handle2->setPos( xPos, yRightPos );
// 				handle2->setZValue( zValue );
				_transferFunctionHandles[c] << handle2;
				connect(handle2, SIGNAL(positionChanged()), this, SLOT(on_TfHandle_moved()));
				_transferFunctionScene.addItem(handle1);
			}
		}
	}

		
/*

		xPos = xStart + _histogram_info->chartWidth/2.0*i;
		//		_equalizerHandles[i].setColor(colors[i]);
		_equalizerHandles[ffi].setColor(Qt::black);
		_equalizerHandles[ffi].setPos(xPos, yPos);
		_equalizerHandles[ffi].setBarHeight(_histogram_info->chartHeight);
		_equalizerHandles[ffi].setZValue(1);
		_equalizerHandles[ffi].setHistogramInfo(_histogram_info);
		_equalizerHandles[ffi].setHandlesPointer(_equalizerHandles);
		_equalizerHandles[fffi].setMidHandlePercentilePosition(&_equalizerMidHandlePercentilePosition);
		_equalizerHandles[ffi].setSpinBoxPointer(ui.minSpinBox);
		_equalizerHistogramScene.addItem(&_equalizerHandles[fi]);
	}
	_equalizerHandles[LEFT_HANDLE].setType(LEFT_HANDLE);
	_equalizerHandles[LEFT_HANDLE].setSpinBoxPointer(ui.minSpinBox);
	_equalizerHandles[MID_HANDLE].setType(MID_HANDLE);
	_equalizerHandles[MID_HANDLE].setSpinBoxPointer(ui.midSpinBox);
	_equalizerHandles[RIGHT_HANDLE].setType(RIGHT_HANDLE);
	_equalizerHandles[RIGHT_HANDLE].setSpinBoxPointer(ui.maxSpinBox);
*/

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

	ui.blueButton->setChecked( true );
}

void QualityMapperDialog::drawGammaCorrection()
{
	int width = 100;
	int height = 100;

	QPixmap *pixmap = new QPixmap(width, height);
	QPainter painter(pixmap);

	/*
	double exp = _equalizerMidHandlePercentilePosition * 2;
	double step = 1.0/width;
	QPoint list[100];
	for (int i=0; i<width; i++)
		list[i] = QPoint(i, floor(pow(i*step,exp)*width));
	
	painter.drawPolyline(list,width);
	*/

	int c = _equalizerMidHandlePercentilePosition*width;
	QPainterPath path;
	path.moveTo(0, height);
	path.quadTo(c, c, width, 0);
	//path.cubicTo(c,c,c,c,width,0);

	painter.drawPath(path);

	
	//painter.drawArc(0, 0, pixmap->width(), pixmap->height(), 0, -90*16);
	
	ui.gammaCorrectionLabel->setPixmap(*pixmap);
	delete pixmap;
}


void QualityMapperDialog::drawTransferFunction()
{
	this->clearItems( REMOVE_TF_LINES | REMOVE_TF_BG | DELETE_REMOVED_ITEMS );

	if ( !_isTransferFunctionInitialized )
		this->initTF();
	
	assert(_transferFunction != 0);

	//building transfer function chart informations
	if ( _transferFunction_info == 0 )
		_transferFunction_info = new CHART_INFO( ui.transferFunctionView->width(), ui.transferFunctionView->height(), _transferFunction->size(), 0.0f, 1.0f, 0.0f, 1.0f );

	//drawing axis and other basic items
	this->drawChartBasics( _transferFunctionScene, _transferFunction_info );

	//	_transferFunctionScene.addLine(0, 0, 100, 430, QPen(Qt::green, 3));

	//questo per il momento è fisso e definito quì, ma dovrà essere gestito nella classe handle
	QPointF pointToRepresentLeft;
	QPointF pointToRepresentRight;
	QPointF previousPoint;
	QRectF pointRectLeft(0, 0, 2.0*MARKERS_RADIUS, 2.0*MARKERS_RADIUS );
	QRectF pointRectRight(0, 0, 2.0*MARKERS_RADIUS, 2.0*MARKERS_RADIUS );

	QColor channelColor;
	QPen drawingPen(Qt::black, 3);
	QBrush drawingBrush ( QColor(32, 32, 32), Qt::SolidPattern );


	//drawing chart points
	//	TfChannel *tf_c = 0;
	TF_CHANNEL_VALUE val;

	QGraphicsItem *current_item = 0;
	qreal zValue = 0;

	for(int c=0; c<NUMBER_OF_CHANNELS; c++)
	{
		//items removed, building new ones
		TYPE_2_COLOR((*_transferFunction)[c].getType(), channelColor);
		drawingPen.setColor( channelColor );
		drawingBrush.setColor( channelColor );
		zValue = ((c + 1)*2.0f);

		for (int i=0; i<(*_transferFunction)[c].size(); i++)
		{
			val = (*_transferFunction)[c][i];

			pointToRepresentLeft.setX( _transferFunction_info->leftBorder + relative2AbsoluteValf( (* val.x), (float)_transferFunction_info->chartWidth ) );
			pointToRepresentLeft.setY( _transferFunction_info->lowerBorder - relative2AbsoluteValf( val.y->getLeftJunctionPoint(), (float)_transferFunction_info->chartHeight ) /*/ _transferFunction_info->maxRoundedY*/ );
			pointToRepresentRight.setX( pointToRepresentLeft.x() );
			pointToRepresentRight.setY( _transferFunction_info->lowerBorder - relative2AbsoluteValf( val.y->getRightJunctionPoint(), (float)_transferFunction_info->chartHeight ) /*/ _transferFunction_info->maxRoundedY*/ );


			//linear interpolation between current point and previous one
			//interpolation will not be executed if the current point is the first of the list
			if (i > 0)
			{
				current_item = _transferFunctionScene.addLine(previousPoint.x(), previousPoint.y(), pointToRepresentLeft.x(), pointToRepresentLeft.y(), drawingPen);
				current_item->setZValue( zValue );
				_transferFunctionLines << current_item;

				if ( pointToRepresentLeft.y() != pointRectRight.y() )
				{
					current_item = _transferFunctionScene.addLine( pointToRepresentLeft.x(), pointToRepresentLeft.y(), pointToRepresentRight.x(), pointToRepresentRight.y(), drawingPen );
					current_item->setZValue( zValue);
					_transferFunctionLines << current_item;
				}
			}

			//drawing single current point
			pointRectLeft.setX(pointToRepresentLeft.x() - MARKERS_RADIUS );
			pointRectLeft.setY(pointToRepresentLeft.y() - MARKERS_RADIUS );
			pointRectLeft.setWidth(2.0*MARKERS_RADIUS);
			pointRectLeft.setHeight(2.0*MARKERS_RADIUS);
			//sostituire con disegno della TfHandle
			//current_item = _transferFunctionScene.addEllipse( pointRectLeft, drawingPen, drawingBrush );
			//current_item->setZValue( zValue );

			if ( pointToRepresentLeft.y() != pointToRepresentRight.y() )
			{
				pointRectRight.setX(pointToRepresentRight.x() - MARKERS_RADIUS );
				pointRectRight.setY(pointToRepresentRight.y() - MARKERS_RADIUS );
				pointRectRight.setWidth(pointRectLeft.width());
				pointRectRight.setHeight(pointRectLeft.height());
				//sostituire con disegno della TfHandle
				//current_item = _transferFunctionScene.addEllipse( pointRectRight, drawingPen, drawingBrush );
				//current_item->setZValue( zValue );
			}

			//updating previous point.
			//So, it's possible to interpolate linearly the current point with the previous one
			previousPoint = pointToRepresentRight;
		}
	}

	// updating colorBand
	this->updateColorBand();

	// drawing partial histogram (UCCIO)
	if (_histogram_info !=0)
	{
		int minIndex = _equalizer_histogram->Interize((float)ui.minSpinBox->value());
		int maxIndex = _equalizer_histogram->Interize((float)ui.maxSpinBox->value());
		drawHistogramBars (_transferFunctionScene, _transferFunction_info, minIndex, maxIndex, QColor(192,192,192));
	}

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


void QualityMapperDialog::on_addPointButton_clicked()
{

}

void QualityMapperDialog::on_savePresetButton_clicked()
{
	QString tfName = ui.presetComboBox->currentText();
	QString tfPath = _transferFunction->saveColorBand( tfName );
	
	QFileInfo fi(tfPath);
	tfName = fi.fileName();
	QString ext = CSV_FILE_EXSTENSION;
	if ( tfName.endsWith( ext ) )
	tfName.remove( tfName.size() - ext.size(), ext.size() );

	KNOWN_EXTERNAL_TFS newTF( tfPath, tfName );
	_knownExternalTFs << newTF;
	_isTransferFunctionInitialized = false;

	// FORSE QUANDO SI SALVA IL PRESET NON C'E' BISOGNO DI CANCELLARE TUTTO (UCCIO)
	this->clearItems( REMOVE_TF_ALL | DELETE_REMOVED_ITEMS );
	this->initTF();
	ui.presetComboBox->setCurrentIndex( 0 );
}


void QualityMapperDialog::on_loadPresetButton_clicked()
{
	QString csvFileName = QFileDialog::getOpenFileName(0, "Open Transfer Function File", QDir::currentPath(), "CSV File (*.csv)");
	if ( !csvFileName.isNull())
	{
		if ( _transferFunction )
		{
			delete _transferFunction;
			_transferFunction = 0;
		}
		_transferFunction = new TransferFunction( csvFileName );
	}

	QFileInfo fi(csvFileName);
	QString tfName = fi.fileName();
	QString ext = CSV_FILE_EXSTENSION;
	if ( tfName.endsWith( ext ) )
		tfName.remove( tfName.size() - ext.size(), ext.size() );

	KNOWN_EXTERNAL_TFS newTF( csvFileName, tfName );
	_knownExternalTFs << newTF;

	_isTransferFunctionInitialized = false;
	this->clearItems( REMOVE_TF_ALL | DELETE_REMOVED_ITEMS );
	this->initTF();
	ui.presetComboBox->setCurrentIndex( 0 );
	this->drawTransferFunction();
}

void QualityMapperDialog::on_presetComboBox_textChanged(const QString &newValue)
{
	//searching among default TFs
	for (int i=0; i<NUMBER_OF_DEFAULT_TF; i++)
	{
		if ( TransferFunction::defaultTFs[i] == newValue )
		{
			if ( _transferFunction )
				delete _transferFunction;

			_transferFunction = new TransferFunction( (DEFAULT_TRANSFER_FUNCTIONS)i );
			this->clearItems( REMOVE_TF_ALL | DELETE_REMOVED_ITEMS );
			this->drawTransferFunction();
			return ;
		}
	}

	//TF selected is not a default one. Maybe it's a loaded or saved one. Searching among known external TFs
	KNOWN_EXTERNAL_TFS external_tf;
	for (int i=0; i<_knownExternalTFs.size(); i++)
	{
		external_tf = _knownExternalTFs.at(i);

		if ( newValue == external_tf.name )
		{
			if ( _transferFunction )
				delete _transferFunction;

			_transferFunction = new TransferFunction( (DEFAULT_TRANSFER_FUNCTIONS)i );
			this->clearItems( REMOVE_TF_ALL | DELETE_REMOVED_ITEMS );
			this->drawTransferFunction();
			return ;
		}
	}
}

void QualityMapperDialog::on_redButton_toggled(bool checked)
{
	if (checked)
	{
		if ( _transferFunction )
		{
			_transferFunction->moveChannelAhead( RED_CHANNEL );
			this->drawTransferFunction();
		}
	}

}

void QualityMapperDialog::on_greenButton_toggled(bool checked)
{
	if (checked)
	{
		if ( _transferFunction )
		{
			_transferFunction->moveChannelAhead( GREEN_CHANNEL );
			this->drawTransferFunction();
		}
	}
}

void QualityMapperDialog::on_blueButton_toggled(bool checked)
{
	if (checked)
	{
		if ( _transferFunction )
		{
			_transferFunction->moveChannelAhead( BLUE_CHANNEL );
			this->drawTransferFunction();
		}
	}
}

void QualityMapperDialog::on_left_right_equalizerHistogram_handle_changed()
{

	if ( _transferFunction )
	{
		this->drawTransferFunction();
	}
}

void QualityMapperDialog::on_TfHandle_moved(TFHandle *sender)
{
/*
	la TFHandle sender deve aggiornare la propria posizione nel KEYS del canale d'appartenenza
	(*_transferFunction)[sender->getChannel()][sender]
*/
	this->drawTransferFunction();
}
