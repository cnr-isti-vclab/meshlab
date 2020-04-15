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

/*  A class implementing Meshlab's Edit interface that is for picking points in 3D
 *
 *
 * @author Oscar Barney
 */

#include <common/gl_defs.h>
#include <common/glu_defs.h>

#include "editpickpoints.h"
#include <meshlab/mainwindow.h>

#include <wrap/gl/picking.h>
#include <wrap/gl/pick.h>
#include <wrap/qt/gl_label.h>
#include <wrap/qt/trackball.h>



#include <math.h>

using namespace vcg;

#define PI 3.14159265


EditPickPointsPlugin::EditPickPointsPlugin()
{
	// initialize to false so we don't end up collecting some weird point in the beginning
	registerPoint = false;
	moveSelectPoint = false;

	pickPointsDialog = 0;
	currentModel = 0;

	overrideCursorShape = 0;
}

//Constants


const QString EditPickPointsPlugin::Info()
{
	return tr("Pick and save 3D points on the mesh");
}

//called
void EditPickPointsPlugin::Decorate(MeshModel &mm, GLArea *gla, QPainter *painter)
{
	//qDebug() << "Decorate " << mm.fileName.c_str() << " ..." << mm.cm.fn;

	if (gla != glArea || mm.cm.fn < 1)
	{
		//qDebug() << "GLarea is different or no faces!!! ";
		return;
	}


	//make sure we picking points on the right meshes!
	if (&mm != currentModel) {
		//now that were are ending tell the dialog to save any points it has to metadata
		pickPointsDialog->savePointsToMetaData();

		//set the new mesh model
		pickPointsDialog->setCurrentMeshModel(&mm, gla);
		currentModel = &mm;
	}

	//We have to calculate the position here because it doesnt work in the mouseEvent functions for some reason
	Point3m pickedPoint;

	if (moveSelectPoint)
	{
		/* qDebug("Found point for move %i %i -> %f %f %f",
				currentMousePosition.x(),
				currentMousePosition.y(),
				pickedPoint[0], pickedPoint[1], pickedPoint[2]); */

				//let the dialog know that this was the pointed picked in case it wants the information
		bool picked = Pick<Point3m>(currentMousePosition.x(), currentMousePosition.y(), pickedPoint);
		pickPointsDialog->selectOrMoveThisPoint(pickedPoint);

		moveSelectPoint = false;
	}
	else
	{
		if (registerPoint)
		{
			 /*qDebug("Found point for add %i %i -> %f %f %f",
					currentMousePosition.x(),
					currentMousePosition.y(),
					pickedPoint[0], pickedPoint[1], pickedPoint[2]); */


					//find the normal of the face we just clicked
			bool picked = Pick<Point3m>(currentMousePosition.x(), currentMousePosition.y(), pickedPoint);
			std::vector<CFaceO*> face;
			int result = GLPickTri<CMeshO>::PickVisibleFace(currentMousePosition.x(), currentMousePosition.y(), mm.cm, face);

			if ((result == 0) || (face[0] == NULL)) {
				qDebug() << "find nearest face failed!";
			}
			else
			{
				CFaceO::NormalType faceNormal = face[0]->N();
				//qDebug() << "found face normal: " << faceNormal[0] << faceNormal[1] << faceNormal[2];

				//if we didn't find a face then don't add the point because the user was probably
				//clicking on another mesh opened inside the glarea
				pickPointsDialog->addMoveSelectPoint(pickedPoint, faceNormal);
			}

			registerPoint = false;
		}
	}

	drawPickedPoints(pickPointsDialog->getPickedPointTreeWidgetItemVector(), mm.cm.bbox, painter);
}

