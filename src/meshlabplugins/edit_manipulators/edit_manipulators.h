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

#include <common/interfaces.h>

class EditManipulatorsPlugin : public QObject, public MeshEditInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshEditInterface)

public:

  enum ManipulatorType	{ManNone, ManMove, ManRotate, ManScale};
  enum ManipulatorMode	{ModNone, ModView, ModX, ModY, ModZ, ModXX, ModYY, ModZZ};

  EditManipulatorsPlugin();
  virtual ~EditManipulatorsPlugin() {}
  static const QString Info();
  virtual bool StartEdit(MeshModel &, GLArea *);
  virtual void EndEdit(MeshModel &, GLArea *);
  virtual void Decorate(MeshModel &, GLArea *,QPainter*);
  virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &, GLArea * );
  virtual void keyReleaseEvent(QKeyEvent *, MeshModel &, GLArea * );

  void DrawManipulators(MeshModel &model, bool onlyActive);
  void DrawMeshBox(MeshModel &model);

private:
  QFont qFont;
  
  vcg::Matrix44f original_Transform;
  vcg::Matrix44f delta_Transform;

  ManipulatorType current_manip;
  ManipulatorMode current_manip_mode;

  bool isMoving;
  vcg::Point2i startdrag;
  vcg::Point2i enddrag;

  float currScreenOffset_X;   // horizontal offset (screen space)
  float currScreenOffset_Y;   // vertical offset (screen space)

  // when the user is dragging, the mouse offset is stored here, 
  // two sets of variables are used, since the offset will be accumulated in 
  // the currOffset* variables when finished dragging
  float displayOffset;        // mouse offset value (single axis)
  float displayOffset_X;      // mouse X offset value
  float displayOffset_Y;      // mouse Y offset value
  float displayOffset_Z;      // mouse Z offset value

  // offset is accumulated here... user can change the offset by dragging mouse until 
  // satisfied, accumulating changes
  // if the user confirms, this offset is applied to the matrix
  float currOffset;     // combined offset value (single axis)
  float currOffset_X;     // X offset value
  float currOffset_Y;     // Y offset value
  float currOffset_Z;     // Z offset value

  vcg::Point3f screen_xaxis;
  vcg::Point3f screen_yaxis;
  vcg::Point3f screen_zaxis;

  void resetOffsets();

  void UpdateMatrix(MeshModel &model, GLArea * gla, bool applymouseoffset);

  void applyMotion(MeshModel &model, GLArea *gla);
  void cancelMotion(MeshModel &model, GLArea *gla);

  bool MyPick(const int &x, const int &y, vcg::Point3f &pp, float mydepth);

signals:
  void suspendEditToggle();
};

#endif
