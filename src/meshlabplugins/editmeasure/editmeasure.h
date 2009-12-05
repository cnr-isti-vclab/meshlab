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
/****************************************************************************
  History
$Log: editmeasure.h,v $
****************************************************************************/

#ifndef EditMeasurePLUGIN_H
#define EditMeasurePLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <common/interfaces.h>
#include <wrap/gui/rubberband.h>

class EditMeasurePlugin : public QObject, public MeshEditInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshEditInterface)

public:
  EditMeasurePlugin();
  virtual ~EditMeasurePlugin() {}
  static const QString Info();
  virtual bool StartEdit(MeshModel &, GLArea *);
  virtual void EndEdit(MeshModel &, GLArea *);
  virtual void Decorate(MeshModel &, GLArea *);
  virtual void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * );
  virtual void mouseReleaseEvent(QMouseEvent *event, MeshModel &, GLArea * );

private:
  QFont qFont;
  vcg::Rubberband rubberband;
  bool was_ready;
  
signals:
  void suspendEditToggle();
};

#endif
