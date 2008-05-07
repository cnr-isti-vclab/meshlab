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
Revision 1.4  2008/02/28 10:12:32  benedetti
fixed sliders behaviour

Revision 1.3  2008/02/22 20:24:42  benedetti
refactored, cleaned up a bit, few feats added

Revision 1.2  2008/02/17 20:57:33  benedetti
updated following new specs (still got to clean up)

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

  void SetUndo(bool);
  void SetFreeze(bool value);
  void endSpecialMode();
  void updateSfn(int);
  void shoutShow();
  float getSnap();

private:
  Ui::straightenerToolBox ui;
  bool special_mode_active;
  bool undo_enabled;
  bool freeze_enabled;
  int sfn;
  bool single_slider_action;
  
  void updateEnabled();
  void draw_toggled(bool, char, char ,QPushButton *);
public slots:
  // axes tab
  void on_swapXwithYPushButton_clicked();
  void on_swapYwithZPushButton_clicked();
  void on_swapZwithXPushButton_clicked();
  void on_flipXandYPushButton_clicked();
  void on_flipYandZPushButton_clicked();
  void on_flipZandXPushButton_clicked();
  void on_rotate90onXPushButton_clicked();
  void on_rotate90onYPushButton_clicked();
  void on_rotate90onZPushButton_clicked();
  void on_alignWithViewPushButton_clicked();

  // origin tab
  void on_bboxXSlider_sliderPressed();
  void on_bboxYSlider_sliderPressed();
  void on_bboxZSlider_sliderPressed();
  void on_bboxXSlider_valueChanged(int);
  void on_bboxYSlider_valueChanged(int);
  void on_bboxZSlider_valueChanged(int);
  void on_bboxXSlider_sliderReleased();
  void on_bboxYSlider_sliderReleased();
  void on_bboxZSlider_sliderReleased();
  void on_centerOnBboxPushButton_clicked();
  void on_centerOnTrackballPushButton_clicked();

  // drag tab
  void on_freehandAxisDraggingPushButton_toggled(bool);
  void on_snapRotationDoubleSpinBox_valueChanged(double);
  void on_freehandMeshDraggingPushButton_toggled(bool);

  // special tab
  void on_drawXAxisOnMeshPushButton_toggled(bool);
  void on_drawYAxisOnMeshPushButton_toggled(bool);
  void on_drawZAxisOnMeshPushButton_toggled(bool);
  void on_drawXAndYAxesOnMeshPushButton_toggled(bool);    
  void on_drawYAndZAxesOnMeshPushButton_toggled(bool);    
  void on_drawZAndXAxesOnMeshPushButton_toggled(bool);    
  void on_getXYPlaneFromSelectionPushButton_clicked();
  void on_getYZPlaneFromSelectionPushButton_clicked();
  void on_getZXPlaneFromSelectionPushButton_clicked();

  // show tab
  void on_specialBboxCheckBox_clicked();
  void on_baseShowAxesCheckBox_clicked();
  void on_baseShowLabelsCheckBox_clicked();
  void on_baseShowValuesCheckBox_clicked();
  void on_currentShowAxesCheckBox_clicked();
  void on_currentShowLabelsCheckBox_clicked();
  void on_currentShowValuesCheckBox_clicked();
  void on_moveShowCheckBox_clicked();
  void on_rotationShowCheckBox_clicked();

  // general buttons
  void on_freezePushButton_clicked();
  void on_undoPushButton_clicked();

signals:
  void begin_action();
  void apply();
  void freeze();
  void undo();
  void rot(float,Point3f);
  void align_with_view();
  void move_axis_to_bbox(int,float);
  void center_on_trackball();
  void draw_on_mesh(bool,char,char);
  void freehand_axis_dragging(bool);
  void set_snap(float);
  void freehand_mesh_dragging(bool);
  void get_plane_from_selection(char,char);
  void update_show(bool,bool,bool,bool,bool,bool,bool,bool,bool);
};

#endif /*EDITSTRAIGHTENERDIALOG_H*/
