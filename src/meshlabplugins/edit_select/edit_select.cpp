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

#include "edit_select.h"
#include <wrap/gl/pick.h>
#include <wrap/qt/device_to_logical.h>
#include <meshlab/glarea.h>


using namespace std;
using namespace vcg;

EditSelectPlugin::EditSelectPlugin(int ConnectedMode) :selectionMode(ConnectedMode) {
	isDragging = false;
}

QString EditSelectPlugin::Info()
{
	return tr("Interactive selection inside a dragged rectangle in screen space");
}

void EditSelectPlugin::suggestedRenderingData(MeshModel & m, MLRenderingData & dt)
{
	MLPerViewGLOptions opts;
	dt.get(opts);
	opts._sel_enabled = true;

	if ((selectionMode == SELECT_FACE_MODE) || (selectionMode == SELECT_CONN_MODE))
		opts._face_sel = true;

	if (selectionMode == SELECT_VERT_MODE)
		opts._vertex_sel = true;
	dt.set(opts);
}

void EditSelectPlugin::keyReleaseEvent(QKeyEvent *, MeshModel &/*m*/, GLArea *gla)
{
	gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"), 1, 1));
}

void EditSelectPlugin::keyPressEvent(QKeyEvent */*event*/, MeshModel &/*m*/, GLArea *gla)
{
	Qt::KeyboardModifiers mod = QApplication::queryKeyboardModifiers();
	if (mod == Qt::AltModifier && selectionMode != SELECT_VERT_MODE)
		gla->setCursor(QCursor(QPixmap(":/images/sel_rect_eye.png"), 1, 1));
	if (mod == (Qt::AltModifier + Qt::ControlModifier))
	{
		if (selectionMode != SELECT_VERT_MODE)
			gla->setCursor(QCursor(QPixmap(":/images/sel_rect_plus_eye.png"), 1, 1));
		else gla->setCursor(QCursor(QPixmap(":/images/sel_rect_plus.png"), 1, 1));
	}
	if (mod == (Qt::AltModifier + Qt::ShiftModifier))
	{
		if (selectionMode != SELECT_VERT_MODE)
			gla->setCursor(QCursor(QPixmap(":/images/sel_rect_minus_eye.png"), 1, 1));
		else gla->setCursor(QCursor(QPixmap(":/images/sel_rect_minus.png"), 1, 1));
	}
	if (mod == Qt::ControlModifier)
		gla->setCursor(QCursor(QPixmap(":/images/sel_rect_plus.png"), 1, 1));
	if (mod == Qt::ShiftModifier)
		gla->setCursor(QCursor(QPixmap(":/images/sel_rect_minus.png"), 1, 1));
}

void EditSelectPlugin::mousePressEvent(QMouseEvent * event, MeshModel &m, GLArea *gla)
{
	LastSelVert.clear();
	LastSelFace.clear();

	if ((event->modifiers() & Qt::ControlModifier) ||
		(event->modifiers() & Qt::ShiftModifier))
	{
		CMeshO::FaceIterator fi;
		for (fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
			if (!(*fi).IsD() && (*fi).IsS())
				LastSelFace.push_back(&*fi);

		CMeshO::VertexIterator vi;
		for (vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
			if (!(*vi).IsD() && (*vi).IsS())
				LastSelVert.push_back(&*vi);
	}

	composingSelMode = SMClear;
	if (event->modifiers() & Qt::ControlModifier)
		composingSelMode = SMAdd;
	if (event->modifiers() & Qt::ShiftModifier)
		composingSelMode = SMSub;

	if (event->modifiers() & Qt::AltModifier)
		selectFrontFlag = true;
	else
		selectFrontFlag = false;

	start = QTLogicalToOpenGL(gla, event->pos());
	cur = start;
	return;
}

void EditSelectPlugin::mouseMoveEvent(QMouseEvent * event, MeshModel & m, GLArea * gla)
{
	prev = cur;
	cur = QTLogicalToOpenGL(gla, event->pos());
	isDragging = true;
	gla->update();

	//    // to avoid too frequent rendering
	//    if(gla->lastRenderingTime() < 200 )
	//    {
	//    }
	//    else{
	//      gla->makeCurrent();
	//      glDrawBuffer(GL_FRONT);
	//      DrawXORRect(gla,true);
	//      glDrawBuffer(GL_BACK);
	//      glFlush();
	//    }
}

void EditSelectPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
	//gla->update();
	if (gla == NULL)
		return;

	gla->updateAllSiblingsGLAreas();
	prev = cur;
	cur = QTLogicalToOpenGL(gla, event->pos());
	isDragging = false;
}

void EditSelectPlugin::DrawXORRect(GLArea * gla, bool doubleDraw)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, QTDeviceWidth(gla), 0, QTDeviceHeight(gla), -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_XOR);
	glColor3f(1, 1, 1);
	if (doubleDraw)
	{
		glBegin(GL_LINE_LOOP);
		glVertex(start);
		glVertex2f(prev.X(), start.Y());
		glVertex(prev);
		glVertex2f(start.X(), prev.Y());
		glEnd();
	}
	glBegin(GL_LINE_LOOP);
	glVertex(start);
	glVertex2f(cur.X(), start.Y());
	glVertex(cur);
	glVertex2f(start.X(), cur.Y());
	glEnd();
	glDisable(GL_LOGIC_OP);

	// Closing 2D
	glPopAttrib();
	glPopMatrix(); // restore modelview
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}