bool EditPickPointsPlugin::StartEdit(MeshModel & mm, GLArea * gla, MLSceneGLSharedDataContext* /*cont*/)
{
	//qDebug() << "StartEdit Pick Points: " << mm.fileName.c_str() << " ..." << mm.cm.fn;

	//if there are no faces then we can't do anything with this plugin
	if (mm.cm.fn < 1)
	{
		if (NULL != pickPointsDialog)
		{
			pickPointsDialog->hide();
		}

		//show message
		QMessageBox::warning(gla->window(), "Edit Pick Points",
			"Sorry, this mesh has no faces on which picked points can sit.",
			QMessageBox::Ok, QMessageBox::Ok);
		return false;
	}

	//get the cursor
	QCursor *cursor = QApplication::overrideCursor();
	if (cursor) overrideCursorShape = cursor->shape();
	else overrideCursorShape = Qt::ArrowCursor;

	//set this so redraw can use it
	glArea = gla;

	//Create GUI window if we don't already have one
	if (pickPointsDialog == 0)
	{
		pickPointsDialog = new PickPointsDialog(this, gla->window());
	}

	currentModel = &mm;

	//set the current mesh
	pickPointsDialog->setCurrentMeshModel(&mm, gla);

	//show the dialog
	pickPointsDialog->show();
	return true;
}

void EditPickPointsPlugin::EndEdit(MeshModel & mm, GLArea * /*gla*/, MLSceneGLSharedDataContext* /*cont*/)
{
	//qDebug() << "EndEdit Pick Points: " << mm.fileName.c_str() << " ..." << mm.cm.fn;

	// some cleaning at the end.

	if ((mm.cm.fn > 0) && (pickPointsDialog != NULL))
	{
		//now that were are ending tell the dialog to save any points it has to metadata
		pickPointsDialog->savePointsToMetaData();

		//remove the dialog from the screen
		pickPointsDialog->hide();

		QApplication::setOverrideCursor(QCursor((Qt::CursorShape)overrideCursorShape));

		this->glArea = 0;
	}
}

void EditPickPointsPlugin::mousePressEvent(QMouseEvent *event, MeshModel &mm, GLArea *gla)
{
	//qDebug() << "mouse press Pick Points: " << mm.fileName.c_str() << " ...";

	//if there are no faces then we can't do anything with this plugin
	if (mm.cm.fn < 1) return;

	if (Qt::RightButton == event->button() &&
		pickPointsDialog->getMode() != PickPointsDialog::ADD_POINT) {

		currentMousePosition =  QPoint(QT2VCG_X(gla, event), QT2VCG_Y(gla, event));        

		pickPointsDialog->recordNextPointForUndo();

		//set flag that we need to add a new point
		moveSelectPoint = true;
	}
}

void EditPickPointsPlugin::mouseMoveEvent(QMouseEvent *event, MeshModel &mm, GLArea *gla)
{
	//qDebug() << "mousemove pick Points: " << mm.fileName.c_str() << " ...";

	//if there are no faces then we can't do anything with this plugin
	if (mm.cm.fn < 1) return;

	if (Qt::RightButton == event->button() &&
		pickPointsDialog->getMode() != PickPointsDialog::ADD_POINT) {

		//qDebug() << "mouse move left button and move mode: ";

		currentMousePosition = QPoint(QT2VCG_X(gla, event), QT2VCG_Y(gla, event)); //event->pos();

		//set flag that we need to add a new point
		registerPoint = true;
	}
}

void EditPickPointsPlugin::mouseReleaseEvent(QMouseEvent *event, MeshModel &mm, GLArea * gla)
{
	//qDebug() << "mouseRelease Pick Points: " << mm.fileName.c_str() << " ...";

	//if there are no faces then we can't do anything with this plugin
	if (mm.cm.fn < 1) return;

	//only add points for the left button
	if (Qt::RightButton == event->button()) { 
		currentMousePosition = QPoint(QT2VCG_X(gla, event), QT2VCG_Y(gla, event));//event->pos();

		//set flag that we need to add a new point
		registerPoint = true;
	}
}

