/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

$Log: stdpardialog.cpp,v $

****************************************************************************/

#include <GL/glew.h>
#include <QToolBar>
#include <QActionGroup>
#include <QDebug>
#include <QFontMetrics>
#include <QSettings>

#include "mainwindow.h"
#include "ui_layerDialog.h"
#include "layerDialog.h"
#include "../common/mlexception.h"

using namespace std;

LayerDialog::LayerDialog(QWidget *parent )    
	: QDockWidget(parent)
{
	ui = new Ui::layerDialog();
	setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow );
	this-> QWidget::setAttribute( Qt::WA_MacAlwaysShowToolWindow);
	setVisible(false);
	ui->setupUi(this);
	mw=qobject_cast<MainWindow *>(parent);
	_previd = -1;
	/*Horrible trick*/
	_docitem = new QTreeWidgetItem();
	_docitem->setHidden(true);

	_renderingtabcontainer = new QGroupBox(this);
	QVBoxLayout* groupboxlay = new QVBoxLayout();
	if (createRenderingParametersTab() != NULL)
	{
		_tabw->setVisible(true);
		groupboxlay->addWidget(_tabw);
	}
	_applytovis = new QCheckBox(QString("apply to all visible layers"),this);
	QString tooltip("When the option is activated all the following rendering actions (e.g. activating/deactivating rendering primitives, changing rendering parameters etc.) taken on a single mesh are propagated to all the other visible meshes (e.g. the ones having the \"open eye\" icon on the layer dialog). <br><br> <b>SHORTCUT</b>: SHIFT");
	_applytovis->setToolTip(tooltip);
	_applytovis->setLayoutDirection(Qt::RightToLeft);
	_applytovis->setChecked(false);
	groupboxlay->addWidget(_applytovis);
	_renderingtabcontainer->setLayout(groupboxlay);
	ui->renderingLayout->addWidget(_renderingtabcontainer);

	// The following connection is used to associate the click with the change of the current mesh.
	connect(ui->meshTreeWidget, SIGNAL(itemPressed(QTreeWidgetItem * , int  )) , this,  SLOT(meshItemClicked(QTreeWidgetItem * , int ) ) );

	connect(ui->meshTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));
	connect(ui->meshTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));

	// The following connection is used to associate the click with the switch between raster and mesh view.
	connect(ui->rasterTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem * , int  )) , this,  SLOT(rasterItemClicked(QTreeWidgetItem * , int ) ) );

	// state buttons
	isRecording = false;
	viewState[0] = viewState[1] = viewState[2] = viewState[3] = "";
	connect(ui->bW1, SIGNAL(clicked()), this, SLOT(clickW1()));
	connect(ui->bW2, SIGNAL(clicked()), this, SLOT(clickW2()));
	connect(ui->bW3, SIGNAL(clicked()), this, SLOT(clickW3()));
	connect(ui->bW4, SIGNAL(clicked()), this, SLOT(clickW4()));
	connect(ui->bV1, SIGNAL(clicked()), this, SLOT(clickV1()));
	connect(ui->bV2, SIGNAL(clicked()), this, SLOT(clickV2()));
	connect(ui->bV3, SIGNAL(clicked()), this, SLOT(clickV3()));
	connect(ui->bV4, SIGNAL(clicked()), this, SLOT(clickV4()));

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->meshTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->rasterTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->decParsTree->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->meshTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
	connect(ui->rasterTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
	connect(ui->decParsTree, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
	setMinimumSize(_tabw->size().width() + 10, minimumSize().height());
}

void LayerDialog::keyPressEvent ( QKeyEvent * event )
{   
	MeshDocument *md = mw->meshDoc();
	if((md != NULL) && (event->key() == Qt::Key_Space ))
	{
		RasterModel *rm= md->nextRaster(md->rm());
		if(rm!=0) md->setCurrentRaster(rm->id());
	}

	if (event->key() == Qt::Key_Shift)
	{
		if (_applytovis != NULL)
			_applytovis->setCheckState(Qt::Checked);
	}

	if (event->key() == Qt::Key_Control)
	{
		isRecording = true;
	}
}

void LayerDialog::keyReleaseEvent(QKeyEvent * event)
{
	if (event->key() == Qt::Key_Shift)
	{
		if (_applytovis != NULL)
			_applytovis->setCheckState(Qt::Unchecked);
	}

	if (event->key() == Qt::Key_Control)
	{
		isRecording = false;
	}
}

void LayerDialog::clickW1()
{
	if (isRecording)
	{
		viewState[0] = mw->GLA()->viewToText();
		isRecording = false;
		ui->bW1->setText(QChar(0x2460));
		mw->meshDoc()->Log.log(0, "Stored View #1");
	}
	else if (viewState[0] != "")
	{
		QDomDocument doc("StringDoc");
		doc.setContent(viewState[0]);
		mw->GLA()->loadViewFromViewStateFile(doc);
		mw->meshDoc()->Log.log(0, "Restored View #1");
	}
	else
		mw->meshDoc()->Log.log(0, "No View to Restore");
}

void LayerDialog::clickW2()
{
	if (isRecording)
	{
		viewState[1] = mw->GLA()->viewToText();
		isRecording = false;
		ui->bW2->setText(QChar(0x2461));
		mw->meshDoc()->Log.log(0, "Stored View #2");
	}
	else if (viewState[1] != "")
	{
		QDomDocument doc("StringDoc");
		doc.setContent(viewState[1]);
		mw->GLA()->loadViewFromViewStateFile(doc);
		mw->meshDoc()->Log.log(0, "Restored View #2");
	}
	else
		mw->meshDoc()->Log.log(0, "No View to Restore");
}

void LayerDialog::clickW3()
{
	if (isRecording)
	{
		viewState[2] = mw->GLA()->viewToText();
		isRecording = false;
		ui->bW3->setText(QChar(0x2462));
		mw->meshDoc()->Log.log(0, "Stored View #3");
	}
	else if (viewState[2] != "")
	{
		QDomDocument doc("StringDoc");
		doc.setContent(viewState[2]);
		mw->GLA()->loadViewFromViewStateFile(doc);
		mw->meshDoc()->Log.log(0, "Restored View #3");
	}
	else
		mw->meshDoc()->Log.log(0, "No View to Restore");
}

void LayerDialog::clickW4()
{
	if (isRecording)
	{
		viewState[3] = mw->GLA()->viewToText();
		isRecording = false;
		ui->bW4->setText(QChar(0x2463));
		mw->meshDoc()->Log.log(0, "Stored View #4");
	}
	else if (viewState[3] != "")
	{
		QDomDocument doc("StringDoc");
		doc.setContent(viewState[3]);
		mw->GLA()->loadViewFromViewStateFile(doc);
		mw->meshDoc()->Log.log(0, "Restored View #4");
	}
	else
		mw->meshDoc()->Log.log(0, "No View to Restore");
}

void LayerDialog::clickV1()
{
	MeshDocument  *md = mw->meshDoc();
	if (isRecording)
	{
		visibilityState[0].clear();
		for(MeshModel& mp : md->meshIterator())
		{
			visibilityState[0].insert(mp.id(), mp.isVisible());
		}
		isRecording = false;
		ui->bV1->setText(QChar(0x2460));
		mw->meshDoc()->Log.log(0, "Stored Visibility #1");
	}
	else if (!visibilityState[0].isEmpty())
	{
		QMapIterator<int, bool> i(visibilityState[0]);
		while (i.hasNext()) {
			i.next();
			if (md->getMesh(i.key()) != NULL)
				mw->GLA()->meshSetVisibility(*md->getMesh(i.key()), i.value());
		}
		updatePerMeshItemVisibility();
		updatePerMeshItemSelectionStatus();
		mw->GLA()->update();
		mw->meshDoc()->Log.log(0, "Restored Visibility #1");
	}
	else
		mw->meshDoc()->Log.log(0, "No Visibility to Restore");
}
void LayerDialog::clickV2()
{
	MeshDocument  *md = mw->meshDoc();
	if (isRecording)
	{
		visibilityState[1].clear();
		for(MeshModel& mp : md->meshIterator())
		{
			visibilityState[1].insert(mp.id(), mp.isVisible());
		}
		isRecording = false;
		ui->bV2->setText(QChar(0x2461));
		mw->meshDoc()->Log.log(0, "Stored Visibility #2");
	}
	else if (!visibilityState[1].isEmpty())
	{
		QMapIterator<int, bool> i(visibilityState[1]);
		while (i.hasNext()) {
			i.next();
			if (md->getMesh(i.key()) != NULL)
				mw->GLA()->meshSetVisibility(*md->getMesh(i.key()), i.value());
		}
		updatePerMeshItemVisibility();
		updatePerMeshItemSelectionStatus();
		mw->GLA()->update();
		mw->meshDoc()->Log.log(0, "Restored Visibility #2");
	}
	else
		mw->meshDoc()->Log.log(0, "No Visibility to Restore");
}
void LayerDialog::clickV3()
{
	MeshDocument  *md = mw->meshDoc();
	if (isRecording)
	{
		visibilityState[2].clear();
		for(MeshModel& mp : md->meshIterator())
		{
			visibilityState[2].insert(mp.id(), mp.isVisible());
		}
		isRecording = false;
		ui->bV3->setText(QChar(0x2462));
		mw->meshDoc()->Log.log(0, "Stored Visibility #3");
	}
	else if (!visibilityState[2].isEmpty())
	{
		QMapIterator<int, bool> i(visibilityState[2]);
		while (i.hasNext()) {
			i.next();
			if (md->getMesh(i.key()) != NULL)
				mw->GLA()->meshSetVisibility(*md->getMesh(i.key()), i.value());
		}
		updatePerMeshItemVisibility();
		updatePerMeshItemSelectionStatus();
		mw->GLA()->update();
		mw->meshDoc()->Log.log(0, "Restored Visibility #3");
	}
	else
		mw->meshDoc()->Log.log(0, "No Visibility to Restore");
}
void LayerDialog::clickV4()
{
	MeshDocument  *md = mw->meshDoc();
	if (isRecording)
	{
		visibilityState[3].clear();
		for(MeshModel& mp : md->meshIterator())
		{
			visibilityState[3].insert(mp.id(), mp.isVisible());
		}
		isRecording = false;
		ui->bV4->setText(QChar(0x2463));
		mw->meshDoc()->Log.log(0, "Stored Visibility #4");
	}
	else if (!visibilityState[3].isEmpty())
	{
		QMapIterator<int, bool> i(visibilityState[3]);
		while (i.hasNext()) {
			i.next();
			if (md->getMesh(i.key()) != NULL)
				mw->GLA()->meshSetVisibility(*md->getMesh(i.key()), i.value());
		}
		updatePerMeshItemVisibility();
		updatePerMeshItemSelectionStatus();
		mw->GLA()->update();
		mw->meshDoc()->Log.log(0, "Restored Visibility #4");
	}
	else
		mw->meshDoc()->Log.log(0, "No Visibility to Restore");
}

void LayerDialog::enterEvent(QEvent* /*event*/)
{
	//activateWindow();		// why it was so?
	setFocus();
}

void LayerDialog::leaveEvent(QEvent* /*event*/)
{
	if (mw != NULL)
		mw->setFocus(Qt::MouseFocusReason);
}

void LayerDialog::meshItemClicked (QTreeWidgetItem * item , int col)
{
	if(item)
	{
		int clickedId= -1;
		MeshTreeWidgetItem *mItem = dynamic_cast<MeshTreeWidgetItem *>(item);
		if (mItem != NULL)
			clickedId= mItem->_meshid;

		MeshDocument  *md= mw->meshDoc();
		if (md == NULL)
			return;

		switch(col)
		{
		case 0 :
		{
			//the user has clicked on one of the eyes


			// NICE TRICK.
			// If the user has pressed ctrl when clicking on the eye icon, only that layer will remain visible
			// Very useful for comparing meshes
			if(QApplication::keyboardModifiers() == Qt::ControlModifier)
			{
				for(MeshModel& mp : md->meshIterator())
					mw->GLA()->meshSetVisibility(mp, false);
			}

			//Toggle visibility of current mesh
			if (mItem != NULL)
				mw->GLA()->meshSetVisibility(*md->getMesh(clickedId), !md->getMesh(clickedId)->isVisible());

			// EVEN NICER TRICK.
			// If the user has pressed alt when clicking on the eye icon, all layers will get visible
			// Very useful after you turned all layer invis using the previous option and want to avoid
			// clicking on all of them...
			if(QApplication::keyboardModifiers() == Qt::AltModifier)
			{
				for(MeshModel& mp : md->meshIterator())
				{
					mw->GLA()->meshSetVisibility(mp, true);
				}
			}

			if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
			{
				for(MeshModel& mp : md->meshIterator())
				{
					mw->GLA()->meshSetVisibility(mp, !mp.isVisible());
				}
				mw->GLA()->meshSetVisibility(*md->getMesh(clickedId), !md->getMesh(clickedId)->isVisible());
			}
			if (mItem != NULL)
				mw->meshDoc()->setCurrentMesh(clickedId);
			updatePerMeshItemVisibility();
		} break;
		case 1 :

		case 2 :

		case 3 :
			//the user has chosen to switch the layer
			if (mItem != NULL)
				mw->meshDoc()->setCurrentMesh(clickedId);
			/*this->updateDecoratorParsView(clickedId);*/
			//updateRenderingDataParsView();
			break;
		}
		if (clickedId != -1)
		{
			MLSceneGLSharedDataContext* shared = mw->currentViewContainer()->sharedDataContext();
			if (shared != NULL)
			{
				MLRenderingData dt;
				shared->getRenderInfoPerMeshView(clickedId,mw->GLA()->context(),dt);
				updateRenderingParametersTab(clickedId,dt);
			}
		}
		//make sure the right row is colored or that they right eye is drawn (open or closed)
		updatePerMeshItemSelectionStatus();
		mw->GLA()->update();
	}
}

void LayerDialog::rasterItemClicked (QTreeWidgetItem * item , int col)
{
	RasterTreeWidgetItem *rItem = dynamic_cast<RasterTreeWidgetItem *>(item);
	if ((rItem) && (mw != NULL) && (mw->meshDoc() != NULL))
	{

		int clickedId= rItem->_rasterid;
		RasterModel* rm = mw->meshDoc()->getRaster(clickedId);
		if (rm != NULL)
		{
			switch(col)
			{
			case 0 :
			{
				//the user has clicked on the "V" or "X"
				MeshDocument  *md= mw->meshDoc();

				// NICE TRICK.
				// If the user has pressed ctrl when clicking on the icon, only that layer will remain visible
				//
				if(QApplication::keyboardModifiers() == Qt::ControlModifier) {
					for(RasterModel& rp: md->rasterIterator()) {
						rp.setVisible(false);
					}
				}

				if(rm->isVisible()){
					rm->setVisible(false);
				}
				else{
					rm->setVisible(true);
				}

				// EVEN NICER TRICK.
				// If the user has pressed alt when clicking on the icon, all layers will get visible
				// Very useful after you turned all layer invis using the previous option and want to avoid
				// clicking on all of them...
				if(QApplication::keyboardModifiers() == Qt::AltModifier) {
					for(RasterModel& rp: md->rasterIterator()) {
						rp.setVisible(true);
					}
				}

				if(QApplication::keyboardModifiers() == Qt::ShiftModifier) {
					for(RasterModel& rp: md->rasterIterator()) {
						rp.setVisible(!rp.isVisible());
					}
					rm->setVisible(!rm->isVisible());
				}

				mw->GLA()->updateRasterSetVisibilities( );
				updatePerRasterItemVisibility();
			}
				// fall through
			case 1 :
			case 2 :
			case 3 :
				if(mw->meshDoc()->rm()->id() != clickedId || mw->GLA()->isRaster()) {
					mw->meshDoc()->setCurrentRaster(clickedId);
					if(mw->GLA()->isRaster())
						mw->GLA()->loadRaster(clickedId);
				}
				break;

			}
			updatePerRasterItemSelectionStatus();
			mw->GLA()->update();
		}
	}
}

void LayerDialog::showEvent ( QShowEvent * /* event*/ )
{
	emit toBeShow();
}

void LayerDialog::showContextMenu(const QPoint& pos)
{
	MeshDocument* doc = mw->meshDoc();
	if (doc == NULL)
		return;
	QObject* sigsender = sender();
	if (sigsender == ui->meshTreeWidget)
	{
		MeshTreeWidgetItem   *mItem = dynamic_cast<MeshTreeWidgetItem   *>(ui->meshTreeWidget->itemAt(pos.x(),pos.y()));
		if(mItem)
		{
			if (doc->getMesh(mItem->_meshid) != NULL)
				mw->meshDoc()->setCurrentMesh(mItem->_meshid);

			foreach (QWidget *widget, QApplication::topLevelWidgets())
			{
				MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
				if (mainwindow)
				{
					mainwindow->meshLayerMenu()->popup(ui->meshTreeWidget->mapToGlobal(pos));
					return;
				}
			}
		}
	}
	// switch layer
	else if (sigsender == ui->rasterTreeWidget)
	{
		RasterTreeWidgetItem *rItem = dynamic_cast<RasterTreeWidgetItem *>(ui->rasterTreeWidget->itemAt(pos.x(),pos.y()));

		if ((rItem) && (mw != NULL) && (mw->meshDoc() != NULL))
		{
			RasterModel* rm = mw->meshDoc()->getRaster(rItem->_rasterid);
			if (rm)
			{
				mw->meshDoc()->setCurrentRaster(rm->id());

				foreach (QWidget *widget, QApplication::topLevelWidgets())
				{
					MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
					if (mainwindow)
					{
						mainwindow->rasterLayerMenu()->popup(ui->rasterTreeWidget->mapToGlobal(pos));
						return;
					}
				}
			}
		}
	}
	else if (sigsender == ui->decParsTree)
	{
		DecoratorParamItem*rItem = dynamic_cast<DecoratorParamItem  *>(ui->decParsTree->itemAt(pos.x(),pos.y()));
		if (rItem != NULL)
			emit removeDecoratorRequested(rItem->act);
	}
}

void LayerDialog::updateLog(const GLLogStream &log)
{
	const QList< pair<int,QString> > &logStringList=log.logStringList();
	ui->logPlainTextEdit->clear();
	//ui->logPlainTextEdit->setFont(QFont("Courier",10));

	pair<int,QString> logElem;
#ifdef __APPLE__
	QString warningColor = this->palette().color(QPalette::HighlightedText).name();
	QString presystemColor = this->palette().color(QPalette::HighlightedText).name();
	QString prefilterColor = this->palette().color(QPalette::Text).name();
#else
	QString warningColor = "red";
	QString presystemColor = "grey";
	QString prefilterColor = "black";
#endif
	QString preWarn    = "<font face=\"courier\" size=3 color=\"" + warningColor +"\"> Warning: " ;
	QString preSystem  = "<font face=\"courier\" size=2 color=\"" + presystemColor +"\">" ;
	QString preFilter  = "<font face=\"courier\" size=2 color=\"" + prefilterColor + "\">" ;

	QString post   = "</font>";
	QString logText;
	for(const pair<int, QString>& logElem : logStringList){
		logText += logElem.second;
		if(logElem.first == GLLogStream::SYSTEM)
			logText = preSystem + logText + post;
		if(logElem.first == GLLogStream::WARNING)
			logText = preWarn + logText + post;
		if(logElem.first == GLLogStream::FILTER)
			logText = preFilter + logText + post;
		logText += "<BR>";
	}
	ui->logPlainTextEdit->appendHtml(logText);
}

void LayerDialog::updateTable()
{
	if (mw == NULL)
		return;
	MLSceneGLSharedDataContext* shared = mw->currentViewContainer()->sharedDataContext();
	if (shared != NULL)
	{
		MLSceneGLSharedDataContext::PerMeshRenderingDataMap dtf;
		if (mw->GLA() != NULL)
			shared->getRenderInfoPerMeshView(mw->GLA()->context(), dtf);
	}
}

void LayerDialog::updateTable(const MLSceneGLSharedDataContext::PerMeshRenderingDataMap& dtf)
{
	//TODO:Check if the current viewer is a GLArea
	if(!isVisible())
		return;
	if(isVisible() && !mw->GLA())
	{
		setVisible(false);
		//The layer dialog cannot be opened unless a new document is opened
		return;
	}
	MeshDocument *md=mw->meshDoc();
	if (md == NULL)
		return;

	ui->meshTreeWidget->clear();

	ui->meshTreeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);
	ui->meshTreeWidget->header()->hide();

	_docitem = new QTreeWidgetItem();
	_docitem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
	MLRenderingData projdt;

	updateProjectName(md->docLabel());

	QList<QTreeWidgetItem*> itms;
	for (MeshModel& mmd : md->meshIterator())
	{
		//Restore mesh visibility according to the current visibility map
		//very good to keep viewer state consistent
		if( mw->GLA()->meshVisibilityMap.contains(mmd.id())) {
			mmd.setVisible(mw->GLA()->meshVisibilityMap.value(mmd.id()));
		}
		else {
			mw->GLA()->meshVisibilityMap[mmd.id()]=true;
			mmd.setVisible(true);
		}
		MLSceneGLSharedDataContext::PerMeshRenderingDataMap::const_iterator rdit = dtf.find(mmd.id());
		if (rdit != dtf.end())
		{
			MLRenderingSideToolbar* rendertb = new MLRenderingSideToolbar(mmd.id(),this);
			rendertb->setIconSize(QSize(16,16));
			rendertb->setAccordingToRenderingData((*rdit));
			connect(rendertb,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SLOT(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
			connect(rendertb, SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)), this, SLOT(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
			connect(rendertb,SIGNAL(activatedAction(MLRenderingAction*)),this,SLOT(actionActivated(MLRenderingAction*)));

			MeshTreeWidgetItem* item = new MeshTreeWidgetItem(&mmd,ui->meshTreeWidget,rendertb);
			item->setExpanded(expandedMap.value(qMakePair(item->_meshid,-1)));

			addDefaultNotes(item,&mmd);
			itms.push_back(item);
			//Adding default annotations
		}
		else
			throw MLException("Something bad happened! Mesh id has not been found in the rendermapmode map.");
	}
	_docitem->addChildren(itms);

	updateTreeWidgetSizes(ui->meshTreeWidget);
	updatePerMeshItemVisibility();
	updatePerMeshItemSelectionStatus();

	if (md->rasterNumber() > 0)
		ui->rasterTreeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	else
		ui->rasterTreeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Ignored);
	ui->rasterTreeWidget->clear();
	ui->rasterTreeWidget->setColumnWidth(0,40);
	ui->rasterTreeWidget->setColumnWidth(1,20);
	ui->rasterTreeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);
	ui->rasterTreeWidget->header()->hide();
	for(RasterModel& rmd: md->rasterIterator()) {
		//Restore raster visibility according to the current visibility map
		//very good to keep viewer state consistent
		if( mw->GLA()->rasterVisibilityMap.contains(rmd.id()))
			rmd.setVisible(mw->GLA()->rasterVisibilityMap.value(rmd.id()));

		RasterTreeWidgetItem *item = new RasterTreeWidgetItem(&rmd);
		ui->rasterTreeWidget->addTopLevelItem(item);
		updatePerRasterItemVisibility();
		updatePerRasterItemSelectionStatus();

		//TODO scommenta quando inserisci tutta la lista dei planes
		//item->setExpanded(expandedMap.value(qMakePair(mmd->id(),-1)));
	}

	updateTreeWidgetSizes(ui->rasterTreeWidget);
}

