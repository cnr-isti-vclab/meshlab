/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <common/meshmodel.h>
#include <wrap/gui/trackball.h>
#include <stdio.h>
#include <QGLWidget>
#include <QShortcut>

#define TEXTX 175
#define TEXTY 200
#define FMETRICX 30
#define FMETRICY 10
#define TRANSLATE 5
#define RADIUS 10
#define VRADIUS 4
#define RECTDIM 30
#define VIEWMODE 0
#define EDITFACEMODE 1
#define SELECTMODE 2
#define SPECIALMODE 3
#define EDITVERTEXMODE 4
#define UNIFYMODE 5
#define SELECTAREA 0
#define SELECTCONNECTED 1
#define SELECTVERTEX 2

using namespace std;
using namespace vcg;

class RenderArea : public QGLWidget
{
    Q_OBJECT

	public:
		enum Mode { View, Edit, EditVert, Select, UnifyVert, NoMode };
		enum EditMode { Scale, Rotate, NoEdit };
		enum SelectMode { Area, Connected, Vertex };

		RenderArea(QWidget *parent = 0, QString path = QString(), MeshModel *m = 0, unsigned textNum = 0);
		~RenderArea();

		void setPen(const QPen &pen);
		void setBrush(const QBrush &brush);
		void setAntialiased(bool antialiased);
		void setTexture(QString path);
		void ChangeMode(int index);
		void ChangeSelectMode(int index);
		void RemapClamp();
		void RemapMod();
		void ClearSelection();
		void InvertSelection();
		void Flip(bool mode);
		void ImportSelection();
		void UnifyCouple();
		void UnifySet();
		void ResetPosition();
		QString GetTextureName();
		void RecalculateSelectionArea();
		void SetDegenerate(bool deg);

	protected:
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseDoubleClickEvent(QMouseEvent *event);
		void wheelEvent(QWheelEvent *event);
		void keyPressEvent(QKeyEvent *event);

	private:
		bool antialiased;	// Antialiasing 
		QImage image;		// Background texture
		unsigned textNum;	// Number of tab (--> index in 'textures' vector)
		QString fileName;	// Name of the texture
		MeshModel *model;	// Ref to the model (for upate)

		Mode mode, oldMode;			// Action type
		EditMode editMode;
		SelectMode selectMode;

		// Trackball data
		vcg::Trackball *tb;
		vcg::Point2f viewport;
		float oldX, oldY;
		int tmpX, tmpY;

		QPen pen;			// For 2D painting
		QBrush brush;

		int panX, panY, tpanX, tpanY, oldPX, oldPY, posVX, posVY;	// Temp for axis
		int maxX, maxY, minX, minY;	// For texCoord out of border

		int selBit, selVertBit;		// User bit: idicates if a face/vertex is selected for editing
		bool selected, selectedV;	// Indicates if the are some selected faces

		QPointF origin;		// Origin for rotate editing
		QRect originR;
		int orX, orY;

		QPoint start, end;	// Selection area
		QRect area;			// Dragged rectangle
		QRectF areaUV;		// Area of selected vertexes

		// Info for interactive editing
		vector<QRect> selRect;	// Vector of buttons area
		QRect vertRect;			// Rectangle for vertexes
		QRect selection;		// Selection area
		QPoint selStart, selEnd;
		int posX, posY, rectX, rectY, oldSRX, oldSRY;	// Stored value
		float degree, scaleX, scaleY;					// Value for edit
		int highlighted, pressed, oldDelta;	// Info about mouse
		QPointF oScale;			// Point of origin for scaling
		int initVX, initVY;		// Old values of viewport
		float B2, Rm, Rq;		// Params for line intersection

		QImage rot, scal;		// Images for buttons

		float zoom;				// Actual value of zoom

		int VCount;				// Vertex counter
		CVertexO *collapse1,	// Pointer to vertex for collapse
				 *collapse2;
		vector<CFaceO*> banList;
		TexCoord2<float> vc1, vc2;
		CVertexO *unifyA, *unifyA1, *unifyB, *unifyB1;	// Pointer to unify segment
		QRect unifyRA, unifyRA1, unifyRB, unifyRB1;
		QPoint uvertA, uvertB, uvertA1, uvertB1;
		float tua, tva, tub, tvb, tua1, tva1, tub1, tvb1;
		CFaceO *firstface, *firstface1;
		bool locked, degenerate;
		GLuint id;

		vector<CVertexO*> path, path1;			// Set of vertex of the path
		vector<Point2f> drawnPath, drawnPath1;	// Set of UV coords found in the path
		bool drawP, drawP1;

		void UpdateUV();
		void UpdateVertex();
		void ResetTrack(bool reset);
		void SelectFaces();
		void SelectConnectedComponent(QPoint e);
		void SelectVertexes();
		void HandleScale(QPoint e);
		void HandleRotate(QPoint e);
		void RotateComponent(float theta);
		void ScaleComponent(float percentX, float percentY);
		void UpdateSelectionArea(int x, int y);
		void UpdateSelectionAreaV(int x, int y);
		void UpdateVertexSelection();
		QPointF ToUVSpace(int x, int y);
		QPoint ToScreenSpace(float u, float v);
		void DrawCircle(QPoint origin);
		void UpdateBoundingArea(QPoint topLeft, QPoint topRight);
		void CountVertexes();
		bool isInside(vector<TexCoord2<float> >, TexCoord2<float>);
		bool isInside(CFaceO* face);
		void ShowFaces();
		void UpdateUnify();
		vector<CVertexO*> FindPath(CVertexO* begin, CVertexO* end, CFaceO* first, int pathN);
		void UpdateUnifyTopology();
		void SetUpRegion(QPoint a, QPoint b, QPoint c);
		void CheckVertex();
		
		void drawSelectedVertexes(int faceindex);
		void drawSelectedFaces(int faceindex);
		void drawEdge(int faceindex);
		void drawUnifyVertexes();
		void drawBackground();
		void drawAxis(QPainter *painter);
		void drawSelectionRectangle(QPainter *painter);
		void drawEditRectangle(QPainter *painter);
		void drawUnifyRectangles(QPainter *painter);
		void resetUnifyData();
		void handleUnifySelection(CMeshO::FaceIterator fi, int j);

		void handlePressView(QMouseEvent *e);
		void handlePressEdit(QMouseEvent *e);
		void handlePressSelect(QMouseEvent *e);
		void handleReleaseView(QMouseEvent *e);
		void handleReleaseEdit(QMouseEvent *e);
		void handleReleaseSelect(QMouseEvent *e);
		void handleMoveEdit(QMouseEvent *e);
		void handleMoveSelect(QMouseEvent *e);

	signals:
		void UpdateModel();

};

#endif