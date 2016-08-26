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

#include <meshlab/glarea.h>
#include "edit_manipulators.h"
#include <wrap/qt/gl_label.h>
#include <wrap/gui/trackball.h>


using namespace vcg;

EditManipulatorsPlugin::EditManipulatorsPlugin()
{
  current_manip = EditManipulatorsPlugin::ManNone;
  current_manip_mode = EditManipulatorsPlugin::ModNone;
  isMoving = false;
  isSnapping = false;
  aroundOrigin = true;
  snapto = 1;
  resetOffsets();
  inputnumberstring= "";
  inputnumbernegative = false;
  inputnumber=0;

  original_Transform = Matrix44m::Identity();
  delta_Transform = Matrix44m::Identity();
}

const QString EditManipulatorsPlugin::Info() 
{
	return tr("Provide tools for moving meshes around the space");
}

void EditManipulatorsPlugin::mousePressEvent(QMouseEvent *event, MeshModel &, GLArea * gla)
{
  isMoving = true;
  startdrag = Point2i(event->x(),event->y());
  gla->update();
}

void EditManipulatorsPlugin::mouseMoveEvent(QMouseEvent * event, MeshModel &model, GLArea * gla)
{
  if(isMoving)
  {
    enddrag = Point2i(event->x(),event->y());
    currScreenOffset_X = enddrag[0] - startdrag[0];
    currScreenOffset_Y = enddrag[1] - startdrag[1];
    UpdateMatrix(model, gla, false);
  }
  gla->update();
}

void EditManipulatorsPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &model, GLArea * gla)
{
  if(isMoving)
  {
    isMoving = false;
    enddrag = Point2i(event->x(),event->y());
    currScreenOffset_X = enddrag[0] - startdrag[0];
    currScreenOffset_Y = enddrag[1] - startdrag[1];
    UpdateMatrix(model, gla, true);
  }
  gla->update();
}

// Apply movement
void EditManipulatorsPlugin::applyMotion(MeshModel &model, GLArea *gla)
{
  // the current matrix already contains the motion... so, just keep it there

  // reset filter data
  current_manip = EditManipulatorsPlugin::ManNone;
  current_manip_mode = EditManipulatorsPlugin::ModNone;
  isMoving = false;
  isSnapping = false;
  aroundOrigin = true;
  snapto = 1.0;
  resetOffsets();

  inputnumberstring= "";
  inputnumbernegative = false;
  inputnumber=0;

  // storing start matrix
  original_Transform = model.cm.Tr;
  delta_Transform = Matrix44m::Identity();

  gla->update();
}

// Cancel movement
void EditManipulatorsPlugin::cancelMotion(MeshModel &model, GLArea *gla)
{
  //restoring old matrix
  model.cm.Tr = original_Transform;

  // reset filter data
  current_manip = EditManipulatorsPlugin::ManNone;
  current_manip_mode = EditManipulatorsPlugin::ModNone;
  isMoving = false;
  isSnapping = false;
  aroundOrigin = true;
  snapto=1.0;
  resetOffsets();

  inputnumberstring= "";
  inputnumbernegative = false;
  inputnumber=0;

  // storing start matrix
  original_Transform = model.cm.Tr;
  delta_Transform = Matrix44m::Identity();

  gla->update();
}

// keyboard press
void EditManipulatorsPlugin::keyPressEvent(QKeyEvent* e, MeshModel& /*model*/, GLArea* /*gla*/)
{
  // shift pressed, entering snapping mode
  if(e->key() == Qt::Key_Shift)
  {
    isSnapping = true;
  }
}

// keyboard commands, just like Blender
void EditManipulatorsPlugin::keyReleaseEvent(QKeyEvent *e, MeshModel &model, GLArea *gla)
{
  // enter will apply, backspace to cancel
  if(current_manip != EditManipulatorsPlugin::ManNone)
  {
	if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return))
    {
      applyMotion(model, gla);
    }
	if (e->key() == Qt::Key_Backspace) 
    {
      cancelMotion(model, gla);
    }
  }

  // shift released, exit snapping mode
  if(e->key() == Qt::Key_Shift)
  {
    isSnapping = false;
  }

  if((current_manip == EditManipulatorsPlugin::ManRotate) || (current_manip == EditManipulatorsPlugin::ManScale))
  {
    if(e->key() == Qt::Key_Space)
    {
      aroundOrigin = !aroundOrigin;
    }
  }

  if(current_manip == EditManipulatorsPlugin::ManNone)   // if no active manipulator, listen to T R S to select one
  {
	  if ((e->key() == Qt::Key_T) || (e->key() == Qt::Key_G)) // translate (I added also G to work like blender)
    {
      current_manip = EditManipulatorsPlugin::ManMove;
      resetOffsets();
      UpdateMatrix(model,gla,false);
    }
	  if (e->key() == Qt::Key_R) // rotate
    {
      current_manip = EditManipulatorsPlugin::ManRotate;
      resetOffsets();
      UpdateMatrix(model,gla,false);
    }
    if (e->key() == Qt::Key_S) // scale
    {
      current_manip = EditManipulatorsPlugin::ManScale;
      resetOffsets();
      UpdateMatrix(model,gla,false);
    }
  }

  if(current_manip != EditManipulatorsPlugin::ManNone)   // if there is an active manipulator, listen to modifiers
  {
	  if (e->key() == Qt::Key_X) // X axis
    {
      if(current_manip_mode == EditManipulatorsPlugin::ModX)
        current_manip_mode = EditManipulatorsPlugin::ModXX;
      else
        current_manip_mode = EditManipulatorsPlugin::ModX;

      resetOffsets();

      UpdateMatrix(model,gla,false);
    }
	  if (e->key() == Qt::Key_Y) // Y axis
    {
      if(current_manip_mode == EditManipulatorsPlugin::ModY)
        current_manip_mode = EditManipulatorsPlugin::ModYY;
      else
        current_manip_mode = EditManipulatorsPlugin::ModY;

      resetOffsets();

      UpdateMatrix(model,gla,false);
    }
	  if (e->key() == Qt::Key_Z) // Z axis
    {
      if(current_manip_mode == EditManipulatorsPlugin::ModZ)
        current_manip_mode = EditManipulatorsPlugin::ModZZ;
      else
        current_manip_mode = EditManipulatorsPlugin::ModZ;

      resetOffsets();

      UpdateMatrix(model,gla,false);
    }
  }
	
  // numerical input
  if(current_manip_mode != EditManipulatorsPlugin::ModNone)  // transform on one axis only
  {
	bool hasNumberChanged = false;

	if (e->key() == Qt::Key_1) { inputnumberstring += "1"; hasNumberChanged = true; }
	if (e->key() == Qt::Key_2) { inputnumberstring += "2"; hasNumberChanged = true; }
	if (e->key() == Qt::Key_3) { inputnumberstring += "3"; hasNumberChanged = true; }
	if (e->key() == Qt::Key_4) { inputnumberstring += "4"; hasNumberChanged = true; }
	if (e->key() == Qt::Key_5) { inputnumberstring += "5"; hasNumberChanged = true; }
	if (e->key() == Qt::Key_6) { inputnumberstring += "6"; hasNumberChanged = true; }
	if (e->key() == Qt::Key_7) { inputnumberstring += "7"; hasNumberChanged = true; }
	if (e->key() == Qt::Key_8) { inputnumberstring += "8"; hasNumberChanged = true; }
	if (e->key() == Qt::Key_9) { inputnumberstring += "9"; hasNumberChanged = true; }

    if (e->key() == Qt::Key_0) 
    {
      if(inputnumberstring.length() == 0)
        inputnumberstring = "0.";
      if((inputnumberstring.length() >= 2) || (inputnumberstring[0]!=0))
        inputnumberstring += "0";

	  hasNumberChanged = true;
    }  

    if (e->key() == Qt::Key_Period)
    {
      if(inputnumberstring.length() == 0)
        inputnumberstring = "0.";
      else if(!inputnumberstring.contains("."))
        inputnumberstring += ".";

	  hasNumberChanged = true;
    }  

	if (e->key() == Qt::Key_Minus)
    {
		inputnumbernegative = !inputnumbernegative;
		hasNumberChanged = true;
	}

	if (!inputnumbernegative)
      inputnumber = inputnumberstring.toFloat();
	else
      inputnumber = -inputnumberstring.toFloat();

	if (hasNumberChanged)
		UpdateMatrix(model, gla, false, true);
  }

  //else e->ignore();
  gla->update();
}