//Reconstruct the correct layout of the treewidget after updating the main table. It is necessary to keep the changing
// (ex. the expansions of the treeWidgetItem) the user does.
void LayerDialog::adaptLayout(QTreeWidgetItem * item)
{
	item->setExpanded(item->isExpanded());
	updateTreeWidgetSizes(ui->meshTreeWidget);

	//Update expandedMap
	MeshTreeWidgetItem *mItem = dynamic_cast<MeshTreeWidgetItem *>(item);
	if(mItem){
		int meshId = mItem->_meshid;
		bool ok;
		int tagId = mItem->text(2).toInt(&ok);
		if(!ok || tagId < 0 )
			//MeshTreeWidgetItems don't have a tag id, so we use -1
			updateExpandedMap(meshId, -1, item->isExpanded());
	}
	else { //Other TreeWidgetItems
		MeshTreeWidgetItem *parent = dynamic_cast<MeshTreeWidgetItem *>(item->parent());
		if(parent){
			int meshId = parent->_meshid;
			bool ok;
			int tagId = item->text(2).toInt(&ok);
			if(!ok)
				updateExpandedMap(meshId, tagId, item->isExpanded());
		}
	}
}

//Add default annotations for each mesh about faces and vertices number
void LayerDialog::addDefaultNotes(QTreeWidgetItem * parent, MeshModel *meshModel)
{
	QTreeWidgetItem *fileItem = new QTreeWidgetItem();
	fileItem->setText(1, QString("File"));
	if(!meshModel->fullName().isEmpty())
		fileItem->setText(2, meshModel->shortName());
	parent->addChild(fileItem);
	updateColumnNumber(fileItem);

	QTreeWidgetItem *vertItem = new QTreeWidgetItem();
	vertItem->setText(1, QString("Vertices"));
	vertItem->setText(2, QString::number(meshModel->cm.vn));
	parent->addChild(vertItem);
	updateColumnNumber(vertItem);

	if(meshModel->cm.en>0){
		QTreeWidgetItem *edgeItem = new QTreeWidgetItem();
		edgeItem->setText(1, QString("Edges"));
		edgeItem->setText(2, QString::number(meshModel->cm.en));
		parent->addChild(edgeItem);
		updateColumnNumber(edgeItem);
	}

	QTreeWidgetItem *faceItem = new QTreeWidgetItem();
	faceItem->setText(1, QString("Faces"));
	faceItem->setText(2, QString::number(meshModel->cm.fn));
	parent->addChild(faceItem);
	updateColumnNumber(faceItem);

	std::vector<std::string> vertScalarNames;
	vcg::tri::Allocator<CMeshO>::GetAllPerVertexAttribute< Scalarm >(meshModel->cm,vertScalarNames);
	std::vector<std::string> vertPointNames;
	vcg::tri::Allocator<CMeshO>::GetAllPerVertexAttribute< Point3m >(meshModel->cm,vertPointNames);
	std::vector<std::string> faceScalarNames;
	vcg::tri::Allocator<CMeshO>::GetAllPerFaceAttribute< Scalarm >(meshModel->cm,faceScalarNames);
	std::vector<std::string> facePointNames;
	vcg::tri::Allocator<CMeshO>::GetAllPerFaceAttribute< Point3m >(meshModel->cm,facePointNames);

	unsigned int totAttributes = vertScalarNames.size() + vertPointNames.size() +
			faceScalarNames.size() + facePointNames.size();
	if (totAttributes > 0){
		QTreeWidgetItem *vertItem = new QTreeWidgetItem();
		QFont f;
		f.setBold(true);
		vertItem->setFont(1, f);
		vertItem->setText(1, QString("Custom Attrs:"));
		parent->addChild(vertItem);
		updateColumnNumber(vertItem);
	}

	for(const std::string& name: vertScalarNames) {
		QTreeWidgetItem *vertItem = new QTreeWidgetItem();
		vertItem->setText(2, QString("Vert (scalar):"));
		vertItem->setText(3, QString(name.c_str()));
		parent->addChild(vertItem);
		updateColumnNumber(vertItem);
	}

	for(const std::string& name: vertPointNames) {
		QTreeWidgetItem *vertItem = new QTreeWidgetItem();
		vertItem->setText(2, QString("Vert (vec3):"));
		vertItem->setText(3, QString(name.c_str()));
		parent->addChild(vertItem);
		updateColumnNumber(vertItem);
	}

	for(const std::string& name: faceScalarNames) {
		QTreeWidgetItem *vertItem = new QTreeWidgetItem();
		vertItem->setText(2, QString("Face (scalar):"));
		vertItem->setText(3, QString(name.c_str()));
		parent->addChild(vertItem);
		updateColumnNumber(vertItem);
	}

	for(const std::string& name: facePointNames) {
		QTreeWidgetItem *vertItem = new QTreeWidgetItem();
		vertItem->setText(2, QString("Face (vec3):"));
		vertItem->setText(3, QString(name.c_str()));
		parent->addChild(vertItem);
		updateColumnNumber(vertItem);
	}

	if (meshModel->cm.textures.size() > 0){
		QTreeWidgetItem *vertItem = new QTreeWidgetItem();
		QFont f;
		f.setBold(true);
		vertItem->setFont(1, f);
		vertItem->setText(1, QString("Textures:"));
		parent->addChild(vertItem);
		updateColumnNumber(vertItem);
	}

	for(const std::string& name: meshModel->cm.textures) {
		QTreeWidgetItem *vertItem = new QTreeWidgetItem();
		vertItem->setText(2, QString(name.c_str()));
		const QImage& img = meshModel->getTexture(name);
		QString size = QString::number(img.width()) + "x" + QString::number(img.height());
		vertItem->setText(3, QString(size));
		parent->addChild(vertItem);
		updateColumnNumber(vertItem);
	}

}

