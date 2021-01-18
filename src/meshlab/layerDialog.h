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


#ifndef LAYER_DIALOG_H
#define LAYER_DIALOG_H

#include <QTreeWidgetItem>
#include <QDockWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QTimer>
#include <common/parameters/rich_parameter_list.h>
#include <common/ml_shared_data_context.h>
#include "ml_render_gui.h"



class MainWindow;
class QTreeWidget;
class GLLogStream;
class MeshModel;
class RasterModel;
class MeshDocument;
class DecoratePluginInterface;
class RichParameterListFrame;
class QGridLayout;
class QToolBar;

#include <QDialog>

namespace Ui
{
    class layerDialog;
}


class MeshTreeWidgetItem : public QTreeWidgetItem
{ 
public:
    MeshTreeWidgetItem(MeshModel* meshmodel,QTreeWidget* tree,MLRenderingToolbar* rendertoolbar);
    ~MeshTreeWidgetItem();
    void updateVisibilityIcon(bool isvisible);

    MLRenderingToolbar* _rendertoolbar;
    int _meshid;
};

class RasterTreeWidgetItem : public QTreeWidgetItem
{
public:
    RasterTreeWidgetItem(RasterModel *);
    void updateVisibilityIcon( bool isvisible );
    //RasterModel *r;
    int _rasterid;
};

class DecoratorParamItem : public QTreeWidgetItem
{
public:
    DecoratorParamItem(QAction* );

    QAction* act;
};

class DecoratorParamsTreeWidget : public QFrame
{
    Q_OBJECT
public:
    DecoratorParamsTreeWidget(QAction* act,MainWindow *mw,QWidget* parent);
    ~DecoratorParamsTreeWidget();

public slots:
    void save();
    void reset();
    void apply();
    void load();

private:
    float osDependentButtonHeightScaleFactor();
    MainWindow* mainWin;
    RichParameterListFrame* frame;
    RichParameterList tmpSet;
    QPushButton* savebut;
    QPushButton* resetbut;
    QPushButton* loadbut;
    QGridLayout* dialoglayout;
};

class LayerDialog : public QDockWidget
{
    Q_OBJECT
public:
    LayerDialog(QWidget *parent = 0);
    ~LayerDialog();
    void updateLog(const GLLogStream& Log);
    void updateDecoratorParsView();
    void updateRenderingParametersTab(int meshid,const MLRenderingData& dt);
    void reset();
	void setCurrentTab(const MLRenderingData& dt);
    MLRenderingParametersTab* createRenderingParametersTab();
protected:
    void keyPressEvent ( QKeyEvent * event );
	void keyReleaseEvent(QKeyEvent * event);
	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);

public slots:
	void updateTable(const MLSceneGLSharedDataContext::PerMeshRenderingDataMap& dtf);
    void rasterItemClicked(QTreeWidgetItem * , int );
    void meshItemClicked(QTreeWidgetItem * , int );
    void showEvent ( QShowEvent * event );
    void showContextMenu(const QPoint& pos);
    void adaptLayout(QTreeWidgetItem * item);
    void updateRenderingDataAccordingToActions(int meshid,const QList<MLRenderingAction*>&);
    void updateRenderingDataAccordingToAction(int meshid,MLRenderingAction*);
    void actionActivated(MLRenderingAction* ract);
	void clickW1();
	void clickW2();
	void clickW3();
	void clickW4();
	void clickAnimSlower();
	void clickAnimStepBackward();
	void clickAnimPlay();
	void clickAnimStepForward();
	void clickAnimFaster();
	void updateAnim();
	void clickV1();
	void clickV2();
	void clickV3();
	void clickV4();

private slots:
	/*WARNING!!! ADDED just to avoid usual mac strange behavior. Please, avoid to use it if it's not absolutely necessary*/
	void updateTable();
private:
    Ui::layerDialog* ui;
    MainWindow *mw;

    QMenu *rasterMenu;
    QAction* addNewRasterAct;
    QAction* removeCurrentRasterAct;
    //It stores if the treeWidgetItems are expanded or not
    QMap< QPair<int ,int> ,  bool> expandedMap;
    
    //QList<QToolBar*> tobedel;
    void addDefaultNotes(QTreeWidgetItem * parent, MeshModel *meshModel);
    void updateColumnNumber(const QTreeWidgetItem * item);
    //QVector<QTreeWidgetItem*> tobedeleted;
    void updateExpandedMap(int meshId, int tagId, bool expanded);
    void updatePerMeshItemSelectionStatus();
    void updatePerRasterItemSelectionStatus();
    void updatePerMeshItemVisibility();
    void updatePerRasterItemVisibility();

    void updateProjectName(const QString& name);

	// state buttons
	bool isRecording;
	QString viewState[4];
	QMap<int, bool> visibilityState[4];

	int animIndex;
	std::vector<int> animMeshIDs;
	int animMsecDelay;
	QTimer* animTimer;

	bool startAnim();
	int stepAnim(int offset);
	void pauseAnim();
	void resetAnim();

    QTreeWidgetItem* _docitem;
    int _previd;
	QGroupBox* _renderingtabcontainer;
	QCheckBox* _applytovis;
public:
	MLRenderingParametersTab* _tabw;
signals:
    void removeDecoratorRequested(QAction* );
    void toBeShow();
};


#endif