void EditManipulatorsPlugin::resetOffsets()
{
  if(current_manip == EditManipulatorsPlugin::ManScale)
  {
    displayOffset = 1;        // mouse offset value (single axis)
    displayOffset_X = 1;      // mouse X offset value
    displayOffset_Y = 1;      // mouse Y offset value
    displayOffset_Z = 1;      // mouse Z offset value
    currOffset = 1;           // combined offset value (single axis)
    currOffset_X = 1;         // X offset value
    currOffset_Y = 1;         // Y offset value
    currOffset_Z = 1;         // Z offset value
  }
  else
  {
    displayOffset = 0;        // mouse offset value (single axis)
    displayOffset_X = 0;      // mouse X offset value
    displayOffset_Y = 0;      // mouse Y offset value
    displayOffset_Z = 0;      // mouse Z offset value
    currOffset = 0;           // combined offset value (single axis)
    currOffset_X = 0;         // X offset value
    currOffset_Y = 0;         // Y offset value
    currOffset_Z = 0;         // Z offset value
  }

  currScreenOffset_X = 0;   // horizontal offset (screen space)
  currScreenOffset_Y = 0;   // vertical offset (screen space)
}

void EditManipulatorsPlugin::DrawMeshBox(MeshModel &model)
{
    Box3m b;
	b = model.cm.bbox;
	Point3m mi=b.min;
	Point3m ma=b.max;
	Point3m d3=(b.max-b.min)/4.0;
	Point3m zz(0,0,0);

  // setup
    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);
    glColor(vcg::Color4b(Color4b::Cyan));

    glPushMatrix();
    glMultMatrix(original_Transform);

	glBegin(GL_LINES);
	glColor3f(1.0, 0.5, 0.5); glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+d3[0],mi[1]+zz[1],mi[2]+zz[2]);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+zz[0],mi[1]+d3[1],mi[2]+zz[2]);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(mi[0],mi[1],mi[2]); glVertex3f(mi[0]+zz[0],mi[1]+zz[1],mi[2]+d3[2]);

	glColor3f(1.0, 0.5, 0.5); glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]-d3[0],mi[1]+zz[1],mi[2]+zz[2]);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]+zz[0],mi[1]+d3[1],mi[2]+zz[2]);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(ma[0],mi[1],mi[2]); glVertex3f(ma[0]+zz[0],mi[1]+zz[1],mi[2]+d3[2]);

	glColor3f(1.0, 0.5, 0.5); glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+d3[0],ma[1]+zz[1],mi[2]+zz[2]);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+zz[0],ma[1]-d3[1],mi[2]+zz[2]);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(mi[0],ma[1],mi[2]); glVertex3f(mi[0]+zz[0],ma[1]+zz[1],mi[2]+d3[2]);

	glColor3f(1.0, 0.5, 0.5); glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]-d3[0],ma[1]+zz[1],mi[2]+zz[2]);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]+zz[0],ma[1]-d3[1],mi[2]+zz[2]);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(ma[0],ma[1],mi[2]); glVertex3f(ma[0]+zz[0],ma[1]+zz[1],mi[2]+d3[2]);

	glColor3f(1.0, 0.5, 0.5); glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+d3[0],mi[1]+zz[1],ma[2]+zz[2]);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+zz[0],mi[1]+d3[1],ma[2]+zz[2]);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(mi[0],mi[1],ma[2]); glVertex3f(mi[0]+zz[0],mi[1]+zz[1],ma[2]-d3[2]);

	glColor3f(1.0, 0.5, 0.5); glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]-d3[0],mi[1]+zz[1],ma[2]+zz[2]);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]+zz[0],mi[1]+d3[1],ma[2]+zz[2]);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(ma[0],mi[1],ma[2]); glVertex3f(ma[0]+zz[0],mi[1]+zz[1],ma[2]-d3[2]);

	glColor3f(1.0, 0.5, 0.5); glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+d3[0],ma[1]+zz[1],ma[2]+zz[2]);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+zz[0],ma[1]-d3[1],ma[2]+zz[2]);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(mi[0],ma[1],ma[2]); glVertex3f(mi[0]+zz[0],ma[1]+zz[1],ma[2]-d3[2]);

	glColor3f(1.0, 0.5, 0.5); glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]-d3[0],ma[1]+zz[1],ma[2]+zz[2]);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]+zz[0],ma[1]-d3[1],ma[2]+zz[2]);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(ma[0],ma[1],ma[2]); glVertex3f(ma[0]+zz[0],ma[1]+zz[1],ma[2]-d3[2]);
	glEnd();


  //drawing mesh origin
    glBegin(GL_LINES);
    glColor3f(1.0, 0.5, 0.5); glVertex3f(-2.0*d3[0], 0.0, 0.0); glVertex3f(2.0*d3[0],0.0,0.0);
	glColor3f(0.5, 1.0, 0.5); glVertex3f(0.0, -2.0*d3[1], 0.0); glVertex3f(0.0, 2.0*d3[1], 0.0);
	glColor3f(0.5, 0.5, 1.0); glVertex3f(0.0, 0.0, -2.0*d3[2]); glVertex3f(0.0, 0.0, 2.0*d3[2]);
	glEnd();

  // restore
  glPopMatrix();
  glPopAttrib();
}