//Add, if necessary, columns to the treeWidget.
//It must be called every time a new treeWidget item is added to the tree.
void LayerDialog::updateColumnNumber(const QTreeWidgetItem * item)
{
	int columnChild= item->columnCount();
	int columnParent = ui->meshTreeWidget->columnCount();
	if(columnChild - columnParent>0)
		ui->meshTreeWidget->setColumnCount(columnParent + (columnChild-columnParent));
}

void LayerDialog::updateExpandedMap(int meshId, int tagId, bool expanded)
{
	expandedMap.insert(qMakePair(meshId, tagId),expanded);
}

void LayerDialog::actionActivated(MLRenderingAction* ract)
{
	if (ract != NULL)
		_tabw->switchTab(ract->meshId(),ract->text());
}

LayerDialog::~LayerDialog()
{
	delete ui;
}

void LayerDialog::updateDecoratorParsView()
{
	if ((mw->GLA() == NULL) || (mw->GLA()->md() == NULL) /*|| (mw->GLA()->md()->mm() == NULL)*/)
		return;

	QStringList expIt;
	int ind=0;
	while(QTreeWidgetItem *item = ui->decParsTree->topLevelItem(ind))
	{
		if (item->isExpanded())
			expIt.push_back(item->text(0));
		++ind;
	}
	ui->decParsTree->clear();
	if (!mw->GLA())
	{
		ui->decParsTree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Ignored);
		return;
	}
	// build the list of the meaningful decorator to be shown (perDocument + perMesh)
	QList<QAction*> decList =  mw->GLA()->iPerDocDecoratorlist;
	if (mw->GLA()->md()->mm() != NULL)
		decList += mw->GLA()->iCurPerMeshDecoratorList();
	if (decList.size() == 0)
	{
		ui->decParsTree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Ignored);
		return;
	}
	QList<QTreeWidgetItem*> treeItem;
	ui->decParsTree->clear();
	for(int ii = 0; ii < decList.size();++ii)
	{
		DecoratePlugin* decPlug =  qobject_cast<DecoratePlugin *>(decList[ii]->parent());
		if (!decPlug)
		{
			mw->GLA()->Log(GLLogStream::SYSTEM,"MeshLab System Error: A Decorator Plugin has been expected.");
			return;
		}
		else
		{
			DecoratorParamItem* item = new DecoratorParamItem(decList[ii]);
			item->setText(0,decList[ii]->text());
			QTreeWidgetItem* childItem = new QTreeWidgetItem();
			item->addChild(childItem);
			DecoratorParamsTreeWidget* dpti = new DecoratorParamsTreeWidget(decList[ii],mw,ui->decParsTree);
			dpti->setAutoFillBackground(true);
			ui->decParsTree->setItemWidget(childItem,0,dpti);
			treeItem.append(item);
		}
	}
	ui->decParsTree->insertTopLevelItems(0,treeItem);
	foreach(QString st,expIt)
	{
		QList<QTreeWidgetItem*> res = ui->decParsTree->findItems(st,Qt::MatchExactly);
		if (res.size() != 0)
			res[0]->setExpanded(true);
	}
	ui->decParsTree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::MinimumExpanding);
}

