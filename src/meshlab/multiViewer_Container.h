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
#ifndef __MULTIVIEWER_CONTAINER_H__
#define __MULTIVIEWER_CONTAINER_H__

#include <GL/glew.h>

#include <QWidget>
#include <QGLWidget>
#include <QVector>
#include <QSplitter>
#include <QSplitterHandle>

#include "../common/meshmodel.h"
#include "../common/ml_shared_data_context.h"

// Class list
class GLArea;
class RichParameterSet;
class MultiViewer_Container;
class MainWindow;

class Splitter : public QSplitter
{
	Q_OBJECT	

public:
	Splitter ( QWidget * parent);
	Splitter(Qt::Orientation orientation, QWidget *parent = 0);
    ~Splitter() {}

	MultiViewer_Container *getRootContainer();

protected:
	QSplitterHandle *createHandle();
};

class SplitterHandle : public QSplitterHandle
{
	Q_OBJECT

public:
	SplitterHandle(Qt::Orientation orientation, QSplitter *parent);
    ~SplitterHandle() {}

protected:
	void mousePressEvent ( QMouseEvent * e ); 
};

class MultiViewer_Container : public Splitter
{
    Q_OBJECT

        typedef vcg::Shot<double> Shot;

public:
    MultiViewer_Container(vcg::QtThreadSafeMemoryInfo& meminfo,bool highprec,size_t perbatchprimitives,QWidget *parent);
    ~MultiViewer_Container();

    void addView(GLArea *viewer, Qt::Orientation);
    void removeView(int);

    GLArea* currentView();
    int getNextViewerId();
    int viewerCounter();

	void updateAllViewer();

    void resetAllTrackBall();
    void update(int id);

    GLArea* getViewer(int id);
    int getViewerByPicking(QPoint);

    void updateTrackballInViewers();

    inline MLSceneGLSharedDataContext* sharedDataContext() {return scenecontext;}

protected:
    void closeEvent(QCloseEvent *event);
public:
    MeshDocument meshDoc;
    MLSceneGLSharedDataContext* scenecontext;

    int currentId;

    GLLogStream *LogPtr() {return &meshDoc.Log;}

signals:
    void updateMainWindowMenus(); //updates the menus of the meshlab MainWindow
    void updateDocumentViewer();

public slots:

        // Called when we change viewer, set the current viewer
	void updateCurrent(int current);

private:
    void patchForCorrectResize(QSplitter* split); 
public:
    QList<GLArea *> viewerList; /// widgets for the OpenGL contexts and images
};

#endif // __MULTIVIEWER_CONTAINER_H__
