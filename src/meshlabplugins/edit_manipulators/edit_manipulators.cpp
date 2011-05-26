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

using namespace vcg;

EditManipulatorsPlugin::EditManipulatorsPlugin()
{
  current_manip = ManipulatorType::ManNone;
  current_manip_mode = ManipulatorMode::ModNone;
  isMoving = false;
  currOffset = 0.0;

  original_Transform = vcg::Matrix44f::Identity();
  delta_Transform = vcg::Matrix44f::Identity();
}

const QString EditManipulatorsPlugin::Info() 
{
	return tr("Provide tools for moving meshes around the space");
}

void EditManipulatorsPlugin::mousePressEvent(QMouseEvent *, MeshModel &, GLArea * gla)
{

  gla->update();
}

void EditManipulatorsPlugin::mouseMoveEvent(QMouseEvent * event, MeshModel &, GLArea * gla)
{

  gla->update();
}

void EditManipulatorsPlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &, GLArea * gla)
{

  gla->update();
}

// Apply movement
void EditManipulatorsPlugin::applyMotion(MeshModel &model, GLArea *gla)
{
  // how to saVE IT PERMANENTLY ? 

  // reset filter data
  current_manip = ManipulatorType::ManNone;
  current_manip_mode = ManipulatorMode::ModNone;
  isMoving = false;
  currOffset = 0.0;

  // storing start matrix
  original_Transform = model.cm.Tr;
  delta_Transform = vcg::Matrix44f::Identity();

  gla->update();
}

// cancel movement, restoring old matrix
void EditManipulatorsPlugin::cancelMotion(MeshModel &model, GLArea *gla)
{
  model.cm.Tr = original_Transform;

  // reset filter data
  current_manip = ManipulatorType::ManNone;
  current_manip_mode = ManipulatorMode::ModNone;
  isMoving = false;
  currOffset = 0.0;

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
      currOffset = 0.0;
      UpdateMatrix(model);
    }
	  if (e->key() == Qt::Key_R) // rotate
    {
      current_manip = ManipulatorType::ManRotate;
      currOffset = 0.0;
      UpdateMatrix(model);
    }
    if (e->key() == Qt::Key_S) // scale
    {
      current_manip = ManipulatorType::ManScale;
      currOffset = 0.0;
      UpdateMatrix(model);
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

      currOffset = 0.0;
      UpdateMatrix(model);
    }
	  if (e->key() == Qt::Key_Y) // Y axis
    {
      if(current_manip_mode == ManipulatorMode::ModY)
        current_manip_mode = ManipulatorMode::ModYY;
      else
        current_manip_mode = ManipulatorMode::ModY;

      currOffset = 0.0;
      UpdateMatrix(model);
    }
	  if (e->key() == Qt::Key_Z) // Z axis
    {
      if(current_manip_mode == ManipulatorMode::ModZ)
        current_manip_mode = ManipulatorMode::ModZZ;
      else
        current_manip_mode = ManipulatorMode::ModZ;

      currOffset = 0.0;
      UpdateMatrix(model);
    }
  }
	
  if (e->key() == Qt::Key_Q) // DEBUG DEBUG
  {
    currOffset += 0.05;
    UpdateMatrix(model);
  }
  if (e->key() == Qt::Key_A) // DEBUG DEBUG
  {
    currOffset -= 0.05;
    UpdateMatrix(model);
  }
  if (e->key() == Qt::Key_W) // DEBUG DEBUG
  {
    currOffset += 5;
    UpdateMatrix(model);
  }
  if (e->key() == Qt::Key_E) // DEBUG DEBUG
  {
    currOffset -= 5;
    UpdateMatrix(model);
  }

  //else e->ignore();
  gla->update();
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

