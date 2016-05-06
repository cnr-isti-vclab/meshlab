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

#include "mainwindow.h"
#include "ui_layerDialog.h"
#include "layerDialog.h"
#include "rendermodeactions.h"

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

    // The following connection is used to associate the click with the change of the current mesh.
    connect(ui->meshTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem * , int  )) , this,  SLOT(meshItemClicked(QTreeWidgetItem * , int ) ) );

    connect(ui->meshTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));
    connect(ui->meshTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));

    // The following connection is used to associate the click with the switch between raster and mesh view.
    connect(ui->rasterTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem * , int  )) , this,  SLOT(rasterItemClicked(QTreeWidgetItem * , int ) ) );

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->meshTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->rasterTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->decParsTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->meshTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
    connect(ui->rasterTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
    connect(ui->decParsTree, SIGNAL(customContextMenuRequested(const QPoint&)),this, SLOT(showContextMenu(const QPoint&)));
}

void LayerDialog::keyPressEvent ( QKeyEvent * event )
{   MeshDocument *md = mw->meshDoc();
    if(event->key() == Qt::Key_Space )
    {
      RasterModel *rm= md->nextRaster(md->rm());
        if(rm!=0) md->setCurrentRaster(rm->id());
    }
}

void LayerDialog::meshItemClicked (QTreeWidgetItem * item , int col)
{
    MeshTreeWidgetItem *mItem = dynamic_cast<MeshTreeWidgetItem *>(item);
    if(mItem)
    {
        int clickedId= mItem->_meshid;
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

                if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
                {
                    foreach(MeshModel *mp, md->meshList)
                    {
                        mw->GLA()->meshSetVisibility(mp, !mp->visible);
                    }
                    mw->GLA()->meshSetVisibility(md->getMesh(clickedId), !md->getMesh(clickedId)->visible);
                }

                mw->meshDoc()->setCurrentMesh(clickedId);
            } break;
        case 1 :

        case 2 :

        case 3 :
            //the user has chosen to switch the layer
            mw->meshDoc()->setCurrentMesh(clickedId);
            this->updateDecoratorParsView();
            break;
        }
        //make sure the right row is colored or that they right eye is drawn (open or closed)
        updateMeshItemSelectionStatus();
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
                //the user has clicked on the "V" or "X"
                MeshDocument  *md= mw->meshDoc();

                // NICE TRICK.
                // If the user has pressed ctrl when clicking on the icon, only that layer will remain visible
                //
                if(QApplication::keyboardModifiers() == Qt::ControlModifier)
                {
                    foreach(RasterModel *rp, md->rasterList)
                    {
                        rp->visible = false;
                    }
                }

                if(rItem->r->visible){
                    rItem->r->visible = false;
                }
                else{
                    rItem->r->visible = true;
                }

                // EVEN NICER TRICK.
                // If the user has pressed alt when clicking on the icon, all layers will get visible
                // Very useful after you turned all layer invis using the previous option and want to avoid
                // clicking on all of them...
                if(QApplication::keyboardModifiers() == Qt::AltModifier)
                {
                    foreach(RasterModel *rp, md->rasterList)
                    {
                        rp->visible = true;
                    }
                }

                if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
                {
                    foreach(RasterModel *rp, md->rasterList)
                    {
                        rp->visible = !rp->visible;
                    }
                    rItem->r->visible = !rItem->r->visible;
                }

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


        //updateTable();
        
        
        mw->GLA()->update();
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
    else if (sigsender == ui->decParsTree)
    {
        DecoratorParamItem*rItem = dynamic_cast<DecoratorParamItem  *>(ui->decParsTree->itemAt(pos.x(),pos.y()));
        if (rItem != NULL)
            emit removeDecoratorRequested(rItem->act);
    }
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
	this->setWindowTitle(md->docLabel());
	
	/*for(int ii = 0;ii < tobedel.size();++ii)
		delete tobedel[ii];*/
	ui->meshTreeWidget->clear();

	ui->meshTreeWidget->setColumnCount(4);
	ui->meshTreeWidget->setColumnWidth(0,40);
	ui->meshTreeWidget->setColumnWidth(1,40);
	//ui->meshTreeWidget->setColumnWidth(2,40);
	ui->meshTreeWidget->header()->hide();
	int maxwidth = 0;
	MeshTreeWidgetItem *selitem = NULL;
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
        MLSceneGLSharedDataContext::PerMeshRenderingDataMap::const_iterator rdit = dtf.find(mmd->id());
        if (rdit != dtf.end())
        {
		    MLRenderToolbar* rendertb = new MLRenderToolbar(mmd->id(),rdit->_mask,this);
            connect(rendertb,SIGNAL(primitiveModalityUpdateRequested(unsigned int,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK)),mw,SLOT(primitiveModalityUpdateRequested(unsigned int,vcg::GLMeshAttributesInfo::PRIMITIVE_MODALITY_MASK)));
		    MLRenderParametersFrame* renderfm = new MLRenderParametersFrame(this);
		    MeshTreeWidgetItem* item = new MeshTreeWidgetItem(mmd,ui->meshTreeWidget,rendertb,renderfm);
		    item->setExpanded(expandedMap.value(qMakePair(mmd->id(),-1)));
		    ui->meshTreeWidget->addTopLevelItem(item);
		    //Adding default annotations
		    addDefaultNotes(item, mmd);
        }
        else
             throw MeshLabException("Something bad happened! Mesh id has not been found in the rendermapmode map.");
	}

	int wid = 0;
	for(int i=0; i< ui->meshTreeWidget->columnCount(); i++)
	{
		ui->meshTreeWidget->resizeColumnToContents(i);
		wid += ui->meshTreeWidget->columnWidth(i);
	}
	ui->meshTreeWidget->setMinimumWidth(wid);
	updateMeshItemSelectionStatus();
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
    fileItem->setText(2, QString("File"));
    if(!meshModel->fullName().isEmpty())
        fileItem->setText(3, meshModel->shortName());
    parent->addChild(fileItem);
    updateColumnNumber(fileItem);

    QTreeWidgetItem *vertItem = new QTreeWidgetItem();
    vertItem->setText(2, QString("Vertices"));
    vertItem->setText(3, QString::number(meshModel->cm.vn));
    parent->addChild(vertItem);
    updateColumnNumber(vertItem);

    if(meshModel->cm.en>0){
        QTreeWidgetItem *edgeItem = new QTreeWidgetItem();
        edgeItem->setText(2, QString("Edges"));
        edgeItem->setText(3, QString::number(meshModel->cm.en));
        parent->addChild(edgeItem);
        updateColumnNumber(edgeItem);
    }

    QTreeWidgetItem *faceItem = new QTreeWidgetItem();
    faceItem->setText(2, QString("Faces"));
    faceItem->setText(3, QString::number(meshModel->cm.fn));
    parent->addChild(faceItem);
    updateColumnNumber(faceItem);

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
        MeshDecorateInterface* decPlug =  qobject_cast<MeshDecorateInterface *>(decList[ii]->parent());
        if (!decPlug)
        {
            mw->GLA()->Logf(GLLogStream::SYSTEM,"MeshLab System Error: A Decorator Plugin has been expected.");
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
    //ui->decParsTree->expandAll();
}

