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
#include <QtGui>

#include <meshlab/glarea.h>
#include "edit_manipulators.h"
#include <wrap/qt/gl_label.h>
#include <wrap/gui/trackball.h>

using namespace vcg;

EditManipulatorsPlugin::EditManipulatorsPlugin()
{
  current_manip = ManipulatorType::ManNone;
  current_manip_mode = ManipulatorMode::ModNone;
  isMoving = false;
  resetOffsets();

  original_Transform = vcg::Matrix44f::Identity();
  delta_Transform = vcg::Matrix44f::Identity();
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
  current_manip = ManipulatorType::ManNone;
  current_manip_mode = ManipulatorMode::ModNone;
  isMoving = false;
  resetOffsets();

  // storing start matrix
  original_Transform = model.cm.Tr;
  delta_Transform = vcg::Matrix44f::Identity();

  gla->update();
}

// Cancel movement
void EditManipulatorsPlugin::cancelMotion(MeshModel &model, GLArea *gla)
{
  //restoring old matrix
  model.cm.Tr = original_Transform;

  // reset filter data
  current_manip = ManipulatorType::ManNone;
  current_manip_mode = ManipulatorMode::ModNone;
  isMoving = false;
  resetOffsets();

  // storing start matrix
  original_Transform = model.cm.Tr;
  delta_Transform = vcg::Matrix44f::Identity();

  gla->update();
}

// keyboard commands, just like Blender
void EditManipulatorsPlugin::keyReleaseEvent(QKeyEvent *e, MeshModel &model, GLArea *gla)
{
  // enter will apply, backspace to cancel
  if(current_manip != ManipulatorType::ManNone)   
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

  if(current_manip == ManipulatorType::ManNone)   // if no active manipulator, listen to G R S to select one
  {
	  if (e->key() == Qt::Key_G) // grab
    {
      current_manip = ManipulatorType::ManMove;
      resetOffsets();
      UpdateMatrix(model,gla,false);
    }
	  if (e->key() == Qt::Key_R) // rotate
    {
      current_manip = ManipulatorType::ManRotate;
      resetOffsets();
      UpdateMatrix(model,gla,false);
    }
    if (e->key() == Qt::Key_S) // scale
    {
      current_manip = ManipulatorType::ManScale;
      resetOffsets();
      UpdateMatrix(model,gla,false);
    }
  }

  if(current_manip != ManipulatorType::ManNone)   // if there is an active manipulator, listen to modifiers
  {
	  if (e->key() == Qt::Key_X) // X axis
    {
      if(current_manip_mode == ManipulatorMode::ModX)
        current_manip_mode = ManipulatorMode::ModXX;
      else
        current_manip_mode = ManipulatorMode::ModX;

      resetOffsets();

      UpdateMatrix(model,gla,false);
    }
	  if (e->key() == Qt::Key_Y) // Y axis
    {
      if(current_manip_mode == ManipulatorMode::ModY)
        current_manip_mode = ManipulatorMode::ModYY;
      else
        current_manip_mode = ManipulatorMode::ModY;

      resetOffsets();

      UpdateMatrix(model,gla,false);
    }
	  if (e->key() == Qt::Key_Z) // Z axis
    {
      if(current_manip_mode == ManipulatorMode::ModZ)
        current_manip_mode = ManipulatorMode::ModZZ;
      else
        current_manip_mode = ManipulatorMode::ModZ;

      resetOffsets();

      UpdateMatrix(model,gla,false);
    }
  }
	
  if (e->key() == Qt::Key_Q) // DEBUG DEBUG
  {
    currOffset += 0.05;
    UpdateMatrix(model,gla,false);
  }
  if (e->key() == Qt::Key_A) // DEBUG DEBUG
  {
    currOffset -= 0.05;
    UpdateMatrix(model,gla,false);
  }
  if (e->key() == Qt::Key_W) // DEBUG DEBUG
  {
    currOffset += 5;
    UpdateMatrix(model,gla,false);
  }
  if (e->key() == Qt::Key_E) // DEBUG DEBUG
  {
    currOffset -= 5;
    UpdateMatrix(model,gla,false);
  }

  //else e->ignore();
  gla->update();
}

