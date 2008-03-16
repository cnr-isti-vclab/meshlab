#include <QtGui>
#include "renderarea.h"
#include "textureeditor.h"
#include <wrap/qt/trackball.h>
#include <wrap/gui/trackball.cpp>
#include <math.h>
#include <stdlib.h>

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
	isDragging = false;
	highlightedPoint = -1;
	highComp = -1;
	highClick = -1;

	model = m;
	oldX = 0; oldY = 0;
	viewport = Point2f(0,0);
	tb = new Trackball();
	tb->center = Point3f(0, 0, 0);
	tb->radius = 1;

	brush = QBrush(Qt::green);

	this->setMouseTracking(true);
	this->setCursor(Qt::PointingHandCursor);
	this->setAttribute(Qt::WA_NoSystemBackground);
}

RenderArea::~RenderArea(){}

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
    painter.setPen(QPen(brush,2));	// <--- customizzabile???
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, antialiased);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	
	painter.begin(painter.device());	// Initialize a GL context
	
	tb->GetView();
	tb->Apply(true);

	int maxX = 0, maxY = 0, minX = 0, minY = 0;	// For texCoord out of border
	if (image != QImage())
	{
	    glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
		glEnable(GL_DEPTH_TEST);
	    glColor3f(1,1,1);
		glLineWidth(2);
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			// While drawning, it counts the number of 'planes'
			if (model->cm.face[i].WT(0).u() > maxX || model->cm.face[i].WT(1).u() > maxX || model->cm.face[i].WT(2).u() > maxX)	maxX++;
			if (model->cm.face[i].WT(0).v() > maxY || model->cm.face[i].WT(1).v() > maxY || model->cm.face[i].WT(2).v() > maxY)	maxY++;
			if (model->cm.face[i].WT(0).u() < minX || model->cm.face[i].WT(1).u() < minX || model->cm.face[i].WT(2).u() < minX)	minX--;
			if (model->cm.face[i].WT(0).v() < minY || model->cm.face[i].WT(1).v() < minY || model->cm.face[i].WT(2).v() < minY)	minY--;
			// First draw the model in u,v space
			if (model->cm.face[i].IsS() && model->cm.face[i].WT(0).n() == textNum)
			{
				glBegin(GL_LINE_LOOP);
					glVertex3f(model->cm.face[i].WT(0).u() * AREADIM, AREADIM - (model->cm.face[i].WT(0).v() * AREADIM), 1);
					glVertex3f(model->cm.face[i].WT(1).u() * AREADIM, AREADIM - (model->cm.face[i].WT(1).v() * AREADIM), 1);
					glVertex3f(model->cm.face[i].WT(2).u() * AREADIM, AREADIM - (model->cm.face[i].WT(2).v() * AREADIM), 1);
				glEnd();
			}
		}
		glDisable(GL_LOGIC_OP);
		glDisable(GL_COLOR_LOGIC_OP);

		// Draw the background behind the model
		for (int x = minX; x < maxX; x++) 
			for (int y = minY; y < maxY; y++)
				painter.drawImage(QRect(x*AREADIM,-y*AREADIM,AREADIM,AREADIM),image,QRect(0,0,image.width(),image.height()));
		
		// and the axis, always in first plane
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0, AREADIM, AREADIM,0,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		// Line and text (native Qt)
		painter.drawLine(0,AREADIM,AREADIM,AREADIM);
		painter.drawLine(0,AREADIM,0,0);
		painter.drawText(TRANSLATE, AREADIM - TRANSLATE, QString("(%1,%2)").arg((float)-viewport.X()/AREADIM).arg((float)viewport.Y()/AREADIM));
		painter.drawText(TRANSLATE, TRANSLATE*3, QString("(%1,%2)").arg((float)-viewport.X()/AREADIM).arg((float)viewport.Y()/AREADIM + 1));
		painter.drawText(AREADIM - TRANSLATE*18, AREADIM - TRANSLATE, QString("(%1,%2)").arg((float)-(viewport.X()/AREADIM) + 1).arg((float)viewport.Y()/AREADIM));
		painter.drawText(TRANSLATE, TRANSLATE*6, QString("V"));
		painter.drawText(AREADIM - TRANSLATE*23, AREADIM - TRANSLATE, QString("U"));
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
    painter.drawRect(QRect(0, 0, width()-1, height()-1));

	painter.end();
}

// Mouse Event:
void RenderArea::mousePressEvent(QMouseEvent *e)
{
	oldX = e->x(); oldY = e->y();
	tmpX = viewport.X(); tmpY = viewport.Y();
	tb->MouseDown(e->x(), AREADIM-e->y(), QT2VCG(e->button(), e->modifiers()));
	this->update();
}

void RenderArea::mouseReleaseEvent(QMouseEvent *e)
{
	tb->MouseUp(e->x(), AREADIM-e->y(), QT2VCG(e->button(), e->modifiers()));
	this->update();
}

