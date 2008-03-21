#include <QtGui>
#include "renderarea.h"
#include "textureeditor.h"
#include <wrap/qt/trackball.h>
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
	model = m;

	// Init
	oldX = 0; oldY = 0;
	viewport = Point2f(0,0);
	tb = new Trackball();
	tb->center = Point3f(0, 0, 0);
	tb->radius = 1;
	panX = 0; panY = 0;
	oldPX = 0; oldPY = 0;

	brush = QBrush(Qt::green);
	mode = View;
	editMode = Move;
	origin = QPoint();

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

	maxX = 0; maxY = 0; minX = 0; minY = 0;
	if (image != QImage())
	{
	    glEnable(GL_COLOR_LOGIC_OP);
		glLogicOp(GL_XOR);
		glEnable(GL_DEPTH_TEST);
	    glColor3f(1,1,1);
		glLineWidth(2);
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			// First draw the model in u,v space
			if (model->cm.face[i].IsS() && model->cm.face[i].WT(0).n() == textNum)
			{
				// While drawning, it counts the number of 'planes'
				if (model->cm.face[i].WT(0).u() > maxX || model->cm.face[i].WT(1).u() > maxX || model->cm.face[i].WT(2).u() > maxX)	maxX++;
				if (model->cm.face[i].WT(0).v() > maxY || model->cm.face[i].WT(1).v() > maxY || model->cm.face[i].WT(2).v() > maxY)	maxY++;
				if (model->cm.face[i].WT(0).u() < minX || model->cm.face[i].WT(1).u() < minX || model->cm.face[i].WT(2).u() < minX)	minX--;
				if (model->cm.face[i].WT(0).v() < minY || model->cm.face[i].WT(1).v() < minY || model->cm.face[i].WT(2).v() < minY)	minY--;

				glBegin(GL_LINE_LOOP);
					glVertex3f(model->cm.face[i].WT(0).u() * AREADIM - panX, AREADIM - (model->cm.face[i].WT(0).v() * AREADIM) - panY, 1);
					glVertex3f(model->cm.face[i].WT(1).u() * AREADIM - panX, AREADIM - (model->cm.face[i].WT(1).v() * AREADIM) - panY, 1);
					glVertex3f(model->cm.face[i].WT(2).u() * AREADIM - panX, AREADIM - (model->cm.face[i].WT(2).v() * AREADIM) - panY, 1);
				glEnd();
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
		glOrtho(0,AREADIM,AREADIM,0,-1,1);
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
		// and the origin of the scale and rotation
		if (origin != QPoint())
		{
			painter.setPen(QPen(QBrush(Qt::black),1));
			painter.setBrush(QBrush(Qt::yellow));
			painter.drawEllipse(originR);
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
    painter.drawRect(QRect(0, 0, width()-1, height()-1));

	painter.end();
}

// Mouse Event:
void RenderArea::mousePressEvent(QMouseEvent *e)
{
	switch(mode)
	{
		case View:
			oldX = e->x(); oldY = e->y();
			tmpX = viewport.X(); tmpY = viewport.Y();
			tb->MouseDown(e->x(), AREADIM-e->y(), QT2VCG(e->button(), e->modifiers()));
			this->update();
			break;
		case Edit:
			switch(editMode)
			{
				case Move:
					tpanX = e->x();
					tpanY = e->y();
					break;
				case Choose:
					origin = QPointF((float)(e->x() - viewport.X())/AREADIM, (float)(AREADIM - e->y() + viewport.Y())/AREADIM);
					originR = QRect(e->x()-RADIUS/2, e->y()-RADIUS/2, RADIUS, RADIUS);
					this->update(originR);
					break;

			}
			break;
	}
}

void RenderArea::mouseReleaseEvent(QMouseEvent *e)
{
	switch(mode)
	{
		case View:
			tb->MouseUp(e->x(), AREADIM-e->y(), QT2VCG(e->button(), e->modifiers()));
			this->update();
			break;
		case Edit:
			oldPX = panX;
			oldPY = panY;
			UpdateUV();
			break;
	}
}

void RenderArea::mouseMoveEvent(QMouseEvent *e)
{
	if((e->buttons() & Qt::LeftButton))	// <---- Se non ci sono facce selezionate -> non si fa nulla
	{
		switch(mode)
		{
			case View:
				tb->track.SetTranslate(Point3f(tmpX + oldX - e->x(), tmpY + oldY - e->y(), 0));
				viewport = Point2f(tmpX + oldX - e->x(), tmpY + oldY - e->y());
				this->update();
				break;
			case Edit:
				if (editMode == Move)
				{
					panX = oldPX + tpanX - e->x();
					panY = oldPY + tpanY - e->y();
					this->update();
				}
				break;
		}
	}
}

void RenderArea::mouseDoubleClickEvent(QMouseEvent *)
{
	ResetTrack();
	// <--- reset dello zoom
	this->update();
}

void RenderArea::wheelEvent(QWheelEvent*e)
{
	switch(mode)
	{
		case View:
			// Zoom
			float WHEEL_STEP = 120.0f;
			float notch = (float)e->delta()/WHEEL_STEP;
		    tb->MouseWheel(notch, QTWheel2VCG(e->modifiers())); 
			tb->track.SetScale(notch);	// <---- ???? parametro ????
			this->update();
			break;
	}
}

void RenderArea::RemapClamp()
{
	// Remap the uv coord out of border using clamp method
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum)
		{
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
	ResetTrack();
	this->update();
	emit UpdateStat(0,0,0,0,0);	// <--------
}

void RenderArea::RemapMod()
{
	// Remap the uv coord out of border using mod function
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum)
		{
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
	ResetTrack();
	this->update();
	emit UpdateStat(0,0,0,0,0);	// <--------
}

void RenderArea::ChangeMode(int index)
{
	// Change the selection mode
	origin = QPoint();
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
				mode = Select;
				this->setCursor(Qt::CrossCursor);
			}
			break;
		default:
			this->setCursor(Qt::ArrowCursor);
			break;
	}
	this->update();
}

