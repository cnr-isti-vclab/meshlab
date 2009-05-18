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


#include <QtGui>
#include "renderarea.h"
#include "textureeditor.h"
#include <wrap/qt/trackball.h>
#include <math.h>
#include <stdlib.h>

#define SELECTIONRECT 100
#define ORIGINRECT 200
#define MAX 100000
#define NOSEL -1
#define AREADIM 400
#define HRECT 6

RenderArea::RenderArea(QWidget *parent, QString textureName, MeshModel *m, unsigned tnum) : QGLWidget(parent)
{
	// Parameters
    antialiased = true;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
	textNum = tnum;
	model = m;
	image = QImage();
	fileName = textureName;
	if(textureName != QString())
	{
		if (QFile(textureName).exists())
		{
			image = QImage(textureName);
			int bestW = pow(2.0,floor(::log(double(image.width()))/::log(2.0)));
			int bestH = pow(2.0,floor(::log(double(image.height()))/::log(2.0)));
			QImage imgGL = image.scaled(bestW,bestH,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
			QImage tmpGL = QGLWidget::convertToGLFormat(imgGL);
			glGenTextures(1, &id);
			image = QImage(tmpGL).mirrored(false, true);
		}
		else textureName = QString();
	}
	
	// Initialization
	oldX = 0; oldY = 0;
	viewport = vcg::Point2f(0,0);
	tmpX = 0; tmpY = 0;
	tb = new Trackball();
	tb->center = Point3f(0, 0, 0);
	tb->radius = 1;
	panX = 0; panY = 0;
	oldPX = 0; oldPY = 0;
	posVX = 0; posVY = 0;
	degenerate = false;

	brush = QBrush(Qt::green);
	mode = View;
	oldMode = NoMode;
	editMode = Scale;
	origin = QPointF();
	start = QPoint();
	end = QPoint();

	selectMode = Area;
	selBit = CFaceO::NewBitFlag();
	selVertBit = CVertexO::NewBitFlag();
	selected = false;
	selectedV = false;

	vertRect = QRect(0,0,RADIUS,RADIUS);
	selRect = vector<QRect>();
	for (int i = 0; i < 4; i++) selRect.push_back(QRect(0,0,RECTDIM,RECTDIM));
	selStart = QPoint(0,0); selEnd = QPoint(0,0);
	degree = 0.0f;
	highlighted = NOSEL;
	pressed = NOSEL;
	posX = 0; posY = 0;
	oScale = QPointF(0,0);
	scaleX = 1; scaleY = 1;
	orX = 0; orY = 0;
	initVX = 0; initVY = 0;
	locked = false;
	drawnPath =	vector<Point2f>();
	drawnPath1 = vector<Point2f>();
	banList = vector<CFaceO*>();
	drawP = false;
	drawP1 = false;
	path =	vector<CVertexO*>();
	path1 =	vector<CVertexO*>();

	zoom = 1;

	rot = QImage(QString(":/images/rotate.png"));
	scal = QImage(QString(":/images/scale.png"));

	this->setMouseTracking(true);
	this->setCursor(Qt::PointingHandCursor);
	this->setAttribute(Qt::WA_NoSystemBackground);
	this->setAttribute(Qt::WA_KeyCompression, true);
	this->setFocusPolicy(Qt::ClickFocus);
}

RenderArea::~RenderArea()
{
	//CFaceO::DeleteBitFlag(selBit);
	//CVertexO::DeleteBitFlag(selVertBit);
}

// Change the Pen of the RenderArea
void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

// Chanhe the Brush of the RenderArea
void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

// Set the Antialiased property
void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

// Change the background texture
void RenderArea::setTexture(QString path)
{
	image = QImage(path);
	fileName = path;
	this->update();
}

// Returns the name of the background image
QString RenderArea::GetTextureName()
{
	return fileName;
}

// Inform the render area that the model has degenerate coordinates
void RenderArea::SetDegenerate(bool deg)
{
	degenerate = deg;
}

void RenderArea::paintEvent(QPaintEvent *)
{
	// Init
    QPainter painter(this);
    painter.setPen(QPen(brush,2));
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, antialiased);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	tb->GetView();
	tb->Apply(true);
	maxX = 0; maxY = 0; minX = 0; minY = 0;
	
	if (model != NULL && model->cm.HasPerWedgeTexCoord() && image != QImage())
	{
	    glEnable(GL_COLOR_LOGIC_OP);
		glEnable(GL_DEPTH_TEST);
		glLineWidth(1);
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			glLogicOp(GL_XOR);
			glColor3f(1,1,1);
			// First draw the model in u,v space
			if (model->cm.face[i].WT(0).n() == textNum)
			{
				// While drawning, it counts the number of 'planes'
				if (model->cm.face[i].WT(0).u() > maxX || model->cm.face[i].WT(1).u() > maxX || model->cm.face[i].WT(2).u() > maxX)	maxX++;
				if (model->cm.face[i].WT(0).v() > maxY || model->cm.face[i].WT(1).v() > maxY || model->cm.face[i].WT(2).v() > maxY)	maxY++;
				if (model->cm.face[i].WT(0).u() < minX || model->cm.face[i].WT(1).u() < minX || model->cm.face[i].WT(2).u() < minX)	minX--;
				if (model->cm.face[i].WT(0).v() < minY || model->cm.face[i].WT(1).v() < minY || model->cm.face[i].WT(2).v() < minY)	minY--;

				drawEdge(i); // Draw the edge of faces
				glDisable(GL_COLOR_LOGIC_OP);
				glColor3f(1.0f, 0.0f, 0.0f);
				//if (selected && model->cm.face[i].IsUserBit(selBit)) drawSelectedFaces(i); // Draw the selected faces
				if (selectedV && mode != UnifyVert)	drawSelectedVertexes(i); // Draw the selected vertex
				glEnable(GL_COLOR_LOGIC_OP);

			}
		}
		if (mode == UnifyVert) drawUnifyVertexes();
		
		glDisable(GL_LOGIC_OP);
		glDisable(GL_COLOR_LOGIC_OP);
		if (minX != 0 || minY != 0 || maxX != 0 || maxY != 0) drawBackground(); // Draw the background behind the model

		// 2D
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0,this->width(),this->height(),0,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		drawAxis(&painter); // and the axis, always in first plane
		drawSelectionRectangle(&painter); // Draw the rectangle of selection
		if (mode != UnifyVert) drawEditRectangle(&painter);
		else drawUnifyRectangles(&painter); // Draw the rectangles for unify
		glDisable(GL_LOGIC_OP);
	  	glPopAttrib();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		
		// Draw blend object
		glDepthMask(GL_FALSE);
		glLogicOp(GL_AND);
		glEnable(GL_BLEND); 
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1.0f, 0.0f, 0.0f, 0.7f);
		for (unsigned i = 0; i < model->cm.face.size(); i++)
			if (selected && model->cm.face[i].IsUserBit(selBit)) drawSelectedFaces(i); // Draw the selected faces
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}
	else painter.drawText(this->visibleRegion().boundingRect().width()/2 - FMETRICX, this->visibleRegion().boundingRect().height()/2 - FMETRICY, tr("NO TEXTURE"));

    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
}

