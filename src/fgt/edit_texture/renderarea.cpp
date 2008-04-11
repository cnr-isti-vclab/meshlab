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

static bool updateTopology = false;	// To update the topology only one time

RenderArea::RenderArea(QWidget *parent, QString textureName, MeshModel *m, unsigned tnum) : QGLWidget(parent)
{
    antialiased = true;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
	image = QImage();
	if(textureName != QString())
	{
		if (QFile(textureName).exists()) image = QImage(textureName);
		else textureName = QString();
	}
	textNum = tnum;
	model = m;
	
	// Init
	oldX = 0; oldY = 0;
	viewport = vcg::Point2f(0,0);
	tmpX = 0; tmpY = 0;
	tb = new Trackball();
	tb->center = Point3f(0, 0, 0);
	tb->radius = 1;
	panX = 0; panY = 0;
	oldPX = 0; oldPY = 0;
	posVX = 0; posVY = 0;

	brush = QBrush(Qt::green);
	mode = View;
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

	zoom = 1;

	rot = QImage(QString(":/images/rotate.png"));
	scal = QImage(QString(":/images/scale.png"));

	this->setMouseTracking(true);
	this->setCursor(Qt::PointingHandCursor);
	this->setAttribute(Qt::WA_NoSystemBackground);
}

RenderArea::~RenderArea()
{
	CFaceO::DeleteBitFlag(selBit);
	CVertexO::DeleteBitFlag(selVertBit);
}

void RenderArea::setPen(const QPen &pen)
{
    this->pen = pen;
    update();
}

void RenderArea::setBrush(const QBrush &brush)
{
    this->brush = brush;
    update();
}

void RenderArea::setAntialiased(bool antialiased)
{
    this->antialiased = antialiased;
    update();
}

void RenderArea::setTexture(QString path)
{
	image = QImage(path);
	fileName = path;
}

void RenderArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(QPen(brush,2));
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, antialiased);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.begin(painter.device());	// Initialize a GL context
	
	tb->GetView();
	tb->Apply(true);

	maxX = 0; maxY = 0; minX = 0; minY = 0;
	if (image != QImage())
	{
	    glEnable(GL_COLOR_LOGIC_OP);
		glEnable(GL_DEPTH_TEST);
		glLineWidth(2);
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			glLogicOp(GL_XOR);
			glColor3f(1,1,1);
			// First draw the model in u,v space
			if (model->cm.face[i].IsS() && model->cm.face[i].WT(0).n() == textNum)
			{
				// While drawning, it counts the number of 'planes'
				if (model->cm.face[i].WT(0).u() > maxX || model->cm.face[i].WT(1).u() > maxX || model->cm.face[i].WT(2).u() > maxX)	maxX++;
				if (model->cm.face[i].WT(0).v() > maxY || model->cm.face[i].WT(1).v() > maxY || model->cm.face[i].WT(2).v() > maxY)	maxY++;
				if (model->cm.face[i].WT(0).u() < minX || model->cm.face[i].WT(1).u() < minX || model->cm.face[i].WT(2).u() < minX)	minX--;
				if (model->cm.face[i].WT(0).v() < minY || model->cm.face[i].WT(1).v() < minY || model->cm.face[i].WT(2).v() < minY)	minY--;

				// Draw the edge of faces
				glBegin(GL_LINE_LOOP);
				for (int j = 0; j < 3; j++)
				{
					if (mode != EditVert)
					{
						if (selected && !model->cm.face[i].IsUserBit(selBit))
							glVertex3f(model->cm.face[i].WT(j).u() * AREADIM , AREADIM - (model->cm.face[i].WT(j).v() * AREADIM), 1);	
						else if (editMode == Scale) glVertex3f((oScale.x() + (model->cm.face[i].WT(j).u() - oScale.x()) * scaleX) * AREADIM - panX/zoom, 
									AREADIM - ((oScale.y() + (model->cm.face[i].WT(j).v() - oScale.y()) * scaleY) * AREADIM) - panY/zoom, 1);							
							 else glVertex3f((origin.x() + (cos(degree) * (model->cm.face[i].WT(j).u() - origin.x()) - sin(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM - panX/zoom,
									AREADIM - ((origin.y() + (sin(degree) * (model->cm.face[i].WT(j).u() - origin.x()) + cos(degree) * (model->cm.face[i].WT(j).v() - origin.y()))) * AREADIM) - panY/zoom, 1);
					}
					else
					{
						if (areaUV.contains(QPointF(model->cm.face[i].WT(j).u(), model->cm.face[i].WT(j).v())) && model->cm.face[i].V(j)->IsUserBit(selVertBit))
							glVertex3f(model->cm.face[i].WT(j).u() * AREADIM - posVX/zoom, AREADIM - (model->cm.face[i].WT(j).v() * AREADIM + posVY/zoom), 1);
						else glVertex3f(model->cm.face[i].WT(j).u() * AREADIM , AREADIM - (model->cm.face[i].WT(j).v() * AREADIM), 1);
					}
				}
				glEnd();

				// Draw the selected faces
				if (selected && model->cm.face[i].IsUserBit(selBit))
				{
					glLogicOp(GL_AND);
					glColor3f(1,0,0);
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

				// Draw the selected vertex
				if (selectedV)
				{
					glDisable(GL_COLOR_LOGIC_OP);
					for (int j = 0; j < 3; j++)
					{
						if(areaUV.contains(QPointF(model->cm.face[i].WT(j).u(), model->cm.face[i].WT(j).v())) && model->cm.face[i].V(j)->IsUserBit(selVertBit))
						{
							glColor3f(1,0,0);
							DrawCircle(QPoint(model->cm.face[i].WT(j).u() * AREADIM - posVX/zoom, AREADIM - model->cm.face[i].WT(j).v() * AREADIM - posVY/zoom));
							glColor3f(0,0,0);
						}
					}
					glEnable(GL_COLOR_LOGIC_OP);
				}
			}
		}
		glDisable(GL_LOGIC_OP);
		glDisable(GL_COLOR_LOGIC_OP);

		// Draw the background behind the model
		if (minX != 0 || minY != 0 || maxX != 0 || maxY != 0)
		{
			for (int x = minX; x < maxX; x++) 
				for (int y = minY; y < maxY; y++)
					painter.drawImage(QRect(x*AREADIM,-y*AREADIM,AREADIM,AREADIM),image,QRect(0,0,image.width(),image.height()));
		}
		else painter.drawImage(QRect(0,0,AREADIM,AREADIM),image,QRect(0,0,image.width(),image.height()));

		// and the axis, always in first plane
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
		// Line and text (native Qt)
		int w = this->visibleRegion().boundingRect().width();
		int h = this->visibleRegion().boundingRect().height();
		painter.drawLine(0,h,w,h);
		painter.drawLine(0,h,0,0);
		// Calculate the coords and draw it
		float ox = (float)-viewport.X()*zoom/(AREADIM*zoom), oy = (float)(AREADIM*zoom - h + viewport.Y()*zoom)/(AREADIM*zoom);
		/*O:*/painter.drawText(TRANSLATE, h - TRANSLATE, QString("(%1,%2)").arg(ox).arg(oy));
		/*Y:*/painter.drawText(TRANSLATE, TRANSLATE*3, QString("(%1,%2)").arg(ox).arg(oy + 1.0f/zoom));
		/*X:*/painter.drawText(w - TRANSLATE*18, h - TRANSLATE, QString("(%1,%2)").arg((float)(w-viewport.X()*zoom)/(AREADIM*zoom)).arg(oy));
		painter.drawText(TRANSLATE, TRANSLATE*6, QString("V"));
		painter.drawText(AREADIM - TRANSLATE*23, AREADIM - TRANSLATE, QString("U"));

		// Draw the rectangle of selection
		if (start != QPoint() && end != QPoint())
		{
			painter.setPen(QPen(QBrush(Qt::gray),1));
			painter.setBrush(QBrush(Qt::NoBrush));
			painter.drawRect(area);
		}

		if (selection != QRect() && (mode == Edit || mode == EditVert))
		{
			// The rectangle of editing
			painter.setPen(QPen(QBrush(Qt::yellow),2));
			painter.setBrush(QBrush(Qt::NoBrush));
			if (mode == Edit)
			{
				painter.drawRect(QRect(selection.x() - posX, selection.y() - posY, selection.width(), selection.height()));
				// Rectangle on the corner
				painter.setPen(QPen(QBrush(Qt::black),1));
				for (unsigned l = 0; l < selRect.size(); l++)
				{
					if (l == highlighted) painter.setBrush(QBrush(Qt::yellow));
					else painter.setBrush(QBrush(Qt::NoBrush));
					painter.drawRect(selRect[l]);
					if (editMode == Scale) painter.drawImage(selRect[l],scal,QRect(0,0,scal.width(),scal.height()));
					else painter.drawImage(selRect[l],rot,QRect(0,0,rot.width(),rot.height()));
				}
	
				// and the origin of the rotation
				if (editMode == Rotate)
				{
					painter.setPen(QPen(QBrush(Qt::black),1));
					if (highlighted == ORIGINRECT) painter.setBrush(QBrush(Qt::blue));
					else painter.setBrush(QBrush(Qt::yellow));
					painter.drawEllipse(QRect(originR.x() - posX - orX, originR.y() - posY - orY, RADIUS, RADIUS));
				}
			}
			else 
			{
				painter.drawRect(QRect(selection.x() - posVX, selection.y() - posVY, selection.width(), selection.height()));
				painter.fillRect(QRect(selection.x() - HRECT/2 - posVX, selection.y() - HRECT/2 - posVY, HRECT, HRECT), QBrush(Qt::black));
				painter.fillRect(QRect(selection.x() + selection.width() - HRECT/2 - posVX, selection.y() - HRECT/2 - posVY, HRECT, HRECT), QBrush(Qt::black));
				painter.fillRect(QRect(selection.x() - HRECT/2 - posVX, selection.y() + selection.height() - HRECT/2 - posVY, HRECT, HRECT), QBrush(Qt::black));
				painter.fillRect(QRect(selection.x() + selection.width() - HRECT/2 - posVX, selection.y() + selection.height() - HRECT/2 - posVY, HRECT, HRECT), QBrush(Qt::black));
				painter.fillRect(QRect(selection.x() - HRECT/2 - posVX + selection.width()/2, selection.y() - HRECT/2 - posVY, HRECT, HRECT), QBrush(Qt::black));
				painter.fillRect(QRect(selection.x() - HRECT/2 - posVX, selection.y() - HRECT/2 - posVY + selection.height()/2, HRECT, HRECT), QBrush(Qt::black));
				painter.fillRect(QRect(selection.x() - HRECT/2 - posVX + selection.width(), selection.y() + selection.height()/2 - HRECT/2 - posVY, HRECT, HRECT), QBrush(Qt::black));
				painter.fillRect(QRect(selection.x() + selection.width()/2 - HRECT/2 - posVX, selection.y() + selection.height() - HRECT/2 - posVY, HRECT, HRECT), QBrush(Qt::black));			
			}
		}

		glDisable(GL_LOGIC_OP);
	  	glPopAttrib();
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	else painter.drawText(TEXTX, TEXTY, tr("NO TEXTURE"));

    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);

	painter.end();
}

// Mouse Event:
void RenderArea::mousePressEvent(QMouseEvent *e)
{
	if ((mode == Edit || mode == EditVert) && highlighted == NOSEL) 
	{
		this->ChangeMode(3);
		pressed = NOSEL;
		selected = false;
		selectedV = false;
		selVertBit = CVertexO::NewBitFlag();
	}
	switch(mode)
	{
		case View:
			oldX = e->x(); oldY = e->y();
			tmpX = viewport.X(); tmpY = viewport.Y();
			tb->MouseDown(e->x(), AREADIM-e->y(), QT2VCG(e->button(), e->modifiers()));
			this->update();
			break;
		case Edit:
			tpanX = e->x();
			tpanY = e->y();
			pressed = highlighted;
			if (highlighted > NOSEL && highlighted < selRect.size() && mode == Edit)
			{
				rectX = selRect[highlighted].center().x();
				rectY = selRect[highlighted].center().y();
				oldSRX = selection.width();
				oldSRY = selection.height();
				scaleX = 1; scaleY = 1;
				int perno;
				if (highlighted == 0) perno = 3;
				else if (highlighted == 1) perno = 2;
				else if (highlighted == 2) perno = 1;
				else perno = 0;
				oScale = QPointF((float)(selRect[perno].center().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - selRect[perno].center().y() + viewport.Y()*zoom)/(AREADIM*zoom));
				B2 = (float)(rectX - originR.center().x())*(rectX - originR.center().x()) + (rectY - originR.center().y())*(rectY - originR.center().y());
				Rm = (float)(rectY - originR.center().y()) / (rectX - originR.center().x());
				Rq = (float) rectY - Rm * rectX;
			}
		case EditVert:
			tpanX = e->x();
			tpanY = e->y();
			pressed = highlighted;
			break;
		case Select:
			start = e->pos();
			end = e->pos();
			break;
	}
}

void RenderArea::mouseReleaseEvent(QMouseEvent *e)
{
	switch(mode)
	{
		case View:
			if (selection != QRect()) 
			{
				UpdateSelectionArea((viewport.X() - initVX)*zoom, (viewport.Y() - initVY)*zoom);
				originR.moveCenter(QPoint(originR.x() + viewport.X() - initVX, originR.y() + viewport.Y() - initVY));
			}
			initVX = viewport.X(); initVY = viewport.Y();
			break;
		case Edit:
			oldPX = panX;
			oldPY = panY;
			if (pressed == ORIGINRECT)	// Drag origin -> Update the position of the rectangle of rotation and the real point in UV
			{
				originR = QRect(originR.x() - posX - orX, originR.y() - posY - orY, RADIUS, RADIUS);
				origin = QPointF((float)(originR.center().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)((AREADIM*zoom) - originR.center().y() + viewport.Y()*zoom)/(AREADIM*zoom));
				orX = 0; orY = 0;
			}
			else if (pressed == SELECTIONRECT && posX != 0)	// Drag selection -> Update the position of the selection area and the rotatation rect
			{
				selection = QRect(selection.x() - posX, selection.y() - posY, selection.width(), selection.height());
				originR.moveCenter(QPoint(originR.center().x() - posX, originR.center().y() - posY));
				origin = QPointF((float)(originR.center().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - originR.center().y() + viewport.Y()*zoom)/(AREADIM*zoom));
				posX = 0; posY = 0;
				if (selected) UpdateUV();
			}
			else if (pressed > NOSEL && pressed < selRect.size())
			{
				if (editMode == Rotate)
				{
					RotateComponent(degree);
					RecalculateSelectionArea();
					degree = 0;
				}
				else if (scaleX != 1 && scaleY != 1) 
				{
					ScaleComponent(scaleX, scaleY);
					RecalculateSelectionArea();
					scaleX = 1; 
					scaleY = 1;
					oScale = QPointF(0,0);
				}
			}
			break;
		case EditVert:
			selection = QRect(selection.x() - posVX, selection.y() - posVY, selection.width(), selection.height());
			if (selectedV) UpdateVertex();
			break;
		case Select:
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
						origin = QPointF((float)(selection.center().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - selection.center().y() + viewport.Y()*zoom)/(AREADIM*zoom));
						originR = QRect(selection.center().x()-RADIUS/2, selection.center().y()-RADIUS/2, RADIUS, RADIUS);
						this->ChangeMode(1);
						this->update(selection);
					}
					break;
				case Connected:
					SelectConnectedComponent(e->pos());
					if (selected)
					{
						selection = QRect(selStart, selEnd);
						UpdateSelectionArea(0,0);
						origin = QPointF((float)(selection.center().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - selection.center().y() + viewport.Y()*zoom)/(AREADIM*zoom));
						originR = QRect(selection.center().x()-RADIUS/2, selection.center().y()-RADIUS/2, RADIUS, RADIUS);
						this->ChangeMode(1);
						this->update();
					}
					break;
				case Vertex:
					if (selectedV)
					{
						selection = QRect(QPoint(selStart.x() - RADIUS/2, selStart.y() - RADIUS/2), QPoint(selEnd.x() + RADIUS/2, selEnd.y() + RADIUS/2));
						this->ChangeMode(4);
						this->update(selection);
					}
					else this->update();
					break;
			}
			break;
	}
}

