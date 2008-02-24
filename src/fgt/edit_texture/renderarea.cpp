#include <QtGui>
#include "renderarea.h"
#include "textureeditor.h"

RenderArea::RenderArea(QWidget *parent, QString textureName, vector<Container> uvmap, bool outRange) : QWidget(parent)
{
    antialiased = true;
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
	if(textureName != QString())
	{
		if (QFile(textureName).exists()) image = QImage(textureName);
		else textureName = QString();
	}
	out = outRange;
	map = uvmap;
	isDragging = false;
	highlightedPoint = -1;
	highComp = -1;
	highClick = -1;
	mode = Point;
	moved = false;

	if (out) RemapRepeat();

	this->setMouseTracking(true);
	this->setCursor(Qt::CrossCursor);
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

void RenderArea::SetUVMap(vector<Container> uv)
{
	map = uv;
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, antialiased);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    //painter.translate(TRANSLATE, TRANSLATE);

	if (image != QImage())
	{
		// Background texture
		if (out)
		{
			// Draw 9 planes..
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
					painter.drawImage(QRect(0 + AREADIM/3 * i,0 + AREADIM/3 * j, AREADIM/3,AREADIM/3),image,QRect(0,0,image.width(),image.height()));
			// ..and the bounds
			painter.setPen(Qt::red);
			painter.drawLine(QPoint(0,AREADIM/3), QPoint(AREADIM,AREADIM/3));
			painter.drawLine(QPoint(0,2*AREADIM/3), QPoint(AREADIM,2*AREADIM/3));
			painter.drawLine(QPoint(AREADIM/3,0), QPoint(AREADIM/3,AREADIM));
			painter.drawLine(QPoint(2*AREADIM/3,0), QPoint(2*AREADIM/3,AREADIM));
			painter.setPen(pen);

		}
		else painter.drawImage(QRect(0,0,AREADIM,AREADIM),image,QRect(0,0,image.width(),image.height()));
	}
	else painter.drawText(TEXTX, TEXTY, tr("NO TEXTURE"));

	if (map.size() > 0)
	{
		// Draw the selected component 
		if (highComp != -1 && mode == Face)
		{
			for (unsigned y = 0; y < connected.size(); y++)
			{
				QVector<QPoint> p;
				if (!map[connected[y]].IsV())
				{
					p.push_back(map[y].GetVertex().center());
					map[y].SetV();
					for (int yy = 0; yy < map[connected[y]].GetAdjSize(); yy++)
					{
						p.push_back(map[map[connected[y]].GetAdjAt(yy)].GetVertex().center());
						map[map[connected[y]].GetAdjAt(yy)].SetV();
					}
				}
				painter.setBrush(Qt::red);
				painter.setOpacity(0.5);
				painter.drawPolygon(QPolygon(p));
			}
			for (unsigned y = 0; y < connected.size(); y++) map[y].ClearV();
		}
		// Draw the lines...
		for (unsigned i = 0; i < map.size(); i++)
		{
			for (int jj = 0; jj < map[i].GetAdjSize(); jj++)
			{
				unsigned d = map[i].GetAdjAt(jj);
				if (d > i)	painter.drawLine(map[i].GetVertex().center(),map[d].GetVertex().center());
			}	
		}
		// ...and the vertexes (as a circle)
		for (unsigned i = 0; i < map.size(); i++)
		{
			painter.setOpacity(1.0);
			painter.setBrush(Qt::yellow);
			if ((i == highClick && mode == Point) || (i == highComp && mode == Face)) painter.setBrush(Qt::blue);
			else if (i == highlightedPoint) painter.setBrush(Qt::red);
			painter.drawEllipse(map[i].GetVertex());
		}
	}
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width()-1, height()-1));
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
	switch(mode)
	{
		case Point:
			if (highlightedPoint != -1) 
			{
				isDragging = true;
				this->update(map[highlightedPoint].GetVertex());
				if (highClick != -1) this->update(map[highClick].GetVertex());
				highClick = highlightedPoint;
				emit UpdateStat(map[highlightedPoint].GetU(), map[highlightedPoint].GetV(), map[highClick].GetCompID(), map[highClick].GetCompID(), NO_CHANGE);
			}
			else
			{
				if (highClick != -1) {this->update(map[highClick].GetVertex()); highClick = -1;}
				emit UpdateStat(0.0,0.0, RESET, RESET, NO_CHANGE);
			}
			break;
		case Face:
			if (highlightedPoint != -1) 
			{
				isDragging = true;
				if (highlightedPoint != highComp)
				{
					highComp = highlightedPoint;
					VisitConnected();
				}
				emit UpdateStat(NO_CHANGE, NO_CHANGE, ENABLECMD, ENABLECMD, connected.size());
			}
			else 
			{
				isDragging = false;
				highComp = -1;
				emit UpdateStat(IGNORECMD, IGNORECMD, DISABLECMD, DISABLECMD, NO_CHANGE);
			}
			this->update();
			break;
	}
}