// Support methods for drawning
void RenderArea::drawSelectedVertexes(int i)
{
	glDisable(GL_COLOR_LOGIC_OP);
	glColor3f(1,0,0);
	if (!isInside(&model->cm.face[i]))
	{
		for (int j = 0; j < 3; j++)
		{	
			if(areaUV.contains(QPointF(model->cm.face[i].WT(j).u(), model->cm.face[i].WT(j).v())) && model->cm.face[i].V(j)->IsUserBit(selVertBit))
					DrawCircle(QPoint((origin.x() + (cos(degree) * (model->cm.face[i].WT(j).u() - origin.x()) - sin(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM - posVX/zoom,
						(AREADIM - ((origin.y() + (sin(degree) * (model->cm.face[i].WT(j).u() - origin.x()) + cos(degree) * (model->cm.face[i].WT(j).v() - origin.y())))) * AREADIM) - posVY/zoom));
		}
	}
	glColor3f(0,0,0);
	glEnable(GL_COLOR_LOGIC_OP);
}

void RenderArea::drawSelectedFaces(int i)
{
	glBegin(GL_TRIANGLES);
	for (int j = 0; j < 3; j++)
	{
		if (editMode == Scale)
			glVertex3f((oScale.x() + (model->cm.face[i].WT(j).u() - oScale.x()) * scaleX) * AREADIM - panX/zoom, 
				AREADIM - ((oScale.y() + (model->cm.face[i].WT(j).v() - oScale.y()) * scaleY) * AREADIM) - panY/zoom, 1);							
		else glVertex3f((origin.x() + (cos(degree) * (model->cm.face[i].WT(j).u() - origin.x()) - sin(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM - panX/zoom,
				AREADIM - ((origin.y() + (sin(degree) * (model->cm.face[i].WT(j).u() - origin.x()) + cos(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM) - panY/zoom, 1);
	}
	glEnd();
}

void RenderArea::drawEdge(int i)
{
	glBegin(GL_LINE_LOOP);
	for (int j = 0; j < 3; j++)
	{
		if (mode != EditVert)
		{
			if (!model->cm.face[i].IsUserBit(selBit))
				glVertex3f(model->cm.face[i].WT(j).u() * AREADIM , AREADIM - (model->cm.face[i].WT(j).v() * AREADIM), 1);	
			else if (editMode == Scale) glVertex3f((oScale.x() + (model->cm.face[i].WT(j).u() - oScale.x()) * scaleX) * AREADIM - panX/zoom, 
						AREADIM - ((oScale.y() + (model->cm.face[i].WT(j).v() - oScale.y()) * scaleY) * AREADIM) - panY/zoom, 1);							
				 else glVertex3f((origin.x() + (cos(degree) * (model->cm.face[i].WT(j).u() - origin.x()) - sin(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM - panX/zoom,
						AREADIM - ((origin.y() + (sin(degree) * (model->cm.face[i].WT(j).u() - origin.x()) + cos(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM) - panY/zoom, 1);
		}
		else
		{
			if (areaUV.contains(QPointF(model->cm.face[i].WT(j).u(), model->cm.face[i].WT(j).v())) && model->cm.face[i].V(j)->IsUserBit(selVertBit) && !isInside(&model->cm.face[i]))
				glVertex3f((origin.x() + (cos(degree) * (model->cm.face[i].WT(j).u() - origin.x()) - sin(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM - posVX/zoom,
						AREADIM - ((origin.y() + (sin(degree) * (model->cm.face[i].WT(j).u() - origin.x()) + cos(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM) - posVY/zoom, 1);
			else glVertex3f(model->cm.face[i].WT(j).u() * AREADIM , AREADIM - (model->cm.face[i].WT(j).v() * AREADIM), 1);
		}
	}
	glEnd();
}

void RenderArea::drawUnifyVertexes()
{
	glDisable(GL_COLOR_LOGIC_OP);
	glColor3f(1,0,0);
	if (uvertA != QPoint()) DrawCircle(QPoint((origin.x() + (cos(degree) * (tua - origin.x()) - sin(degree) * (tva - origin.y()))) * AREADIM - posVX/zoom,
								AREADIM - ((origin.y() + (sin(degree) * (tua - origin.x()) + cos(degree) * (tva - origin.y()))) * AREADIM) - posVY/zoom));
	if (uvertB != QPoint()) DrawCircle(QPoint((origin.x() + (cos(degree) * (tub - origin.x()) - sin(degree) * (tvb - origin.y()))) * AREADIM - posVX/zoom,
								AREADIM - ((origin.y() + (sin(degree) * (tub - origin.x()) + cos(degree) * (tvb - origin.y()))) * AREADIM) - posVY/zoom));
	if (uvertA1 != QPoint()) DrawCircle(QPoint((origin.x() + (cos(degree) * (tua1 - origin.x()) - sin(degree) * (tva1 - origin.y()))) * AREADIM - posVX/zoom,
								AREADIM - ((origin.y() + (sin(degree) * (tua1 - origin.x()) + cos(degree) * (tva1 - origin.y()))) * AREADIM) - posVY/zoom));
	if (uvertB1 != QPoint()) DrawCircle(QPoint((origin.x() + (cos(degree) * (tub1 - origin.x()) - sin(degree) * (tvb1 - origin.y()))) * AREADIM - posVX/zoom,
								AREADIM - ((origin.y() + (sin(degree) * (tub1 - origin.x()) + cos(degree) * (tvb1 - origin.y()))) * AREADIM) - posVY/zoom));
	glColor3f(0,0,0);
	glEnable(GL_COLOR_LOGIC_OP);
}

void RenderArea::drawBackground()
{
	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	for (int x = minX; x < maxX; x++)
	{
		for (int y = minY; y < maxY; y++)			
		{
		    glBegin(GL_QUADS);
				glTexCoord2f(0.0,0.0); 
				glVertex3f(0.0 + x*AREADIM,0.0 - y*AREADIM,0.0);
				glTexCoord2f(1.0,0.0); 
				glVertex3f(1.0*AREADIM + x*AREADIM,0.0 - y*AREADIM,0.0);
				glTexCoord2f(1.0,1.0); 
				glVertex3f(1.0*AREADIM + x*AREADIM,1.0*AREADIM - y*AREADIM,0.0);
				glTexCoord2f(0.0,1.0); 
				glVertex3f(0.0 + x*AREADIM,1.0*AREADIM - y*AREADIM,0.0);
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);
}

void RenderArea::drawAxis(QPainter* painter)
{
	// Line and text (native Qt)
	int w = this->visibleRegion().boundingRect().width();
	int h = this->visibleRegion().boundingRect().height();
	painter->drawLine(0,h,w,h);
	painter->drawLine(0,h,0,0);
	// Calculate the coords and draw it
	float ox = (float)-viewport.X()*zoom/(AREADIM*zoom), oy = (float)(AREADIM*zoom - h + viewport.Y()*zoom)/(AREADIM*zoom);
	/*O:*/painter->drawText(TRANSLATE, h - TRANSLATE, QString("(%1,%2)").arg(ox).arg(oy));
	/*Y:*/painter->drawText(TRANSLATE, TRANSLATE*3, QString("(%1,%2)").arg(ox).arg((float)(AREADIM*zoom + viewport.Y()*zoom)/(AREADIM*zoom)));
	/*X:*/painter->drawText(w - TRANSLATE*18, h - TRANSLATE, QString("(%1,%2)").arg((float)(w-viewport.X()*zoom)/(AREADIM*zoom)).arg(oy));
	painter->drawText(TRANSLATE, TRANSLATE*6, QString("V"));
	painter->drawText(w - TRANSLATE*23, h - TRANSLATE, QString("U"));
}

void RenderArea::drawSelectionRectangle(QPainter *painter)
{
	if (start != QPoint() && end != QPoint())
	{
		painter->setPen(QPen(QBrush(Qt::gray),1));
		painter->setBrush(QBrush(Qt::NoBrush));
		painter->drawRect(area);
	}
}

void RenderArea::drawEditRectangle(QPainter *painter)
{
	if (selection != QRect() && (mode == Edit || mode == EditVert))
	{
		// The rectangle of editing
		painter->setPen(QPen(QBrush(Qt::yellow),2));
		painter->setBrush(QBrush(Qt::NoBrush));
		if (mode == Edit) painter->drawRect(QRect(selection.x() - posX, selection.y() - posY, selection.width(), selection.height()));
		else painter->drawRect(QRect(selection.x() - posVX, selection.y() - posVY, selection.width(), selection.height()));
		if (mode == Edit || (mode == EditVert && VCount > 1))
		{
			// Rectangle on the corner
			painter->setPen(QPen(QBrush(Qt::black),1));
			for (unsigned l = 0; l < selRect.size(); l++)
			{
				if (l == highlighted) painter->setBrush(QBrush(Qt::yellow));
				else painter->setBrush(QBrush(Qt::NoBrush));
				painter->drawRect(selRect[l]);
				if (editMode == Scale && mode == Edit) painter->drawImage(selRect[l],scal,QRect(0,0,scal.width(),scal.height()));
				else painter->drawImage(selRect[l],rot,QRect(0,0,rot.width(),rot.height()));
			}	
			// and the origin of the rotation
			if ((editMode == Rotate && mode == Edit) || mode == EditVert)
			{
				painter->setPen(QPen(QBrush(Qt::black),1));
				if (highlighted == ORIGINRECT) painter->setBrush(QBrush(Qt::blue));
				else painter->setBrush(QBrush(Qt::yellow));
				if (mode == Edit) painter->drawEllipse(QRect(originR.x() - posX - orX, originR.y() - posY - orY, RADIUS, RADIUS));
				else painter->drawEllipse(QRect(originR.x() - posVX - orX, originR.y() - posVY - orY, RADIUS, RADIUS));
			}
		}
	}
}

void RenderArea::drawUnifyRectangles(QPainter *painter)
{
	if (unifyRA != QRect())
	{
		painter->setPen(QPen(QBrush(Qt::blue),1));
		painter->setBrush(QBrush(Qt::NoBrush));
		painter->drawRect(unifyRA);
		painter->drawText(unifyRA.center().x() - RADIUS*2, unifyRA.center().y(), tr("A"));
	}
	if (unifyRB != QRect())
	{
		painter->drawRect(unifyRB);
		painter->drawText(unifyRB.center().x() - RADIUS*2, unifyRB.center().y(), tr("B"));
	}
	if (unifyRA != QRect() && unifyRB != QRect()) painter->drawLine(unifyRA.center(), unifyRB.center());
	if (unifyRA1 != QRect())
	{
		painter->drawRect(unifyRA1);
		painter->drawText(unifyRA1.center().x() - RADIUS*2, unifyRA1.center().y(), tr("A'"));
	}
	if (unifyRB1 != QRect())
	{
		painter->drawRect(unifyRB1);
		painter->drawText(unifyRB1.center().x() - RADIUS*2, unifyRB1.center().y(), tr("B'"));
	}
	if (unifyRA1 != QRect() && unifyRB1 != QRect())	painter->drawLine(unifyRA1.center(), unifyRB1.center());
	if (drawP)
	{
		painter->setPen(QPen(QBrush(Qt::red),2));
		for (unsigned k = 0; k < drawnPath.size()-1; k++)
			painter->drawLine(ToScreenSpace(drawnPath[k].X(), drawnPath[k].Y()), ToScreenSpace(drawnPath[k+1].X(),drawnPath[k+1].Y()));			
	}
	if (drawP1)
	{
		for (unsigned k = 0; k < drawnPath1.size()-1; k++)
			painter->drawLine(ToScreenSpace(drawnPath1[k].X(), drawnPath1[k].Y()), ToScreenSpace(drawnPath1[k+1].X(),drawnPath1[k+1].Y()));
	}
}
// Mouse Event:
void RenderArea::mousePressEvent(QMouseEvent *e)
{
	if((e->buttons() & Qt::LeftButton))
	{
		if ((mode == Edit || mode == EditVert) && highlighted == NOSEL) 
		{
			this->ChangeMode(SPECIALMODE);
			pressed = NOSEL;
			selected = false;
			selectedV = false;
			selVertBit = CVertexO::NewBitFlag();
			for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearUserBit(selBit);
		}
		switch(mode)
		{
			case View:
				handlePressView(e);
				break;
			case Edit:
				handlePressEdit(e);
				break;
			case EditVert:
				handlePressEdit(e);
				break;
			case Select:
				handlePressSelect(e);
				break;
			case UnifyVert:
				handlePressSelect(e);
				break;
		}
	}
	else if((e->buttons() & Qt::MidButton))
	{
		// Pan
		oldMode = mode;
		this->ChangeMode(VIEWMODE);
		oldX = e->x(); oldY = e->y();
		tmpX = viewport.X(); tmpY = viewport.Y();
		tb->MouseDown(e->x(), AREADIM-e->y(), QT2VCG(e->button(), e->modifiers()));
		this->update();
	}
}

void RenderArea::handlePressView(QMouseEvent *e)
{
	oldX = e->x(); oldY = e->y();
	tmpX = viewport.X(); tmpY = viewport.Y();
	tb->MouseDown(e->x(), AREADIM-e->y(), QT2VCG(e->button(), e->modifiers()));
	this->update();
}

void RenderArea::handlePressSelect(QMouseEvent *e)
{
	start = e->pos();
	end = e->pos();
}

void RenderArea::handlePressEdit(QMouseEvent *e)
{
	tpanX = e->x();
	tpanY = e->y();
	pressed = highlighted;
	if (highlighted > NOSEL && highlighted < selRect.size())
	{
		rectX = selRect[highlighted].center().x();
		rectY = selRect[highlighted].center().y();
		if (mode == Edit)
		{
			oldSRX = selection.width();
			oldSRY = selection.height();
			scaleX = 1; scaleY = 1;
		}
		int perno;
		if (highlighted == 0) perno = 3;
		else if (highlighted == 1) perno = 2;
		else if (highlighted == 2) perno = 1;
		else perno = 0;
		oScale = ToUVSpace(selRect[perno].center().x(), selRect[perno].center().y());
		B2 = (float)(rectX - originR.center().x())*(rectX - originR.center().x()) + (rectY - originR.center().y())*(rectY - originR.center().y());
		Rm = (float)(rectY - originR.center().y()) / (rectX - originR.center().x());
		Rq = (float) rectY - Rm * rectX;
	}
}

void RenderArea::mouseReleaseEvent(QMouseEvent *e)
{
	switch(mode)
	{
		case View:
			handleReleaseView(e);
			break;
		case Edit:
			handleReleaseEdit(e);
			break;
		case EditVert:
			handleReleaseEdit(e);
			break;
		case Select:
			handleReleaseSelect(e);
			break;
		case UnifyVert:
			area = QRect();
			locked = false;
			if (unifyRB1 != QRect()) UnifySet();
			this->update();
			break;
	}
}

void RenderArea::handleReleaseView(QMouseEvent *e)
{
	if (selection != QRect()) 
	{
		UpdateSelectionArea((viewport.X() - initVX)*zoom, (viewport.Y() - initVY)*zoom);
		originR.moveCenter(QPoint(originR.x() + (viewport.X() - initVX)*zoom, originR.y() + (viewport.Y() - initVY)*zoom));
		origin = ToUVSpace(originR.center().x(), originR.center().y());
	}
	initVX = viewport.X(); initVY = viewport.Y();
	if (oldMode != NoMode)
	{
		this->ChangeMode(oldMode);
		oldMode = NoMode;
	}
}

void RenderArea::handleReleaseEdit(QMouseEvent *e)
{
	oldPX = panX;
	oldPY = panY;
	if (pressed == ORIGINRECT)	// Drag origin -> Update the position of the rectangle of rotation and the real point in UV
	{
		originR = QRect(originR.x() - posX - orX, originR.y() - posY - orY, RADIUS, RADIUS);
		origin = ToUVSpace(originR.center().x(), originR.center().y());
		orX = 0; orY = 0;
	}
	else if (pressed == SELECTIONRECT)
	{
		if (mode == Edit && posX != 0)	// Drag selection -> Update the position of the selection area and the rotatation rect
		{
			selection = QRect(selection.x() - posX, selection.y() - posY, selection.width(), selection.height());
			originR.moveCenter(QPoint(originR.center().x() - posX, originR.center().y() - posY));
			origin = ToUVSpace(originR.center().x(), originR.center().y());
			posX = 0; posY = 0;
			if (selected) UpdateUV();
		}
		else if (mode == EditVert && posVX != 0)
		{
			selection = QRect(selection.x() - posVX, selection.y() - posVY, selection.width(), selection.height());
			originR.moveCenter(QPoint(originR.center().x() - posVX, originR.center().y() - posVY));
			origin = ToUVSpace(originR.center().x(), originR.center().y());
			if (selectedV) UpdateVertex();
		}
	}
	else if (pressed > NOSEL && pressed < selRect.size())
	{
		if (editMode == Rotate && mode == Edit)
		{
			RotateComponent(degree);
			RecalculateSelectionArea();
			degree = 0;
		}
		else if (scaleX != 1 && scaleY != 1 && mode == Edit) 
		{
			ScaleComponent(scaleX, scaleY);
			RecalculateSelectionArea();
			scaleX = 1; 
			scaleY = 1;
			oScale = QPointF(0,0);
		}
		else if (mode == EditVert)
		{
			selStart = QPoint(MAX,MAX);
			selEnd = QPoint(-MAX,-MAX);
			RotateComponent(degree);
			selection = QRect(QPoint(selStart.x() - VRADIUS, selStart.y() - VRADIUS), QPoint(selEnd.x() + VRADIUS, selEnd.y() + VRADIUS));
			QPointF a = ToUVSpace(selection.x(), selection.y());
			QPointF b = ToUVSpace(selection.bottomRight().x(),selection.bottomRight().y());
			areaUV = QRectF(a, QSizeF(b.x()-a.x(), b.y()-a.y()));
			UpdateSelectionAreaV(0,0);
			degree = 0;
		}
	}
}

void RenderArea::handleReleaseSelect(QMouseEvent *e)
{
	start = QPoint();
	end = QPoint();
	area = QRect();
	switch (selectMode)
	{
		case Area:
			if (selected)
			{
				selection = QRect(selStart, selEnd);
				UpdateSelectionArea(0,0);
				origin = ToUVSpace(selection.center().x(), selection.center().y());
				originR = QRect(selection.center().x()-RADIUS/2, selection.center().y()-RADIUS/2, RADIUS, RADIUS);
				this->ChangeMode(EDITFACEMODE);
				this->ShowFaces();
			}
			break;
		case Connected:
			SelectConnectedComponent(e->pos());
			if (selected)
			{
				selection = QRect(selStart, selEnd);
				UpdateSelectionArea(0,0);
				RecalculateSelectionArea();
				origin = ToUVSpace(selection.center().x(), selection.center().y());
				originR = QRect(selection.center().x()-RADIUS/2, selection.center().y()-RADIUS/2, RADIUS, RADIUS);
				this->ChangeMode(EDITFACEMODE);
				this->ShowFaces();
			}
			break;
		case Vertex:
			if (selectedV)
			{
				CountVertexes();
				selection = QRect(QPoint(selStart.x() - RADIUS/2, selStart.y() - RADIUS/2), QPoint(selEnd.x() + RADIUS/2, selEnd.y() + RADIUS/2));
				if (VCount > 1)
				{
					UpdateSelectionAreaV(0,0);
					origin = ToUVSpace(selection.center().x(), selection.center().y());
					originR = QRect(selection.center().x()-RADIUS/2, selection.center().y()-RADIUS/2, RADIUS, RADIUS);
				}
				this->ChangeMode(EDITVERTEXMODE);
			}
			break;
	}
	this->update();
}

void RenderArea::mouseMoveEvent(QMouseEvent *e)
{
	int sx = (e->x() - oldX)/zoom, sy = (e->y() - oldY)/zoom;
	if((e->buttons() & Qt::LeftButton) && image != QImage())
	{
		if (mode == View)
		{
			tb->Translate(Point3f(- oldX + e->x(), - oldY + e->y(), zoom));
			viewport = Point2f(tmpX + sx, tmpY + sy);
			this->update();
		}
		else if (mode == Edit || mode == EditVert)	handleMoveEdit(e);
		else if (mode == Select || mode == UnifyVert) handleMoveSelect(e);
	}
	else if((e->buttons() & Qt::MidButton) && image != QImage()) // Pan
	{
		tb->Translate(Point3f(- oldX + e->x(), - oldY + e->y(), zoom));
		viewport = Point2f(tmpX + sx, tmpY + sy);
		this->update();
	}
	else if (image != QImage())// No click
	{
		if (mode == Edit || mode == EditVert)
		{
			for (unsigned y = 0; y < selRect.size(); y++)
			{
				if (selRect[y].contains(e->pos()) && (mode == Edit || (mode == EditVert && VCount > 1)))
				{
					if (highlighted != y) this->update(selRect[y]);
					highlighted = y;
					return;
				}
			}
			if (originR.contains(e->pos()) && ((mode == Edit && editMode == Rotate) || mode == EditVert))
			{
				if (highlighted != ORIGINRECT) this->update(originR);
				highlighted = ORIGINRECT;
				return;
			}
			if (selection.contains(e->pos()))
			{
				if (highlighted == ORIGINRECT) this->update(originR);
				else if (highlighted < selRect.size()) this->update(selRect[highlighted]);
				highlighted = SELECTIONRECT;
				return;
			}
			if (highlighted != NOSEL)
			{
				if (highlighted == ORIGINRECT) this->update(originR);
				else if (highlighted < selRect.size()) this->update(selRect[highlighted]);
			}
			highlighted = NOSEL;
		}
	}
}

void RenderArea::handleMoveEdit(QMouseEvent *e)
{
	int tX = tpanX - e->x();
	int tY = tpanY - e->y();
	if (pressed == SELECTIONRECT)
	{
		// Move the selection ara
		if (mode == Edit)
		{
			panX = oldPX + tpanX - e->x();
			panY = oldPY + tpanY - e->y();
		}
		if (tX != 0 || tY != 0)
		{
			QPoint ta = QPoint(selection.topLeft().x() - tX, selection.topLeft().y() - tY);
			QPoint tb = QPoint(selection.topRight().x() - tX, selection.topRight().y() - tY);
			QPoint tc = QPoint(selection.bottomLeft().x() - tX, selection.bottomLeft().y() - tY);
			QPoint td = QPoint(selection.bottomRight().x() - tX, selection.bottomRight().y() - tY);
			if (mode == Edit)
			{
				posX = tX; posY = tY;
				selRect[0].moveCenter(ta); selRect[1].moveCenter(tb);
				selRect[2].moveCenter(tc); selRect[3].moveCenter(td);
			}
			else
			{
				posVX = tX; posVY = tY;
				selRect[0].moveBottomRight(ta); selRect[1].moveBottomLeft(tb);
				selRect[2].moveTopRight(tc); selRect[3].moveTopLeft(td);
			}
		}
	}
	else if (pressed == ORIGINRECT)
	{
		orX = tX;
		orY = tY;
		this->update(originR);
	}
	else if (pressed > NOSEL && pressed < selRect.size())  
	{
		if (editMode == Scale && mode == Edit) HandleScale(e->pos());
		else HandleRotate(e->pos());
	}
	this->update();
}

void RenderArea::handleMoveSelect(QMouseEvent *e)
{
	if ((mode == Select && selectMode != Connected) || mode == UnifyVert)
	{
		end = e->pos();
		int x1, x2, y1, y2;
		if (start.x() < end.x()) {x1 = start.x(); x2 = end.x();} else {x1 = end.x(); x2 = start.x();}
		if (start.y() < end.y()) {y1 = start.y(); y2 = end.y();} else {y1 = end.y(); y2 = start.y();}
		area = QRect(x1,y1,x2-x1,y2-y1);
		if (mode == Select)
		{
			if (selectMode == Area) SelectFaces();
			else SelectVertexes();
		}
		else if (!locked) SelectVertexes();
		this->update();
	}
}

void RenderArea::mouseDoubleClickEvent(QMouseEvent *e)
{
	switch(mode)
	{
		case View:	// Center the screen on the mouse click position
			viewport = Point2f(viewport.X() - e->x()/zoom + (this->visibleRegion().boundingRect().width()/zoom)/2,
				viewport.Y() - e->y()/zoom + (this->visibleRegion().boundingRect().height()/zoom)/2);
			oldX = 0; oldY = 0;
			tb->track.SetTranslate(Point3f(viewport.X(), viewport.Y(), 1));
			tb->Scale(zoom);
			this->update();
			break;
		case Edit:	// Change edit mode
			if (selection.contains(e->pos()))
			{
				if (editMode == Rotate) editMode = Scale;
				else editMode = Rotate;
				this->update();
			}
			break;
	}
}

void RenderArea::wheelEvent(QWheelEvent*e)
{
	if (image != QImage())
	{
		// Handle the zoom for any mode
		int cwx = viewport.X() - (this->visibleRegion().boundingRect().width()/zoom)/2;
		int cwy = viewport.Y() - (this->visibleRegion().boundingRect().height()/zoom)/2;
		if (e->delta() > 0) zoom /= 0.75; 
		else zoom *= 0.75; 
		// Change the viewport, putting the center of the screen on the mouseposition
		cwx += (this->visibleRegion().boundingRect().width()/zoom)/2;
		cwy += (this->visibleRegion().boundingRect().height()/zoom)/2;
		viewport = Point2f(cwx, cwy);
		ResetTrack(false);
		tb->Scale(zoom);
		if (selectedV) 
		{
			if (mode == UnifyVert) UpdateUnify();
			else UpdateVertexSelection();
		}
		else if (selected) RecalculateSelectionArea();
		originR.moveCenter(ToScreenSpace(origin.x(), origin.y()));
		initVX = viewport.X(); initVY = viewport.Y();
		this->update();
	}
}

void RenderArea::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_H) ResetPosition();
	else e->ignore();
}

// Remap the uv coord out of border using clamp method
void RenderArea::RemapClamp()
{
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum && !model->cm.face[i].IsD())
		{
			model->cm.face[i].ClearUserBit(selBit);
			for (unsigned j = 0; j < 3; j++)
			{
				if (model->cm.face[i].WT(j).u() > 1) model->cm.face[i].WT(j).u() = 1;
				else if (model->cm.face[i].WT(j).u() < 0) model->cm.face[i].WT(j).u() = 0;
				if (model->cm.face[i].WT(j).v() > 1) model->cm.face[i].WT(j).v() = 1;
				else if (model->cm.face[i].WT(j).v() < 0) model->cm.face[i].WT(j).v() = 0;
			}
		}
	}
	panX = 0; panY = 0; tpanX = 0; tpanY = 0; oldPX = 0; oldPY = 0;
	ResetTrack(true);
	vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(model->cm);
	selection = QRect();
	this->update();
	emit UpdateModel();
}

// Remap the uv coord out of border using mod function
void RenderArea::RemapMod()
{
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum && !model->cm.face[i].IsD())
		{
			model->cm.face[i].ClearUserBit(selBit);
			for (unsigned j = 0; j < 3; j++)
			{
				float u = model->cm.face[i].WT(j).u();
				float v = model->cm.face[i].WT(j).v();
				if (u < 0) u = u + (int)u + 1;
				else if (u > 1) u = u - (int)u;
				if (v < 0) v = v + (int)v + 1; 
				else if (v > 1) v = v - (int)v;
				model->cm.face[i].WT(j).u() = u;
				model->cm.face[i].WT(j).v() = v;
			}
		}
	}
	panX = 0; panY = 0; tpanX = 0; tpanY = 0; oldPX = 0; oldPY = 0;
	ResetTrack(true);
	vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(model->cm);
	selection = QRect();
	this->update();
	emit UpdateModel();
}

// Change the tool's action mode
void RenderArea::ChangeMode(int modenumber)
{
	if (mode == UnifyVert && modenumber != UNIFYMODE)
	{
		resetUnifyData();
		drawnPath.clear(); drawnPath1.clear();
		drawP = false; drawP1 = false;
	}
	switch(modenumber)
	{
		case VIEWMODE:
			if (mode != View)
			{
				mode = View;
				this->setCursor(Qt::PointingHandCursor);
			}
			break;
		case EDITFACEMODE:
			if (mode != Edit)
			{
				mode = Edit;
				highlighted = SELECTIONRECT;
				this->setCursor(Qt::SizeAllCursor);
			}
			break;
		case SELECTMODE:
			if (mode != Select)
			{
				if (selection != QRect())
				{
					if (selectMode == Vertex) 
					{
						mode = EditVert; selectedV = true;
						UpdateSelectionAreaV(0,0);
						for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearUserBit(selBit);
					}
					else 
					{
						mode = Edit; selected = true;
						for (unsigned i = 0; i < model->cm.vert.size(); i++) model->cm.vert[i].ClearUserBit(selVertBit);
					}
					this->setCursor(Qt::SizeAllCursor);
				}
				else
				{
					mode = Select;
					for (unsigned i = 0; i < model->cm.face.size(); i++) 
					{
						model->cm.face[i].ClearUserBit(selBit);	model->cm.face[i].ClearS();
					}
					for (unsigned i = 0; i < model->cm.vert.size(); i++) model->cm.vert[i].ClearUserBit(selVertBit);
					emit UpdateModel();
					this->setCursor(Qt::CrossCursor);
				}
			}
			break;
		case SPECIALMODE:	// For internal use... reset the selection
			mode = Select;
			for (unsigned i = 0; i < model->cm.face.size(); i++) 
			{
				model->cm.face[i].ClearUserBit(selBit);	model->cm.face[i].ClearS();
			}
			selection = QRect();
			this->setCursor(Qt::CrossCursor);
			emit UpdateModel();
			break;
		case EDITVERTEXMODE:
			if (mode != EditVert)
			{
				if (selection != QRect())
				{
					mode = EditVert; selectedV = true;
					this->setCursor(Qt::SizeAllCursor);
				}
				else
				{
					mode = Select;
					this->setCursor(Qt::CrossCursor);
				}
			}
			break;
		case UNIFYMODE:
			if (mode != UnifyVert)
			{
				mode = UnifyVert;
				resetUnifyData();
				if (selected) {for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearS();}
				selection = QRect(); selected = false; selectedV = false;
				for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearUserBit(selBit);
				selVertBit = CVertexO::NewBitFlag();
				this->setCursor(Qt::CrossCursor);
				model->updateDataMask(MeshModel::MM_VERTFACETOPO | MeshModel::MM_FACEFACETOPO);
				//model->cm.face.EnableVFAdjacency();
				//model->cm.vert.EnableVFAdjacency();
				//model->cm.face.EnableFFAdjacency();
				UpdateUnifyTopology();
			}
			break;
	}
	this->update();
}

void RenderArea::resetUnifyData()
{
	unifyRA = QRect();
	unifyRA1 = QRect();
	unifyRB = QRect();
	unifyRB1 = QRect();
	uvertA = QPoint();
	uvertA1 = QPoint();
	uvertB = QPoint();
	uvertB1 = QPoint();
}

// Change the selection function
void RenderArea::ChangeSelectMode(int selectindex)
{
	switch(selectindex)
	{
		case SELECTAREA:
			if (selectMode != Area && selectMode != Connected) selection = QRect();
			selectMode = Area;
			break;
		case SELECTCONNECTED:
			if (selectMode != Connected && selectMode != Area) selection = QRect();
			selectMode = Connected;
			break;
		case SELECTVERTEX:
			if (selectMode != Vertex) selection = QRect();
			selectMode = Vertex;
			break;
		default:
			selectMode = Area;
			break;
	}
	if (selectedV && selectMode != Vertex) 
	{
		areaUV = QRectF();
		selVertBit = CVertexO::NewBitFlag();
		selectedV = false;
	}
	if (selected && selectMode == Vertex) 
	{
		selected = false;
		//selBit = CFaceO::NewBitFlag();
		for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearUserBit(selBit);
		for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearS();
		emit UpdateModel();
	}
}

void RenderArea::RotateComponent(float theta)
{
	// Calcolate the new position of the vertex of the selected component after a rotation.
	// The rotation is done around the selected point
	if (origin != QPoint())
	{
		float sinv = sin(theta);
		float cosv = cos(theta);
		if (selected)
		{
			for (unsigned i = 0; i < model->cm.face.size(); i++)
			{
				if (model->cm.face[i].WT(0).n() == textNum && !model->cm.face[i].IsD() && (!selected || (selected && model->cm.face[i].IsUserBit(selBit))))
				for (unsigned j = 0; j < 3; j++)
				{
					float u = origin.x() + (cosv * (model->cm.face[i].WT(j).u() - origin.x()) - sinv * (model->cm.face[i].WT(j).v() - origin.y()));
					float v = origin.y() + (sinv * (model->cm.face[i].WT(j).u() - origin.x()) + cosv * (model->cm.face[i].WT(j).v() - origin.y()));
					model->cm.face[i].WT(j).u() = u;
					model->cm.face[i].WT(j).v() = v;
				}
			}
		}
		else if (selectedV)
		{
			for (unsigned i = 0; i < model->cm.face.size(); i++)
			{
				for (unsigned j = 0; j < 3; j++)
				{
					if (areaUV.contains(QPointF(model->cm.face[i].WT(j).u(),model->cm.face[i].WT(j).v()))
						&& model->cm.face[i].V(j)->IsUserBit(selVertBit) && !model->cm.face[i].V(j)->IsD())
					{
						float u = origin.x() + (cosv * (model->cm.face[i].WT(j).u() - origin.x()) - sinv * (model->cm.face[i].WT(j).v() - origin.y()));
						float v = origin.y() + (sinv * (model->cm.face[i].WT(j).u() - origin.x()) + cosv * (model->cm.face[i].WT(j).v() - origin.y()));
						model->cm.face[i].WT(j).u() = u;
						model->cm.face[i].WT(j).v() = v;
						QPoint tmp = ToScreenSpace(u, v);
						UpdateBoundingArea(tmp,tmp);
					}
				}
			}
		}
		this->update();
		emit UpdateModel();
	}
}

void RenderArea::ScaleComponent(float percX, float percY)
{
	// Scale the selected component. The origin is set to the clicked point
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum && selected && model->cm.face[i].IsUserBit(selBit) && !model->cm.face[i].IsD())
		for (unsigned j = 0; j < 3; j++)
		{
			float x = oScale.x() + (model->cm.face[i].WT(j).u() - oScale.x()) * percX;
			float y = oScale.y() + (model->cm.face[i].WT(j).v() - oScale.y()) * percY;
			model->cm.face[i].WT(j).u() = x;
			model->cm.face[i].WT(j).v() = y;
		}
	}
	this->update();
	emit UpdateModel();
}

void RenderArea::UpdateUV()
{
	// After a move of component, re-calculate the new UV coordinates
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum && model->cm.face[i].IsUserBit(selBit) && !model->cm.face[i].IsD())
		{
			for (unsigned j = 0; j < 3; j++)
			{
				model->cm.face[i].WT(j).u() = model->cm.face[i].WT(j).u() - (float)panX/(AREADIM*zoom);
				model->cm.face[i].WT(j).v() = model->cm.face[i].WT(j).v() + (float)panY/(AREADIM*zoom);
			}
		}
	}
	panX = 0; panY = 0; tpanX = 0; tpanY = 0; oldPX = 0; oldPY = 0;
	this->update();
	emit UpdateModel();
}

void RenderArea::UpdateVertex()
{
	// After a move of vertex, re-calculate the new UV coordinates
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (!isInside(&model->cm.face[i]))
		{
			for (unsigned j = 0; j < 3; j++)
			{
				if (areaUV.contains(QPointF(model->cm.face[i].WT(j).u(),model->cm.face[i].WT(j).v()))
					&& model->cm.face[i].V(j)->IsUserBit(selVertBit) && !model->cm.face[i].V(j)->IsD())
				{
					model->cm.face[i].WT(j).u() = model->cm.face[i].WT(j).u() - (float)posVX/(AREADIM*zoom);
					model->cm.face[i].WT(j).v() = model->cm.face[i].WT(j).v() + (float)posVY/(AREADIM*zoom);
				}
			}
		}
	}
	areaUV.moveCenter(QPointF(areaUV.center().x() - (float)posVX/(AREADIM*zoom), areaUV.center().y() + (float)posVY/(AREADIM*zoom)));
	tpanX = 0; tpanY = 0; posVX = 0; posVY = 0;
	this->update();
	emit UpdateModel();
}

void RenderArea::ResetTrack(bool resetViewPort)
{
	// Reset the viewport of the trackball
	tb->center = Point3f(0, 0, 0);
	tb->track.SetScale(1);
	if (resetViewPort) viewport = Point2f(0,0);
	oldX = 0; oldY = 0;
	tb->track.SetTranslate(Point3f(viewport.X(), viewport.Y(), 1));
}

void RenderArea::SelectFaces()
{
	// Check if a face is inside the rectangle of selection and mark it
	selStart = QPoint(MAX,MAX);
	selEnd = QPoint(-MAX,-MAX);
	selected = false;
	selection = QRect();
	CMeshO::FaceIterator fi;
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).WT(0).n() == textNum && !(*fi).IsD())
		{
	        (*fi).ClearUserBit(selBit);
			QVector<QPoint> t = QVector<QPoint>(); 
			t.push_back(ToScreenSpace((*fi).WT(0).u(), (*fi).WT(0).v()));
			t.push_back(ToScreenSpace((*fi).WT(1).u(), (*fi).WT(1).v()));
			t.push_back(ToScreenSpace((*fi).WT(2).u(), (*fi).WT(2).v()));
			QRegion r = QRegion(QPolygon(t));
			if (r.intersects(area))
			{
				(*fi).SetUserBit(selBit);
				UpdateBoundingArea(r.boundingRect().topLeft(), r.boundingRect().bottomRight());
				if (!selected) selected = true;
			}
		}
	}
}

void RenderArea::SelectVertexes()
{
	// Check if a vertex is inside the rectangle of selection
	selStart = QPoint(MAX,MAX);
	selEnd = QPoint(-MAX,-MAX);
	selectedV = false;
	selection = QRect();
	QPointF a = ToUVSpace(area.x(), area.y());
	QPointF b = ToUVSpace(area.bottomRight().x(),area.bottomRight().y());
	areaUV = QRectF(a, QSizeF(b.x()-a.x(), b.y()-a.y()));
	CMeshO::FaceIterator fi;
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).WT(0).n() == textNum && !(*fi).IsD())
		{
			for (int j = 0; j < 3; j++)
			{
				QPoint tmp = ToScreenSpace((*fi).WT(j).u(), (*fi).WT(j).v());
				if (area.contains(tmp))
				{
					(*fi).V(j)->SetUserBit(selVertBit);
					UpdateBoundingArea(tmp, tmp);
					if (!selectedV) selectedV = true;
					if (mode == UnifyVert && !locked)
					{
						locked = true;
						handleUnifySelection(fi, j);
						return;
					}
				}
			}
		}
	}
	if (mode != UnifyVert) CheckVertex();
}

void RenderArea::CheckVertex()
{
	// Search for unselected vertexes in UV Space
	banList.clear();
	CMeshO::FaceIterator fi;
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).WT(0).n() == textNum && !(*fi).IsD())
		{
			bool go = false;
			for (int j = 0; j < 3; j++)
			{
				if ((*fi).V(j)->IsUserBit(selVertBit))
				{
					if (!areaUV.contains(QPointF((*fi).WT(j).u(), (*fi).WT(j).v()))) go = true;
					else {go = false; break;}
				}
			}	
			if (go) banList.push_back(&(*fi));
		}
	}
}

void RenderArea::handleUnifySelection(CMeshO::FaceIterator fi, int j)
{
	if (unifyRA == QRect())
	{ 
		unifyRA = QRect(QPoint(selStart.x() - VRADIUS, selStart.y() - VRADIUS), QPoint(selEnd.x() + VRADIUS, selEnd.y() + VRADIUS));
		unifyA = (*fi).V(j);
		firstface = &(*fi);
		uvertA = ToScreenSpace((*fi).WT(j).u(), (*fi).WT(j).v());
		tua = (*fi).WT(j).u(); tva = (*fi).WT(j).v();
		drawnPath.clear();
		drawnPath.push_back(Point2f(tua,tva));
	}
	else if (unifyRB == QRect()) 
	{ 
		unifyRB = QRect(QPoint(selStart.x() - VRADIUS, selStart.y() - VRADIUS), QPoint(selEnd.x() + VRADIUS, selEnd.y() + VRADIUS));
		unifyB = (*fi).V(j);
		uvertB = ToScreenSpace((*fi).WT(j).u(), (*fi).WT(j).v());
		tub = (*fi).WT(j).u(); tvb = (*fi).WT(j).v();
		path.clear();
		if (unifyA->IsB() && unifyB->IsB())
		{
			path = FindPath(unifyA, unifyB, firstface, 0);
			drawP = true;
		}
		this->update();
	}
	else if (unifyRA1 == QRect()) 
	{ 
		firstface1 = &(*fi);
		unifyRA1 = QRect(QPoint(selStart.x() - VRADIUS, selStart.y() - VRADIUS), QPoint(selEnd.x() + VRADIUS, selEnd.y() + VRADIUS)); 
		unifyA1 = (*fi).V(j);
		uvertA1 = ToScreenSpace((*fi).WT(j).u(), (*fi).WT(j).v());
		tua1 = (*fi).WT(j).u(); tva1 = (*fi).WT(j).v();
		drawnPath1.clear();
		drawnPath1.push_back(Point2f(tua1,tva1));
	}
	else if (unifyRB1 == QRect()) 
	{ 
		unifyRB1 = QRect(QPoint(selStart.x() - VRADIUS, selStart.y() - VRADIUS), QPoint(selEnd.x() + VRADIUS, selEnd.y() + VRADIUS)); 
		unifyB1 = (*fi).V(j);
		uvertB1 = ToScreenSpace((*fi).WT(j).u(), (*fi).WT(j).v());
		tub1 = (*fi).WT(j).u(); tvb1 = (*fi).WT(j).v();
		path1.clear();
		if (unifyA1->IsB() && unifyB1->IsB())
		{
			path1 = FindPath(unifyA1, unifyB1, firstface1, 1);
			drawP1 = true;
		}
		this->update();
	}
}

void RenderArea::SelectConnectedComponent(QPoint e)
{
	// Select a series of faces with the same UV coord on the edge	
	selStart = QPoint(MAX,MAX);
	selEnd = QPoint(-MAX,-MAX);
	selected = false;
	//selBit = CFaceO::NewBitFlag();
	for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearUserBit(selBit);
	unsigned index = 0;
	vector<CFaceO*> Q = vector<CFaceO*>();
	
	// Search the clicked face
	for(unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum)
		{
			QVector<QPoint> t = QVector<QPoint>(); 
			t.push_back(ToScreenSpace(model->cm.face[i].WT(0).u(), model->cm.face[i].WT(0).v()));
			t.push_back(ToScreenSpace(model->cm.face[i].WT(1).u(), model->cm.face[i].WT(1).v()));
			t.push_back(ToScreenSpace(model->cm.face[i].WT(2).u(), model->cm.face[i].WT(2).v()));
			QRegion r = QRegion(QPolygon(t));
			if (r.contains(e))
			{
				Q.push_back(&model->cm.face[i]);
				model->cm.face[i].SetUserBit(selBit);
				UpdateBoundingArea(r.boundingRect().topLeft(), r.boundingRect().bottomRight());
				selected = true;
				break;
			}
		}
	}
	// Select all the adjacentfaces
	while (index < Q.size())
	{
		for (int j = 0; j < 3; j++)
		{
			CFaceO* p = Q[index]->FFp(j);
			if (p != 0 && !p->IsUserBit(selBit))
			{
				p->SetUserBit(selBit);
				Q.push_back(p);
				QPoint tmp = ToScreenSpace(p->WT(j).u(), p->WT(j).v());
				UpdateBoundingArea(tmp, tmp);
			}
		}
		index++;
	}
}

// Clear every selected faces or vertexes
void RenderArea::ClearSelection()
{
	//selBit = CFaceO::NewBitFlag();
	for (unsigned i = 0; i < model->cm.face.size(); i++) 
	{ 
		model->cm.face[i].ClearUserBit(selBit);
		model->cm.face[i].ClearS();
	}
	selVertBit = CVertexO::NewBitFlag();
	selection = QRect();
	this->update();
	emit UpdateModel();
}

// Invert selected faces
void RenderArea::InvertSelection()
{
	if (selected)
	{
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			if (model->cm.face[i].WT(0).n() == textNum)
			{
				if (model->cm.face[i].IsUserBit(selBit)) model->cm.face[i].ClearUserBit(selBit);
				else model->cm.face[i].SetUserBit(selBit);
			}
		}
		RecalculateSelectionArea();
		originR.moveCenter(selection.center());
		origin = ToUVSpace(originR.center().x(), originR.center().y());
		this->update();
		ShowFaces();
	}
	else if (selectedV)
	{
		for (unsigned i = 0; i < model->cm.vert.size(); i++)
		{
			if (model->cm.vert[i].IsUserBit(selVertBit)) model->cm.vert[i].ClearUserBit(selVertBit);
			else model->cm.vert[i].SetUserBit(selVertBit);
		}
		UpdateVertexSelection();
		this->update();
	}
}

