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
#ifndef __MAULTIVIEWER_CONTAINER_H__
#define __MAULTIVIEWER_CONTAINER_H__

#include <QWidget>
#include <QVector>
#include <QSplitter>

#include "../common/meshmodel.h"


// Class list

class RichParameterSet;
class Viewer;

class MultiViewer_Container : public QSplitter
{
	Q_OBJECT

	/*typedef vcg::Shot<double> Shot;*/

public:
	MultiViewer_Container(QWidget *parent);
    ~MultiViewer_Container();
	
	void addView(Viewer* viewer,  Qt::Orientation);
	void removeView(int);
	
	Viewer* currentView();
    int getNextViewerId();
	int viewerCounter();

	void updateAll();
	void update(int id);

	MeshDocument meshDoc;
	int currentId;

signals:
    void updateMainWindowMenus(); //updates the menus of the meshlab MainWindow

public slots:

	// Called when we change viewer, set the current viewer
    void updateCurrent(int current);
	
private:
	QVector<Viewer *> viewerList; /// widgets for the OpenGL contexts and images
};

#endif // __MAULTIVIEWER_CONTAINER_H__