//----------------------------------------------------------------------------------

void EditManipulatorsPlugin::DrawCubes(float r, float g, float b)
{
  glColor4f(r,g,b,1.0f);
  glBegin (GL_LINES);
      // mid line
    glVertex3f( 0.0f,  0.0f, -1.0f);
    glVertex3f( 0.0f,  0.0f,  1.0f);
  glEnd ();

  glBegin (GL_LINES);
      // right cube
    glVertex3f(  0.0f,  0.0f,  1.0f);
    glVertex3f(  0.1f,  0.0f,  1.1f);
    glVertex3f(  0.0f,  0.0f,  1.0f);
    glVertex3f( -0.1f,  0.0f,  1.1f);
    glVertex3f(  0.0f,  0.0f,  1.0f);
    glVertex3f(  0.0f, -0.1f,  1.1f);
    glVertex3f(  0.0f,  0.0f,  1.0f);
    glVertex3f(  0.0f,  0.1f,  1.1f);
    glVertex3f(  0.0f,  0.0f,  1.2f);
    glVertex3f(  0.1f,  0.0f,  1.1f);
    glVertex3f(  0.0f,  0.0f,  1.2f);
    glVertex3f( -0.1f,  0.0f,  1.1f);
    glVertex3f(  0.0f,  0.0f,  1.2f);
    glVertex3f(  0.0f, -0.1f,  1.1f);
    glVertex3f(  0.0f,  0.0f,  1.2f);
    glVertex3f(  0.0f,  0.1f,  1.1f);
  glEnd ();

  glBegin (GL_LINES);
      // left cube
    glVertex3f(  0.0f,  0.0f, -1.0f);
    glVertex3f(  0.1f,  0.0f, -1.1f);
    glVertex3f(  0.0f,  0.0f, -1.0f);
    glVertex3f( -0.1f,  0.0f, -1.1f);
    glVertex3f(  0.0f,  0.0f, -1.0f);
    glVertex3f(  0.0f, -0.1f, -1.1f);
    glVertex3f(  0.0f,  0.0f, -1.0f);
    glVertex3f(  0.0f,  0.1f, -1.1f);
    glVertex3f(  0.0f,  0.0f, -1.2f);
    glVertex3f(  0.1f,  0.0f, -1.1f);
    glVertex3f(  0.0f,  0.0f, -1.2f);
    glVertex3f( -0.1f,  0.0f, -1.1f);
    glVertex3f(  0.0f,  0.0f, -1.2f);
    glVertex3f(  0.0f, -0.1f, -1.1f);
    glVertex3f(  0.0f,  0.0f, -1.2f);
    glVertex3f(  0.0f,  0.1f, -1.1f);
  glEnd ();


    // right cube
  glColor4f(std::min(1.0f,r+0.2f), std::min(1.0f,g+0.2f), std::min(1.0f,b+0.2f),0.5f);
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0f,  0.0f,  1.2f);
    glVertex3f(  0.0f,  0.1f,  1.1f);
    glVertex3f( -0.1f,  0.0f,  1.1f);
    glVertex3f(  0.0f, -0.1f,  1.1f);
    glVertex3f(  0.1f,  0.0f,  1.1f);
    glVertex3f(  0.0f,  0.1f,  1.1f);
  glEnd();
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0f,  0.0f,  1.0f);
    glVertex3f(  0.0f,  0.1f,  1.1f);
    glVertex3f( -0.1f,  0.0f,  1.1f);
    glVertex3f(  0.0f, -0.1f,  1.1f);
    glVertex3f(  0.1f,  0.0f,  1.1f);
    glVertex3f(  0.0f,  0.1f,  1.1f);
  glEnd();

      // left cube
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0f,  0.0f, -1.2f);
    glVertex3f(  0.0f,  0.1f, -1.1f);
    glVertex3f( -0.1f,  0.0f, -1.1f);
    glVertex3f(  0.0f, -0.1f, -1.1f);
    glVertex3f(  0.1f,  0.0f, -1.1f);
    glVertex3f(  0.0f,  0.1f, -1.1f);
  glEnd();
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0f,  0.0f, -1.0f);
    glVertex3f(  0.0f,  0.1f, -1.1f);
    glVertex3f( -0.1f,  0.0f, -1.1f);
    glVertex3f(  0.0f, -0.1f, -1.1f);
    glVertex3f(  0.1f,  0.0f, -1.1f);
    glVertex3f(  0.0f,  0.1f, -1.1f);
  glEnd();
}

void EditManipulatorsPlugin::DrawArrows(float r, float g, float b)
{
  glColor4f(r,g,b,1.0f);
  glBegin (GL_LINES);
      // mid line
    glVertex3f( 0.0f,  0.0f, -1.1f);
    glVertex3f( 0.0f,  0.0f,  1.1f);

      // right arrow
    glVertex3f(  0.0f,  0.0f,  1.1f);
    glVertex3f(  0.1f,  0.1f,  0.9f);
    glVertex3f(  0.0f,  0.0f,  1.1f);
    glVertex3f( -0.1f,  0.1f,  0.9f);
    glVertex3f(  0.0f,  0.0f,  1.1f);
    glVertex3f(  0.1f, -0.1f,  0.9f);
    glVertex3f(  0.0f,  0.0f,  1.1f);
    glVertex3f( -0.1f, -0.1f,  0.9f);

      // left arrow
    glVertex3f(  0.0f,  0.0f, -1.1f);
    glVertex3f(  0.1f,  0.1f, -0.9f);
    glVertex3f(  0.0f,  0.0f, -1.1f);
    glVertex3f( -0.1f,  0.1f, -0.9f);
    glVertex3f(  0.0f,  0.0f, -1.1f);
    glVertex3f(  0.1f, -0.1f, -0.9f);
    glVertex3f(  0.0f,  0.0f, -1.1f);
    glVertex3f( -0.1f, -0.1f, -0.9f);
  glEnd ();

    // right arrow
  glColor4f(std::min(1.0f,r+0.2f), std::min(1.0f,g+0.2f), std::min(1.0f,b+0.2f), 0.5f);
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0f,  0.0f,  1.1f);
    glVertex3f(  0.1f,  0.1f,  0.9f);
    glVertex3f( -0.1f,  0.1f,  0.9f);
    glVertex3f( -0.1f, -0.1f,  0.9f);
    glVertex3f(  0.1f, -0.1f,  0.9f);
    glVertex3f(  0.1f,  0.1f,  0.9f);
  glEnd();
      // left arrow
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0f,  0.0f, -1.1f);
    glVertex3f(  0.1f,  0.1f, -0.9f);
    glVertex3f( -0.1f,  0.1f, -0.9f);
    glVertex3f( -0.1f, -0.1f, -0.9f);
    glVertex3f(  0.1f, -0.1f, -0.9f);
    glVertex3f(  0.1f,  0.1f, -0.9f);
  glEnd();
}

