/****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005                                                \/)\/    *
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
$Log: meshedit.cpp,v $
****************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "sampleedit.h"
#include <wrap/gl/pick.h>
#include<wrap/qt/gl_label.h>

using namespace std;
using namespace vcg;

SampleEditPlugin::SampleEditPlugin() 
{
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(12);

	haveToPick = false;
	pickmode = 0; // 0 face 1 vertex
	curFacePtr = 0;
	curVertPtr = 0;
	pIndex = 0;
}

const QString SampleEditPlugin::Info() 
{
	return tr("Return detailed info about a picked face or vertex of the model.");
}
 
void SampleEditPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
	gla->update();
	cur = event->pos();
	haveToPick = true;
	curFacePtr = 0;
	curVertPtr = 0;
	pIndex = 0;
}
  
void SampleEditPlugin::Decorate(MeshModel &m, GLArea * gla, QPainter *p)
{
	// first of all, the picking
	if (haveToPick)
	{
		glPushMatrix();
		glMultMatrix(m.cm.Tr);

		if (pickmode == 0)
		{
			NewFaceSel.clear();
			GLPickTri<CMeshO>::PickVisibleFace(cur.x(), gla->height() - cur.y(), m.cm, NewFaceSel);
			if (NewFaceSel.size() > 0)
			{
				curFacePtr = NewFaceSel[pIndex];
			}
			haveToPick = false;
		}
		else if (pickmode == 1)
		{
			NewVertSel.clear();
			GLPickTri<CMeshO>::PickVert(cur.x(), gla->height() - cur.y(), m.cm, NewVertSel,15,15);
			if (NewVertSel.size() > 0)
			{
				curVertPtr = NewVertSel[pIndex];
			}
			haveToPick = false;
		}

		glPopMatrix();
	}
    
	// then, the status
	QString line1 = "";
	QString line2 = "";
	QString line3 = "";

	if (pickmode == 0)
		line1 = "Face Picking - T for vertex<br>";
	else if (pickmode == 1)
		line1 = "Vertex Picking - T for face<br>";

	if ((curFacePtr != 0) || (curVertPtr != 0))
	{
		if (pickmode == 0)
		{
			line2 = QString("FACE# %1<br>").arg(QString::number(tri::Index(m.cm, curFacePtr)));
			if (NewFaceSel.size() > 1)
				line3 = "Q to cycle selected faces<br>";
			line3 += "P to log the details of the face";
		}
		else if (pickmode == 1)
		{
			line2 = QString("VERTEX# %1<br>").arg(QString::number(tri::Index(m.cm, curVertPtr)));
			if (NewVertSel.size() > 1)
				line3 = "Q to cycle selected vertices<br>";
			line3 += "P to log the details of the vertex";
		}
	}

	this->RealTimeLog("Geometry Info", m.shortName(),
		"%s %s %s", line1.toStdString().c_str(), line2.toStdString().c_str(), line3.toStdString().c_str());

	// finally, the actual decoration
	if (curFacePtr)
	{
		glPushMatrix();
		glMultMatrix(m.cm.Tr);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_LINE_BIT | GL_DEPTH_BUFFER_BIT);
		glLineWidth(2.0f);
		glDepthFunc(GL_ALWAYS);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glDisable(GL_LIGHTING);
		glColor(vcg::Color4b(vcg::Color4b::DarkRed));
		glBegin(GL_LINE_LOOP);
		glVertex(curFacePtr->P(0));
		glVertex(curFacePtr->P(1));
		glVertex(curFacePtr->P(2));
		glEnd();

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glColor(vcg::Color4b(vcg::Color4b::Red));
		glBegin(GL_LINE_LOOP);
		glVertex(curFacePtr->P(0));
		glVertex(curFacePtr->P(1));
		glVertex(curFacePtr->P(2));
		glEnd();

		drawFace(curFacePtr, m, gla, p);

		glPopAttrib();
		glPopMatrix();
	}

	if (curVertPtr)
	{
		glPushMatrix();
		glMultMatrix(m.cm.Tr);
		glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_POINT_BIT | GL_DEPTH_BUFFER_BIT);
		glPointSize(6.0f);
		glDepthFunc(GL_ALWAYS);
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glDisable(GL_LIGHTING);
		glColor(vcg::Color4b(vcg::Color4b::DarkBlue));
		glBegin(GL_POINTS);
		glVertex(curVertPtr->P());
		glEnd();

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glColor(vcg::Color4b(vcg::Color4b::Blue));
		glBegin(GL_POINTS);
		glVertex(curVertPtr->P());
		glEnd();

		drawVert(curVertPtr, m, gla, p);

		glPopAttrib();
		glPopMatrix();
	}
}

void SampleEditPlugin::drawFace(CMeshO::FacePointer fp, MeshModel &m, GLArea *gla, QPainter *p)
{
	QString buf = QString("f#%1 - v#(%2 %3 %4)").arg(QString::number(tri::Index(m.cm, fp))).arg(QString::number(tri::Index(m.cm, fp->V(0)))).arg(QString::number(tri::Index(m.cm, fp->V(1)))).arg(QString::number(tri::Index(m.cm, fp->V(2))));
	if (m.hasDataMask(MeshModel::MM_FACECOLOR))
		buf += QString(" - color(%1 %2 %3 %4)").arg(QString::number(fp->C()[0])).arg(QString::number(fp->C()[1])).arg(QString::number(fp->C()[2])).arg(QString::number(fp->C()[3]));
	Point3m c = Barycenter(*fp);
	vcg::glLabel::render(p, c, buf);
	for (int i = 0; i<3; ++i)
    {
		QString buf;
		buf =QString("fv%1:v#%2 - pos[%3 %4 %5]").arg(QString::number(i)).arg(QString::number(fp->V(i) - &m.cm.vert[0])).arg(QString::number(fp->P(i)[0])).arg(QString::number(fp->P(i)[1])).arg(QString::number(fp->P(i)[2]));
		if( m.hasDataMask(MeshModel::MM_VERTQUALITY) )
			buf +=QString(" - Q(%1)").arg(QString::number(fp->V(i)->Q()));
		if (m.hasDataMask(MeshModel::MM_VERTCOLOR))
			buf += QString(" - color(%1 %2 %3 %4)").arg(QString::number(fp->V(i)->C()[0])).arg(QString::number(fp->V(i)->C()[1])).arg(QString::number(fp->V(i)->C()[2])).arg(QString::number(fp->V(i)->C()[3]));
		if( m.hasDataMask(MeshModel::MM_WEDGTEXCOORD) )
			buf +=QString(" - [W]uv(%1 %2) id:%3").arg(QString::number(fp->WT(i).U())).arg(QString::number(fp->WT(i).V())).arg(QString::number(fp->WT(i).N()));
		if( m.hasDataMask(MeshModel::MM_VERTTEXCOORD) )
			buf +=QString(" - [V]uv(%1 %2) id:%3").arg(QString::number(fp->V(i)->T().U())).arg(QString::number(fp->V(i)->T().V())).arg(QString::number(fp->V(i)->T().N()));
		
		vcg::glLabel::render(p,fp->V(i)->P(),buf);
    }
}

void SampleEditPlugin::drawVert(CMeshO::VertexPointer vp, MeshModel &m, GLArea *gla, QPainter *p)
{
	QString buf;
	buf = QString("v#%1 - pos[%2 %3 %4]").arg(QString::number(vp - &m.cm.vert[0])).arg(QString::number(vp->P()[0])).arg(QString::number(vp->P()[1])).arg(QString::number(vp->P()[2]));
	if (m.hasDataMask(MeshModel::MM_VERTQUALITY))
		buf += QString(" - Q(%1)").arg(QString::number(vp->Q()));
	if (m.hasDataMask(MeshModel::MM_VERTCOLOR))
		buf += QString(" - color(%1 %2 %3 %4)").arg(QString::number(vp->C()[0])).arg(QString::number(vp->C()[1])).arg(QString::number(vp->C()[2])).arg(QString::number(vp->C()[3]));
	if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD))
		buf += QString(" - [V]uv(%1 %2) id:%3").arg(QString::number(vp->T().U())).arg(QString::number(vp->T().V())).arg(QString::number(vp->T().N()));
	
	vcg::glLabel::render(p, vp->P(), buf);
}

void SampleEditPlugin::keyReleaseEvent(QKeyEvent *e, MeshModel &m, GLArea *gla)
{
	if (e->key() == Qt::Key_T) // toggle pick mode
	{
		pickmode = (pickmode + 1) % 2;
		curFacePtr = 0;
		curVertPtr = 0;
		pIndex = 0;
		gla->update();
	}

	if ((e->key() == Qt::Key_Q) && ((curFacePtr != 0) || (curVertPtr != 0))) // tab to next selected mode
	{
		if (pickmode == 0)
		{
			pIndex = (pIndex + 1) % NewFaceSel.size();
			curFacePtr = NewFaceSel[pIndex];
		}
		else if (pickmode == 1)
		{
			pIndex = (pIndex + 1) % NewVertSel.size();
			curVertPtr = NewVertSel[pIndex];
		}
		gla->update();
	}

	if ((e->key() == Qt::Key_P) && ((curFacePtr != 0) || (curVertPtr != 0))) // tab to next selected mode
	{
		// print on log
		if (pickmode == 0)
		{
			this->Log(GLLogStream::FILTER, "------");
			this->Log(GLLogStream::FILTER, "face# %i : vert# (%i %i %i)", tri::Index(m.cm, curFacePtr), tri::Index(m.cm, curFacePtr->V(0)), tri::Index(m.cm, curFacePtr->V(1)), tri::Index(m.cm, curFacePtr->V(2)));
			if (m.hasDataMask(MeshModel::MM_FACECOLOR))
				this->Log(GLLogStream::FILTER, "face color (%i %i %i %i)", curFacePtr->C()[0], curFacePtr->C()[1], curFacePtr->C()[2], curFacePtr->C()[3]);
			for (int i = 0; i < 3; ++i)
			{
				this->Log(GLLogStream::FILTER, "face vert %i : vert# %i", i, tri::Index(m.cm, curFacePtr->V(i)));
				this->Log(GLLogStream::FILTER, "position [%f %f %f]", curFacePtr->V(i)->P()[0], curFacePtr->V(i)->P()[1], curFacePtr->V(i)->P()[2]);
				this->Log(GLLogStream::FILTER, "normal [%f %f %f]", curFacePtr->V(i)->N()[0], curFacePtr->V(i)->N()[1], curFacePtr->V(i)->N()[2]);
				if (m.hasDataMask(MeshModel::MM_VERTQUALITY))
					this->Log(GLLogStream::FILTER, "quality %f", curFacePtr->V(i)->Q());
				if (m.hasDataMask(MeshModel::MM_VERTCOLOR))
					this->Log(GLLogStream::FILTER, "color (%f %f %f %f)", curFacePtr->V(i)->C()[0], curFacePtr->V(i)->C()[1], curFacePtr->V(i)->C()[2], curFacePtr->V(i)->C()[3]);
				if (m.hasDataMask(MeshModel::MM_WEDGTEXCOORD))
					this->Log(GLLogStream::FILTER, "wedge UV (%f %f) texID %i)", curFacePtr->WT(i).U(), curFacePtr->WT(i).V(), curFacePtr->WT(i).N());
				if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD))
					this->Log(GLLogStream::FILTER, "vertex UV (%f %f) texID %i)", curFacePtr->V(i)->T().U(), curFacePtr->V(i)->T().U(), curFacePtr->V(i)->T().N());
			}
			this->Log(GLLogStream::FILTER, "------");
		}
		else if (pickmode == 1)
		{
			this->Log(GLLogStream::FILTER, "------");
			this->Log(GLLogStream::FILTER, "vertex# %i", tri::Index(m.cm, curVertPtr));
			this->Log(GLLogStream::FILTER, "position [%f %f %f]", curVertPtr->P()[0], curVertPtr->P()[1], curVertPtr->P()[2]);
			this->Log(GLLogStream::FILTER, "normal [%f %f %f]", curVertPtr->N()[0], curVertPtr->N()[1], curVertPtr->N()[2]);
			if (m.hasDataMask(MeshModel::MM_VERTQUALITY))
				this->Log(GLLogStream::FILTER, "quality %f", curVertPtr->Q());
			if (m.hasDataMask(MeshModel::MM_VERTCOLOR))
				this->Log(GLLogStream::FILTER, "color (%f %f %f %f)", curVertPtr->C()[0], curVertPtr->C()[1], curVertPtr->C()[2], curVertPtr->C()[3]);
			if (m.hasDataMask(MeshModel::MM_VERTTEXCOORD))
				this->Log(GLLogStream::FILTER, "vertex UV (%f %f) texID %i)", curVertPtr->T().U(), curVertPtr->T().U(), curVertPtr->T().N());
			this->Log(GLLogStream::FILTER, "------");
		}
		gla->update();
	}
}

bool SampleEditPlugin::StartEdit(MeshModel &/*m*/, GLArea * gla, MLSceneGLSharedDataContext* /*cont*/)
{
	gla->setCursor(QCursor(QPixmap(":/images/cur_info.png"),1,1));	
	connect(this, SIGNAL(suspendEditToggle()), gla, SLOT(suspendEditToggle()));
	return true;
}

void SampleEditPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/)
{
	haveToPick = false;
	pickmode = 0; // 0 face 1 vertex
	curFacePtr = 0;
	curVertPtr = 0;
	pIndex = 0;
}