void EditManipulatorsPlugin::resetOffsets()
{
  if(current_manip == ManipulatorType::ManScale)
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
  Box3f b;
	b = model.cm.bbox;
	Point3f mi=b.min;
	Point3f ma=b.max;
	Point3f d3=(b.max-b.min)/4.0;
	Point3f zz(0,0,0);

  // setup
	glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_POINT_BIT | GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1.0);
	glColor(Color4b::Cyan);

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

  // restore
  glPopMatrix();
  glPopAttrib();
}


//----------------------------------------------------------------------------------

void EditManipulatorsPlugin::DrawCubes(float r, float g, float b)
{
  glColor4f(r,g,b,1.0);
  glBegin (GL_LINES);
      // mid line
    glVertex3f( 0.0,  0.0, -1.0);
    glVertex3f( 0.0,  0.0,  1.0);
  glEnd ();

  glBegin (GL_LINES);
      // right cube
    glVertex3f(  0.0,  0.0,  1.0);
    glVertex3f(  0.1,  0.0,  1.1);
    glVertex3f(  0.0,  0.0,  1.0);
    glVertex3f( -0.1,  0.0,  1.1);
    glVertex3f(  0.0,  0.0,  1.0);
    glVertex3f(  0.0, -0.1,  1.1);
    glVertex3f(  0.0,  0.0,  1.0);
    glVertex3f(  0.0,  0.1,  1.1);
    glVertex3f(  0.0,  0.0,  1.2);
    glVertex3f(  0.1,  0.0,  1.1);
    glVertex3f(  0.0,  0.0,  1.2);
    glVertex3f( -0.1,  0.0,  1.1);
    glVertex3f(  0.0,  0.0,  1.2);
    glVertex3f(  0.0, -0.1,  1.1);
    glVertex3f(  0.0,  0.0,  1.2);
    glVertex3f(  0.0,  0.1,  1.1);
  glEnd ();

  glBegin (GL_LINES);
      // left cube
    glVertex3f(  0.0,  0.0, -1.0);
    glVertex3f(  0.1,  0.0, -1.1);
    glVertex3f(  0.0,  0.0, -1.0);
    glVertex3f( -0.1,  0.0, -1.1);
    glVertex3f(  0.0,  0.0, -1.0);
    glVertex3f(  0.0, -0.1, -1.1);
    glVertex3f(  0.0,  0.0, -1.0);
    glVertex3f(  0.0,  0.1, -1.1);
    glVertex3f(  0.0,  0.0, -1.2);
    glVertex3f(  0.1,  0.0, -1.1);
    glVertex3f(  0.0,  0.0, -1.2);
    glVertex3f( -0.1,  0.0, -1.1);
    glVertex3f(  0.0,  0.0, -1.2);
    glVertex3f(  0.0, -0.1, -1.1);
    glVertex3f(  0.0,  0.0, -1.2);
    glVertex3f(  0.0,  0.1, -1.1);
  glEnd ();


    // right cube
  glColor4f(std::min(1.0f,r+0.2f), std::min(1.0f,g+0.2f), std::min(1.0f,b+0.2f),0.5);
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0,  0.0,  1.2);
    glVertex3f(  0.0,  0.1,  1.1);
    glVertex3f( -0.1,  0.0,  1.1);
    glVertex3f(  0.0, -0.1,  1.1);
    glVertex3f(  0.1,  0.0,  1.1);
    glVertex3f(  0.0,  0.1,  1.1);
  glEnd();
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0,  0.0,  1.0);
    glVertex3f(  0.0,  0.1,  1.1);
    glVertex3f( -0.1,  0.0,  1.1);
    glVertex3f(  0.0, -0.1,  1.1);
    glVertex3f(  0.1,  0.0,  1.1);
    glVertex3f(  0.0,  0.1,  1.1);
  glEnd();

      // left cube
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0,  0.0, -1.2);
    glVertex3f(  0.0,  0.1, -1.1);
    glVertex3f( -0.1,  0.0, -1.1);
    glVertex3f(  0.0, -0.1, -1.1);
    glVertex3f(  0.1,  0.0, -1.1);
    glVertex3f(  0.0,  0.1, -1.1);
  glEnd();
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0,  0.0, -1.0);
    glVertex3f(  0.0,  0.1, -1.1);
    glVertex3f( -0.1,  0.0, -1.1);
    glVertex3f(  0.0, -0.1, -1.1);
    glVertex3f(  0.1,  0.0, -1.1);
    glVertex3f(  0.0,  0.1, -1.1);
  glEnd();
}