// Flip the selected faces (mode = true -> horizontal, mode = false -> vertical)
void RenderArea::Flip(bool mode)
{
	if (selected)
	{
		QPointF mid = ToUVSpace(selection.center().x(), selection.center().y());
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			if (model->cm.face[i].WT(0).n() == textNum)
			{
				if (model->cm.face[i].IsUserBit(selBit))
				{
					for (int j = 0; j < 3; j++)
					{
						if (mode) model->cm.face[i].WT(j).u() = 2.0f * mid.x() - model->cm.face[i].WT(j).u();
						else model->cm.face[i].WT(j).v() = 2.0f * mid.y() - model->cm.face[i].WT(j).v();
					}
				}		
			}
		}
		RecalculateSelectionArea();
		this->update();
	}
}

// Collapse a couple of vertexes
void RenderArea::UnifyCouple()
{
	// Calculate the average coordinates and unify a couple of vertexes
	if (VCount == 2)
	{
		float tu = (vc1.u() + vc2.u())/2.0;
		float tv = (vc1.v() + vc2.v())/2.0;

		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			for (unsigned j = 0; j < 3; j++)
			{
				if (!isInside(&model->cm.face[i]) && (model->cm.face[i].V(j) == collapse1 || model->cm.face[i].V(j) == collapse2))
				{
					model->cm.face[i].WT(j).u() = tu;
					model->cm.face[i].WT(j).v() = tv;
				}
			}
		}
		selectedV = false;
		selVertBit = CVertexO::NewBitFlag();
		areaUV = QRectF();
		selection = QRect();
		this->ChangeMode(SELECTMODE);
		this->update();
		emit UpdateModel();
	}
}