void EditManipulatorsPlugin::DrawCircle(float r, float g, float b)
{
  int nside =32;
  const double pi2 = 3.14159265 * 2.0;

  glColor4f(r,g,b,1.0);
  glBegin (GL_LINE_LOOP);
  for (double i = 0; i < nside; i++) {
    glNormal3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
    glVertex3d (cos (i * pi2 / nside), sin (i * pi2 / nside), 0.0);
  }
  glEnd();
  glColor4f(std::min(1.0f,r+0.2f), std::min(1.0f,g+0.2f), std::min(1.0f,b+0.2f), 0.5f);
  glBegin (GL_TRIANGLE_FAN);
  glVertex3d (0.0, 0.0, 0.0);
  int renderangle;
  if (displayOffset>=0)
   renderangle = int(displayOffset)%360;
  else
   renderangle = 360 - (int(-displayOffset)%360);

  for (double i = 0; i<=renderangle; i++) 
  {
    glVertex3d (cos (i * pi2 / 360.0), sin (i * pi2 / 360.0), 0.0);
  }
  glEnd();


}

//----------------------------------------------------------------------------------

void  EditManipulatorsPlugin::DrawTranslateManipulators(MeshModel &model, GLArea *gla)
{  
  glPushMatrix();

  Point3m mesh_boxcenter, mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis, new_mesh_origin;
  mesh_boxcenter = original_Transform * model.cm.bbox.Center();
  mesh_origin = original_Transform.GetColumn3(3);
  new_mesh_origin = model.cm.Tr.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);
  float manipsize = model.cm.bbox.Diag() / 2.0;
  Matrix44f track_rotation;
  gla->trackball.track.rot.ToMatrix(track_rotation);

  glLineWidth(2.0);

  switch(current_manip_mode) 
  {
    case EditManipulatorsPlugin::ModNone:
      glTranslate(new_mesh_origin);      
      glScale(manipsize);
      glMultMatrix(Inverse(track_rotation));
	    glRotatef (90.0f, 0.0f, 1.0f, 0.0f);
	    DrawArrows(1.0f,0.8f,0.5f);
	    glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
	    DrawArrows(1.0f,0.8f,0.5f);
      break;
    case EditManipulatorsPlugin::ModX:
      glTranslate(new_mesh_origin);
      glScale(manipsize);
	    glRotatef (90.0f, 0.0f, 1.0f, 0.0f);
	    DrawArrows(1.0f,0.0f,0.0f);
      break;
    case EditManipulatorsPlugin::ModY:
      glTranslate(new_mesh_origin);
      glScale(manipsize);
	    glRotatef (90, 1, 0, 0);
	    DrawArrows(0,1.0,0);
      break;
    case EditManipulatorsPlugin::ModZ:
      glTranslate(new_mesh_origin);
      glScale(manipsize);
	    DrawArrows(0,0,1.0);
      break;
    case EditManipulatorsPlugin::ModXX:
      glMultMatrix(model.cm.Tr);
      glScale(manipsize);
	    glRotatef (90, 0, 1, 0);
	    DrawArrows(1.0,0.5,0.5);
      break;
    case EditManipulatorsPlugin::ModYY:
      glMultMatrix(model.cm.Tr);
      glScale(manipsize);
	    glRotatef (90, 1, 0, 0);
	    DrawArrows(0.5,1.0,0.5);
      break;
    case EditManipulatorsPlugin::ModZZ:
      glMultMatrix(model.cm.Tr);
      glScale(manipsize);
	    DrawArrows(0.5,0.5,1.0);
      break;
    default: ;
  }
	
  glLineWidth(1.0);
	glPopMatrix();
}

void  EditManipulatorsPlugin::DrawScaleManipulators(MeshModel &model, GLArea *gla)
{  
  glPushMatrix();

  Point3m mesh_boxcenter, mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis;
  mesh_boxcenter = original_Transform * model.cm.bbox.Center();
  mesh_origin = original_Transform.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);
  float manipsize(model.cm.bbox.Diag() / 2.0);
  Matrix44f original_rot = original_Transform;
  original_rot.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
  Matrix44f track_rotation;
  gla->trackball.track.rot.ToMatrix(track_rotation);

  glLineWidth(2.0);
	
  switch(current_manip_mode) 
  {
    case EditManipulatorsPlugin::ModNone:
      if(!aroundOrigin)
        glTranslate(mesh_boxcenter);
      else
        glTranslate(mesh_origin);
      glScale(manipsize);
      glMultMatrix(Inverse(track_rotation));
	    glRotatef (90.0f, 0.0f, 1.0f, 0.0f);
	    DrawCubes(1.0f,0.8f,0.5f);
      glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
	    DrawCubes(1.0f,0.8f,0.5f);
      break;
    case EditManipulatorsPlugin::ModX:
      if(!aroundOrigin)
        glTranslate(mesh_boxcenter);
      else
        glTranslate(mesh_origin);
      glScale(manipsize);
	    glRotatef (90.0f, 0.0f, 1.0f, 0.0f);
	    DrawCubes(1.0f,0.0f,0.0f);
      break;
    case EditManipulatorsPlugin::ModY:
      if(!aroundOrigin)
        glTranslate(mesh_boxcenter);
      else
        glTranslate(mesh_origin);
      glScale(manipsize);
	    glRotatef (90.0f, 1.0f, 0.0f, 0.0f);
	    DrawCubes(0.0f,1.0f,0.0f);
      break;
    case EditManipulatorsPlugin::ModZ:
      if(!aroundOrigin)
        glTranslate(mesh_boxcenter);
      else
        glTranslate(mesh_origin);
      glScale(manipsize);
	    DrawCubes(0,0,1.0);
      break;
    case EditManipulatorsPlugin::ModXX:
		if (!aroundOrigin)
		{
			glTranslate(mesh_boxcenter);
			glMultMatrix(original_rot);
		}
		else
			glMultMatrix(original_Transform);
		glScale(manipsize);
		glRotatef (90, 0, 1, 0);
		DrawCubes(1.0,0.5,0.5);
      break;
    case EditManipulatorsPlugin::ModYY:
		if (!aroundOrigin)
		{
			glTranslate(mesh_boxcenter);
			glMultMatrix(original_rot);
		}
		else
			glMultMatrix(original_Transform);
		glScale(manipsize);
		glRotatef (90, 1, 0, 0);
		DrawCubes(0.5,1.0,0.5);
      break;
    case EditManipulatorsPlugin::ModZZ:
		if (!aroundOrigin)
		{
			glTranslate(mesh_boxcenter);
			glMultMatrix(original_rot);
		}
		else
			glMultMatrix(original_Transform);
		glScale(manipsize);
		DrawCubes(0.5,0.5,1.0);
      break;
    default: ;
  }
	
  glLineWidth(1.0);
	glPopMatrix ();
}