void LayerDialog::updatePerMeshItemSelectionStatus()
{
	MeshDocument* md = mw->meshDoc();
	if (md == nullptr)
		return;
	MeshModel* mm = md->mm();
	if (mm != nullptr) {
		for(int ii = 0; ii < ui->meshTreeWidget->topLevelItemCount();++ii) {
			MeshTreeWidgetItem* item = dynamic_cast<MeshTreeWidgetItem*>(ui->meshTreeWidget->topLevelItem(ii));
			if ((item != NULL)) {
				if((unsigned int)item->_meshid == mm->id()) {
					ui->meshTreeWidget->setCurrentItem(item);
					_tabw->updatePerMeshRenderingAction(item->_rendertoolbar->getRenderingActions());
					QString meshLabel = mm->label();
					if (meshLabel.size() > 40){
						QString fin = meshLabel.right(6);
						meshLabel.truncate(30);
						meshLabel += "...";
						meshLabel += fin;
					}
					_renderingtabcontainer->setTitle(meshLabel);
					updateDecoratorParsView();
				}
			}
		}
	}
	if (md->meshNumber() == 0)
		_renderingtabcontainer->setTitle(QString());
}

void LayerDialog::updatePerRasterItemSelectionStatus()
{
	MeshDocument* md = mw->meshDoc();
	if (md == NULL)
		return;
	for(int ii = 0; ii < ui->rasterTreeWidget->topLevelItemCount();++ii)
	{
		RasterTreeWidgetItem* item = dynamic_cast<RasterTreeWidgetItem*>(ui->rasterTreeWidget->topLevelItem(ii));
		RasterModel* rm = md->rm();
		if ((item != NULL) && (rm != NULL))
		{
			if(item->_rasterid == rm->id()) {
				ui->rasterTreeWidget->setCurrentItem(item);
			}
		}
	}
}