// Unify a set of border vertexes among the selected path
void RenderArea::UnifySet()
{
	// Unify a set of vertexes
	if (path.size() == path1.size() && drawP && drawP1)
	{
		for (unsigned i = 0; i < path.size(); i++)
		{
			float mu = (drawnPath[i].X() + drawnPath1[i].X())/2.0;
			float mv = (drawnPath[i].Y() + drawnPath1[i].Y())/2.0;
			int n = path[i]->VFi();
			CFaceO *next = path[i]->VFp();
			while (n != -1)
			{
				if (drawnPath[i].X() == next->WT(n).u() && drawnPath[i].Y() == next->WT(n).v())
				{
					next->WT(n).u() = mu;
					next->WT(n).v() = mv;
				}
				CFaceO* previus = next;
				next = next->VFp(n);
				n = previus->VFi(n);
				if (next == 0) break;
			}
			int n1 = path1[i]->VFi();
			CFaceO *next1 = path1[i]->VFp();
			while (n1 != -1)
			{
				if (drawnPath1[i].X() == next1->WT(n1).u() && drawnPath1[i].Y() == next1->WT(n1).v())
				{
					next1->WT(n1).u() = mu;
					next1->WT(n1).v() = mv;
				}
				CFaceO* previus1 = next1;
				next1 = next1->VFp(n1);
				n1 = previus1->VFi(n1);
				if (next1 == 0) break;
			}
		}
		UpdateUnifyTopology();
	}
	selectedV = false;
	for (unsigned i = 0; i < model->cm.vert.size(); i++) model->cm.vert[i].ClearUserBit(selVertBit);
	areaUV = QRectF();
	selection = QRect();
	unifyA = 0; unifyB = 0; unifyA1 = 0; unifyB1 = 0;
	firstface = 0; firstface1 = 0;
	drawP = false; drawP1 = false;
	uvertA = QPoint(); uvertA1 = QPoint(); uvertB = QPoint(); uvertB1 = QPoint();
	unifyRA = QRect(); unifyRA1 = QRect(); unifyRB = QRect(); unifyRB1 = QRect();
	this->update();
	emit UpdateModel();
}