void EditManipulatorsPlugin::DrawManipulators(MeshModel &model, bool onlyActive)
{
  Point3f mesh_origin, mesh_xaxis, mesh_yaxis, mesh_zaxis;
  mesh_origin = original_Transform.GetColumn3(3);
  mesh_xaxis = original_Transform.GetColumn3(0);
  mesh_yaxis = original_Transform.GetColumn3(1);
  mesh_zaxis = original_Transform.GetColumn3(2);

  // setup
  glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT );
	glDisable(GL_LIGHTING);

  switch(current_manip) 
  {
    case ManipulatorType::ManMove:
      
      break;
    case ManipulatorType::ManRotate: 
      
      break;
    case ManipulatorType::ManScale: 
      
      break;
    default: ;
  }

  // move

  // rotate

  // scale

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
  // write manipulator data
  int ln=0;
  QString StatusString = "MANIPULATOR: ";

  if(current_manip == ManipulatorType::ManNone)
  {
    StatusString += "none ";
  }
  else
  {
    switch(current_manip) 
    {
      case ManipulatorType::ManMove: 
        StatusString += " Translate ";
        break;
      case ManipulatorType::ManRotate: 
        StatusString += " Rotate ";
        break;
      case ManipulatorType::ManScale: 
        StatusString += " Scale ";
        break;
      default: ;
    }

    switch(current_manip_mode) 
    {
      case ManipulatorMode::ModX: 
        StatusString += "- X global";
        break;
      case ManipulatorMode::ModY: 
        StatusString += "- Y global";
        break;
      case ManipulatorMode::ModZ: 
        StatusString += "- Z global";
        break;
      case ManipulatorMode::ModXX: 
        StatusString += "- X local";
        break;
      case ManipulatorMode::ModYY: 
        StatusString += "- Y local";
        break;
      case ManipulatorMode::ModZZ: 
        StatusString += "- Z local";
        break;
      default: ;
    }

    if(current_manip_mode != ManipulatorMode::ModNone)
    {
      StatusString += QString(" - %1").arg(currOffset);
    }
  }

  glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, StatusString);

  if(current_manip == ManipulatorType::ManNone)   
  {
    glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "G to translate, R to rotate, S to scale");
  }
  else
  {
    glLabel::render2D(painter,glLabel::TOP_LEFT,ln++, "RETURN to apply, BACKSPACE to cancel");
  }

  // render original mesh BBox
  DrawMeshBox(model);

  // render active manipulator
  DrawManipulators(model, true);

  assert(!glGetError());
}

void EditManipulatorsPlugin::UpdateMatrix(MeshModel &model)
{  
  Matrix44f newmatrix;
  Point3f   axis;

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
      Matrix44f old_rot;
      Point3f old_trasl;
      Point3f new_scale;

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
        default: axis = Point3f(0.0, 0.0, 0.0); // it should never arrive here, anyway
      }

      switch(current_manip) 
      {
        case ManipulatorType::ManMove:
          delta_Transform.SetTranslate(axis * currOffset);
          newmatrix = delta_Transform * original_Transform;
          break;
        case ManipulatorType::ManRotate: 
          delta_Transform.SetRotateDeg(currOffset, axis);
          old_trasl = original_Transform.GetColumn3(3);
          old_rot= original_Transform;
          old_rot.SetColumn(3, Point3f(0.0, 0.0, 0.0));
          newmatrix = delta_Transform * old_rot;
          newmatrix.SetColumn(3, old_trasl);
          break;
        case ManipulatorType::ManScale:
          new_scale[0] = (axis[0]==0)?1.0:(axis[0] * currOffset);
          new_scale[1] = (axis[1]==0)?1.0:(axis[1] * currOffset);
          new_scale[2] = (axis[2]==0)?1.0:(axis[2] * currOffset);
          delta_Transform.SetScale(new_scale);
          old_trasl = original_Transform.GetColumn3(3);
          old_rot= original_Transform;
          old_rot.SetColumn(3, Point3f(0.0, 0.0, 0.0));
          newmatrix = delta_Transform * old_rot;
          newmatrix.SetColumn(3, old_trasl);
          //newmatrix = delta_Transform * original_Transform;
          break;
        default: newmatrix = original_Transform;  // it should never arrive here, anyway
      }
    }
    else   // transform on full space ? on view space ?
    {
      newmatrix = original_Transform;
    }


    model.cm.Tr = newmatrix;
  }
}

bool EditManipulatorsPlugin::StartEdit(MeshModel &model, GLArea *gla )
{
  gla->setCursor(QCursor(QPixmap(":/images/cur_manipulators.png"),15,15));	
  connect(this, SIGNAL(suspendEditToggle()),gla,SLOT(suspendEditToggle()) );

  // reset filter data
  current_manip = ManipulatorType::ManNone;
  current_manip_mode = ManipulatorMode::ModNone;
  isMoving = false;
  currOffset = 0.0;

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