void LayerDialog::reset()
{
	ui->meshTreeWidget->clear();
	ui->decParsTree->clear();
	ui->rasterTreeWidget->clear();
	if (_applytovis != NULL)
		_applytovis->setChecked(false);
}

void LayerDialog::setCurrentTab(const MLRenderingData& dt)
{
	if ((mw == NULL) || (mw->meshDoc() == NULL) || (mw->meshDoc()->mm() == NULL))
		return;
	MLRenderingAction* act = NULL;
	int id = mw->meshDoc()->mm()->id();
	if (dt.isPrimitiveActive(MLRenderingData::PR_SOLID))
	{
		act = new MLRenderingSolidAction(id, NULL);
	}
	else
	{
		if (dt.isPrimitiveActive(MLRenderingData::PR_WIREFRAME_EDGES))
		{
			act = new MLRenderingWireAction(id, NULL);
		}
		else
		{
			if (dt.isPrimitiveActive(MLRenderingData::PR_POINTS))
			{
				act = new MLRenderingPointsAction(id, NULL);
			}
		}
	}
	actionActivated(act);
	delete act;
}

void LayerDialog::updateRenderingParametersTab(int meshid,const MLRenderingData& dt )
{
	if (_tabw != NULL)
	{
		_tabw->setAssociatedMeshId(meshid);
		MeshModel* mm;
		if ((mw != NULL) && (mw->meshDoc() != NULL))
		{
			mm = mw->meshDoc()->getMesh(meshid);
			if (mm != NULL)
			{
				if (mm->hasDataMask(MeshModel::MM_VERTTEXCOORD) && !(mm->hasDataMask(MeshModel::MM_WEDGTEXCOORD)))
					_tabw->setTextureAction(MLRenderingData::ATT_NAMES::ATT_VERTTEXTURE);
				else
					_tabw->setTextureAction(MLRenderingData::ATT_NAMES::ATT_WEDGETEXTURE);
				_tabw->updateVisibility(mm);
			}
		}

		_tabw->updateGUIAccordingToRenderingData(dt);
		
		//QRect tmpsize = geometry();
		//tmpsize.setWidth(_tabw->width() + 10);
		//setGeometry(tmpsize);
	}
}