vector<CVertexO*> RenderArea::FindPath(CVertexO* begin, CVertexO* end, CFaceO* first, int pathN)
{
	// Find a path from the selected vertex, walking from LEFT TO RIGHT only on border edge...
	vector<CVertexO*> path = vector<CVertexO*>();
	unsigned index = 0;
	vector<CFaceO*> Q = vector<CFaceO*>();
	Q.push_back(first);
	path.push_back(begin);
	first->SetV();
	bool finish = false;
	int notcontrol = -1;
	int lastadd = 0, lastex = -1;
	/* Add all the adjacent faces to the vertex.
	   For each face in Stack Q  
	     if the edge is a border one and contains the last added vertex and is on the right
		    enqueue all adjacent faces to the other vertex
			add the other vertex to the path
	*/
	float tu, tv;
	if (pathN == 0) {tu = tua; tv = tva;}
	else {tu = tua1; tv = tva1;}
	CFaceO* nextb, *previusb;
	int nb = begin->VFi();
	nextb = begin->VFp(), previusb = first;
	while (nb != -1)
	{
		if (tu == nextb->WT(nb).u() && tv == nextb->WT(nb).v() && nextb != first) Q.push_back(nextb);
		previusb = nextb;
		nextb = nextb->VFp(nb);
		nb = previusb->VFi(nb);
		if (nextb == 0) break;
	}
	bool verso;
	if (pathN == 0) 
	{
		if (tua < tub) verso = true;
		else verso = false;
	}
	else
	{
		if (tua1 < tub1) verso = true;
		else verso = false;
	}

	if (begin->IsB() && end->IsB())
	while (index < Q.size())
	{
		bool excluded = false;
		int oldsize = Q.size();
		CFaceO* p = Q[index];
		float oldu;
		if (pathN == 0) oldu = tua;
		else oldu = tua1;
		if (!p->IsV() || (p->IsV() && index == 0))
		{
			for (int j = 0; j < 3; j++)
			{
				if (p->IsB(j) && j != notcontrol)
				{
					notcontrol = j;
					CVertexO *tmp1 = p->V(j);
					CVertexO *tmp2 = p->V((j+1)%3);
					bool added = false;
					int n, oldn;
					CFaceO *next, *previus;
					float tu, tv;
					if (tmp1 == path[path.size()-1] && ((verso && p->WT(j).u() >= oldu) || (!verso && p->WT(j).u() <= oldu)))
					{
						oldu = p->WT(j).u();
						path.push_back(tmp2);
						if (pathN == 0) drawnPath.push_back(Point2f(p->WT((j+1)%3).u(),p->WT((j+1)%3).v()));
						else drawnPath1.push_back(Point2f(p->WT((j+1)%3).u(),p->WT((j+1)%3).v()));
						if (tmp2 == end) {finish = true; break;}
						n = tmp2->VFi(), oldn = (j+1)%3;
						next = tmp2->VFp(), previus = p;
						added = true;
						lastadd = index; lastex = j;
					}
					else if (tmp2 == path[path.size()-1] && ((verso && p->WT(j).u() >= oldu) || (!verso && p->WT(j).u() <= oldu)))
					{
						oldu = p->WT(j).u();
						path.push_back(tmp1);
						if (pathN == 0) drawnPath.push_back(Point2f(p->WT(j).u(),p->WT(j).v()));
						else drawnPath1.push_back(Point2f(p->WT(j).u(),p->WT(j).v()));
						if (tmp1 == end) {finish = true; break;}
						n = tmp1->VFi(), oldn = j;
						next = tmp1->VFp(), previus = p;
						added = true;
						lastadd = index; lastex = j;
					}
					if (added)
					{
						index = Q.size()-1;
						tu = p->WT(oldn).u(); tv = p->WT(oldn).v();
						while (n != -1)
						{
							if (tu == next->WT(n).u() && tv == next->WT(n).v() && next != p && !next->IsV()) 
								Q.push_back(next);
							previus = next;
							oldn = n;
							next = next->VFp(n);
							n = previus->VFi(n);
							if (next == 0) break;
						}
						break;
					}
					else excluded = true;
				}
			}
			p->SetV();
			if (!p->IsB(0) && !p->IsB(1) && !p->IsB(2) && index != 0) excluded = true;
		}
		if (finish) break;
		// casi degeneri...
		if (oldsize == Q.size() && !excluded)
		{
			if (index == Q.size()-1) {index = lastadd; p->ClearV();}	// Force to search again the same face...
		}
		else notcontrol = -1;
		index++;
		if (index == Q.size() && end != path[path.size()-1]) 
		{
			index = lastadd; 
			notcontrol = lastex;
			Q[index]->ClearV();
		}
	}

	for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearV();
	return path;
}