void RenderArea::mouseMoveEvent(QMouseEvent *e)
{
	if((e->buttons() & Qt::LeftButton))
	{
		int sx = (e->x() - oldX)/zoom, sy = (e->y() - oldY)/zoom;
		switch(mode)
		{
			case View:
				tb->Translate(Point3f(- oldX + e->x(), - oldY + e->y(), zoom));
				viewport = Point2f(tmpX + sx, tmpY + sy);
				this->update();
				break;
			case Edit:
				if (pressed == SELECTIONRECT)
				{
					// Move the selection ara
					panX = oldPX + tpanX - e->x();
					panY = oldPY + tpanY - e->y();
					posX = tpanX - e->x();
					posY = tpanY - e->y();
					if (posX != 0)
					{
						selRect[0].moveCenter(QPoint(selection.topLeft().x() - posX, selection.topLeft().y() - posY));
						selRect[1].moveCenter(QPoint(selection.topRight().x() - posX, selection.topRight().y() - posY));
						selRect[2].moveCenter(QPoint(selection.bottomLeft().x() - posX, selection.bottomLeft().y() - posY));
						selRect[3].moveCenter(QPoint(selection.bottomRight().x() - posX, selection.bottomRight().y() - posY));
					}
					this->update();
				}
				else if (pressed == ORIGINRECT)
				{
					// Move the origin rect inside the selection area
					orX = tpanX - e->x();
					/*int tx = originR.center().x() - orX;
					if (tx < selection.x()) orX = - selection.x() + originR.center().x();
					else if (tx > selection.x() + selection.width()) orX = originR.center().x() - (selection.x() + selection.width());
					*/
					orY = tpanY - e->y();
					/*int ty = originR.y() - posY - orY;
					if (ty < selection.y()) orY = - selection.y() + originR.center().y();
					else if (ty > selection.y() + selection.height()) orY = originR.center().y() - (selection.y() + selection.height());
					*/
					this->update(originR);
				}
				else if (pressed > NOSEL && pressed < selRect.size())  
				{
					if (editMode == Scale) HandleScale(e->pos());
					else HandleRotate(e->pos());
				}
				break;
			case EditVert:
				if (pressed == SELECTIONRECT)
				{
					// Move the selection ara
					posVX = tpanX - e->x();
					posVY = tpanY - e->y();
				}
				this->update();
				break;
			case Select:
				if (selectMode != Connected)
				{
					end = e->pos();
					int x1, x2, y1, y2;
					if (start.x() < end.x()) {x1 = start.x(); x2 = end.x();} else {x1 = end.x(); x2 = start.x();}
					if (start.y() < end.y()) {y1 = start.y(); y2 = end.y();} else {y1 = end.y(); y2 = start.y();}
					area = QRect(x1,y1,x2-x1,y2-y1);
					if (selectMode == Area) SelectFaces();
					else SelectVertexes();
					this->update(area);
				}
				break;
		}
	}
	else 
	{
		if (mode == Edit || mode == EditVert)
		{
			for (unsigned y = 0; y < selRect.size(); y++)
			{
				if (selRect[y].contains(e->pos()) && mode == Edit)
				{
					if (highlighted != y) this->update(selRect[y]);
					highlighted = y;
					return;
				}
			}
			if (originR.contains(e->pos()) && editMode == Rotate)
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

void RenderArea::mouseDoubleClickEvent(QMouseEvent *e)
{
	switch(mode)
	{
		case View:	// Reset trackball position
			zoom = 1;
			ResetTrack(true);
			if (selected) RecalculateSelectionArea();
			this->update();
			break;
		case Edit:	// Change edit mode
			if (selection.contains(e->pos()))
			{
				if (editMode == Rotate) editMode = Scale;
				else editMode = Rotate;
				this->update(originR);
			}
			break;
	}
}

void RenderArea::wheelEvent(QWheelEvent*e)
{
	// Handle the zoom
	float oldZoom = zoom;
	if (e->delta() > 0) zoom += 0.25;
	else if (zoom != 0.25f) zoom -= 0.25;
	// Change the viewport, putting the center of the screen on the mouseposition
	if (zoom != oldZoom)
	{
		viewport = Point2f(viewport.X() - e->x()/oldZoom + (this->visibleRegion().boundingRect().width()/zoom)/2,
			viewport.Y() - e->y()/oldZoom + (this->visibleRegion().boundingRect().height()/zoom)/2);
		ResetTrack(false);
		tb->Scale(zoom);
		if (selectedV) UpdateVertexSelection();
		else 
		{
			RecalculateSelectionArea();
			UpdateSelectionArea(0,0);
			originR.moveCenter(QPoint(origin.x() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - (origin.y() * AREADIM*zoom) + viewport.Y()*zoom));
			origin = QPointF((float)(originR.center().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - originR.center().y() + viewport.Y()*zoom)/(AREADIM*zoom));
		}
		this->update();
	}
}

void RenderArea::RemapClamp()
{
	// Remap the uv coord out of border using clamp method
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum)
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
	this->update();
	emit UpdateModel();
}

void RenderArea::RemapMod()
{
	// Remap the uv coord out of border using mod function
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum)
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
	this->update();
	emit UpdateModel();
}

void RenderArea::ChangeMode(int index)
{
	// Change the selection mode
	switch(index)
	{
		case 0:
			if (mode != View)
			{
				mode = View;
				this->setCursor(Qt::PointingHandCursor);
			}
			break;
		case 1:
			if (mode != Edit)
			{
				mode = Edit;
				this->setCursor(QCursor(QBitmap(":/images/sel_move.png")));
			}
			break;
		case 2:
			if (mode != Select)
			{
				if (selection != QRect())
				{
					if (selectMode == Vertex) mode = EditVert;
					else mode = Edit; 
					selected = true;
					this->setCursor(QCursor(QBitmap(":/images/sel_move.png")));
				}
				else
				{
					mode = Select;
					this->setCursor(Qt::CrossCursor);
				}
			}
			break;
		case 3:	// For internal use... reset the selction
			mode = Select;
			for (unsigned i = 0; i < model->cm.face.size(); i++) model->cm.face[i].ClearUserBit(selBit);
			selection = QRect();
			this->setCursor(Qt::CrossCursor);
			break;
		case 4:
			if (mode != EditVert)
			{
				if (selection != QRect())
				{
					mode = EditVert; 
					selectedV = true;
					this->setCursor(QCursor(QBitmap(":/images/sel_move.png")));
				}
				else
				{
					mode = Select;
					this->setCursor(Qt::CrossCursor);
				}
			}
			break;
	}
	this->update();
}

void RenderArea::ChangeSelectMode(int index)
{
	// Change the function of the mouse selection
	switch(index)
	{
		case 0:
			if (selectMode != Area && selectMode != Connected) selection = QRect();
			selectMode = Area;
			break;
		case 1:
			if (selectMode != Connected && selectMode != Area) selection = QRect();
			selectMode = Connected;
			break;
		case 2:
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
	}
	if (selected && selectMode == Vertex) selBit = CFaceO::NewBitFlag();
}

void RenderArea::RotateComponent(float alfa)
{
	// Calcolate the new position of the vertex of the selected component after a rotation.
	// The rotation is done around the selected point
	if (origin != QPoint())
	{
		float theta = alfa;// * 3.14159f / 180.0f; // Convert degree to radiant. PI is a finite number -> lost of precision
		float sinv = sin(theta);
		float cosv = cos(theta);
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			if (model->cm.face[i].WT(0).n() == textNum && (!selected || (selected && model->cm.face[i].IsUserBit(selBit))))
			for (unsigned j = 0; j < 3; j++)
			{
				float u = origin.x() + (cosv * (model->cm.face[i].WT(j).u() - origin.x()) - sinv * (model->cm.face[i].WT(j).v() - origin.y()));
				float v = origin.y() + (sinv * (model->cm.face[i].WT(j).u() - origin.x()) + cosv * (model->cm.face[i].WT(j).v() - origin.y()));
				model->cm.face[i].WT(j).u() = u;
				model->cm.face[i].WT(j).v() = v;
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
		if (model->cm.face[i].WT(0).n() == textNum && selected && model->cm.face[i].IsUserBit(selBit))
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
		if (model->cm.face[i].WT(0).n() == textNum && model->cm.face[i].IsUserBit(selBit))
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
		for (unsigned j = 0; j < 3; j++)
		{
			if (areaUV.contains(QPointF(model->cm.face[i].WT(j).u(),model->cm.face[i].WT(j).v()))
				&& model->cm.face[i].V(j)->IsUserBit(selVertBit))
			{
				model->cm.face[i].WT(j).u() = model->cm.face[i].WT(j).u() - (float)posVX/(AREADIM*zoom);
				model->cm.face[i].WT(j).v() = model->cm.face[i].WT(j).v() + (float)posVY/(AREADIM*zoom);
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
		if ((*fi).WT(0).n() == textNum)
		{
	        (*fi).ClearUserBit(selBit);
			QVector<QPoint> t = QVector<QPoint>(); 
			t.push_back(QPoint((*fi).WT(0).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - ((*fi).WT(0).v() * AREADIM*zoom) + viewport.Y()*zoom));
			t.push_back(QPoint((*fi).WT(1).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - ((*fi).WT(1).v() * AREADIM*zoom) + viewport.Y()*zoom));
			t.push_back(QPoint((*fi).WT(2).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - ((*fi).WT(2).v() * AREADIM*zoom) + viewport.Y()*zoom));
			QRegion r = QRegion(QPolygon(t));
			if (r.intersects(area))
			{
				(*fi).SetUserBit(selBit);
				if (r.boundingRect().topLeft().x() < selStart.x()) selStart.setX(r.boundingRect().topLeft().x());
				if (r.boundingRect().topLeft().y() < selStart.y()) selStart.setY(r.boundingRect().topLeft().y());
				if (r.boundingRect().bottomRight().x() > selEnd.x()) selEnd.setX(r.boundingRect().bottomRight().x());
				if (r.boundingRect().bottomRight().y() > selEnd.y()) selEnd.setY(r.boundingRect().bottomRight().y());
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
	QPointF a = QPointF((float)(area.x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - area.y() + viewport.Y()*zoom)/(AREADIM*zoom));
	QPointF b = QPointF((float)(area.bottomRight().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - area.bottomRight().y() + viewport.Y()*zoom)/(AREADIM*zoom));
	areaUV = QRectF(a, QSizeF(b.x()-a.x(), b.y()-a.y()));
	CMeshO::FaceIterator fi;
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).WT(0).n() == textNum)
		{
			for (int j = 0; j < 3; j++)
			{
				QPoint tmp = QPoint((*fi).WT(j).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - ((*fi).WT(j).v() * AREADIM*zoom) + viewport.Y()*zoom);
				if (area.contains(tmp))
				{
					(*fi).V(j)->SetUserBit(selVertBit);
					if (tmp.x() < selStart.x()) selStart.setX(tmp.x());
					if (tmp.y() < selStart.y()) selStart.setY(tmp.y());
					if (tmp.x() > selEnd.x()) selEnd.setX(tmp.x());
					if (tmp.y() > selEnd.y()) selEnd.setY(tmp.y());
					if (!selectedV) selectedV = true;
				}
			}
		}
	}
}

void RenderArea::SelectConnectedComponent(QPoint e)
{
	// Select a series of faces with the same UV coord on the edge	
	if(!model->cm.face.FFAdjacencyEnabled)
	{
		model->cm.face.EnableFFAdjacency();
		if (!updateTopology) 
		{
			vcg::tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(model->cm);
			updateTopology = true;
		}
	}
	selStart = QPoint(MAX,MAX);
	selEnd = QPoint(-MAX,-MAX);
	selected = false;
	selBit = CFaceO::NewBitFlag();
	int index = 0;
	vector<CFaceO*> Q = vector<CFaceO*>();
	
	// Search the clicked face
	for(unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum)
		{
			QVector<QPoint> t = QVector<QPoint>(); 
			t.push_back(QPoint(model->cm.face[i].WT(0).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - (model->cm.face[i].WT(0).v() * AREADIM*zoom) + viewport.Y()*zoom));
			t.push_back(QPoint(model->cm.face[i].WT(1).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - (model->cm.face[i].WT(1).v() * AREADIM*zoom) + viewport.Y()*zoom));
			t.push_back(QPoint(model->cm.face[i].WT(2).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - (model->cm.face[i].WT(2).v() * AREADIM*zoom) + viewport.Y()*zoom));
			QRegion r = QRegion(QPolygon(t));
			if (r.contains(e))
			{
				Q.push_back(&model->cm.face[i]);
				model->cm.face[i].SetUserBit(selBit);
				selStart.setX(r.boundingRect().topLeft().x());
				selStart.setY(r.boundingRect().topLeft().y());
				selEnd.setX(r.boundingRect().bottomRight().x());
				selEnd.setY(r.boundingRect().bottomRight().y());
				break;
			}
		}
	}
	
	while (index < Q.size())
	{
		for (int j = 0; j < 3; j++)
		{
			CFaceO* p = Q[index]->FFp(j);
			if (!p->IsUserBit(selBit) && (Q[index]->WT(0) == p->WT(0) || Q[index]->WT(0) == p->WT(1) || Q[index]->WT(0) == p->WT(2) || 
				Q[index]->WT(1) == p->WT(0) || Q[index]->WT(1) == p->WT(1) || Q[index]->WT(1) == p->WT(2) || 
				Q[index]->WT(2) == p->WT(0) || Q[index]->WT(2) == p->WT(1) || Q[index]->WT(2) == p->WT(2)))
			{
				p->SetUserBit(selBit);
				Q.push_back(p);
				QPoint tmp = QPoint(p->WT(j).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - (p->WT(j).v() * AREADIM*zoom) + viewport.Y()*zoom);
				if (tmp.x() < selStart.x()) selStart.setX(tmp.x());
				if (tmp.y() < selStart.y()) selStart.setY(tmp.y());
				if (tmp.x() > selEnd.x()) selEnd.setX(tmp.x());
				if (tmp.y() > selEnd.y()) selEnd.setY(tmp.y());
				selected = true;
			}
		}
		index++;
	}
}

void RenderArea::ClearSelection()
{
	// Clear every selection
	selBit = CFaceO::NewBitFlag();
	selVertBit = CVertexO::NewBitFlag();
	selection = QRect();
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
			//selRect[2].moveCenter(QPoint(tx, selRect[2].center().y())); // THERE IS A BUG IN QT: movecenter doesn't work
			//selRect[1].moveCenter(QPoint(selRect[1].center().x(),ty));     if coords are negative.......
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
			//selRect[3].moveCenter(QPoint(tx, selRect[3].center().y()));
			//selRect[0].moveCenter(QPoint(selRect[0].center().x(), ty));	
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
			//selRect[0].moveCenter(QPoint(tx, selRect[0].center().y()));
			//selRect[3].moveCenter(QPoint(selRect[3].center().x(), ty));
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
			//selRect[1].moveCenter(QPoint(tx, selRect[1].center().y()));
			//selRect[2].moveCenter(QPoint(selRect[2].center().x(), ty));
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
	origin = QPointF((float)(originR.center().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - originR.center().y() + viewport.Y()*zoom)/(AREADIM*zoom));
	
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
	float ny = (float) (e.x() - Rq) / Rm;
	switch(highlighted)
	{
	case 0:
		if ( ny > e.y()) degree = -degree;
		break;
	case 1:
		if (ny < e.y()) degree = -degree;
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

void RenderArea::RecalculateSelectionArea()
{
	// Find the new size of the selection rectangle after a rotation or a scale
	selStart = QPoint(MAX,MAX);
	selEnd = QPoint(-MAX,-MAX);
	CMeshO::FaceIterator fi;
	for(fi = model->cm.face.begin(); fi != model->cm.face.end(); ++fi)
	{
		if ((*fi).IsUserBit(selBit))
		{
			QVector<QPoint> t = QVector<QPoint>(); 
			t.push_back(QPoint((*fi).WT(0).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - ((*fi).WT(0).v() * AREADIM*zoom) + viewport.Y()*zoom));
			t.push_back(QPoint((*fi).WT(1).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - ((*fi).WT(1).v() * AREADIM*zoom) + viewport.Y()*zoom));
			t.push_back(QPoint((*fi).WT(2).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - ((*fi).WT(2).v() * AREADIM*zoom) + viewport.Y()*zoom));
			QRegion r = QRegion(QPolygon(t));
			if (r.boundingRect().topLeft().x() < selStart.x()) selStart.setX(r.boundingRect().topLeft().x());
			if (r.boundingRect().topLeft().y() < selStart.y()) selStart.setY(r.boundingRect().topLeft().y());
			if (r.boundingRect().bottomRight().x() > selEnd.x()) selEnd.setX(r.boundingRect().bottomRight().x());
			if (r.boundingRect().bottomRight().y() > selEnd.y()) selEnd.setY(r.boundingRect().bottomRight().y());
		}
	}
	if (selected)
	{
		selection = QRect(selStart, selEnd);
		UpdateSelectionArea(0,0);
	}
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
		if ((*fi).WT(0).n() == textNum)
		{
			for (int j = 0; j < 3; j++)
			{
				QPoint tmp = QPoint((*fi).WT(j).u() * AREADIM*zoom + viewport.X()*zoom, AREADIM*zoom - ((*fi).WT(j).v() * AREADIM*zoom) + viewport.Y()*zoom);
				if ((*fi).V(j)->IsUserBit(selVertBit))
				{
					if (tmp.x() < selStart.x()) selStart.setX(tmp.x());
					if (tmp.y() < selStart.y()) selStart.setY(tmp.y());
					if (tmp.x() > selEnd.x()) selEnd.setX(tmp.x());
					if (tmp.y() > selEnd.y()) selEnd.setY(tmp.y());
					if (!selectedV) selectedV = true;
				}
			}
		}
	}
	selection = QRect(QPoint(selStart.x() - RADIUS/2, selStart.y() - RADIUS/2), QPoint(selEnd.x() + RADIUS/2, selEnd.y() + RADIUS/2));
	QPointF a = QPointF((float)(selection.x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - selection.y() + viewport.Y()*zoom)/(AREADIM*zoom));
	QPointF b = QPointF((float)(selection.bottomRight().x() - viewport.X()*zoom)/(AREADIM*zoom), (float)(AREADIM*zoom - selection.bottomRight().y() + viewport.Y()*zoom)/(AREADIM*zoom));
	areaUV = QRectF(a, QSizeF(b.x()-a.x(), b.y()-a.y()));
}
 
void RenderArea::DrawCircle(QPoint origin)
{
	// Draw a circle
	float DEG2RAD = 3.14159f/180.0f;
	float r = (float)RADIUS/2/zoom;
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < 360; i++)
	{
		float degInRad = i*DEG2RAD;
		glVertex3f(origin.x() + cos(degInRad)*r,origin.y() + sin(degInRad)*r,1.1f);
	}
	glEnd();
}
