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

#include "../../../vcglib/wrap/ply/plylib.h"
#include "../../../vcglib/wrap/io_trimesh/import_ply.h"
#include "../../../vcglib/wrap/io_trimesh/export_ply.h"

#include <QWidget>
#include <QVector>
#include <QSplitter>

#include "../common/meshmodel.h"
#include "viewer.h"
#include "layerDialog.h"


// Class list

class RichParameterSet;

class MultiViewer_Container : public QSplitter
{
	Q_OBJECT

public:
	MultiViewer_Container(QWidget *parent);
    ~MultiViewer_Container();
	
	/// load an image in the painter widget, the imageNumber is the index of imageList that has the image wanted (0-base index!)
	void addView(Viewer* viewer,  Qt::Orientation);//bool horiz);
	void removeView(int);
	/// update all the visual widgets at one time (glw, imageVis, tree, imageMag)
	void updateAll();
	Viewer* currentView();
    int getNextViewerId();

	int viewerCounter();
	void updateLayout();

	void connectToLayerDialog(Viewer* viewer);

	LayerDialog *layerDialog;
	MeshDocument meshDoc;
	int currentId;

public slots:

	// Called when we change viewer, set the current viewer
    void updateCurrent(int current);
	
private:
	int idCounter;
	QVector<Viewer *> viewerList; /// widgets for the OpenGL contexts and images

	void keyPressEvent(QKeyEvent *keyEv);
	// other stuff
	void resizeEvent ( QResizeEvent *);
	
};

#endif // __MAULTIVIEWER_CONTAINER_H__