void EditManipulatorsPlugin::DrawArrows(float r, float g, float b)
{
  glColor4f(r,g,b,1.0);
  glBegin (GL_LINES);
      // mid line
    glVertex3f( 0.0,  0.0, -1.1);
    glVertex3f( 0.0,  0.0,  1.1);

      // right arrow
    glVertex3f(  0.0,  0.0,  1.1);
    glVertex3f(  0.1,  0.1,  0.9);
    glVertex3f(  0.0,  0.0,  1.1);
    glVertex3f( -0.1,  0.1,  0.9);
    glVertex3f(  0.0,  0.0,  1.1);
    glVertex3f(  0.1, -0.1,  0.9);
    glVertex3f(  0.0,  0.0,  1.1);
    glVertex3f( -0.1, -0.1,  0.9);

      // left arrow
    glVertex3f(  0.0,  0.0, -1.1);
    glVertex3f(  0.1,  0.1, -0.9);
    glVertex3f(  0.0,  0.0, -1.1);
    glVertex3f( -0.1,  0.1, -0.9);
    glVertex3f(  0.0,  0.0, -1.1);
    glVertex3f(  0.1, -0.1, -0.9);
    glVertex3f(  0.0,  0.0, -1.1);
    glVertex3f( -0.1, -0.1, -0.9);
  glEnd ();

    // right arrow
  glColor4f(std::min(1.0f,r+0.2f), std::min(1.0f,g+0.2f), std::min(1.0f,b+0.2f), 0.5);
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0,  0.0,  1.1);
    glVertex3f(  0.1,  0.1,  0.9);
    glVertex3f( -0.1,  0.1,  0.9);
    glVertex3f( -0.1, -0.1,  0.9);
    glVertex3f(  0.1, -0.1,  0.9);
    glVertex3f(  0.1,  0.1,  0.9);
  glEnd();
      // left arrow
  glBegin (GL_TRIANGLE_FAN);
    glVertex3f(  0.0,  0.0, -1.1);
    glVertex3f(  0.1,  0.1, -0.9);
    glVertex3f( -0.1,  0.1, -0.9);
    glVertex3f( -0.1, -0.1, -0.9);
    glVertex3f(  0.1, -0.1, -0.9);
    glVertex3f(  0.1,  0.1, -0.9);
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
  glEnd ();

  glColor4f(std::min(1.0f,r+0.2f), std::min(1.0f,g+0.2f), std::min(1.0f,b+0.2f), 0.5);
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
  glEnd ();


}

//----------------------------------------------------------------------------------

