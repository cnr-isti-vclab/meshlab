/****************************************************************************
* Rgb Triangulations Plugin                                                 *
*                                                                           *
* Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
* Copyright(C) 2007                                                         *
* DISI - Department of Computer Science                                     *
* University of Genova                                                      *
*                                                                           *
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
****************************************************************************/

#ifndef RGBT_H
#define RGBT_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QDockWidget>
#include <QHash>
#include <QDialog>
#include <vector>
#include <list>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/math/matrix44.h>

#include "widgetRgbT.h"
#include "rgbInfo.h"
#include "utilities.h"

#include "selectiveRefinement.h"
#include "topologicalOp.h"

namespace rgbt
{

class SelRefThread;

/** the main class of the plugin */
class RgbTPlugin : public QObject, public MeshEditInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshEditInterface)
    QList <QAction *> actionList;

    
    typedef TopologicalOp<CMeshO,RgbInfo::VERTEXC,RgbInfo::FACEC > TopologicalOpC;
    
public:
    
    typedef RgbTriangle<CMeshO> RgbTriangleC;
    typedef RgbVertex<CMeshO> RgbVertexC;
    typedef Utilities::EdgeFIType EdgeFIType;
    typedef CMeshO::FacePointer FacePointer;
    
    RgbTPlugin();
    virtual ~RgbTPlugin();
    
    virtual const QString Info(QAction *);
    virtual const PluginInfo &Info();
    
    virtual void StartEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void Decorate(QAction * /*mode*/, MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent (QAction *, QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseMoveEvent (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseReleaseEvent (QAction *,QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    //	  virtual void wheelEvent         (QAction *QWheelEvent*e, MeshModel &/*m*/, GLArea * );
    virtual QList<QAction *> actions() const;
    
public slots:
    void debugColorCGRG();
    void debugEdgeCollapse();
    void debugEdgeSplit();
    
    void edgeSplit();
    void vertexRemoval();
    
    void start();
    void stop();
    void step();
    void resume();
    void pause();
    
    void startThread();    
    void killThread();
    
    void pickEdgeOutside();
    void pickEdgeBox();
    void pickFace();
    
    
private:

    GLArea* current_gla;
    
    WidgetRgbT* widgetRgbT;
    QDockWidget* paint_dock;
    QPoint cur;
    bool isDragging;

    typedef enum {SMAdd, SMClear,SMSub} SelMode;
    SelMode selMode;

    /// List of selected faces
    list<CMeshO::FacePointer> selectedFaces;
    /// MeshModel in edit by the plugin
    MeshModel *m;
    /// Additional info needed by RGB Triangulation
    RgbInfo* rgbInfo;
    
    
    SelectiveRefinement* sr;
    TopologicalOpC* to;
    
    bool getFaceAtMouse(MeshModel &m,QPoint &mid, CMeshO::FacePointer& val);
    bool getFacesAtMouse(MeshModel &m,QPoint &mid, vector<CMeshO::FacePointer> & val);
    
    void updateSelectedFaces(MeshModel &m);
    
    bool pickEdgeLenght(double& d);
    
    void DrawXORRect(GLArea * gla, bool doubleDraw);
    
    QPoint startp;
    QPoint prevp;

    SelRefThread* srThread;

};

class SelRefThread : public QThread
{
public:
	SelRefThread(GLArea* gla, SelectiveRefinement* sr,int delay) : gla(gla), sr(sr), delay(delay) 
	{
		assert(gla);
		assert(sr);
	} 
	void run();
	
	bool reqTerm;
private:
	GLArea* gla;
	SelectiveRefinement* sr;
	int delay;
};

}

#endif