void  EditManipulatorsPlugin::DrawRotateManipulators(MeshModel &model, GLArea *gla)
{  
  glPushMatrix();

  Point3m mesh_boxcenter, mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis, new_mesh_origin;
  mesh_boxcenter = original_Transform * model.cm.bbox.Center();
  mesh_origin = original_Transform.GetColumn3(3);
  new_mesh_origin = model.cm.Tr.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);
  float manipsize = model.cm.bbox.Diag() / 2.0;
  Matrix44f original_rot = original_Transform;
  original_rot.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
  Matrix44f track_rotation;
  gla->trackball.track.rot.ToMatrix(track_rotation);
  
  glLineWidth(2.0);

  switch(current_manip_mode) 
  {
    case EditManipulatorsPlugin::ModNone:
      if(!aroundOrigin)
        glTranslate(mesh_boxcenter);
      else
        glTranslate(mesh_origin);
      glScale(manipsize);
      glMultMatrix(Inverse(track_rotation));
	    DrawCircle(1.0f,0.8f,0.5f);
      break;
    case EditManipulatorsPlugin::ModX:
       if(!aroundOrigin)
        glTranslate(mesh_boxcenter);
      else
        glTranslate(mesh_origin);
      glScale(manipsize);
	    glRotatef (90, 0, 1, 0);
	    DrawCircle(1.0,0,0);
      break;
    case EditManipulatorsPlugin::ModY:
       if(!aroundOrigin)
        glTranslate(mesh_boxcenter);
      else
        glTranslate(mesh_origin);
      glScale(manipsize);
	    glRotatef (-90, 1, 0, 0);
	    DrawCircle(0,1.0,0);
      break;
    case EditManipulatorsPlugin::ModZ:
      if(!aroundOrigin)
        glTranslate(mesh_boxcenter);
      else
        glTranslate(mesh_origin);
      glScale(manipsize);
	    DrawCircle(0,0,1.0);
      break;
    case EditManipulatorsPlugin::ModXX:
		if(!aroundOrigin)
		{
			glTranslate(mesh_boxcenter);
			glMultMatrix(original_rot);
		}
		else
			glMultMatrix(original_Transform);
		glScale(manipsize);
		glRotatef (90, 0, 1, 0);
		DrawCircle(1.0,0.5,0.5);
		break;
    case EditManipulatorsPlugin::ModYY:
		if(!aroundOrigin)
		{
			glTranslate(mesh_boxcenter);
			glMultMatrix(original_rot);
		}
		else
			glMultMatrix(original_Transform);
		glScale(manipsize);
		glRotatef (-90, 1, 0, 0);
		DrawCircle(0.5,1.0,0.5);
		break;
    case EditManipulatorsPlugin::ModZZ:
		if (!aroundOrigin)
		{
			glTranslate(mesh_boxcenter);
			glMultMatrix(original_rot);
		}
		else
			glMultMatrix(original_Transform);
		glScale(manipsize);
		DrawCircle(0.5,0.5,1.0);
		break;
    default: ;
  }
	
  glLineWidth(1.0);
	glPopMatrix();
}

void EditManipulatorsPlugin::DrawManipulators(MeshModel &model, GLArea *gla, bool /*onlyActive*/)
{
  Point3m mesh_boxcenter, mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis;
  mesh_boxcenter = original_Transform * model.cm.bbox.Center();
  mesh_origin = original_Transform.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);

  // setup
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);

  switch(current_manip) 
  {
    case EditManipulatorsPlugin::ManMove:
      DrawTranslateManipulators(model, gla);      
      break;
    case EditManipulatorsPlugin::ManRotate:
      DrawRotateManipulators(model, gla);
      break;
    case EditManipulatorsPlugin::ManScale:
      DrawScaleManipulators(model, gla);
      break;
    default: ;
  }

  if(current_manip_mode != EditManipulatorsPlugin::ModNone)
  {
    glBegin(GL_LINES);

    switch(current_manip_mode) 
    {
      case EditManipulatorsPlugin::ModX:
        glColor3f(1.0,0,0);
        if(aroundOrigin || (current_manip == EditManipulatorsPlugin::ManMove))
        {
          glVertex(mesh_origin + Point3m(Scalarm(-10.0),Scalarm(0.0),Scalarm(0.0))); 	
          glVertex(mesh_origin + Point3m(Scalarm(10.0), Scalarm(0.0),Scalarm(0.0)));
        }
        else
        {
          glVertex(mesh_boxcenter + Point3m(Scalarm(-10.0),Scalarm(0.0),Scalarm(0.0))); 	
          glVertex(mesh_boxcenter + Point3m(Scalarm(10.0),Scalarm(0.0),Scalarm(0.0)));
        }
        break;
      case EditManipulatorsPlugin::ModY:
        glColor3f(0,1.0,0);
        if(aroundOrigin || (current_manip == EditManipulatorsPlugin::ManMove))
        {
          glVertex(mesh_origin + Point3m(Scalarm(0.0),Scalarm(-10.0),Scalarm(0.0))); 	
          glVertex(mesh_origin + Point3m(Scalarm(0.0),Scalarm(10.0),Scalarm(0.0)));
        }
        else
        {
          glVertex(mesh_boxcenter + Point3m(Scalarm(0.0), Scalarm(-10.0),Scalarm(0.0))); 	
          glVertex(mesh_boxcenter + Point3m(Scalarm(0.0), Scalarm(10.0), Scalarm(0.0)));
        }
        break;
      case EditManipulatorsPlugin::ModZ:
        glColor3f(0,0,1.0);
        if(aroundOrigin || (current_manip == EditManipulatorsPlugin::ManMove))
        {
          glVertex(mesh_origin + Point3m(Scalarm(0.0),Scalarm(0.0),Scalarm(-10.0))); 	
          glVertex(mesh_origin + Point3m(Scalarm(0.0),Scalarm(0.0),Scalarm(10.0)));
        }
        else
        {
          glVertex(mesh_boxcenter + Point3m(Scalarm(0.0),Scalarm(0.0), Scalarm(-10.0))); 	
          glVertex(mesh_boxcenter + Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(10.0)));
        }
        break;
      case EditManipulatorsPlugin::ModXX:
        glColor3f(1.0f,0.5f,0.5f);
        if(aroundOrigin || (current_manip == EditManipulatorsPlugin::ManMove))
        {
          glVertex(mesh_origin + (mesh_xaxis * Scalarm(-10.0))); 	
          glVertex(mesh_origin + (mesh_xaxis * Scalarm(10.0)));
        }
        else
        {
          glVertex(mesh_boxcenter + (mesh_xaxis * Scalarm(-10.0))); 	
          glVertex(mesh_boxcenter + (mesh_xaxis * Scalarm(10.0)));
        }
        break;
      case EditManipulatorsPlugin::ModYY:
        glColor3f(0.5,1.0,0.5);
        if(aroundOrigin || (current_manip == EditManipulatorsPlugin::ManMove))
        {
          glVertex(mesh_origin + (mesh_yaxis * Scalarm(-10.0))); 	
          glVertex(mesh_origin + (mesh_yaxis * Scalarm(10.0)));
        }
        else
        {
          glVertex(mesh_boxcenter + (mesh_yaxis * Scalarm(-10.0))); 	
          glVertex(mesh_boxcenter + (mesh_yaxis * Scalarm(10.0)));
        }
        break;
      case EditManipulatorsPlugin::ModZZ:
        glColor3f(0.5,0.5,1.0);
        if(aroundOrigin || (current_manip == EditManipulatorsPlugin::ManMove))
        {
          glVertex(mesh_origin + (mesh_zaxis * Scalarm(-10.0))); 	
          glVertex(mesh_origin + (mesh_zaxis * Scalarm(10.0)));
        }
        else
        {
          glVertex(mesh_boxcenter + (mesh_zaxis * Scalarm(-10.0))); 	
          glVertex(mesh_boxcenter + (mesh_zaxis * Scalarm(10.0)));
        }
        break;
      default: ;
    }

    glEnd();
  }

  // restore
  glPopAttrib();
}

