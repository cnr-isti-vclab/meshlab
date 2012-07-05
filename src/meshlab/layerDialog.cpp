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
#include <QtGui>

#include "ui_layerDialog.h"
#include "layerDialog.h"
#include "mainwindow.h"

using namespace std;

LayerDialog::LayerDialog(QWidget *parent )    : QDockWidget(parent)
{
	ui = new Ui::layerDialog();
  setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow );
  this-> QWidget::setAttribute( Qt::WA_MacAlwaysShowToolWindow);
	setVisible(false);
	LayerDialog::ui->setupUi(this);
	mw=qobject_cast<MainWindow *>(parent);
	this->layout();

	tagMenu = new QMenu(this);
	removeTagAct = new QAction(tr("&Remove Tag"),this);
	tagMenu->addAction(removeTagAct);
	connect(removeTagAct, SIGNAL(triggered()), this, SLOT(removeTag()));

	updateTagAct = new QAction(tr("&Update Tag"),this);
	tagMenu->addAction(updateTagAct);
	//TODO connect(updateTagAct, SIGNAL(triggered()), this, SLOT(?????????????));

	// The following connection is used to associate the click with the change of the current mesh. 
  connect(ui->meshTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem * , int  )) , this,  SLOT(meshItemClicked(QTreeWidgetItem * , int ) ) );
	
	connect(ui->meshTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));
	connect(ui->meshTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));

	// The following connection is used to associate the click with the switch between raster and mesh view. 
  connect(ui->rasterTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem * , int  )) , this,  SLOT(rasterItemClicked(QTreeWidgetItem * , int ) ) );

  /*connect(ui->addButton, SIGNAL(clicked()), mw, SLOT(importMesh()) );
	connect(ui->deleteButton, SIGNAL(clicked()), mw, SLOT(delCurrentMesh()) );*/

	this->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->meshTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->rasterTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui->meshTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
	connect(ui->rasterTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
	//connect(ui->menuButton, SIGNAL(clicked()), this, SLOT(showLayerMenu()));
	//connect(mw,SIGNAL(selectedDecoration(GLArea*,QAction*)),this,SLOT(addParamsToDecorationDialog(GLArea*,QAction*)));
	//connect(mw,SIGNAL(unSelectedDecoration(GLArea*,QAction*)),this,SLOT(removeParamsFromDecorationDialog(GLArea*,QAction*)));
}

void LayerDialog::keyPressEvent ( QKeyEvent * event )
{
  if(event->key() == Qt::Key_Space )
    mw->meshDoc()->advanceCurrentRaster(1);
}

void LayerDialog::meshItemClicked (QTreeWidgetItem * item , int col)
{
  MeshTreeWidgetItem *mItem = dynamic_cast<MeshTreeWidgetItem *>(item);
  if(mItem)
  {
    int clickedId= mItem->m->id();
    switch(col)
    {
    case 0 :
      {
        //the user has clicked on one of the eyes
        MeshDocument  *md= mw->meshDoc();

        // NICE TRICK.
        // If the user has pressed ctrl when clicking on the eye icon, only that layer will remain visible
        // Very useful for comparing meshes
        if(QApplication::keyboardModifiers() == Qt::ControlModifier)
        {
          foreach(MeshModel *mp, md->meshList)
            {
              mw->GLA()->meshSetVisibility(mp, false);
            }
        }

        //Toggle visibility of current mesh
        mw->GLA()->meshSetVisibility(md->getMesh(clickedId), !md->getMesh(clickedId)->visible);

        // EVEN NICER TRICK.
        // If the user has pressed alt when clicking on the eye icon, all layers will get visible
        // Very useful after you turned all layer invis using the previous option and want to avoid
        // clicking on all of them...
        if(QApplication::keyboardModifiers() == Qt::AltModifier)
        {
          foreach(MeshModel *mp, md->meshList)
            {
              mw->GLA()->meshSetVisibility(mp, true);
            }
        }
      } break;
    case 1 :

    case 2 :

    case 3 :
      //the user has chosen to switch the layer
      mw->meshDoc()->setCurrentMesh(clickedId);
      break;
    }
    //make sure the right row is colored or that they right eye is drawn (open or closed)
    updateTable();
    mw->GLA()->update();
  }
}