void EditSelectPlugin::Decorate(MeshModel &m, GLArea * gla)
{
	if (isDragging)
	{
		DrawXORRect(gla, false);
		vector<CMeshO::FacePointer>::iterator fpi;
		// Starting Sel
		vector<CMeshO::FacePointer> NewSelFace;
		Point2f mid = (start + cur) / 2;
		Point2f wid = vcg::Abs(start - cur);

		glPushMatrix();
		glMultMatrix(m.cm.Tr);
		if (selectionMode == SELECT_VERT_MODE)
		{
			//m.cm.selvert.clear();
			vector<CMeshO::VertexPointer> NewSelVert;
			vector<CMeshO::VertexPointer>::iterator vpi;

			GLPickTri<CMeshO>::PickVert(mid[0], mid[1], m.cm, NewSelVert, wid[0], wid[1]);
			glPopMatrix();
			tri::UpdateSelection<CMeshO>::VertexClear(m.cm);

			switch (composingSelMode)
			{
			case SMSub:  // Subtract mode : The faces in the rect must be de-selected
				for (vpi = LastSelVert.begin(); vpi != LastSelVert.end(); ++vpi)
					(*vpi)->SetS();
				for (vpi = NewSelVert.begin(); vpi != NewSelVert.end(); ++vpi)
					(*vpi)->ClearS();
				break;
			case SMAdd:  // Subtract mode : The faces in the rect must be de-selected
				for (vpi = LastSelVert.begin(); vpi != LastSelVert.end(); ++vpi)
					(*vpi)->SetS();
			case SMClear:  // Subtract mode : The faces in the rect must be de-selected
				for (vpi = NewSelVert.begin(); vpi != NewSelVert.end(); ++vpi)
					(*vpi)->SetS();
				break;
			}
			//for (unsigned int ii = 0; ii < m.cm.VN(); ++ii)
			//{
			//	CVertexO& vv = m.cm.vert[ii];
			//	if (!vv.IsD() && vv.IsS())
			//	{
			//		m.cm.selvert.push_back(Point3m::Construct(vv.cP()));
			//		++m.cm.svn;
			//	}
			//}
			gla->updateSelection(m.id(), true,false);
		}
		else
		{
			//m.cm.selface.clear();
			if (selectFrontFlag)	GLPickTri<CMeshO>::PickVisibleFace(mid[0], mid[1], m.cm, NewSelFace, wid[0], wid[1]);
			else                GLPickTri<CMeshO>::PickFace(mid[0], mid[1], m.cm, NewSelFace, wid[0], wid[1]);

			//    qDebug("Pickface: rect %i %i - %i %i",mid.x(),mid.y(),wid.x(),wid.y());
			//    qDebug("Pickface: Got  %i on %i",int(NewSelFace.size()),int(m.cm.face.size()));
			glPopMatrix();
			tri::UpdateSelection<CMeshO>::FaceClear(m.cm);
			switch (composingSelMode)
			{
			case SMSub:  // Subtract mode : The faces in the rect must be de-selected
				if (selectionMode == SELECT_CONN_MODE)
				{
					for (fpi = NewSelFace.begin(); fpi != NewSelFace.end(); ++fpi)
						(*fpi)->SetS();
					tri::UpdateSelection<CMeshO>::FaceConnectedFF(m.cm);
					NewSelFace.clear();
					for (CMeshO::FaceIterator fi = m.cm.face.begin(); fi != m.cm.face.end(); ++fi)
						if (!(*fi).IsD() && (*fi).IsS()) NewSelFace.push_back(&*fi);
				}
				// Normal case: simply deselect what has been selected.
				for (fpi = LastSelFace.begin(); fpi != LastSelFace.end(); ++fpi)
					(*fpi)->SetS();
				for (fpi = NewSelFace.begin(); fpi != NewSelFace.end(); ++fpi)
					(*fpi)->ClearS();
				break;
			case SMAdd:
				for (fpi = LastSelFace.begin(); fpi != LastSelFace.end(); ++fpi)
					(*fpi)->SetS();
			case SMClear:
				for (fpi = NewSelFace.begin(); fpi != NewSelFace.end(); ++fpi)
					(*fpi)->SetS();
				if (selectionMode == SELECT_CONN_MODE)
					tri::UpdateSelection<CMeshO>::FaceConnectedFF(m.cm);
				break;
			}
			gla->updateSelection(m.id(), false, true);
			isDragging = false;
		}

	}
}

bool EditSelectPlugin::StartEdit(MeshModel & m, GLArea * gla, MLSceneGLSharedDataContext* /*cont*/)
{
	if (gla == NULL)
		return false;
	GLenum err = glewInit();

	if (err != GLEW_OK)
		return false;
	gla->setCursor(QCursor(QPixmap(":/images/sel_rect.png"), 1, 1));

	//connect(this, SIGNAL(setDecorator(QString, bool)), gla, SLOT(setDecorator(QString, bool)));

	//setDecorator("Show Selected Faces", true);
	//setDecorator("Show Selected Vertices", true);

	if (selectionMode == SELECT_CONN_MODE)
		m.updateDataMask(MeshModel::MM_FACEFACETOPO);
	return true;
}
