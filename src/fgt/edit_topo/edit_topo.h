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

#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <meshlab/glarea.h>

#include <stdlib.h>

#include <wrap/gl/pick.h>
#include <wrap/gl/picking.h>

#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/color.h>

#include "edit_topodialog.h"


struct Vtx
{
	Point3f V;
	QString vName;

    inline bool operator == (const Vtx &b) const
    {
		return ((V==b.V) && (vName==b.vName));
    }

    inline bool operator != (const Vtx &b) const
    {
		return ((V!=b.V) || (vName!=b.vName));
    }
};

struct Edg
{
	Vtx v[2];

    inline bool operator == (const Edg &b) const
    {
		return (((v[0]==b.v[0]) && (v[1]==b.v[1]))
				||((v[1]==b.v[0]) && (v[0]==b.v[1])));
    }

    inline bool operator != (const Edg &b) const
    {
		return ( ((v[0] != b.v[0])&&(v[0] != b.v[1])) 
					||	((v[1] != b.v[0])&&(v[1] != b.v[1])) );
	}

	inline bool containsVtx(const Vtx &vt) const
	{
		bool toRet = false;
			for(int j=0; j<2; j++)
				if(v[j] == vt)
					toRet = true;

		return toRet;
	}
};

struct Fce
{
	Edg e[3];

	bool selected;

    inline bool operator == (const Fce &f) const
    {
		return (containsEdg(f.e[0]) && containsEdg(f.e[1]) && containsEdg(f.e[2]));
	}

    inline bool operator != (const Fce &f) const
    {
		return (!(containsEdg(f.e[0]) && containsEdg(f.e[1]) && containsEdg(f.e[2])));
	}

	inline bool containsEdg(const Edg &ed) const
	{
		bool toRet = false;
		for(int i=0; i<3; i++)
			if(e[i]==ed)
				toRet=true;

		return toRet;
	}

	inline bool containsVtx(const Vtx &vt) const
	{
		bool toRet = false;
		for(int i=0; i<3; i++)
			for(int j=0; j<2; j++)
				if(e[i].v[j] == vt)
					toRet = true;

		return toRet;
	}
};


class edit_topo : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)

	
public:
	edit_topo();
	virtual ~edit_topo();

	virtual const QString Info(QAction *);
	virtual const PluginInfo &Info();
	virtual void StartEdit(QAction *, MeshModel &, GLArea *);
	virtual void EndEdit(QAction *, MeshModel &, GLArea *);
	virtual void Decorate(QAction *, MeshModel &, GLArea *);
	virtual void mousePressEvent (QAction *, QMouseEvent *, MeshModel &, GLArea * );
	virtual void mouseMoveEvent (QAction *, QMouseEvent *, MeshModel &, GLArea * );
	virtual void mouseReleaseEvent (QAction *, QMouseEvent *event, MeshModel &, GLArea * );
	virtual QList<QAction *> actions() const;
	
	QPoint mousePos;
	int mouseRealY;

	int nameVtxCount;
	bool reDraw;
	bool click;
	CMeshO::FacePointer currentFacePointer;
	

	double mvmatrix[16]; //modelview
	double projmatrix[16]; //projection
	GLint viewport[4]; //viewport

	//Vtx *stack;	
	//Vtx stack[30];
	QList<Vtx> stack;
	QList<Edg> Estack;
	QList<Fce> Fstack;

	QList<Point3f> in;
	QList<Point3f> out;

	Vtx lastPoint;
	Point3f cursorPoint;

	Vtx connectStart;
	Vtx connectEnd;
	
	bool getFaceAtMouse(MeshModel &m,CMeshO::FacePointer& val);
	bool getVertexAtMouse(MeshModel &m,CMeshO::VertexPointer& value);
	int  getNearest(QPointF center, QPointF *points,int num);
	float distancePointSegment(QPointF p, QPointF segmentP1,QPointF segmentP2);
	float distancePointPoint(QPointF P1, QPointF P2);

	void drawFace(CMeshO::FacePointer fp, MeshModel &m, GLArea * gla);
	
	void drawPoint(MeshModel &m, float pSize, Color4b colorBack, Color4b colorFront, Point3f p);
	void drawPoint(MeshModel &m, float pSize, Color4b colorBack, Color4b colorFront, QList<Vtx> list);
	void drawLine(MeshModel &m, float pSize, float lSize, Color4b colorBack, Color4b colorFront, Point3f p1, Point3f p2);
	void drawLabel(QList<Vtx> list);
	void drawLabel(Vtx v);

	Vtx getVisibleVertexNearestToMouse(QList<Vtx> list);

	inline void updateMatrixes() {
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
	}

public slots:
	void on_mesh_create();
	void on_fuffa();

private:
	GLArea *parentGla;
	QList <QAction *> actionList;


	edit_topodialog *edit_topodialogobj;
};

#endif