void EditPickPointsPlugin::drawPickedPoints(
	std::vector<PickedPointTreeWidgetItem*> &pointVector, Box3m &boundingBox, QPainter *painter)
{
	assert(glArea);
	Point3m size = boundingBox.Dim();
	//how we scale the object indicating the normal at each selected point
	Scalarm scaleFactor = (size[0] + size[1] + size[2]) / 90.0;

	//qDebug() << "scaleFactor: " << scaleFactor;

	glPushAttrib(GL_ALL_ATTRIB_BITS);

	// enable color tracking
	glEnable(GL_COLOR_MATERIAL);

	//draw the things that we always want to show, like the names
	glDepthFunc(GL_ALWAYS);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	//set point attributes
	glPointSize(4.5);
	bool showNormal = pickPointsDialog->showNormal();
	bool showPin = pickPointsDialog->drawNormalAsPin();

	for (int i = 0; i < pointVector.size(); ++i)
	{
		PickedPointTreeWidgetItem * item = pointVector[i];
		//if the point has been set (it may not be if a template has been loaded)
		if (item->isActive()) {
			Point3m point = item->getPoint();
			glColor(vcg::Color4b(vcg::Color4b::Blue));
			glLabel::render(painter, point, QString(item->getName()));

			//draw the dot if we arnt showing the normal or showing the normal as a line
			if (!showNormal || !showPin)
			{
				if (item->isSelected()) glColor(vcg::Color4b(Color4b::Green));

				glBegin(GL_POINTS);
				glVertex(point);
				glEnd();
			}
		}
	}


	//now draw the things that we want drawn if they are not ocluded
	//we can see in bright red
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glMatrixMode(GL_MODELVIEW);

	Point3m yaxis(Scalarm(0), Scalarm(1), Scalarm(0));

	for (int i = 0; i < pointVector.size(); ++i)
	{
		PickedPointTreeWidgetItem * item = pointVector[i];
		//if the point has been set (it may not be if a template has been loaded)
		if (item->isActive()) {
			Point3m point = item->getPoint();

			if (showNormal)
			{
				Point3m normal = item->getNormal();

				if (showPin)
				{
					//dot product
					Scalarm angle = (Angle(normal, yaxis) * 180.0 / PI);

					//cross product
					Point3m axis = yaxis^normal;
					//qDebug() << "angle: " << angle << " x" << axis[0] << " y" << axis[1] << " z" << axis[2];

					//bluegreen and a little clear
					glColor4f(0.0f, 1.0f, 0.0f, 0.7f);
					//glColor(Color4b::Green);

					glPushMatrix();

					//move the pin to where it needs to be
					glTranslatef(point[0], point[1], point[2]);
					glRotatef(angle, axis[0], axis[1], axis[2]);
					glScalef(0.2*scaleFactor, 1.5*scaleFactor, 0.2*scaleFactor);

					glBegin(GL_TRIANGLES);

					//front
					glNormal3f(0, -1, 1);
					glVertex3f(0, 0, 0);
					glVertex3f(1, 1, 1);
					glVertex3f(-1, 1, 1);

					//right
					glNormal3f(1, -1, 0);
					glVertex3f(0, 0, 0);
					glVertex3f(1, 1, -1);
					glVertex3f(1, 1, 1);

					//left
					glNormal3f(-1, -1, 0);
					glVertex3f(0, 0, 0);
					glVertex3f(-1, 1, 1);
					glVertex3f(-1, 1, -1);

					//back
					glNormal3f(0, -1, -1);
					glVertex3f(0, 0, 0);
					glVertex3f(-1, 1, -1);
					glVertex3f(1, 1, -1);

					//top

					//if it is selected color it green
					if (item->isSelected())	glColor4f(0.0f, 0.0f, 1.0f, 0.7f);


					glNormal3f(0, 1, 0);
					glVertex3f(1, 1, 1);
					glVertex3f(1, 1, -1);
					glVertex3f(-1, 1, -1);

					glNormal3f(0, 1, 0);
					glVertex3f(1, 1, 1);
					glVertex3f(-1, 1, -1);
					glVertex3f(-1, 1, 1);

					//change back
					if (item->isSelected())	glColor4f(0.0f, 1.0f, 0.0f, 0.7f);

					glEnd();
					glPopMatrix();
				}
				else
				{
					glColor(vcg::Color4b(vcg::Color4b::Green));

					glBegin(GL_LINES);
					glVertex(point);
					glVertex(point + (normal*scaleFactor));
					glEnd();
				}
			}
			glColor(vcg::Color4b(vcg::Color4b::Red));
			//glArea->renderText(point[0], point[1], point[2], QString(item->getName()) );
		}
	}
	glDisable(GL_BLEND);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_DEPTH_TEST);
	glPopAttrib();

}
