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
  float currOffset;

  void UpdateMatrix(MeshModel &model);

  void applyMotion(MeshModel &model, GLArea *gla);
  void cancelMotion(MeshModel &model, GLArea *gla);

signals:
  void suspendEditToggle();
};

#endif