void LayerDialog::renderingModalityChanged(const MLRenderingData& data )
{
    for(int ii = 0;ii < ui->meshTreeWidget->topLevelItemCount();++ii)
    {
        //QTreeWidgetItem is NOT derived from QObject. The usual qobject_cast is not effective
        MeshTreeWidgetItem* itm = dynamic_cast<MeshTreeWidgetItem*>(ui->meshTreeWidget->topLevelItem(ii));
        if (itm != NULL)
            itm->_rendertoolbar->setPrimitiveModality(data._mask);
    }
}

void LayerDialog::updateMeshItemSelectionStatus()
{
    MeshDocument* md = mw->meshDoc();
    if (md == NULL)
        return;
    for(int ii = 0; ii < ui->meshTreeWidget->topLevelItemCount();++ii)
    {
        MeshTreeWidgetItem* item = dynamic_cast<MeshTreeWidgetItem*>(ui->meshTreeWidget->topLevelItem(ii));
        MeshModel* mm = md->mm();
        if ((item != NULL) && (mm != NULL))
        {
            if(item->_meshid == mm->id()) 
            {
                item->setBackground(1,QBrush(Qt::yellow));
                item->setForeground(1,QBrush(Qt::blue));
                item->setBackground(2,QBrush(Qt::yellow));
                item->setForeground(2,QBrush(Qt::blue));
                item->setBackground(3,QBrush(Qt::yellow));
                item->setForeground(3,QBrush(Qt::blue));
                ui->meshTreeWidget->setCurrentItem(item);
            }
            else
            {
                item->setBackground(1,QBrush());
                item->setForeground(1,QBrush());
                item->setBackground(2,QBrush());
                item->setForeground(2,QBrush());
                item->setBackground(3,QBrush());
                item->setForeground(3,QBrush());
            }
        }
    }
}