void  EditManipulatorsPlugin::DrawTranslateManipulators(MeshModel &model, GLArea *gla)
{  
  glPushMatrix ();

  Point3f mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis, new_mesh_origin;
  new_mesh_origin = model.cm.Tr.GetColumn3(3);
  mesh_origin = original_Transform.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);
  float manipsize = model.cm.bbox.Diag() / 2.0;
  Matrix44f track_rotation;
  gla->trackball.track.rot.ToMatrix(track_rotation);

  glLineWidth(2.0);

  switch(current_manip_mode) 
  {
    case ManipulatorMode::ModNone:
      glTranslate(new_mesh_origin);      
      glScale(manipsize);
      glMultMatrix(Inverse(track_rotation));
	    glRotatef (90, 0, 1, 0);
	    DrawArrows(1.0,0.8,0.5);
	    glRotatef (90, 1, 0, 0);
	    DrawArrows(1.0,0.8,0.5);
      break;
    case ManipulatorMode::ModX: 
      glTranslate(new_mesh_origin);
      glScale(manipsize);
	    glRotatef (90, 0, 1, 0);
	    DrawArrows(1.0,0,0);
      break;
    case ManipulatorMode::ModY: 
      glTranslate(new_mesh_origin);
      glScale(manipsize);
	    glRotatef (90, 1, 0, 0);
	    DrawArrows(0,1.0,0);
      break;
    case ManipulatorMode::ModZ: 
      glTranslate(new_mesh_origin);
      glScale(manipsize);
	    DrawArrows(0,0,1.0);
      break;
    case ManipulatorMode::ModXX: 
      glMultMatrix(model.cm.Tr);
      glScale(manipsize);
	    glRotatef (90, 0, 1, 0);
	    DrawArrows(1.0,0.5,0.5);
      break;
    case ManipulatorMode::ModYY: 
      glMultMatrix(model.cm.Tr);
      glScale(manipsize);
	    glRotatef (90, 1, 0, 0);
	    DrawArrows(0.5,1.0,0.5);
      break;
    case ManipulatorMode::ModZZ: 
      glMultMatrix(model.cm.Tr);
      glScale(manipsize);
	    DrawArrows(0.5,0.5,1.0);
      break;
    default: ;
  }
	
  glLineWidth(1.0);
	glPopMatrix ();
}

void  EditManipulatorsPlugin::DrawScaleManipulators(MeshModel &model, GLArea *gla)
{  
  glPushMatrix ();

  Point3f mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis, new_mesh_origin;
  new_mesh_origin = model.cm.Tr.GetColumn3(3);
  mesh_origin = original_Transform.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);
  float manipsize = model.cm.bbox.Diag() / 2.0;
  Matrix44f track_rotation;
  gla->trackball.track.rot.ToMatrix(track_rotation);

  glLineWidth(2.0);
	
  switch(current_manip_mode) 
  {
    case ManipulatorMode::ModNone: 
      glTranslate(mesh_origin);
      glScale(manipsize);
      glMultMatrix(Inverse(track_rotation));
	    glRotatef (90, 0, 1, 0);
	    DrawCubes(1.0,0.8,0.5);
      glRotatef (90, 1, 0, 0);
	    DrawCubes(1.0,0.8,0.5);
      break;
    case ManipulatorMode::ModX: 
      glTranslate(mesh_origin);
      glScale(manipsize);
	    glRotatef (90, 0, 1, 0);
	    DrawCubes(1.0,0,0);
      break;
    case ManipulatorMode::ModY: 
      glTranslate(mesh_origin);
      glScale(manipsize);
	    glRotatef (90, 1, 0, 0);
	    DrawCubes(0,1.0,0);
      break;
    case ManipulatorMode::ModZ: 
      glTranslate(mesh_origin);
      glScale(manipsize);
	    DrawCubes(0,0,1.0);
      break;
    case ManipulatorMode::ModXX: 
      glMultMatrix(original_Transform);
      glScale(manipsize);
	    glRotatef (90, 0, 1, 0);
	    DrawCubes(1.0,0.5,0.5);
      break;
    case ManipulatorMode::ModYY: 
      glMultMatrix(original_Transform);
      glScale(manipsize);
	    glRotatef (90, 1, 0, 0);
	    DrawCubes(0.5,1.0,0.5);
      break;
    case ManipulatorMode::ModZZ: 
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
  glPushMatrix ();

  Point3f mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis, new_mesh_origin;
  new_mesh_origin = model.cm.Tr.GetColumn3(3);
  mesh_origin = original_Transform.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);
  float manipsize = model.cm.bbox.Diag() / 2.0;
  Matrix44f track_rotation;
  gla->trackball.track.rot.ToMatrix(track_rotation);
  
  glLineWidth(2.0);

  switch(current_manip_mode) 
  {
    case ManipulatorMode::ModNone:     
      glTranslate(mesh_origin);
      glScale(manipsize);
      glMultMatrix(Inverse(track_rotation));
	    DrawCircle(1.0,0.8,0.5);
      break;
    case ManipulatorMode::ModX: 
      glTranslate(mesh_origin);
      glScale(manipsize);
	    glRotatef (90, 0, 1, 0);
	    DrawCircle(1.0,0,0);
      break;
    case ManipulatorMode::ModY: 
      glTranslate(mesh_origin);
      glScale(manipsize);
	    glRotatef (90, 1, 0, 0);
	    DrawCircle(0,1.0,0);
      break;
    case ManipulatorMode::ModZ: 
      glTranslate(mesh_origin);
      glScale(manipsize);
	    DrawCircle(0,0,1.0);
      break;
    case ManipulatorMode::ModXX: 
      glMultMatrix(original_Transform);
      glScale(manipsize);
	    glRotatef (90, 0, 1, 0);
	    DrawCircle(1.0,0.5,0.5);
      break;
    case ManipulatorMode::ModYY: 
      glMultMatrix(original_Transform);
      glScale(manipsize);
	    glRotatef (90, 1, 0, 0);
	    DrawCircle(0.5,1.0,0.5);
      break;
    case ManipulatorMode::ModZZ: 
      glMultMatrix(original_Transform);
      glScale(manipsize);
	    DrawCircle(0.5,0.5,1.0);
      break;
    default: ;
  }
	
  glLineWidth(1.0);
	glPopMatrix ();
}

