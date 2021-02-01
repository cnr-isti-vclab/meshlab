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
#ifndef EditManipulatorsPLUGIN_H
#define EditManipulatorsPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <common/plugins/interfaces/edit_plugin_interface.h>

class EditManipulatorsPlugin : public QObject, public EditPluginInterface
{
  Q_OBJECT
  Q_INTERFACES(EditPluginInterface)

public:

  enum ManipulatorType	{ManNone, ManMove, ManRotate, ManScale};
  enum ManipulatorMode	{ModNone, ModView, ModX, ModY, ModZ, ModXX, ModYY, ModZZ};

  EditManipulatorsPlugin();
  virtual ~EditManipulatorsPlugin() {}
  static const QString Info();
  virtual bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
  virtual void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/, MLSceneGLSharedDataContext* /*cont*/);
  virtual void Decorate(MeshModel &, GLArea *,QPainter*);
  virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &, GLArea * );
  virtual void keyReleaseEvent(QKeyEvent *, MeshModel &, GLArea * );
  virtual void keyPressEvent(QKeyEvent *, MeshModel &, GLArea *);

  void DrawManipulators(MeshModel &model, GLArea *gla, bool onlyActive);
  void DrawMeshBox(MeshModel &model);

private:
  QFont qFont;
  
  Matrix44m original_Transform;
  Matrix44m delta_Transform;

  ManipulatorType current_manip;
  ManipulatorMode current_manip_mode;

  bool aroundOrigin;

  bool isSnapping;
  Scalarm  snapto;

  QString inputnumberstring;
  bool inputnumbernegative;
  Scalarm   inputnumber;

  bool isMoving;
  vcg::Point2i startdrag;
  vcg::Point2i enddrag;

  Scalarm currScreenOffset_X;   // horizontal offset (screen space)
  Scalarm currScreenOffset_Y;   // vertical offset (screen space)

  // when the user is dragging, the mouse offset is stored here, 
  // two sets of variables are used, since the offset will be accumulated in 
  // the currOffset* variables when finished dragging
  Scalarm displayOffset;        // mouse offset value (single axis)
  Scalarm displayOffset_X;      // mouse X offset value
  Scalarm displayOffset_Y;      // mouse Y offset value
  Scalarm displayOffset_Z;      // mouse Z offset value

  // offset is accumulated here... user can change the offset by dragging mouse until 
  // satisfied, accumulating changes
  // if the user confirms, this offset is applied to the matrix
  Scalarm currOffset;     // combined offset value (single axis)
  Scalarm currOffset_X;     // X offset value
  Scalarm currOffset_Y;     // Y offset value
  Scalarm currOffset_Z;     // Z offset value

  Point3m screen_xaxis;
  Point3m screen_yaxis;
  Point3m screen_zaxis;

  void DrawCircle(float r, float g, float b);
  void DrawArrows(float r, float g, float b);
  void DrawCubes(float r, float g, float b);
  void DrawRotateManipulators(MeshModel &model, GLArea *gla);
  void DrawScaleManipulators(MeshModel &model, GLArea *gla);
  void DrawTranslateManipulators(MeshModel &model, GLArea *gla); 

  void resetOffsets();

  void UpdateMatrix(MeshModel &model, GLArea * gla, bool applymouseoffset, bool useinputnumber=false);

  void applyMotion(MeshModel &model, GLArea *gla);
  void cancelMotion(MeshModel &model, GLArea *gla);

  bool MyPick(const int &x, const int &y, Point3m &pp, float mydepth);

signals:
  void suspendEditToggle();
};

#endif