void RenderArea::mouseReleaseEvent(QMouseEvent *)
{
	isDragging = false;
	if(highlightedPoint != -1) this->update(map[highlightedPoint].GetVertex());
	if (mode == Face && moved) {UpdateUV(); moved = false;}
}

void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
	if (isDragging)
	{
		QPoint tmp = event->pos();
		QRect r = map[highlightedPoint].GetVertex();
		if (event->pos().x() < 0) tmp.setX(0);
		if (event->pos().x() > AREADIM) tmp.setX(AREADIM);
		if (event->pos().y() < 0) tmp.setY(0);
		if (event->pos().y() > AREADIM) tmp.setY(AREADIM);
		switch(mode)
		{
		case Point:
			r.moveCenter(tmp);
			map[highlightedPoint].SetVertex(r);
			if (!out) 
			{
				map[highlightedPoint].SetU((float)map[highlightedPoint].GetVertex().center().x() / AREADIM);
				map[highlightedPoint].SetV((float)(AREADIM - map[highlightedPoint].GetVertex().center().y()) / AREADIM);
			}
			else 
			{
				map[highlightedPoint].SetU(((float)map[highlightedPoint].GetVertex().center().x() / (AREADIM / 3)) - 1);
				map[highlightedPoint].SetV(((float)(AREADIM - map[highlightedPoint].GetVertex().center().y()) / (AREADIM / 3)) - 1);
			}
			emit UpdateStat(map[highlightedPoint].GetU(),map[highlightedPoint].GetV(), map[highlightedPoint].GetCompID(), IGNORECMD, NO_CHANGE);
			this->update();
			break;
		case Face:
			int x = tmp.x() - r.center().x();
			int y = tmp.y() - r.center().y();
			UpdateComponentPos(x,y);
			moved = true;
			break;
		}
	}
	else
	{
		for (unsigned i = 0; i < map.size(); i++)
		{
			if (map[i].GetVertex().contains(event->pos()))
			{
				if (i != highlightedPoint)
				{
					// There are some problems if vertex are near, so I update a larger area..
					if (highlightedPoint != -1) this->update(map[highlightedPoint].GetVertex());
					highlightedPoint = i;
					this->update(map[highlightedPoint].GetVertex());
				}
				return;
			}
		}
		if (highlightedPoint != -1)
		{
			this->update(map[highlightedPoint].GetVertex());
			highlightedPoint = -1;
		}
	}
	return;
}

void RenderArea::RemapRepeat()
{
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
}

void RenderArea::RemapClamp()
{
	// Remap the uv coord out of border using clamp method
	out = false;
	for (unsigned i = 0; i < map.size(); i++)
	{
		float u = map[i].GetU();
		float v = map[i].GetV();
		map[i].SetVertex(GetClampVertex(u, v, i));
	}
	this->update();
}

void RenderArea::RemapMod()
{
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
}

void RenderArea::UpdateVertex(float u, float v)
{
	// Update the position of the vertexes from user spin box input
	QRect r;
	if (!out) r = GetClampVertex(u, v, -1);
	else r = GetRepeatVertex(u, v, -1);
	map[highClick].SetU(u);
	map[highClick].SetV(v);
	map[highClick].SetVertex(r);
	this->update();
}

void RenderArea::VisitConnected()
{
	// Visit the vertex-tree and initialize the vector 'connected' adding the index of the face with FF adjacency.
	connected.clear();
	int id = map[highComp].GetCompID();
	for (unsigned i = 0; i < map.size(); i++)
	{
		if (map[i].GetCompID() == id) connected.push_back(i);
	}
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
}

void RenderArea::RotateComponent(float alfa)
{
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
}

void RenderArea::ScaleComponent(int perc)
{
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
}

void RenderArea::UpdateUV()
{
	// After a move of component, re-calculate the new UV coordinates
	for (unsigned i = 0; i < connected.size(); i++)
	{
		float u = (float)map[connected[i]].GetVertex().center().x() / AREADIM;
		float v = (float)(AREADIM - map[connected[i]].GetVertex().center().y()) / AREADIM;
		UpdateSingleUV(connected[i], u, v);
	}
}

void RenderArea::UpdateSingleUV(int index, float u, float v)
{
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
}

QRect RenderArea::GetRepeatVertex(float u, float v, int index)
{
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
}

QRect RenderArea::GetClampVertex(float u, float v, int index)
{
	// Return the new position of the vertex in the RenderArea space in 'Clamp' mode.
	// If the passed index is valid, also update the UV coord
	// The function is called from the Remap (--> change UV) and from the UpdateVertex
	if (u < 0) u = 0;
	else if (u > 1) u = 1;
	if (v < 0) v = 0;
	else if (v > 1) v = 1;
	if (index != -1) UpdateSingleUV(index, u, v);
	return QRect(u * AREADIM - RADIUS/2, (AREADIM - (v * AREADIM)) - RADIUS/2, RADIUS, RADIUS);
}