void EditManipulatorsPlugin::Decorate(MeshModel &model, GLArea *gla, QPainter* /*painter*/)
{
  //
  Point3m center, right, top, front;

  MyPick(gla->width()*0.5f, gla->height()*0.5f, center, 0.5f);
  MyPick(gla->width()*0.99f, gla->height()*0.5f, right, 0.5f);
  MyPick(gla->width()*0.5f, gla->height()*0.01f, top, 0.5f);
  MyPick(gla->width()*0.5f, gla->height()*0.5f, front, 0.01f);

  screen_xaxis = (right - center) * Scalarm(2.0);
  screen_yaxis = (top - center)   * Scalarm(2.0);
  screen_zaxis = (front - center) * Scalarm(2.0);

  float diag = sqrt(screen_xaxis.Norm()*screen_xaxis.Norm() + screen_yaxis.Norm()*screen_yaxis.Norm());

  // calculate snapping factor
  if(current_manip != EditManipulatorsPlugin::ManNone)
  {
    switch(current_manip) 
    {
      case EditManipulatorsPlugin::ManMove:
        snapto = powf(10.f,ceil(log10(diag))-2);
        break;
      case EditManipulatorsPlugin::ManRotate:
        snapto = 1.0;
        break;
      case EditManipulatorsPlugin::ManScale:
        snapto = 0.1;
        break;
      default: ;
    }
  }

  // write manipulator data
  QString StatusString1 = "";
  QString StatusString2 = "";
  QString HelpString1   = "";
  QString HelpString2   = "";
  QString HelpString3   = "";

  if(current_manip == EditManipulatorsPlugin::ManNone)
  {
    StatusString1 += "  NONE ";
  }
  else
  {
    switch(current_manip) 
    {
      case EditManipulatorsPlugin::ManMove:
        StatusString1 += "  Translate";
        break;
      case EditManipulatorsPlugin::ManRotate:
        if(aroundOrigin) StatusString1 += "Rotate around Mesh Origin";
                    else StatusString1 += "Rotate around BBox center";
        break;
      case EditManipulatorsPlugin::ManScale:
        if(aroundOrigin) StatusString1 += "Scale around Mesh Origin";
                    else StatusString1 += "Scale around BBox center";
        break;
      default: ;
    }
    StatusString2="<br>";
    switch(current_manip_mode) 
    {
      case EditManipulatorsPlugin::ModX: StatusString2 += "X global"; break;
      case EditManipulatorsPlugin::ModY: StatusString2 += "Y global"; break;
      case EditManipulatorsPlugin::ModZ: StatusString2 += "Z global"; break;
      case EditManipulatorsPlugin::ModXX: StatusString2 += "X local"; break;
      case EditManipulatorsPlugin::ModYY: StatusString2 += "Y local"; break;
      case EditManipulatorsPlugin::ModZZ: StatusString2 += "Z local"; break;
      default: 
        if((current_manip == EditManipulatorsPlugin::ManMove) || (current_manip == EditManipulatorsPlugin::ManRotate))
          StatusString2 += "viewport";
        else if(current_manip == EditManipulatorsPlugin::ManScale)
          StatusString2 += "uniform";
        break;
    }

    // display offset factor in single axis 
    if(current_manip_mode != EditManipulatorsPlugin::ModNone)
    {
      StatusString2 += QString("   ::   %1").arg(displayOffset);
    }

    // viewport translation, display the XYZ offsets
    if((current_manip_mode == EditManipulatorsPlugin::ModNone) && (current_manip == EditManipulatorsPlugin::ManMove))
    {
      StatusString2 += QString("  -  %1  %2  %3").arg(displayOffset_X).arg(displayOffset_Y).arg(displayOffset_Z);
    }

    // viewport rotation, display rotation angle
    if((current_manip_mode == EditManipulatorsPlugin::ModNone) && (current_manip == EditManipulatorsPlugin::ManRotate))
    {
      StatusString2 += QString("  -  %1").arg(displayOffset);      
    }

    // uniform scale, display scale factor
    if((current_manip_mode == EditManipulatorsPlugin::ModNone) && (current_manip == EditManipulatorsPlugin::ManScale))
    {
      StatusString2 += QString("  -  %1").arg(displayOffset);      
    }

    if(isSnapping)
    {
      StatusString2 += QString("  -  Snapping: %1").arg(snapto); 
    }
  }

  if(current_manip == EditManipulatorsPlugin::ManNone)
  {
    HelpString1 = "<br><i> press T to translate, R to rotate, S to scale</i>";
  }
  else
  {
    switch(current_manip)
    {
    case EditManipulatorsPlugin::ManMove   : HelpString1 = "<br> LEFT CLICK and DRAG to move"; break;
    case EditManipulatorsPlugin::ManRotate : HelpString1 = "<br> LEFT CLICK and DRAG to rotate"; break;
    case EditManipulatorsPlugin::ManScale  : HelpString1 = "<br> LEFT CLICK and DRAG to scale"; break;
    }

    if((current_manip != EditManipulatorsPlugin::ManMove) || (current_manip_mode != EditManipulatorsPlugin::ModNone))
      HelpString1 += "  -  hold SHIFT to snap";

    HelpString2="<br>";
    switch(current_manip_mode) 
    {
      case EditManipulatorsPlugin::ModX:  HelpString2 = "<br> press X to switch to X local"; break;
      case EditManipulatorsPlugin::ModY:  HelpString2 = "<br> press Y to switch to Y local"; break;
      case EditManipulatorsPlugin::ModZ:  HelpString2 = "<br> press Z to switch to Z local"; break;
      case EditManipulatorsPlugin::ModXX: HelpString2 = "<br> press X to switch to X global"; break;
      case EditManipulatorsPlugin::ModYY: HelpString2 = "<br> press Y to switch to Y global"; break;
      case EditManipulatorsPlugin::ModZZ: HelpString2 = "<br> press Z to switch to Z global"; break;
      default:   HelpString2 = "<br> press X Y Z to select an axis";    break;
    }
    
    if((current_manip == EditManipulatorsPlugin::ManRotate) || (current_manip == EditManipulatorsPlugin::ManScale))
    {
      if(aroundOrigin)  HelpString2 += "  -  press SPACE to pivot on BBox center";
                  else  HelpString2 += "  -  press SPACE to pivot on Mesh Origin";
    }

    HelpString3 = "<br>press RETURN to apply, BACKSPACE to cancel";
  }

  this->RealTimeLog("Manipulator","",qPrintable("<b>"+StatusString1+"</b>"+StatusString2+HelpString1+HelpString2+HelpString3));

  // render original mesh BBox
  DrawMeshBox(model);

  // render active manipulator
  DrawManipulators(model, gla, true);

  assert(!glGetError());
}