void LayerDialog::reset()
{
    ui->meshTreeWidget->clear();
    ui->decParsTree->clear();
    ui->rasterTreeWidget->clear();
}

MeshTreeWidgetItem::MeshTreeWidgetItem(MeshModel* meshmodel,QTreeWidget* tree,MLRenderToolbar* rendertoolbar,MLRenderParametersFrame* frame)
    :QTreeWidgetItem(tree),_rendertoolbar(rendertoolbar),_frame(frame)
{
    if (meshmodel != NULL)
    {
        if(meshmodel->visible)
            setIcon(0,QIcon(":/images/layer_eye_open.png"));
        else
            setIcon(0,QIcon(":/images/layer_eye_close.png"));
        setText(1, QString::number(meshmodel->id()));

        QString meshName = meshmodel->label();
        if (meshmodel->meshModified())
            meshName += " *";
        if (_rendertoolbar != NULL)
            tree->setItemWidget(this,3,_rendertoolbar);
        setText(2, meshName);
        _meshid = meshmodel->id();
    }
}

MeshTreeWidgetItem::MeshTreeWidgetItem(QTreeWidget* tree,MLRenderToolbar* rendertoolbar,MLRenderParametersFrame* frame)
    :QTreeWidgetItem(tree),_rendertoolbar(rendertoolbar),_frame(frame)
{
    setIcon(0,QIcon(":/images/layer_eye_open.png"));

    if (rendertoolbar != NULL)
        tree->setItemWidget(this,3,_rendertoolbar);
    _meshid = -1;
}

MeshTreeWidgetItem::~MeshTreeWidgetItem()
{
    /*if (addwid->parent() == NULL)
        delete addwid;*/
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
        mw->GLA()->Logf(GLLogStream::SYSTEM,"MeshLab System Error: A Decorator Plugin has been expected.");
    else
    {
        decPlug->initGlobalParameterSet(act,tmpSet);
        if (tmpSet.paramList.size() != 0)
        {
            const RichParameterSet& currSet = mw->currentGlobalPars();

            /********************************************************************************************************************/
            //WARNING! The hardwired original value is maintained in the defValue contained inside the tmpSet's parameters
            //the register system saved value instead is in the defValues of the params inside the current globalParameters set
            /********************************************************************************************************************/

            for(int jj = 0;jj < tmpSet.paramList.size();++jj)
            {
                RichParameter* par = currSet.findParameter(tmpSet.paramList[jj]->name);
                tmpSet.setValue(tmpSet.paramList[jj]->name,*(par->val));
            }

            dialoglayout = new QGridLayout();
            frame = new StdParFrame(parent,mw->GLA());
            frame->loadFrameContent(tmpSet,mw->meshDoc());
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
    //delete applybut;
    delete loadbut;
    delete frame;
    delete dialoglayout;
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
        RichParameterSet& currSet = mainWin->currentGlobalPars();
        RichParameter* par = currSet.findParameter(tmpSet.paramList[ii]->name);
        par->pd->defVal->set(*(tmpSet.paramList[ii]->val));
    }
}

void DecoratorParamsTreeWidget::reset()
{
    for(int ii = 0;ii < frame->stdfieldwidgets.size();++ii)
        frame->stdfieldwidgets[ii]->resetValue();
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
        mainWin->GLA()->updateAllPerMeshDecorators();
}

void DecoratorParamsTreeWidget::load()
{
    for(int ii = 0;ii < tmpSet.paramList.size();++ii)
    {
        const RichParameter& defPar = *(mainWin->currentGlobalPars().findParameter(tmpSet.paramList[ii]->name));
        tmpSet.paramList[ii]->val->set(*(defPar.pd->defVal));
        frame->stdfieldwidgets.at(ii)->setWidgetValue(*(tmpSet.paramList[ii]->val));
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

DecoratorParamItem::DecoratorParamItem( QAction* action)
    :QTreeWidgetItem(),act(action)
{

}
