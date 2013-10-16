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
#ifndef LAYER_DIALOG_H
#define LAYER_DIALOG_H
#include <QtGui>
#include <QTreeWidgetItem>
#include <QDockWidget>
#include "../common/filterparameter.h"

class MainWindow;
class QTreeWidget;
class GLLogStream;
class MeshModel;
class RasterModel;
class MeshDocument;
class MeshDecorateInterface;
class StdParFrame;
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
	MeshTreeWidgetItem(MeshModel *,QTreeWidget* tree,QWidget* additional);
	~MeshTreeWidgetItem();

	MeshModel* m;
};

class RasterTreeWidgetItem : public QTreeWidgetItem
{
	public:
	RasterTreeWidgetItem(RasterModel *);

	RasterModel *r;
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
	StdParFrame* frame;
	RichParameterSet tmpSet;
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
	void updateLog(GLLogStream &Log);
	 void updateDecoratorParsView();

public slots:
  void keyPressEvent ( QKeyEvent * event );
  void updateTable();
  void rasterItemClicked(QTreeWidgetItem * , int );
  void meshItemClicked(QTreeWidgetItem * , int );
  void showEvent ( QShowEvent * event );
  void showContextMenu(const QPoint& pos);
  void adaptLayout(QTreeWidgetItem * item);

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

	//it maintains mapping between the main toolbar action and the per mesh corresponding action in the side toolbar. 
	//used when an action in the main toolbar is selected. A signal is emitted informing the current meshtreewidgetitem that it has to update its own side toolbar.
	QMap<QAction*, QMap<MeshTreeWidgetItem*,QAction*> > maintb_sidetb_map;

signals:
	void removeDecoratorRequested(QAction* );
};


#endif