void LayerDialog::rasterItemClicked (QTreeWidgetItem * item , int col)
{
	RasterTreeWidgetItem *rItem = dynamic_cast<RasterTreeWidgetItem *>(item);
  if(rItem)
	{
		int clickedId= rItem->r->id();

		switch(col)
		{
		case 0 :
			{
				if(rItem->r->visible){
					rItem->r->visible = false;
				}
				else{
					rItem->r->visible = true;
				}
				//Update current GLArea visibility 
				mw->GLA()->updateRasterSetVisibilities( );
			}
    case 1 :
    case 2 :
    case 3 :
          if(mw->meshDoc()->rm()->id() != clickedId || mw->GLA()->isRaster() )
          {
            mw->meshDoc()->setCurrentRaster(clickedId);
            if(mw->GLA()->isRaster())
              mw->GLA()->loadRaster(clickedId);
          }
      break;

		}
		updateTable();
		mw->GLA()->update();
	}
}

void LayerDialog::showEvent ( QShowEvent * /* event*/ )
{
	updateTable();
}

void LayerDialog::showContextMenu(const QPoint& pos)
{
	QObject* sigsender = sender();
	if (sigsender == ui->meshTreeWidget)
	{
		MeshTreeWidgetItem   *mItem = dynamic_cast<MeshTreeWidgetItem   *>(ui->meshTreeWidget->itemAt(pos.x(),pos.y()));
		QTreeWidgetItem     *qtItem = dynamic_cast<QTreeWidgetItem *>     (ui->meshTreeWidget->itemAt(pos.x(),pos.y()));

		if(mItem)
		{ 
			if (mItem->m) mw->meshDoc()->setCurrentMesh(mItem->m->id());

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
		else 
		{
			if(qtItem)
			{
				bool ok;
				int idToRemove = qtItem->text(2).toInt(&ok);
				if(ok)
				{
					removeTagAct->setData(idToRemove);
					tagMenu->popup(mapToGlobal(pos));
				}
			}
		}
	}
	// switch layer
	else
	{
		if (sigsender == ui->rasterTreeWidget)
		{
			RasterTreeWidgetItem *rItem = dynamic_cast<RasterTreeWidgetItem *>(ui->rasterTreeWidget->itemAt(pos.x(),pos.y()));
	
			if (rItem)
			{ 
				if (rItem->r)
				{
					mw->meshDoc()->setCurrentRaster(rItem->r->id());

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
	}
}

void LayerDialog::removeTag()
{
  MeshDocument *md=mw->meshDoc();
	md->removeTag(removeTagAct->data().toInt());
	updateTable();
}

void LayerDialog::updateLog(GLLogStream &log)
{
	QList< pair<int,QString> > &logStringList=log.S;
	ui->logPlainTextEdit->clear();
	//ui->logPlainTextEdit->setFont(QFont("Courier",10));

	pair<int,QString> logElem;
	QString preWarn    = "<font face=\"courier\" size=3 color=\"red\"> Warning: " ;
	QString preSystem  = "<font face=\"courier\" size=2 color=\"grey\">" ;
	QString preFilter  = "<font face=\"courier\" size=2 color=\"black\">" ;

	QString post   = "</font>";
    QString logText;
	foreach(logElem, logStringList){
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
	//TODO:Check if the current viewer is a GLArea
	if(!isVisible()) return;
	if(isVisible() && !mw->GLA())
	{
		setVisible(false);
		//The layer dialog cannot be opened unless a new document is opened
		return;
	}
	MeshDocument *md=mw->meshDoc();
	this->setWindowTitle(md->docLabel());

	ui->meshTreeWidget->clear();
	ui->meshTreeWidget->setColumnCount(4);
	ui->meshTreeWidget->setColumnWidth(0,40);
	ui->meshTreeWidget->setColumnWidth(1,40);
	//ui->meshTreeWidget->setColumnWidth(2,40);
	ui->meshTreeWidget->header()->hide();
	foreach(MeshModel* mmd, md->meshList)
	{
	  //Restore mesh visibility according to the current visibility map
	  //very good to keep viewer state consistent
	  if( mw->GLA()->meshVisibilityMap.contains(mmd->id()))
		mmd->visible = mw->GLA()->meshVisibilityMap.value(mmd->id());
	  else
	  {
		mw->GLA()->meshVisibilityMap[mmd->id()]=true;
		mmd->visible=true;
	  }

		MeshTreeWidgetItem *item = new MeshTreeWidgetItem(mmd);
		if(mmd== mw->GLA()->mm()) {
			item->setBackground(1,QBrush(Qt::yellow));
			item->setForeground(1,QBrush(Qt::blue));
			item->setBackground(2,QBrush(Qt::yellow));
			item->setForeground(2,QBrush(Qt::blue));
			item->setBackground(3,QBrush(Qt::yellow));
			item->setForeground(3,QBrush(Qt::blue));
		}
		ui->meshTreeWidget->addTopLevelItem(item);

		item->setExpanded(expandedMap.value(qMakePair(mmd->id(),-1)));

		//Adding default annotations
		addDefaultNotes(item, mmd);

		//Adding tags
		QList<TagBase *> tags = md->getMeshTags(mmd->id());
		foreach( TagBase *tag, tags)
			addTreeWidgetItem(item, tag, *md, mmd);
	}
		
  for(int i=0; i< ui->meshTreeWidget->columnCount(); i++)
		ui->meshTreeWidget->resizeColumnToContents(i);

	//RasterTreewWidget

	if (md->rasterList.size() > 0)
		ui->rasterTreeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	else
		ui->rasterTreeWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Ignored);
	ui->rasterTreeWidget->clear();
	ui->rasterTreeWidget->setColumnCount(4);
	ui->rasterTreeWidget->setColumnWidth(0,40);
	ui->rasterTreeWidget->setColumnWidth(1,20);
	//TODO The fourth column is fake... solo per ora, E' per evitare che l'ultimacolonna si allunghi indefinitivamente
	//mettere una lunghezza fissa e' inutile perche' non so quanto e' lungo il nome.
	ui->rasterTreeWidget->header()->hide();
	foreach(RasterModel* rmd, md->rasterList)
	{
		//Restore raster visibility according to the current visibility map
		//very good to keep viewer state consistent
		if( mw->GLA()->rasterVisibilityMap.contains(rmd->id()))	
			rmd->visible =mw->GLA()->rasterVisibilityMap.value(rmd->id());

		RasterTreeWidgetItem *item = new RasterTreeWidgetItem(rmd);
		if(rmd== mw->meshDoc()->rm()) {
			item->setBackground(1,QBrush(Qt::yellow));
			item->setForeground(1,QBrush(Qt::blue));
			item->setBackground(2,QBrush(Qt::yellow));
			item->setForeground(2,QBrush(Qt::blue));
			item->setBackground(3,QBrush(Qt::yellow));
			item->setForeground(3,QBrush(Qt::blue));
		}
		ui->rasterTreeWidget->addTopLevelItem(item);

		//TODO scommenta quando inserisci tutta la lista dei planes
		//item->setExpanded(expandedMap.value(qMakePair(mmd->id(),-1)));
	}

	for(int i=2; i< ui->rasterTreeWidget->columnCount(); i++)
		ui->rasterTreeWidget->resizeColumnToContents(i);
}

//Reconstruct the correct layout of the treewidget after updating the main table. It is necessary to keep the changing 
// (ex. the expansions of the treeWidgetItem) the user does.
void LayerDialog::adaptLayout(QTreeWidgetItem * item)
{
	item->setExpanded(item->isExpanded());
	for(int i=3; i< ui->meshTreeWidget->columnCount(); i++)
		ui->meshTreeWidget->resizeColumnToContents(i);

	//Update expandedMap
	MeshTreeWidgetItem *mItem = dynamic_cast<MeshTreeWidgetItem *>(item);
	if(mItem){
		int meshId = mItem->m->id();
		bool ok;
    int tagId = mItem->text(2).toInt(&ok);
    if(ok && tagId >=0 )
			//MeshTreeWidgetItems don't have a tag id, so we use -1
			updateExpandedMap(meshId, -1, item->isExpanded());
	}
	else { //Other TreeWidgetItems
		MeshTreeWidgetItem *parent = dynamic_cast<MeshTreeWidgetItem *>(item->parent());
		if(parent){
			int meshId = parent->m->id();
			bool ok;
			int tagId = item->text(2).toInt(&ok);
			if(ok)
				updateExpandedMap(meshId, tagId, item->isExpanded());
		}
	}
}

//Add default annotations for each mesh about faces and vertices number
void LayerDialog::addDefaultNotes(QTreeWidgetItem * parent, MeshModel *meshModel)
{
  QTreeWidgetItem *fileItem = new QTreeWidgetItem();
  fileItem->setText(2, QString("File"));
  if(!meshModel->fullName().isEmpty())
        fileItem->setText(3, meshModel->shortName());
  parent->addChild(fileItem);
  updateColumnNumber(fileItem);

  QTreeWidgetItem *faceItem = new QTreeWidgetItem();
  faceItem->setText(2, QString("Faces"));
  faceItem->setText(3, QString::number(meshModel->cm.fn));
  parent->addChild(faceItem);
  updateColumnNumber(faceItem);

  if(meshModel->cm.en>0){
    QTreeWidgetItem *edgeItem = new QTreeWidgetItem();
    edgeItem->setText(2, QString("Edges"));
    edgeItem->setText(3, QString::number(meshModel->cm.en));
    parent->addChild(edgeItem);
    updateColumnNumber(edgeItem);
  }
  QTreeWidgetItem *vertItem = new QTreeWidgetItem();
  vertItem->setText(2, QString("Vertices"));
  vertItem->setText(3, QString::number(meshModel->cm.vn));
  parent->addChild(vertItem);
  updateColumnNumber(vertItem);

  std::vector<std::string> AttribNameVector;
  vcg::tri::Allocator<CMeshO>::GetAllPerVertexAttribute< float >(meshModel->cm,AttribNameVector);
  for(int i = 0; i < (int) AttribNameVector.size(); i++)
  {
      QTreeWidgetItem *vertItem = new QTreeWidgetItem();
      vertItem->setText(2, QString("Vert Attr."));
      vertItem->setText(3, "float "+QString(AttribNameVector[i].c_str()));
      parent->addChild(vertItem);
      updateColumnNumber(vertItem);
  }
  AttribNameVector.clear();
  vcg::tri::Allocator<CMeshO>::GetAllPerVertexAttribute< vcg::Point3f >(meshModel->cm,AttribNameVector);
  for(int i = 0; i < (int) AttribNameVector.size(); i++)
  {
      QTreeWidgetItem *vertItem = new QTreeWidgetItem();
      vertItem->setText(2, QString("Vert Attr."));
      vertItem->setText(3, "Point3f "+QString(AttribNameVector[i].c_str()));
      parent->addChild(vertItem);
      updateColumnNumber(vertItem);
  }
  vcg::tri::Allocator<CMeshO>::GetAllPerFaceAttribute< float >(meshModel->cm,AttribNameVector);
  for(int i = 0; i < (int) AttribNameVector.size(); i++)
  {
      QTreeWidgetItem *vertItem = new QTreeWidgetItem();
      vertItem->setText(2, QString("Face Attr."));
      vertItem->setText(3, "float "+QString(AttribNameVector[i].c_str()));
      parent->addChild(vertItem);
      updateColumnNumber(vertItem);
  }
  AttribNameVector.clear();
  vcg::tri::Allocator<CMeshO>::GetAllPerFaceAttribute< vcg::Point3f >(meshModel->cm,AttribNameVector);
  for(int i = 0; i < (int) AttribNameVector.size(); i++)
  {
      QTreeWidgetItem *vertItem = new QTreeWidgetItem();
      vertItem->setText(2, QString("Face Attr."));
      vertItem->setText(3, "Point3f "+QString(AttribNameVector[i].c_str()));
      parent->addChild(vertItem);
      updateColumnNumber(vertItem);
  }

}

//Add a new item (not a MeshTreeWidgetItem but a tag item) to the treeWidget
void LayerDialog::addTreeWidgetItem(QTreeWidgetItem *parent, TagBase *tag, MeshDocument &md, MeshModel *mm)
{
	QMap<QString,MeshFilterInterface *>::const_iterator msi;
	for(msi =  mw->pluginManager().stringFilterMap.begin(); msi != mw->pluginManager().stringFilterMap.end();++msi)
	{
		MeshFilterInterface * iFilter= msi.value();
		if(msi.key() == tag->filterOwner)
		{
			QTreeWidgetItem *item = iFilter->tagDump(tag,md,mm);
			parent->addChild(item);
			updateColumnNumber(item);
			item->setExpanded(expandedMap.value(qMakePair(mm->id(),tag->id())));
		}
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


LayerDialog::~LayerDialog()
{
	delete ui;
}

void LayerDialog::updateDecoratorParsView() 
{
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
	if (mw->GLA()->iDecoratorsList.size() == 0)
	{
		ui->decParsTree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Ignored);
		return;
	}
	QList<QAction*>& decList =  mw->GLA()->iDecoratorsList;
	QList<QTreeWidgetItem*> treeItem;
	for(int ii = 0; ii < decList.size();++ii)
	{
		MeshDecorateInterface* decPlug =  qobject_cast<MeshDecorateInterface *>(decList[ii]->parent());
		if (!decPlug)
		{
			mw->GLA()->log->Logf(GLLogStream::SYSTEM,"MeshLab System Error: A Decorator Plugin has been expected.");
			return;
		}
		else
		{
			QTreeWidgetItem* item = new QTreeWidgetItem();
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
	//ui->decParsTree->expandAll();
}

MeshTreeWidgetItem::MeshTreeWidgetItem(MeshModel *meshModel)
{
  if(meshModel->visible) setIcon(0,QIcon(":/images/layer_eye_open.png"));
                    else setIcon(0,QIcon(":/images/layer_eye_close.png"));
  setText(1, QString::number(meshModel->id()));

  QString meshName = meshModel->label();
  if (meshModel->meshModified())
    meshName += " *";
  setText(2, meshName);

  this->m=meshModel;
}

RasterTreeWidgetItem::RasterTreeWidgetItem(RasterModel *rasterModel)
{
  if(rasterModel->visible) setIcon(0,QIcon(":/images/ok.png"));
                      else setIcon(0,QIcon(":/images/stop.png"));

  setText(1, QString::number(rasterModel->id()));

  QString rasterName = rasterModel->label();
  setText(2, rasterName);

  this->r=rasterModel;
}

DecoratorParamsTreeWidget::DecoratorParamsTreeWidget(QAction* act,MainWindow *mw,QWidget* parent)
:QFrame(parent),mainWin(mw),frame(NULL),savebut(NULL),resetbut(NULL),loadbut(NULL),dialoglayout(NULL)
{
	MeshDecorateInterface* decPlug =  qobject_cast<MeshDecorateInterface *>(act->parent());
	if (!decPlug)
		mw->GLA()->log->Logf(GLLogStream::SYSTEM,"MeshLab System Error: A Decorator Plugin has been expected.");
	else
	{
		decPlug->initGlobalParameterSet(act,tmpSet);
		if (tmpSet.paramList.size() != 0)
		{
			for(int jj = 0;jj < tmpSet.paramList.size();++jj)
			{
				RichParameterSet currSet = mw->currentGlobalPars();
				RichParameter* par = currSet.findParameter(tmpSet.paramList[jj]->name);
				tmpSet.setValue(tmpSet.paramList[jj]->name,*(par->val));
			}
			
			dialoglayout = new QGridLayout(parent);
			frame = new StdParFrame(parent,mw->GLA());
			frame->loadFrameContent(tmpSet,mw->meshDoc());
			savebut = new QPushButton("Save",parent);
			resetbut = new QPushButton("Reset",parent);
			loadbut = new QPushButton("Load",parent);
			int countel = frame->stdfieldwidgets.size();
			dialoglayout->addWidget(savebut,countel,0);
			dialoglayout->addWidget(resetbut,countel,1);
			dialoglayout->addWidget(loadbut,countel,2);
			dialoglayout->addWidget(frame,0,0,countel,3);
			this->setLayout(dialoglayout);
			int si = dialoglayout->rowCount();
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
	//delete applybut;
	delete loadbut;
	delete frame;
	delete dialoglayout;
	//delete dialoglayout;
}

void DecoratorParamsTreeWidget::save()
{
	apply();
	for(int ii = 0;ii < tmpSet.paramList.size();++ii)
	{
		QDomDocument doc("MeshLabSettings");
		RichParameterXMLVisitor v(doc);
		RichParameter* p = tmpSet.paramList[ii];
		p->accept(v);
		doc.appendChild(v.parElem);
		QString docstring =  doc.toString();
		qDebug("Writing into Settings param with name %s and content ****%s****",qPrintable(p->name),qPrintable(docstring));
		QSettings setting;
		setting.setValue(p->name,QVariant(docstring));
		p->pd->defVal->set(*p->val);
	}
}

void DecoratorParamsTreeWidget::reset()
{
	//qDebug("resetting the value of param %s to the hardwired default",qPrintable(curPar->name));
	for(int ii = 0;ii < tmpSet.paramList.size();++ii)
	{
		const RichParameter& defPar = *(mainWin->currentGlobalPars().findParameter(tmpSet.paramList[ii]->name));
		tmpSet.paramList[ii]->val->set(*(defPar.val));
		frame->stdfieldwidgets.at(ii)->setWidgetValue(*(tmpSet.paramList[ii]->val));
	}
	apply();
}

void DecoratorParamsTreeWidget::apply()
{
	RichParameterSet& current = mainWin->currentGlobalPars();
	for(int ii = 0;ii < frame->stdfieldwidgets.size();++ii)
	{
		frame->stdfieldwidgets[ii]->collectWidgetValue();
		RichParameter* r = frame->stdfieldwidgets[ii]->rp;
		current.setValue(r->name,*(r->val));
	}
	mainWin->updateCustomSettings();
  if(mainWin->GLA())
    mainWin->GLA()->updateDecoration();
}

void DecoratorParamsTreeWidget::load()
{
	for(int ii = 0;ii < frame->stdfieldwidgets.size();++ii)
		frame->stdfieldwidgets[ii]->resetValue();
}
