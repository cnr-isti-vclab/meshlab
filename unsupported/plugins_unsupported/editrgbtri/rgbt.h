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
#include <common/interfaces.h>

#include "widgetRgbT.h"
#include "rgbInfo.h"

#include "topologicalOp.h"
#include "interactiveEdit.h"

namespace rgbt
{

/** the main class of the plugin */
class RgbTPlugin : public QObject, public MeshEditInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshEditInterface)
    
    typedef TopologicalOp<CMeshO,RgbInfo::VERTEXC,RgbInfo::FACEC > TopologicalOpC;
    
public:
    
    typedef RgbTriangle<CMeshO> RgbTriangleC;
    typedef RgbVertex<CMeshO> RgbVertexC;
    typedef CMeshO::FacePointer FacePointer;
	typedef CMeshO::VertexPointer VertexPointer;
    typedef EdgeFI<FacePointer> EdgeFIType;
    typedef Point3<float> PointType;
    
    
    RgbTPlugin();
    virtual ~RgbTPlugin();
    
    static const QString Info();
    
    virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
    virtual void mousePressEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseMoveEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
    
public slots:
    
    void edgeSplit();
    void vertexRemoval();
    
    
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
    
    TopologicalOpC* to;
    InteractiveEdit* ie;
    RgbInteractiveEdit* rgbie;
    
    
    bool getFaceAtMouse(MeshModel &m,QPoint &mid, CMeshO::FacePointer& val);
    bool getFacesAtMouse(MeshModel &m,QPoint &mid, vector<CMeshO::FacePointer> & val);
    
    void updateSelectedFaces(MeshModel &m);
    
    bool pickEdgeLenght(double& d);
    
    void DrawXORRect(GLArea * gla, bool doubleDraw);

    // Extract the common edge between 2 triangles
    bool commonEdge(CMeshO::FacePointer fp1, CMeshO::FacePointer fp2, EdgeFIType* edge = 0);
    // Extract the common vertex
    bool commonVertex(vector<FacePointer> fc, EdgeFIType* vert = 0);
    
    
    QPoint startp;
    QPoint prevp;

};


}

#endif