MLRenderingParametersTab* LayerDialog::createRenderingParametersTab()
{

	MLRenderingSideToolbar* tmptool = new MLRenderingSideToolbar();
	tmptool->setVisible(false);
	_tabw = new MLRenderingParametersTab(-1, tmptool->getRenderingActions());
	_tabw->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);
	connect(_tabw,SIGNAL(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)),this,SLOT(updateRenderingDataAccordingToActions(int,const QList<MLRenderingAction*>&)));
	connect(_tabw,SIGNAL(updateRenderingDataAccordingToAction(int,MLRenderingAction*)),this,SLOT(updateRenderingDataAccordingToAction(int,MLRenderingAction*)));
	connect(_tabw, SIGNAL(updateLayerTableRequested()), this, SLOT(updateTable()));
	_tabw->setVisible(false);
	delete tmptool;
	return _tabw;
}

void LayerDialog::updateTreeWidgetSizes(QTreeWidget* tree)
{
	for(int i=0; i< tree->columnCount(); i++) {
		int oldSize = tree->columnWidth(i);
		tree->resizeColumnToContents(i);
		if (tree->columnWidth(i) > 200){
			tree->setColumnWidth(i, oldSize);
		}
	}
}

void LayerDialog::updateProjectName( const QString& name )
{
	setWindowTitle(name);
	QFileInfo fi(name);
	_docitem->setText(1,"");
	QFont fnt;
	fnt.setBold(true);
	_docitem->setFont(2,fnt);
	_docitem->setText(2,fi.baseName());
	//ui->meshTreeWidget->addTopLevelItem(_docitem);
	//ui->meshTreeWidget->setItemWidget(_docitem,3,_globaldoctool);
}