void RenderArea::mouseMoveEvent(QMouseEvent *e)
{
	if((e->buttons() & Qt::LeftButton))
	{
		tb->track.SetTranslate(Point3f(tmpX + oldX - e->x(), tmpY + oldY - e->y(), 0));
		viewport = Point2f(tmpX + oldX - e->x(), tmpY + oldY - e->y());
		this->update();
	}
}

void RenderArea::mouseDoubleClickEvent(QMouseEvent *)
{
	// Reset of the trackball
	tb->center = Point3f(0, 0, 0);
	viewport = Point2f(0,0);
	oldX = 0; oldY = 0;
	tb->track.SetTranslate(Point3f(0,0,0));
	// <--- reset dello zoom
	this->update();
}

void RenderArea::wheelEvent(QWheelEvent*e)
{
	// Zoom
	float WHEEL_STEP = 120.0f;
	float notch = (float)e->delta()/WHEEL_STEP;
    tb->MouseWheel(notch, QTWheel2VCG(e->modifiers())); 
	tb->track.SetScale(notch);	// <---- ???? parametro ????
	this->update();
}		

void RenderArea::RemapRepeat()
{
	/*
	// Remap the uv in 9 planes: the main plain is in the middle, the coordinates over 
	// the border will be mapped in the other planes
	out = true;
	for (unsigned i = 0; i < map.size(); i++)
	{
		// Cast needed to precision...
		float u = map[i].GetU();
		float v = map[i].GetV();
		map[i].SetVertex(GetRepeatVertex(u,v,i));
	}
	this->update();
	*/
}

void RenderArea::RemapClamp()
{
	/*
	// Remap the uv coord out of border using clamp method
	out = false;
	for (unsigned i = 0; i < map.size(); i++)
	{
		float u = map[i].GetU();
		float v = map[i].GetV();
		map[i].SetVertex(GetClampVertex(u, v, i));
	}
	this->update();
	*/
}

void RenderArea::RemapMod()
{
	/*
	// Remap the uv coord out of border using mod function
	out = false;
	for (unsigned i = 0; i < map.size(); i++)
	{
		float u = map[i].GetU();
		float v = map[i].GetV();
		if (u < 0) {while (u<0) u++;}
		else if (u > 1) {while (u>1) u--;}
		if (v < 0) {while(v<0) v++;}
		else if (v > 1) {while(v>1) v--;}
		map[i].SetVertex(QRect(u * AREADIM - RADIUS/2, (AREADIM - (v * AREADIM)) - RADIUS/2, RADIUS, RADIUS));
		UpdateSingleUV(i, u, v);
	}
	this->update();
	*/
}

void RenderArea::UpdateVertex(float u, float v)
{
	/*
	// Update the position of the vertexes from user spin box input
	QRect r;
	if (!out) r = GetClampVertex(u, v, -1);
	else r = GetRepeatVertex(u, v, -1);
	map[highClick].SetU(u);
	map[highClick].SetV(v);
	map[highClick].SetVertex(r);
	this->update();
	*/
}

void RenderArea::VisitConnected()
{
	/*
	// Visit the vertex-tree and initialize the vector 'connected' adding the index of the face with FF adjacency.
	connected.clear();
	int id = map[highComp].GetCompID();
	for (unsigned i = 0; i < map.size(); i++)
	{
		if (map[i].GetCompID() == id) connected.push_back(i);
	}
	*/
}

void RenderArea::ChangeMode(int index)
{
	// Change the selection mode
	switch(index)
	{
		case 0:
			mode = Point;
			break;
		case 1:
			mode = Face;
			break;
		case 2:
			mode = Smooth;
			break;
	}
	this->update();
}


void RenderArea::UpdateComponentPos(int x, int y)
{
	/*
	// Update the position of all vertexes of the connected component
	for (unsigned i = 0; i < connected.size(); i++)
	{
		// The MoveCenter func create some approximation errors....
		QPoint p = map[connected[i]].GetVertex().topLeft();
		QRect n = QRect(p.x() + x , p.y() + y, RADIUS, RADIUS);
		map[connected[i]].SetVertex(n);
	}
	this->update();
	// The UV Coord will be updated after mouse-release event
	*/
}