void EditManipulatorsPlugin::DrawManipulators(MeshModel &model, GLArea *gla, bool onlyActive)
{
  Point3f mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis, new_mesh_origin;
  new_mesh_origin = model.cm.Tr.GetColumn3(3);
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
    case ManipulatorType::ManMove:
      DrawTranslateManipulators(model, gla);      
      break;
    case ManipulatorType::ManRotate:
      DrawRotateManipulators(model, gla);
      break;
    case ManipulatorType::ManScale: 
      DrawScaleManipulators(model, gla);
      break;
    default: ;
  }

  if(current_manip_mode != ManipulatorMode::ModNone)
  {
    glBegin(GL_LINES);

    switch(current_manip_mode) 
    {
      case ManipulatorMode::ModX: 
        glColor3f(1.0,0,0);
        glVertex(mesh_origin + Point3f(-10.0, 0.0, 0.0)); 	glVertex(mesh_origin + Point3f(10.0, 0.0, 0.0));
        break;
      case ManipulatorMode::ModY: 
        glColor3f(0,1.0,0);
        glVertex(mesh_origin + Point3f(0.0, -10.0, 0.0)); 	glVertex(mesh_origin + Point3f(0.0, 10.0, 0.0));
        break;
      case ManipulatorMode::ModZ: 
        glColor3f(0,0,1.0);
        glVertex(mesh_origin + Point3f(0.0, 0.0, -10.0)); 	glVertex(mesh_origin + Point3f(0.0, 0.0, 10.0));
        break;
      case ManipulatorMode::ModXX: 
        glColor3f(1.0,0.5,0.5);
        glVertex(mesh_origin + (mesh_xaxis * -10.0f)); 	glVertex(mesh_origin + (mesh_xaxis * 10.0f));
        break;
      case ManipulatorMode::ModYY: 
        glColor3f(0.5,1.0,0.5);
        glVertex(mesh_origin + (mesh_yaxis * -10.0f)); 	glVertex(mesh_origin + (mesh_yaxis * 10.0f));
        break;
      case ManipulatorMode::ModZZ: 
        glColor3f(0.5,0.5,1.0);
        glVertex(mesh_origin + (mesh_zaxis * -10.0f)); 	glVertex(mesh_origin + (mesh_zaxis * 10.0f));
        break;
      default: ;
    }

    glEnd();
  }

  // restore
  glPopAttrib();
}

