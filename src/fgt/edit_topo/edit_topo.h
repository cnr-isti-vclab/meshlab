/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2008                                                \/)\/    *
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
$Log: edit_topo.h,v $
****************************************************************************/

#ifndef edit_topo_H
#define edit_topo_H

#include <common/interfaces.h>
#include <meshlab/glarea.h>

#include <stdlib.h>

#include <wrap/gl/pick.h>
#include <wrap/gl/picking.h>

#include "edit_topodialog.h"
#include "edit_topomeshbuilder.h"


//**************************************************************
//	class edit_topo 
//		retopology main class
//
class edit_topo : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
public:
	edit_topo();
	virtual ~edit_topo();

	static const QString Info();
	virtual bool StartEdit(MeshModel &, GLArea *);
	virtual void EndEdit(MeshModel &, GLArea *);
	virtual void Decorate(MeshModel &, GLArea *);
	virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
	virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * );
	virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &, GLArea * );
	
	// Mouse position tracking
	QPoint mousePos;
	int mouseRealY;

	// retopology model builder object
	RetopMeshBuilder rm;

	// simple counter for vertex naming
	int nameVtxCount;

	// used to reDraw the model (force gla->update)
	bool reDraw;
	// to manage on click
	bool click;
	// to manage d&d
	bool drag_click;
	// to prevent new meshmodel generation after first algorithm use
	bool first_model_generated;

	CMeshO::FacePointer currentFacePointer;
	
	// user defined vertices stack
	// this simple stack contains all user selected
	// vertices. It's dinamically in the edit process
	QList<Vtx> stack;
	// user defined edges stack
	// this simple stack contains all user defined
	// edges. It's dinamically in the edit process
	QList<Edg> Estack;
	// user defined faces stack
	// this simple stack contains all user selected
	// faces. It's dinamically in the edit process
	QList<Fce> Fstack;

	// temporary stack used in the d&d process
	QList<Fce> drag_stack;
	Vtx drag_vtx;

	// used to draw "not found" closest vertices
	QList<Point3f> out;

	Vtx lastPoint;
	Point3f cursorPoint;

	Vtx connectStart;
	Vtx connectEnd;	
	double _md;

	// Topology definition methods
	bool isVertexVisible(Point3f v);
	bool getFaceAtMouse(MeshModel &m,CMeshO::FacePointer& val);
	bool getVertexAtMouse(MeshModel &m,CMeshO::VertexPointer& value);
	int  getNearest(QPointF center, QPointF *points,int num);
	float distancePointSegment(QPointF p, QPointF segmentP1,QPointF segmentP2);
	float distancePointPoint(QPointF P1, QPointF P2);
	bool pointInTriangle(const QPointF &p, const QPointF &a, const QPointF &b, const QPointF &c);
	bool getVisibleVertexNearestToMouse(QList<Vtx> list, Vtx &out);
	bool getVisibleEdgeNearestToMouse(QList<Edg> listE, Edg &ret);

	// Decoration methods
	void drawFace(CMeshO::FacePointer fp);
	void drawPoint(MeshModel &m, float pSize, Color4b colorFront, Point3f p);
	void drawPoint(MeshModel &m, float pSize, Color4b colorFront, QList<Vtx> list);
	void drawLine(Color4b colorBack, Color4b colorFront, Point3f p1, Point3f p2);
	void drawLabel(QList<Vtx> list);
	void drawLabel(Vtx v);

	inline void updateMatrixes() 
	{
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
	}

	// Ui managed functions (events)
public slots:
	void on_mesh_create();
	void on_update_request();

private:
	GLArea *parentGla;

	double mvmatrix[16]; 
	double projmatrix[16]; 
	GLint viewport[4]; 

	// Edit modes
	void editAddVertex(MeshModel &m);
	void editAddVertexFree();
	void editDeleteVertex();
	void editConnectVertex();
	void editSelectFace();
	void editDeconnectEdge();
	void editDragAndDropVertex();
	void editEdgeSplit();
	void editEdgeCollapse();

	// Edit decoration modes
	void editDecoStandard(MeshModel &m);
	void editDecoOnlyVertex(MeshModel &m);
	void editDecoDragAndDropVertex(MeshModel &m);
	void editDecoFaceSelect(MeshModel &m);
	void editDecoVertexSelect(MeshModel &m);
	void editDecoDeleteVertexConnect(MeshModel &m);
	void editDecoDeleteVertexSelect(MeshModel &m);
	void editDecoDeleteVertexDeconnect(MeshModel &m);
	void editDecoCollapse(MeshModel &m);
	void editDecoSplit(MeshModel &m);

	edit_topodialog *edit_topodialogobj;

	// Gui
	QDockWidget* dock;
};

#endif