void RenderArea::RotateComponent(float alfa)
{
	/*
	// Calcolate the new position of the vertex of the selected component after a rotation.
	// The rotation is done around the selected vertex (= highComp)
	QPoint origin = map[highComp].GetVertex().center();
	float theta = -alfa * 3.14159f / 180.0f;	// Convert degree to radiant. PI is a finite number -> lost of precision
	for (unsigned i = 0; i < connected.size(); i++)
	{
		int x = origin.x() + ((map[connected[i]].GetVertex().center().x() - origin.x()) * cos(theta)
			- ((map[connected[i]].GetVertex().center().y() - origin.y()) * sin(theta)));
		int y = origin.y() + ((map[connected[i]].GetVertex().center().y() - origin.y()) * cos(theta) 
			+ ((map[connected[i]].GetVertex().center().x() - origin.x()) * sin(theta)));
		QRect n = map[connected[i]].GetVertex();
		n.moveCenter(QPoint(x, y));
		map[connected[i]].SetVertex(n);
		UpdateSingleUV(connected[i], (float)map[connected[i]].GetVertex().center().x() / AREADIM,
			(float)(AREADIM - map[connected[i]].GetVertex().center().y()) / AREADIM);
	}
	this->update();
	*/
}

void RenderArea::ScaleComponent(int perc)
{
	/*
	// Scale the selected component. The origin is set to the selected vertex ( = highComp)
	QPoint origin = map[highComp].GetVertex().center();
	map[highComp].SetV();
	float p = (float) perc / 100.0f;
	for (unsigned i = 0; i < connected.size(); i++)
	{
		QPoint next = map[connected[i]].GetVertex().center();
		int x = origin.x() + (map[connected[i]].GetVertex().center().x() - origin.x()) * p;
		int y = origin.y() + (map[connected[i]].GetVertex().center().y() - origin.y()) * p;
		if (x < 0) x = 0; else if (x > AREADIM) x = AREADIM;
		if (y < 0) y = 0; else if (y > AREADIM) y = AREADIM;
		QRect n = map[connected[i]].GetVertex();
		n.moveCenter(QPoint(x, y));
		map[connected[i]].SetVertex(n);
		UpdateSingleUV(connected[i], (float)map[connected[i]].GetVertex().center().x() / AREADIM,
			(float)(AREADIM - map[connected[i]].GetVertex().center().y()) / AREADIM);
	}
	this->update();
	*/
}

void RenderArea::UpdateUV()
{
	/*
	// After a move of component, re-calculate the new UV coordinates
	for (unsigned i = 0; i < connected.size(); i++)
	{
		float u = (float)map[connected[i]].GetVertex().center().x() / AREADIM;
		float v = (float)(AREADIM - map[connected[i]].GetVertex().center().y()) / AREADIM;
		UpdateSingleUV(connected[i], u, v);
	}
	*/
}

void RenderArea::UpdateSingleUV(int index, float u, float v)
{
	/*
	// Update the UV Coord of the vertex map[index]
	if (!out)
	{
		if (u < 0.0f) u = 0.0f; else if (u > 1.0f) u = 1.0f;
		if (v < 0.0f) v = 0.0f; else if (v > 1.0f) v = 1.0f;
	}
	else
	{
		u = (u * 3) - 1;
		v = (v * 3) - 1;
	}
	map[index].SetU(u);
	map[index].SetV(v);
	*/
}

QRect RenderArea::GetRepeatVertex(float u, float v, int index)
{
	/*
	// Return the new position of the vertex in the RenderArea space in 'Repeat' mode.
	// If the passed index is valid, also update the UV coord
	// The function is called from the Remap (--> change UV) and from the UpdateVertex
	float realu, realv;
	if (u < 0) 
	{
		while (u < 0) u++; 
		realu = u; 
		u = (float)u * AREADIM/3;
	}
	else if (u > 1) 
	{ 
		while (u > 1) u--; 
		realu = u; 
		u = (float)u * AREADIM/3 + 2*AREADIM/3; 
	}
	else 
	{
		realu = u; 
		u = (float) u * AREADIM/3 + AREADIM/3;
	}
	if (v < 0) 
	{
		while (v < 0) v++; 
		realv = v; 
		v = (float)v * AREADIM/3;
	}
	else if (v > 1) 
	{ 
		while (v > 1) v--; 
		realv = v; 
		v = (float)v * AREADIM/3 + 2*AREADIM/3; 
	}
	else 
	{
		realv = v; 
		v = (float)v * AREADIM/3 + AREADIM/3;
	}
	if (index != -1) 
	{
		map[index].SetU(realu);
		map[index].SetV(realv);	
	}
	return QRect(u  - RADIUS/2, AREADIM - v - RADIUS/2, RADIUS, RADIUS);
	*/
	return QRect();
}

QRect RenderArea::GetClampVertex(float u, float v, int index)
{
	/*
	// Return the new position of the vertex in the RenderArea space in 'Clamp' mode.
	// If the passed index is valid, also update the UV coord
	// The function is called from the Remap (--> change UV) and from the UpdateVertex
	if (u < 0) u = 0;
	else if (u > 1) u = 1;
	if (v < 0) v = 0;
	else if (v > 1) v = 1;
	if (index != -1) UpdateSingleUV(index, u, v);
	return QRect(u * AREADIM - RADIUS/2, (AREADIM - (v * AREADIM)) - RADIUS/2, RADIUS, RADIUS);
	*/
	return QRect();
}