void EditManipulatorsPlugin::Decorate(MeshModel &model, GLArea *gla, QPainter* painter)
{
  //
  Point3f center, right, top, front;

  MyPick(gla->width()*0.5, gla->height()*0.5, center, 0.5);
  MyPick(gla->width()*0.99, gla->height()*0.5, right, 0.5);
  MyPick(gla->width()*0.5, gla->height()*0.01, top, 0.5);
  MyPick(gla->width()*0.5, gla->height()*0.5, front, 0.01);

  screen_xaxis = (right - center) * 2.0;
  screen_yaxis = (top - center)   * 2.0;
  screen_zaxis = (front - center) * 2.0;

  // write manipulator data
  int ln=0;
  QString StatusString = "MANIPULATOR:";

  if(current_manip == ManipulatorType::ManNone)
  {
    StatusString += "  none ";
  }
  else
  {
    switch(current_manip) 
    {
      case ManipulatorType::ManMove: 
        StatusString += "  Translate";
        break;
      case ManipulatorType::ManRotate: 
        StatusString += "  Rotate   ";
        break;
      case ManipulatorType::ManScale: 
        StatusString += "  Scale    ";
        break;
      default: ;
    }

    switch(current_manip_mode) 
    {
      case ManipulatorMode::ModX: 
        StatusString += "  X global";
        break;
      case ManipulatorMode::ModY: 
        StatusString += "  Y global";
        break;
      case ManipulatorMode::ModZ: 
        StatusString += "  Z global";
        break;
      case ManipulatorMode::ModXX: 
        StatusString += "  X local";
        break;
      case ManipulatorMode::ModYY: 
        StatusString += "  Y local";
        break;
      case ManipulatorMode::ModZZ: 
        StatusString += "  Z local";
        break;
      default: 
        if((current_manip == ManipulatorType::ManMove) || (current_manip == ManipulatorType::ManRotate))
          StatusString += "  viewport";
        else if(current_manip == ManipulatorType::ManScale)
          StatusString += "  uniform";
        break;
    }

    // display offset factor in single axis 
    if(current_manip_mode != ManipulatorMode::ModNone)
    {
      StatusString += QString("  %1").arg(displayOffset);
    }

    // viewport translation, display the XYZ offsets
    if((current_manip_mode == ManipulatorMode::ModNone) && (current_manip == ManipulatorType::ManMove))
    {
      StatusString += QString("  %1   %2   %3").arg(displayOffset_X).arg(displayOffset_Y).arg(displayOffset_Z);
    }

    // viewport rotation, display rotation angle
    if((current_manip_mode == ManipulatorMode::ModNone) && (current_manip == ManipulatorType::ManRotate))
    {
      StatusString += QString("  %1").arg(displayOffset);      
    }

    // uniform scale, display scale factor
    if((current_manip_mode == ManipulatorMode::ModNone) && (current_manip == ManipulatorType::ManScale))
    {
      StatusString += QString("  %1").arg(displayOffset);      
    }
  }

  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, StatusString);

  if(current_manip == ManipulatorType::ManNone)   
  {
    glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "G to translate, R to rotate, S to scale");
  }
  else
  {
    if(current_manip == ManipulatorType::ManMove)
      glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "LEFT CLICK and DRAG to move");
    else if(current_manip == ManipulatorType::ManRotate)
      glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "LEFT CLICK and DRAG to rotate");
    else if(current_manip == ManipulatorType::ManScale)
      glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "LEFT CLICK and DRAG to scale");
    glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "RETURN to apply, BACKSPACE to cancel");
  }


  //
  //glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("screenOff - %1 %2").arg(currScreenOffset_X).arg(currScreenOffset_Y));
  //glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("center - %1 %2 %3").arg(center[0]).arg(center[1]).arg(center[2]));
  //glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("right  - %1 %2 %3").arg(right[0]).arg(right[1]).arg(right[2]));
  //glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("top    - %1 %2 %3").arg(top[0]).arg(top[1]).arg(top[2]));
  //glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("screen x - %1 %2 %3").arg(screen_xaxis[0]).arg(screen_xaxis[1]).arg(screen_xaxis[2]));
  //glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("screen y - %1 %2 %3").arg(screen_yaxis[0]).arg(screen_yaxis[1]).arg(screen_yaxis[2]));
  //glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, QString("screen z - %1 %2 %3").arg(screen_zaxis[0]).arg(screen_zaxis[1]).arg(screen_zaxis[2]));

  // render original mesh BBox
  DrawMeshBox(model);

  // render active manipulator
  DrawManipulators(model, gla, true);

  assert(!glGetError());
}