void RenderArea::HandleScale(QPoint e)
{
	// Move the rectangle of scaling and resize the selction area
	int tx = rectX - tpanX + e.x();
	int ty = rectY - tpanY + e.y();
	switch(highlighted)
	{
		case 0:	// Top Left
			if (tx > selection.x() + selection.width() - RECTDIM) tx = selection.x() + selection.width() - RECTDIM;
			if (ty > selection.y() + selection.height() - RECTDIM) ty = selection.y() + selection.height() - RECTDIM;
			selRect[0].moveCenter(QPoint(tx, ty));
			selRect[2] = QRect(tx - RECTDIM/2, selRect[2].y(), RECTDIM, RECTDIM);
			selRect[1] = QRect(selRect[1].x(), ty - RECTDIM/2, RECTDIM, RECTDIM);
			selection.setTopLeft(selRect[0].center());
			selection.setBottomLeft(selRect[2].center());
			selection.setTopRight(selRect[1].center());
			this->update(selRect[1]);
			break;
		case 1: // Top Right
			if (tx < selection.x() + RECTDIM) tx = selection.x() +  RECTDIM;
			if (ty > selection.y() + selection.height() - RECTDIM) ty = selection.y() + selection.height() - RECTDIM;
			selRect[1].moveCenter(QPoint(tx, ty));
			selRect[3] = QRect(tx - RECTDIM/2, selRect[3].y(), RECTDIM, RECTDIM);
			selRect[0] = QRect(selRect[0].x(), ty - RECTDIM/2, RECTDIM, RECTDIM);
			selection.setTopRight(selRect[1].center());
			selection.setTopLeft(selRect[0].center());
			selection.setBottomRight(selRect[3].center());
			this->update(selRect[0]);
			break;
		case 2:	// Bottom Left
			if (tx > selection.x() + selection.width() - RECTDIM) tx = selection.x() + selection.width() - RECTDIM;
			if (ty < selection.y() + RECTDIM) ty = selection.y() + RECTDIM;
			selRect[2].moveCenter(QPoint(tx, ty));
			selRect[0] = QRect(tx - RECTDIM/2, selRect[0].y(), RECTDIM, RECTDIM);
			selRect[3] = QRect(selRect[3].x(), ty - RECTDIM/2, RECTDIM, RECTDIM);
			selection.setTopLeft(selRect[0].center());
			selection.setBottomLeft(selRect[2].center());
			selection.setBottomRight(selRect[3].center());
			this->update(selRect[3]);
			break;
		case 3:	// Bottom Right
			if (tx < selection.x() + RECTDIM) tx = selection.x() + RECTDIM;
			if (ty < selection.y() + RECTDIM) ty = selection.y() + RECTDIM;
			selRect[3].moveCenter(QPoint(tx, ty));
			selRect[1] = QRect(tx - RECTDIM/2, selRect[1].y(), RECTDIM, RECTDIM);
			selRect[2] = QRect(selRect[2].x(), ty - RECTDIM/2, RECTDIM, RECTDIM);
			selection.setTopRight(selRect[1].center());
			selection.setBottomLeft(selRect[2].center());
			selection.setBottomRight(selRect[3].center());
			this->update(selRect[2]);
			break;
	}
	this->update(selRect[highlighted]);
	this->update(selRect[(highlighted+2)%selRect.size()]);
	originR.moveCenter(selection.center());
	origin = ToUVSpace(originR.center().x(), originR.center().y());
	// calculate scaling
	scaleX = (float)selection.width() / oldSRX;
	scaleY = (float)selection.height() / oldSRY;

	this->update(selection);
}