void LayerDialog::updatePerMeshItemVisibility()
{
	if (mw == NULL)
		return;
	MeshDocument* md = mw->meshDoc();
	if (md == NULL)
		return;
	bool allhidden = true;
	for(int ii = 0;ii < ui->meshTreeWidget->topLevelItemCount();++ii) {
		QTreeWidgetItem* qitm = ui->meshTreeWidget->topLevelItem(ii);
		MeshTreeWidgetItem* mitm = dynamic_cast<MeshTreeWidgetItem*>(qitm);
		if (mitm != NULL)
		{
			MeshModel* mm = md->getMesh(mitm->_meshid);
			if (mm != NULL)
			{
				allhidden = allhidden && !(mm->isVisible());
				mitm->updateVisibilityIcon(mm->isVisible());
			}
		}
	}

	if ((!(md->meshNumber() == 0)) && allhidden)
		_docitem->setIcon(0,QIcon(":/images/layer_eye_close.png"));
	else
		_docitem->setIcon(0,QIcon(":/images/layer_eye_open.png"));
}

void LayerDialog::updatePerRasterItemVisibility()
{
	if (mw == NULL)
		return;
	MeshDocument* md = mw->meshDoc();
	if (md == NULL)
		return;
	for(int ii = 0;ii < ui->rasterTreeWidget->topLevelItemCount();++ii) {
		QTreeWidgetItem* qitm = ui->rasterTreeWidget->topLevelItem(ii);
		RasterTreeWidgetItem* mitm = dynamic_cast<RasterTreeWidgetItem*>(qitm);
		if (mitm != NULL) {
			RasterModel* mm = md->getRaster(mitm->_rasterid);
			if (mm != NULL)
				mitm->updateVisibilityIcon(mm->isVisible());
		}
	}
}

void LayerDialog::updateRenderingDataAccordingToActions(int meshid,const QList<MLRenderingAction*>& acts)
{
	if (mw == NULL)
		return;
	if (!_applytovis->isChecked())
		mw->updateRenderingDataAccordingToActions(meshid,acts);
	else {
		MLRenderingSideToolbar* sidetool = qobject_cast<MLRenderingSideToolbar*>(sender());
		/*a MLRenderingSideToolBar is not mutual exclusive (except if the control modifier is pressed). in this case i have not to consider  the call to this function and instead rely on the signal calling the updateRenderingDataAccordingToAction( int meshid,MLRenderingAction* act) function */
		if ((sidetool == NULL) || ((sidetool != NULL) && (QApplication::queryKeyboardModifiers() & Qt::ControlModifier)))
			mw->updateRenderingDataAccordingToActionsToAllVisibleLayers(acts);
	}
}

void LayerDialog::updateRenderingDataAccordingToAction( int meshid,MLRenderingAction* act)
{
	if (mw == NULL)
		return;
	if (!_applytovis->isChecked()) {
		MLRenderingSideToolbar* sidetool = qobject_cast<MLRenderingSideToolbar*>(sender());
		/*in the normal case a MLRenderingSideToolBar should be managed by the updateRenderingDataAccordingToActions(int meshid,const QList<MLRenderingAction*>& acts) function*/
		if (sidetool == NULL)
			mw->updateRenderingDataAccordingToAction(meshid, act);
	}
	else
		mw->updateRenderingDataAccordingToActionToAllVisibleLayers(act);
}

MeshTreeWidgetItem::MeshTreeWidgetItem(MeshModel* meshmodel,QTreeWidget* tree,MLRenderingToolbar* rendertoolbar):
	QTreeWidgetItem(tree),_rendertoolbar(rendertoolbar)
{
	if (meshmodel != NULL) {
		updateVisibilityIcon(meshmodel->isVisible());
		setText(1, QString::number(meshmodel->id()));

		QFileInfo inf = meshmodel->label();
		QString meshName = inf.completeBaseName();
		if (meshmodel->meshModified())
			meshName += " *";
		if (_rendertoolbar != NULL)
			tree->setItemWidget(this,3,_rendertoolbar);

		setText(2, meshName);
		setToolTip(2, inf.absoluteFilePath());
		_meshid = meshmodel->id();
	}
}