void EditManipulatorsPlugin::UpdateMatrix(MeshModel &model, GLArea * gla, bool applymouseoffset)
{  
  Matrix44f newmatrix;
  Matrix44f old_rot;
  Point3f old_trasl;
  Point3f new_scale;
  Point3f axis;
  float mouseXoff;
  float mouseYoff;

  Point3f mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis;
  mesh_origin = original_Transform.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);

  delta_Transform.SetIdentity();
  newmatrix.SetIdentity();

  if(current_manip == ManipulatorType::ManNone)
  {
    model.cm.Tr = original_Transform;
  }
  else
  {

    if(current_manip_mode != ManipulatorMode::ModNone)  // transform on one axis only
    {

      switch(current_manip_mode)          // which axis is active
      {
        case ManipulatorMode::ModX: 
          axis = Point3f(1.0, 0.0, 0.0);
          break;
        case ManipulatorMode::ModY: 
          axis = Point3f(0.0, 1.0, 0.0);
          break;
        case ManipulatorMode::ModZ: 
          axis = Point3f(0.0, 0.0, 1.0);
          break;
        case ManipulatorMode::ModXX: 
          axis = mesh_xaxis;
          break;
        case ManipulatorMode::ModYY: 
          axis = mesh_yaxis;
          break;
        case ManipulatorMode::ModZZ: 
          axis = mesh_zaxis;
          break;
        default: axis = Point3f(1.0, 1.0, 1.0); // it should never arrive here, anyway
      }

      if(current_manip == ManipulatorType::ManMove)
      {
        // mouse offset -> single axis translation
        float xsign = ((screen_xaxis*axis)>0.0)?1.0:-1.0;
        float ysign = ((screen_yaxis*axis)>0.0)?1.0:-1.0;
        mouseXoff = xsign * screen_xaxis.Norm() * (currScreenOffset_X/float(gla->width()));
        mouseYoff = ysign * screen_yaxis.Norm() * (currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + mouseXoff + mouseYoff;

        delta_Transform.SetTranslate(axis * displayOffset);  
        newmatrix = delta_Transform * original_Transform;
      }
      else if(current_manip == ManipulatorType::ManRotate)
      {
        // mouse offset -> single axis rotation
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + (360.0 * (mouseXoff + mouseYoff));

        if((displayOffset > 360.0) || (displayOffset < -360.0))
          displayOffset = 360.0;

        delta_Transform.SetRotateDeg(displayOffset, axis);
        old_trasl = original_Transform.GetColumn3(3);
        old_rot= original_Transform;
        old_rot.SetColumn(3, Point3f(0.0, 0.0, 0.0));
        newmatrix = delta_Transform * old_rot;
        newmatrix.SetColumn(3, old_trasl);
      }
      else if(current_manip == ManipulatorType::ManScale)
      {       
        // mouse offset -> single axis scaling
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + (2.0 * (mouseXoff + mouseYoff));

        new_scale[0] = (axis[0]==0)?1.0:(axis[0] * displayOffset);
        new_scale[1] = (axis[1]==0)?1.0:(axis[1] * displayOffset);
        new_scale[2] = (axis[2]==0)?1.0:(axis[2] * displayOffset);
        delta_Transform.SetScale(new_scale);
        old_trasl = original_Transform.GetColumn3(3);
        old_rot= original_Transform;
        old_rot.SetColumn(3, Point3f(0.0, 0.0, 0.0));
        newmatrix = delta_Transform * old_rot;
        newmatrix.SetColumn(3, old_trasl);
      }
      else
        newmatrix = original_Transform;  // it should never arrive here, anyway
      
    }
    else   // transform on full space ? on view space ?
    {


      if(current_manip == ManipulatorType::ManMove)
      {
        // mouse offset -> viewport translation
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (currScreenOffset_Y/float(gla->height()));

        displayOffset_X = currOffset_X + (screen_xaxis[0] * mouseXoff) + (screen_yaxis[0] * mouseYoff);
        displayOffset_Y = currOffset_Y + (screen_xaxis[1] * mouseXoff) + (screen_yaxis[1] * mouseYoff);        
        displayOffset_Z = currOffset_Z + (screen_xaxis[2] * mouseXoff) + (screen_yaxis[2] * mouseYoff);

        delta_Transform.SetTranslate(Point3f(displayOffset_X,displayOffset_Y,displayOffset_Z));  
        newmatrix = delta_Transform * original_Transform;
      }

      if(current_manip == ManipulatorType::ManRotate)
      {
        // mouse offset -> viewport rotation
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + (360.0 * (mouseXoff + mouseYoff));

        delta_Transform.SetRotateDeg(displayOffset, screen_zaxis);
        old_trasl = original_Transform.GetColumn3(3);
        old_rot= original_Transform;
        old_rot.SetColumn(3, Point3f(0.0, 0.0, 0.0));
        newmatrix = delta_Transform * old_rot;
        newmatrix.SetColumn(3, old_trasl);
      }

      if(current_manip == ManipulatorType::ManScale)
      {
        // mouse offset -> uniform scaling
        mouseXoff = (currScreenOffset_X/float(gla->width()));
        mouseYoff = (-currScreenOffset_Y/float(gla->height()));
        displayOffset = currOffset + (2.0 * (mouseXoff + mouseYoff));

        new_scale[0] = displayOffset;
        new_scale[1] = displayOffset;
        new_scale[2] = displayOffset;
        delta_Transform.SetScale(new_scale);
        old_trasl = original_Transform.GetColumn3(3);
        old_rot= original_Transform;
        old_rot.SetColumn(3, Point3f(0.0, 0.0, 0.0));
        newmatrix = delta_Transform * old_rot;
        newmatrix.SetColumn(3, old_trasl);
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


bool EditManipulatorsPlugin::MyPick(const int &x, const int &y, Point3f &pp, float mydepth)
{
	double res[3];
	GLdouble mm[16],pm[16]; GLint vp[4];
	glGetDoublev(GL_MODELVIEW_MATRIX,mm);
	glGetDoublev(GL_PROJECTION_MATRIX,pm);
	glGetIntegerv(GL_VIEWPORT,vp);
	
	gluUnProject(x,y,mydepth,mm,pm,vp,&res[0],&res[1],&res[2]);
	pp=Point3f(res[0],res[1],res[2]);
  return true;
}

bool EditManipulatorsPlugin::StartEdit(MeshModel &model, GLArea *gla )
{
  gla->setCursor(QCursor(QPixmap(":/images/cur_manipulators.png"),15,15));	
  connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );

  // reset filter data
  current_manip = ManipulatorType::ManNone;
  current_manip_mode = ManipulatorMode::ModNone;
  isMoving = false;
  resetOffsets();

  // storing start matrix
  original_Transform = model.cm.Tr;
  delta_Transform = vcg::Matrix44f::Identity();

  gla->update();
	return true;
}

void EditManipulatorsPlugin::EndEdit(MeshModel &model, GLArea *gla)
{
  cancelMotion(model, gla);     // something interrupted the filter... canceling 
}