void RenderArea::HandleRotate(QPoint e)
{
	// Calculate the angle of rotazion
	float A2 = (e.x() - originR.center().x())*(e.x() - originR.center().x()) + (e.y() - originR.center().y())*(e.y() - originR.center().y());
	float C2 = (rectX - e.x())*(rectX - e.x()) + (rectY - e.y()) * (rectY -e.y());
	degree = acos((C2 - A2 - B2) / (-2*sqrt(A2)*sqrt(B2)));
	float ny = (float) Rm * e.x() + Rq;
	switch(highlighted)
	{
	case 0:
		if (ny > e.y()) degree = -degree;
		break;
	case 1:
		if (ny < e.y()) 
			degree = -degree;
		break;
	case 2:
		if (ny > e.y()) degree = -degree;
		break;
	case 3:
		if (ny < e.y()) degree = -degree;
		break;
	}
	this->update();
}

// Find the new size of the selection rectangle after an edit
void RenderArea::RecalculateSelectionArea()
{
	selStart = QPoint(MAX,MAX);
	selEnd = QPoint(-MAX,-MAX);
	CMeshO::FaceIterator fi;
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).IsUserBit(selBit) && !(*fi).IsD())
		{
			QPoint a = ToScreenSpace((*fi).WT(0).u(), (*fi).WT(0).v());
			QPoint b = ToScreenSpace((*fi).WT(1).u(), (*fi).WT(1).v());
			QPoint c = ToScreenSpace((*fi).WT(2).u(), (*fi).WT(2).v());
			// >> There's a BUG IN QREGION: I can't create a region if points are too near!!!! <<
			SetUpRegion(a, b, c);
		}
	}
	if (selected && selStart.x() < selEnd.x() && selStart.y() < selEnd.y())
	{
		selection = QRect(selStart, selEnd);
		UpdateSelectionArea(0,0);
	}
}