void EditManipulatorsPlugin::UpdateMatrix(MeshModel &model, GLArea * gla, bool applymouseoffset, bool useinputnumber)
{  
  Matrix44m newmatrix;

  Matrix44m old_rotation; 
  Matrix44m old_translation;  
  Matrix44m old_meshcenter;
  Matrix44m old_meshuncenter;
  Matrix44m old_meshcenter_untr;
  Matrix44m old_meshuncenter_untr;

  Point3m new_scale;
  Point3m axis;
  Scalarm mouseXoff;
  Scalarm mouseYoff;

  Point3m mesh_boxcenter, mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis;
  mesh_boxcenter = model.cm.bbox.Center();
  mesh_origin = original_Transform.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);
  vcg::Normalize(mesh_xaxis);
  vcg::Normalize(mesh_yaxis);
  vcg::Normalize(mesh_zaxis);

  delta_Transform.SetIdentity();
  newmatrix.SetIdentity();

  if(current_manip == EditManipulatorsPlugin::ManNone)
  {
    model.cm.Tr = original_Transform;
  }
  else
  {

    if(current_manip_mode != EditManipulatorsPlugin::ModNone)  // transform on one axis only
    {

      switch(current_manip_mode)          // which axis is active
      {
        case EditManipulatorsPlugin::ModX:
          axis = Point3m(1.0, 0.0, 0.0);
          break;
        case EditManipulatorsPlugin::ModY:
          axis = Point3m(0.0, 1.0, 0.0);
          break;
        case EditManipulatorsPlugin::ModZ:
          axis = Point3m(0.0, 0.0, 1.0);
          break;
        case EditManipulatorsPlugin::ModXX:
          axis = mesh_xaxis;
          break;
        case EditManipulatorsPlugin::ModYY:
          axis = mesh_yaxis;
          break;
        case EditManipulatorsPlugin::ModZZ:
          axis = mesh_zaxis;
          break;
        default: axis = Point3m(1.0, 1.0, 1.0); // it should never arrive here, anyway
      }

      if(current_manip == EditManipulatorsPlugin::ManMove)
      {
        // mouse offset -> single axis translation
        float xsign = ((screen_xaxis*axis)>0.0)?1.0:-1.0;
        float ysign = ((screen_yaxis*axis)>0.0)?1.0:-1.0;
        mouseXoff = xsign * screen_xaxis.Norm() * (currScreenOffset_X/float(gla->width()));
        mouseYoff = ysign * screen_yaxis.Norm() * (currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + mouseXoff + mouseYoff;

        // snapping
        if(isSnapping)
        {
          displayOffset /= snapto; 
          displayOffset = floor(displayOffset+0.5);
          displayOffset *= snapto; 
        }

        if(useinputnumber)
          displayOffset = inputnumber;

        delta_Transform.SetTranslate(axis * displayOffset);  
        newmatrix = delta_Transform * original_Transform;
      }
      else if(current_manip == EditManipulatorsPlugin::ManRotate)
      {
        // mouse offset -> single axis rotation
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + (360.0 * (mouseXoff + mouseYoff));
        if((displayOffset > 360.0) || (displayOffset < -360.0))
          displayOffset = 360.0;

        // snapping
        if(isSnapping)
        {
          displayOffset = floor(displayOffset+0.5);
        }

        if(useinputnumber)
          displayOffset = inputnumber;

        delta_Transform.SetRotateDeg(displayOffset, axis);

        old_rotation = original_Transform;
        old_rotation.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
        old_translation.SetTranslate(original_Transform.GetColumn3(3));
        old_meshcenter.SetTranslate(old_rotation * (-mesh_boxcenter));
        old_meshuncenter.SetTranslate(old_rotation * mesh_boxcenter);

        if(aroundOrigin)
          newmatrix = old_translation * delta_Transform * old_rotation;
        else
          newmatrix = old_translation * old_meshuncenter * delta_Transform * old_meshcenter * old_rotation;
      }
      else if(current_manip == EditManipulatorsPlugin::ManScale)
      {       
        // mouse offset -> single axis scaling
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + (2.0 * (mouseXoff + mouseYoff));

        // snapping
        if(isSnapping)
        {
          displayOffset /= snapto; 
          displayOffset = floor(displayOffset+0.5);
          displayOffset *= snapto; 
        }

        if(useinputnumber)
          displayOffset = inputnumber;


		if ((current_manip_mode == EditManipulatorsPlugin::ModX) || (current_manip_mode == EditManipulatorsPlugin::ModXX))
		{
			delta_Transform.SetScale(Point3m(displayOffset, Scalarm(1.0), Scalarm(1.0)));
		}
		if ((current_manip_mode == EditManipulatorsPlugin::ModY) || (current_manip_mode == EditManipulatorsPlugin::ModYY))
		{
			delta_Transform.SetScale(Point3m(Scalarm(1.0), displayOffset, Scalarm(1.0)));
		}
		if ((current_manip_mode == EditManipulatorsPlugin::ModZ) || (current_manip_mode == EditManipulatorsPlugin::ModZZ))
		{
			delta_Transform.SetScale(Point3m(Scalarm(1.0), Scalarm(1.0), displayOffset));
		}

        old_rotation = original_Transform;
        old_rotation.SetColumn(3, Point3m(Scalarm(0.0),Scalarm(0.0),Scalarm(0.0)));
        old_translation.SetTranslate(original_Transform.GetColumn3(3));
		old_meshcenter.SetTranslate(old_rotation * (-mesh_boxcenter));
		old_meshuncenter.SetTranslate(old_rotation * mesh_boxcenter);
		old_meshcenter_untr.SetTranslate(-mesh_boxcenter);
		old_meshuncenter_untr.SetTranslate(mesh_boxcenter);

		if ((current_manip_mode == EditManipulatorsPlugin::ModX) || (current_manip_mode == EditManipulatorsPlugin::ModY) || (current_manip_mode == EditManipulatorsPlugin::ModZ))
		{
			if (aroundOrigin)
				newmatrix = old_translation * delta_Transform * old_rotation;
			else
				newmatrix = old_translation * old_meshuncenter * delta_Transform * old_meshcenter * old_rotation;
		}
		else // local axis
		{
			if (aroundOrigin)
				newmatrix = old_translation * old_rotation * delta_Transform;
			else
				newmatrix = old_translation * old_rotation * old_meshuncenter_untr * delta_Transform * old_meshcenter_untr;
		}


      }
      else
        newmatrix = original_Transform;  // it should never arrive here, anyway
      
    }
    else   // transform on full space ? on view space ?
    {


      if(current_manip == EditManipulatorsPlugin::ManMove)
      {
        // mouse offset -> viewport translation
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (currScreenOffset_Y/float(gla->height()));

        displayOffset_X = currOffset_X + (screen_xaxis[0] * mouseXoff) + (screen_yaxis[0] * mouseYoff);
        displayOffset_Y = currOffset_Y + (screen_xaxis[1] * mouseXoff) + (screen_yaxis[1] * mouseYoff);        
        displayOffset_Z = currOffset_Z + (screen_xaxis[2] * mouseXoff) + (screen_yaxis[2] * mouseYoff);

        // snapping
        if(isSnapping)
        {
          displayOffset_X /= snapto; 
          displayOffset_X = floor(displayOffset_X+0.5);
          displayOffset_X *= snapto; 
          displayOffset_Y /= snapto; 
          displayOffset_Y = floor(displayOffset_Y+0.5);
          displayOffset_Y *= snapto; 
          displayOffset_Z /= snapto; 
          displayOffset_Z = floor(displayOffset_Z+0.5);
          displayOffset_Z *= snapto; 
        }

        delta_Transform.SetTranslate(Point3m(displayOffset_X,displayOffset_Y,displayOffset_Z));  
        newmatrix = delta_Transform * original_Transform;
      }

      if(current_manip == EditManipulatorsPlugin::ManRotate)
      {
        // mouse offset -> viewport rotation
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + (360.0 * (mouseXoff + mouseYoff));
        if((displayOffset > 360.0) || (displayOffset < -360.0))
          displayOffset = 360.0;

        // snapping
        if(isSnapping)
        {
          displayOffset = floor(displayOffset+0.5);
        }

        if(useinputnumber)
          displayOffset = inputnumber;

        delta_Transform.SetRotateDeg(displayOffset, screen_zaxis);

        old_rotation = original_Transform;
        old_rotation.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
        old_translation.SetTranslate(original_Transform.GetColumn3(3));
		old_meshcenter.SetTranslate(old_rotation * (-mesh_boxcenter));
		old_meshuncenter.SetTranslate(old_rotation * mesh_boxcenter);

        if(aroundOrigin)
          newmatrix = old_translation * delta_Transform * old_rotation;
        else
          newmatrix = old_translation * old_meshuncenter * delta_Transform * old_meshcenter * old_rotation;
      }

      if(current_manip == EditManipulatorsPlugin::ManScale)
      {
        // mouse offset -> uniform scaling
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (-currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + (2.0 * (mouseXoff + mouseYoff));

        // snapping
        if(isSnapping)
        {
          displayOffset /= snapto; 
          displayOffset = floor(displayOffset+0.5);
          displayOffset *= snapto; 
        }

        if(useinputnumber)
          displayOffset = inputnumber;

        new_scale[0] = displayOffset;
        new_scale[1] = displayOffset;
        new_scale[2] = displayOffset;

        delta_Transform.SetScale(new_scale);

        old_rotation = original_Transform;
        old_rotation.SetColumn(3, Point3m(Scalarm(0.0), Scalarm(0.0), Scalarm(0.0)));
        old_translation.SetTranslate(original_Transform.GetColumn3(3));
        old_meshcenter.SetTranslate(-mesh_boxcenter);
        old_meshuncenter.SetTranslate(mesh_boxcenter);
		old_meshcenter_untr.SetTranslate(-mesh_boxcenter);
		old_meshuncenter_untr.SetTranslate(mesh_boxcenter);

        if(aroundOrigin)
			newmatrix = old_translation * old_rotation * delta_Transform;
        else
			newmatrix = old_translation * old_rotation * old_meshuncenter_untr * delta_Transform * old_meshcenter_untr;
      }

    }
    model.cm.Tr = newmatrix;
  }

  if(applymouseoffset)
  {
    // user finished dragging... accumulation of mouse offset into current offset
    currOffset = displayOffset;
    currOffset_X = displayOffset_X;
    currOffset_Y = displayOffset_Y;
    currOffset_Z = displayOffset_Z;
  }
}


bool EditManipulatorsPlugin::MyPick(const int &x, const int &y, Point3m &pp, float mydepth)
{
	double res[3];
	GLdouble mm[16],pm[16]; GLint vp[4];
	glGetDoublev(GL_MODELVIEW_MATRIX,mm);
	glGetDoublev(GL_PROJECTION_MATRIX,pm);
	glGetIntegerv(GL_VIEWPORT,vp);
	
	gluUnProject(x,y,mydepth,mm,pm,vp,&res[0],&res[1],&res[2]);
	pp=Point3m(Scalarm(res[0]),Scalarm(res[1]),Scalarm(res[2]));
    return true;
}

bool EditManipulatorsPlugin::StartEdit(MeshModel &model, GLArea *gla )
{
  gla->setCursor(QCursor(QPixmap(":/images/cur_manipulators.png"),15,15));	
  connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );

  // reset filter data
  current_manip = EditManipulatorsPlugin::ManNone;
  current_manip_mode = EditManipulatorsPlugin::ModNone;
  isMoving = false;
  isSnapping = false;
  aroundOrigin = true;
  snapto = 1.0;
  resetOffsets();

  inputnumberstring= "";
  inputnumbernegative = false;
  inputnumber=0;

  // storing start matrix
  original_Transform = model.cm.Tr;
  delta_Transform = Matrix44m::Identity();

  gla->update();
	return true;
}

void EditManipulatorsPlugin::EndEdit(MeshModel &model, GLArea *gla)
{
  cancelMotion(model, gla);     // something interrupted the filter... canceling 
}