MeshTreeWidgetItem::~MeshTreeWidgetItem()
{
}

void MeshTreeWidgetItem::updateVisibilityIcon( bool isvisible )
{
	if(isvisible)
		setIcon(0,QIcon(":/images/layer_eye_open.png"));
	else
		setIcon(0,QIcon(":/images/layer_eye_close.png"));
}

RasterTreeWidgetItem::RasterTreeWidgetItem(RasterModel *rasterModel)
{
	if (rasterModel != NULL)
	{
		updateVisibilityIcon(rasterModel->isVisible());
		setText(1, QString::number(rasterModel->id()));

		QFileInfo fi(rasterModel->label());
		QString rasterName = fi.fileName();
		setText(2, rasterName);

		_rasterid =rasterModel->id();
	}
}

void RasterTreeWidgetItem::updateVisibilityIcon( bool isvisible )
{
	if(isvisible)
		setIcon(0,QIcon(":/images/ok.png"));
	else
		setIcon(0,QIcon(":/images/stop.png"));
}


DecoratorParamsTreeWidget::DecoratorParamsTreeWidget(QAction* act,MainWindow *mw,QWidget* parent) :
	QFrame(parent),mainWin(mw),frame(NULL),savebut(NULL),resetbut(NULL),loadbut(NULL),dialoglayout(NULL)
{
	DecoratePlugin* decPlug =  qobject_cast<DecoratePlugin *>(act->parent());
	if (!decPlug) {
		mw->GLA()->Log(GLLogStream::SYSTEM, "MeshLab System Error: A Decorator Plugin has been expected.");
	}
	else {
		decPlug->initGlobalParameterList(act,tmpSet);
		if (tmpSet.size() != 0) {
			const RichParameterList& currSet = mw->currentGlobalPars();
			RichParameterList defSet = tmpSet;

			/********************************************************************************************************************/
			//WARNING! The hardwired original value is maintained in the defValue contained inside the tmpSet's parameters
			//the register system saved value instead is in the defValues of the params inside the current globalParameters set
			/********************************************************************************************************************/

			for(RichParameter& p : tmpSet) {
				const RichParameter& par = currSet.getParameterByName(p.name());
				tmpSet.setValue(p.name(),par.value());
			}

			dialoglayout = new QGridLayout();
			frame = new RichParameterListFrame(tmpSet, defSet, parent, mw->GLA());
			savebut = new QPushButton("Save",parent);
			resetbut = new QPushButton("Reset",parent);
			loadbut = new QPushButton("Load",parent);
			//int countel = frame->stdfieldwidgets.size();
			dialoglayout->addWidget(savebut,1,0);
			dialoglayout->addWidget(resetbut,1,1);
			dialoglayout->addWidget(loadbut,1,2);
			dialoglayout->addWidget(frame,0,0,1,3);
			this->setLayout(dialoglayout);
			this->layout()->setContentsMargins(0,0,0,0);
			frame->layout()->setContentsMargins(0,0,0,0);
			QFont font = frame->font();
			font.setPointSizeF(3 * font.pointSizeF() / 4.0f);
			frame->setFont(font);
			savebut->setFont(font);
			resetbut->setFont(font);
			loadbut->setFont(font);

			savebut ->setMaximumSize( savebut ->maximumSize().width(), savebut ->minimumSizeHint().height()/osDependentButtonHeightScaleFactor());
			resetbut->setMaximumSize( resetbut->maximumSize().width(), resetbut->minimumSizeHint().height()/osDependentButtonHeightScaleFactor());
			loadbut ->setMaximumSize( loadbut ->maximumSize().width(), loadbut ->minimumSizeHint().height()/osDependentButtonHeightScaleFactor());

			connect(frame,SIGNAL(parameterChanged()),this,SLOT(apply()));
			connect(resetbut,SIGNAL(clicked()),this,SLOT(reset()));
			connect(savebut,SIGNAL(clicked()),this,SLOT(save()));
			connect(loadbut,SIGNAL(clicked()),this,SLOT(load()));
		}
	}
}

DecoratorParamsTreeWidget::~DecoratorParamsTreeWidget()
{
	delete savebut;
	delete resetbut;
	delete loadbut;
	delete frame;
	delete dialoglayout;
}

void DecoratorParamsTreeWidget::save()
{
	apply();
	RichParameterList& current = mainWin->currentGlobalPars();
	for(auto& p : *frame) {
		RichParameter& rp = current.getParameterByName(p.first);
		QDomDocument doc("MeshLabSettings");
		doc.appendChild(rp.fillToXMLDocument(doc));
		QString docstring =  doc.toString();
		QSettings setting;
		setting.setValue(rp.name(),QVariant(docstring));
		tmpSet.setValue(rp.name(), rp.value());
	}
}

void DecoratorParamsTreeWidget::reset()
{
	for(auto& p : *frame)
		p.second->resetValue();
	apply();
}

void DecoratorParamsTreeWidget::apply()
{
	RichParameterList& current = mainWin->currentGlobalPars();
	for(auto& p : *frame) {
		current.setValue(
					p.first,
					p.second->widgetValue());
	}
	mainWin->updateCustomSettings();
	if (mainWin->GLA())
		mainWin->GLA()->updateAllDecorators();
}

void DecoratorParamsTreeWidget::load()
{
	for(auto& p : *frame) {
		QString pn = p.first;
		p.second->setWidgetValue(tmpSet.getParameterByName(pn).value());
	}
	apply();
}

float DecoratorParamsTreeWidget::osDependentButtonHeightScaleFactor()
{
#if defined(Q_OS_WIN)
	return 1.5f;
#elif defined(Q_OS_MAC)
	return 2.0f;
#else
	return 1.5f;
#endif
}

DecoratorParamItem::DecoratorParamItem( QAction* action):
	QTreeWidgetItem(), act(action)
{
}