void RenderArea::ChangeEditMode(int index)
{
	// Change the function of the mouse press
	if (index == 0) editMode = Move;
	else editMode = Choose;
}

void RenderArea::RotateComponent(float alfa)
{
	// Calcolate the new position of the vertex of the selected component after a rotation.
	// The rotation is done around the selected point
	if (origin != QPoint())
	{
		float theta = alfa * 3.14159f / 180.0f; // Convert degree to radiant. PI is a finite number -> lost of precision
		float sinv = sin(theta);
		float cosv = cos(theta);
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			if (model->cm.face[i].WT(0).n() == textNum)
			for (unsigned j = 0; j < 3; j++)
			{
				float u = origin.x() + (cosv * (model->cm.face[i].WT(j).u() - origin.x()) - sinv * (model->cm.face[i].WT(j).v() - origin.y()));
				float v = origin.y() + (sinv * (model->cm.face[i].WT(j).u() - origin.x()) + cosv * (model->cm.face[i].WT(j).v() - origin.y()));
				model->cm.face[i].WT(j).u() = u;
				model->cm.face[i].WT(j).v() = v;
			}
		}
		this->update();
		emit UpdateStat(0,0,0,0,0);	// <--------
	}
}

void RenderArea::ScaleComponent(int perc)
{
	// Scale the selected component. The origin is set to the clicked point
	if (origin != QPoint())
	{
		float p = (float) perc / 100.0f;
		for (unsigned i = 0; i < model->cm.face.size(); i++)
		{
			if (model->cm.face[i].WT(0).n() == textNum)
			for (unsigned j = 0; j < 3; j++)
			{
				float x = origin.x() + (model->cm.face[i].WT(j).u() - origin.x()) * p;
				float y = origin.y() + (model->cm.face[i].WT(j).v() - origin.y()) * p;
				model->cm.face[i].WT(j).u() = x;
				model->cm.face[i].WT(j).v() = y;
			}
		}
	}
	this->update();
	emit UpdateStat(0,0,0,0,0);
}

void RenderArea::UpdateUV()
{
	// After a move of component, re-calculate the new UV coordinates
	for (unsigned i = 0; i < model->cm.face.size(); i++)
	{
		if (model->cm.face[i].WT(0).n() == textNum)
		{
			for (unsigned j = 0; j < 3; j++)
			{
				model->cm.face[i].WT(j).u() = model->cm.face[i].WT(j).u() - (float)panX/AREADIM;
				model->cm.face[i].WT(j).v() = model->cm.face[i].WT(j).v() + (float)panY/AREADIM;
			}
		}
	}
	panX = 0; panY = 0; tpanX = 0; tpanY = 0; oldPX = 0; oldPY = 0;
	this->update();
	emit UpdateStat(0,0,0,0,0);	// <--------
}

void RenderArea::ResetTrack()
{
	// Reset the center of the trackball
	tb->center = Point3f(0, 0, 0);
	viewport = Point2f(0,0);
	oldX = 0; oldY = 0;
	tb->track.SetTranslate(Point3f(0,0,0));
}