void RenderArea::SetUpRegion(QPoint a, QPoint b, QPoint c)
{
	// Avoid a bug in Qt by calculating the region manually
	if (a.x() < selStart.x()) selStart.setX(a.x());
	if (b.x() < selStart.x()) selStart.setX(b.x());
	if (c.x() < selStart.x()) selStart.setX(c.x());
	if (a.y() < selStart.y()) selStart.setY(a.y());
	if (b.y() < selStart.y()) selStart.setY(b.y());
	if (c.y() < selStart.y()) selStart.setY(c.y());
	if (a.x() > selEnd.x()) selEnd.setX(a.x());
	if (b.x() > selEnd.x()) selEnd.setX(b.x());
	if (c.x() > selEnd.x()) selEnd.setX(c.x());
	if (a.y() > selEnd.y()) selEnd.setY(a.y());
	if (b.y() > selEnd.y()) selEnd.setY(b.y());
	if (c.y() > selEnd.y()) selEnd.setY(c.y());
}

void RenderArea::UpdateSelectionArea(int x, int y)
{
	// Update the buttons of the selection area
	selection.moveCenter(QPoint(selection.center().x() + x, selection.center().y() + y));
	selRect[0].moveCenter(selection.topLeft());
	selRect[1].moveCenter(selection.topRight());
	selRect[2].moveCenter(selection.bottomLeft());
	selRect[3].moveCenter(selection.bottomRight());
}

void RenderArea::UpdateSelectionAreaV(int x, int y)
{
	// Update the buttons of the selection area for vertexes
	selection.moveCenter(QPoint(selection.center().x() + x, selection.center().y() + y));
	selRect[0].moveBottomRight(selection.topLeft());
	selRect[1].moveBottomLeft(selection.topRight());
	selRect[2].moveTopRight(selection.bottomLeft());
	selRect[3].moveTopLeft(selection.bottomRight());
}

void RenderArea::UpdateVertexSelection()
{
	// Recalcultate the rectangle for selection of vertexes
	selStart = QPoint(MAX,MAX);
	selEnd = QPoint(-MAX,-MAX);
	selectedV = false;
	selection = QRect();
	CMeshO::FaceIterator fi;
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).WT(0).n() == textNum && !(*fi).IsD())
		{
			for (int j = 0; j < 3; j++)
			{
				QPoint tmp = ToScreenSpace((*fi).WT(j).u(), (*fi).WT(j).v());
				if ((*fi).V(j)->IsUserBit(selVertBit) && areaUV.contains(QPointF((*fi).WT(j).u(), (*fi).WT(j).v())))
				{
					UpdateBoundingArea(tmp,tmp);
					if (!selectedV) selectedV = true;
				}
			}
		}
	}
	selection = QRect(QPoint(selStart.x() - VRADIUS, selStart.y() - VRADIUS), QPoint(selEnd.x() + VRADIUS, selEnd.y() + VRADIUS));
	QPointF a = ToUVSpace(selection.x(), selection.y());
	QPointF b = ToUVSpace(selection.bottomRight().x(),selection.bottomRight().y());
	areaUV = QRectF(a, QSizeF(b.x()-a.x(), b.y()-a.y()));
	UpdateSelectionAreaV(0,0);
}

void RenderArea::UpdateUnify()
{
	// Update the Unify infrastructure after a zoom
	if (unifyRA != QRect())
	{
		unifyRA.moveCenter(ToScreenSpace(tua, tva));
		uvertA = unifyRA.topLeft();
	}
	if (unifyRA1 != QRect())
	{
		unifyRA1.moveCenter(ToScreenSpace(tua1, tva1));
		uvertA1 = unifyRA1.center();
	}
	if (unifyRB != QRect())
	{
		unifyRB.moveCenter(ToScreenSpace(tub, tvb));
		uvertB = unifyRB.center();
	}
	if (unifyRB1 != QRect())
	{
		unifyRB1.moveCenter(ToScreenSpace(tub1, tvb1));
		uvertB1 = unifyRB1.center();
	}
}

QPointF RenderArea::ToUVSpace(int x, int y)
{
	// Convert a point from screen-space to uv-space
	return QPointF((float)(x - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - y + viewport.Y()*zoom)/(AREADIM*zoom));
}

QPoint RenderArea::ToScreenSpace(float u, float v)
{
	// Convert a point from uv-space to screen space
	return QPoint(u * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - (v * AREADIM*zoom) + viewport.Y()*zoom);
}

void RenderArea::DrawCircle(QPoint origin)
{
	// Draw a circle in the RenderArea using openGL
	float DEG2RAD = 3.14159f/180.0f;
	float r = (float)VRADIUS/zoom;
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 360; i++)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(origin.x() + cos(degInRad)*r,origin.y() + sin(degInRad)*r,2.0f);
	}
	glEnd();
}

void RenderArea::UpdateBoundingArea(QPoint topLeft, QPoint topRight)
{
	// Update the bounding box (selection rectangle) of the selected faces/vertexes
	if (topLeft.x() < selStart.x()) selStart.setX(topLeft.x());
	if (topLeft.y() < selStart.y()) selStart.setY(topLeft.y());
	if (topRight.x() > selEnd.x()) selEnd.setX(topRight.x());
	if (topRight.y() > selEnd.y()) selEnd.setY(topRight.y());
}

// Import the face selected from the meshlab GLArea in the pluging
void RenderArea::ImportSelection()
{
	for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearUserBit(selBit);
	selStart = QPoint(MAX,MAX);
	selEnd = QPoint(-MAX,-MAX);
	CMeshO::FaceIterator fi;
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).IsS() && !(*fi).IsD())
		{
			if (!selected) selected = true;
			(*fi).SetUserBit(selBit);
			QPoint a = ToScreenSpace((*fi).WT(0).u(), (*fi).WT(0).v());
			QPoint b = ToScreenSpace((*fi).WT(1).u(), (*fi).WT(1).v());
			QPoint c = ToScreenSpace((*fi).WT(2).u(), (*fi).WT(2).v());
			SetUpRegion(a, b, c);
		}
	}
	if (selected)
	{
		selection = QRect(selStart, selEnd);
		UpdateSelectionArea(0,0);
		originR.moveCenter(ToScreenSpace(origin.x(), origin.y()));
		origin = ToUVSpace(originR.center().x(), originR.center().y());
	}
	ChangeMode(EDITFACEMODE);
	this->update();
}

void RenderArea::CountVertexes()
{
	// Count the number of selected UV vertexes (not so easy...)
	VCount = 0;
	collapse1 = 0;
	collapse2 = 0;
	CMeshO::FaceIterator fi;
	vector< TexCoord2<float> > tmpCoord = vector< TexCoord2<float> >();
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).WT(0).n() == textNum)
		{
			for (int j = 0; j < 3; j++)
			{
				if ((*fi).V(j)->IsUserBit(selVertBit))
				{
					if (!isInside(tmpCoord, (*fi).WT(j)) && areaUV.contains(QPointF((*fi).WT(j).u(), (*fi).WT(j).v())))
					{
						VCount++;
						if (collapse1 == 0) {collapse1 = (*fi).V(j); vc1 = (*fi).WT(j); }
						else if (collapse2 == 0) {collapse2 = (*fi).V(j); vc2 = (*fi).WT(j); }
						tmpCoord.push_back((*fi).WT(j));
					}
				}
			}
		}
	}
}

void RenderArea::UpdateUnifyTopology()
{
	// Update the topology needed for unify of edge
	vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(model->cm);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(model->cm);
	model->clearDataMask(MeshModel::MM_FACEFLAGBORDER);
	vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(model->cm);
	vcg::tri::UpdateFlags<CMeshO>::VertexBorderFromFace(model->cm);
}

bool RenderArea::isInside(vector<TexCoord2<float> > tmpCoord, TexCoord2<float> act)
{
	// Support function for search in a vector
	for (unsigned i = 0; i < tmpCoord.size(); i++)
	{
		if (tmpCoord[i] == act) return true;
	}
	return false;
}

bool RenderArea::isInside(CFaceO* face)
{
	for (int h = 0; h < banList.size(); h++)
	{
		if (face == banList[h]) return true;
	}
	return false;
}

void RenderArea::ShowFaces()
{
	// Set up model for display the selected faces
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].IsUserBit(selBit)) model->cm.face[i].SetS();
		else model->cm.face[i].ClearS();
	}
	emit UpdateModel();
}

// Reset the position of the viewport
void RenderArea::ResetPosition()
{
	zoom = 1;
	ResetTrack(true);
	if (selected) RecalculateSelectionArea();
	else if (selectedV) UpdateVertexSelection();
	this->update();
}
