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
$Log$
Revision 1.1  2008/02/16 14:29:35  benedetti
first version


****************************************************************************/

#ifndef EDITSTRAIGHTENERDIALOG_H
#define EDITSTRAIGHTENERDIALOG_H

#include <QWidget>

#include <vcg/space/point3.h>

#include "ui_editstraightener.h"

using namespace vcg;

class EditStraightenerDialog : public QWidget
{
  Q_OBJECT

public:
  EditStraightenerDialog(QWidget *parent=0);
  virtual ~EditStraightenerDialog() {}

  void enableUndo();
  void disableUndo();
  void endSpecialMode();
  void updateSfn(int);
  void shoutShow();

private:
  Ui::straightenerToolBox ui;
  bool special_mode_active;
  bool undo_enabled;
  int sfn;
  
  void updateEnabled();

public slots:
  // general buttons
  void on_applyPushButton_clicked();
  void on_applyAndFreezePushButton_clicked();
  void on_undoPushButton_clicked();
  void on_resetAllPushButton_clicked();

  // axes tab
  void on_swapXwithYPushButton_clicked();
  void on_swapYwithZPushButton_clicked();
  void on_swapZwithXPushButton_clicked();
  void on_flipXandYPushButton_clicked();
  void on_flipYandZPushButton_clicked();
  void on_flipZandXPushButton_clicked();
  void on_alignWithViewPushButton_clicked();
  void on_resetAxesPushButton_clicked();

  // origin tab
  void on_bboxXSlider_valueChanged(int);
  void on_bboxYSlider_valueChanged(int);
  void on_bboxZSlider_valueChanged(int);
  void on_bboxXSlider_sliderPressed();
  void on_bboxYSlider_sliderPressed();
  void on_bboxZSlider_sliderPressed();
  void on_centerOnBboxPushButton_clicked();
  void on_centerOnTrackbalPushButton_clicked();
  void on_resetOriginPushButton_clicked();

  // special tab
  void on_drawZAxisOnMeshPushButton_toggled(bool);
  void on_drawZAndYAxesOnMeshPushButton_toggled(bool);    
  void on_getXYPlaneFromSelectionPushButton_clicked();
  void on_freehandMeshDraggingPushButton_toggled(bool);
  void on_snapRotationDoubleSpinBox_valueChanged(double);

  // show tab
  void on_baseShowAxesCheckBox_clicked();
  void on_baseShowLabelsCheckBox_clicked();
  void on_baseShowValuesCheckBox_clicked();
  void on_currentShowAxesCheckBox_clicked();
  void on_currentShowLabelsCheckBox_clicked();
  void on_currentShowValuesCheckBox_clicked();
  void on_moveShowCheckBox_clicked();
  void on_rotationShowCheckBox_clicked();
  
signals:
  void apply();
  void freeze();
  void undo();
  void reset_axes();
  void reset_origin();  
  void flip(Point3f);
  void align_with_view();
  void move_axis_to_bbox(int,float);
  void center_on_trackball();
  void draw_on_mesh(bool,bool);
  void get_xy_plane_from_selection();
  void freehand_mesh_dragging(bool);
  void set_snap(float);
  void update_show(bool,bool,bool,bool,bool,bool,bool,bool);
  void begin_action();
};

#endif /*EDITSTRAIGHTENERDIALOG_H*/
