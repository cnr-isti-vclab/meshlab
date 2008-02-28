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

#include "editstraightenerdialog.h"

using namespace vcg;

EditStraightenerDialog::EditStraightenerDialog(QWidget *parent)
  :QWidget(parent),special_mode_active(false),undo_enabled(false),
freeze_enabled(false),sfn(0),single_slider_action(true)
{
  ui.setupUi(this);
  updateEnabled();
}

void EditStraightenerDialog::SetUndo(bool value)
{
  if(undo_enabled==value)
    return;
  undo_enabled=value;
  updateEnabled();
}

void EditStraightenerDialog::SetFreeze(bool value)
{
  if(freeze_enabled==value)
    return;
  freeze_enabled=value;
  updateEnabled();
}

void EditStraightenerDialog::endSpecialMode()
{
  if(!special_mode_active)
    return;
  special_mode_active=false;
  ui.freehandAxisDraggingPushButton->setChecked(false);
  ui.freehandMeshDraggingPushButton->setChecked(false);
  ui.drawXAxisOnMeshPushButton->setChecked(false);
  ui.drawYAxisOnMeshPushButton->setChecked(false);
  ui.drawZAxisOnMeshPushButton->setChecked(false);
  ui.drawXAndYAxesOnMeshPushButton->setChecked(false);
  ui.drawYAndZAxesOnMeshPushButton->setChecked(false);
  ui.drawZAndXAxesOnMeshPushButton->setChecked(false);
  updateEnabled();
}

void EditStraightenerDialog::updateSfn(int n)
{
  if(n==sfn)
    return;
  sfn=n;
  updateEnabled();
}

void EditStraightenerDialog::shoutShow()
{
  emit update_show(ui.specialBboxCheckBox->isChecked(),
                   ui.baseShowAxesCheckBox->isChecked(),
                   ui.baseShowLabelsCheckBox->isChecked(),
                   ui.baseShowValuesCheckBox->isChecked(),
                   ui.currentShowAxesCheckBox->isChecked(),
                   ui.currentShowLabelsCheckBox->isChecked(),
                   ui.currentShowValuesCheckBox->isChecked(),
                   ui.moveShowCheckBox->isChecked(),
                   ui.rotationShowCheckBox->isChecked());
}

float EditStraightenerDialog::getSnap()
{
  return float(ui.snapRotationDoubleSpinBox->value());
}

void EditStraightenerDialog::updateEnabled()
{
  ui.swapGroupBox->setEnabled(!special_mode_active);
  ui.flipGroupBox->setEnabled(!special_mode_active);
  ui.rotate90GroupBox->setEnabled(!special_mode_active);
  ui.alignWithViewPushButton->setEnabled(!special_mode_active);
  ui.moveWrtBboxGroupBox->setEnabled(!special_mode_active);
  ui.centerOnGroupBox->setEnabled(!special_mode_active);
  ui.freehandAxisGroupBox->setEnabled(!special_mode_active);
  ui.freehandMeshDraggingPushButton->setEnabled(!special_mode_active);
  ui.drawOnMeshGroupBox->setEnabled(!special_mode_active);
  ui.drawXAxisOnMeshPushButton->setEnabled(!special_mode_active);
  ui.drawYAxisOnMeshPushButton->setEnabled(!special_mode_active);
  ui.drawZAxisOnMeshPushButton->setEnabled(!special_mode_active);
  ui.drawXAndYAxesOnMeshPushButton->setEnabled(!special_mode_active);
  ui.drawYAndZAxesOnMeshPushButton->setEnabled(!special_mode_active);
  ui.drawZAndXAxesOnMeshPushButton->setEnabled(!special_mode_active);
  ui.getPlaneFomSelectionGroupBox->setEnabled(!special_mode_active && (sfn > 0)); // different
  ui.undoPushButton->setEnabled(!special_mode_active && undo_enabled); // different
  ui.freezePushButton->setEnabled(!special_mode_active && freeze_enabled); // different
}

void EditStraightenerDialog::draw_toggled(bool checked,char c1,char c2,QPushButton *button)
{
  if(special_mode_active==checked)
    return;
  special_mode_active=checked;
  emit draw_on_mesh(checked,c1,c2);
  updateEnabled();
  ui.drawOnMeshGroupBox->setEnabled(true);
  button->setEnabled(true);
}

// slots
void EditStraightenerDialog::on_swapXwithYPushButton_clicked()
{
  emit rot(180,Point3f(1,1,0));
}

void EditStraightenerDialog::on_swapYwithZPushButton_clicked()
{
  emit rot(180,Point3f(0,1,1));
}

void EditStraightenerDialog::on_swapZwithXPushButton_clicked()
{
  emit rot(180,Point3f(1,0,1));
}

void EditStraightenerDialog::on_flipXandYPushButton_clicked()
{
  emit rot(180,Point3f(0,0,1));
}

void EditStraightenerDialog::on_flipYandZPushButton_clicked()
{
  emit rot(180,Point3f(1,0,0));
}

void EditStraightenerDialog::on_flipZandXPushButton_clicked()
{
  emit rot(180,Point3f(0,1,0));
}

void EditStraightenerDialog::on_rotate90onXPushButton_clicked()
{
  emit rot(90,Point3f(1,0,0));
}

void EditStraightenerDialog::on_rotate90onYPushButton_clicked()
{
  emit rot(90,Point3f(0,1,0));
}

void EditStraightenerDialog::on_rotate90onZPushButton_clicked()
{
  emit rot(90,Point3f(0,0,1));
}

void EditStraightenerDialog::on_alignWithViewPushButton_clicked()
{
  emit align_with_view();
}

void EditStraightenerDialog::on_bboxXSlider_sliderPressed()
{
  emit begin_action();
  single_slider_action=false;
}

void EditStraightenerDialog::on_bboxYSlider_sliderPressed()
{
  emit begin_action();
  single_slider_action=false;
}

void EditStraightenerDialog::on_bboxZSlider_sliderPressed()
{
  emit begin_action();
  single_slider_action=false;
}

void EditStraightenerDialog::on_bboxXSlider_valueChanged(int value)
{
  if (single_slider_action)
    emit begin_action();
  emit move_axis_to_bbox(0,value/100.0f);
  if (single_slider_action)
  	emit apply();
}

void EditStraightenerDialog::on_bboxYSlider_valueChanged(int value)
{
  if (single_slider_action)
    emit begin_action();
  emit move_axis_to_bbox(1,value/100.0f);
  if (single_slider_action)
  	emit apply();
}

void EditStraightenerDialog::on_bboxZSlider_valueChanged(int value)
{
  if (single_slider_action)
    emit begin_action();
  emit move_axis_to_bbox(2,value/100.0f);
  if (single_slider_action)
  	emit apply();
}

void EditStraightenerDialog::on_bboxXSlider_sliderReleased()
{
  single_slider_action=true;
  emit apply();
}

void EditStraightenerDialog::on_bboxYSlider_sliderReleased()
{
  single_slider_action=true;
  emit apply();
}

void EditStraightenerDialog::on_bboxZSlider_sliderReleased()
{
  single_slider_action=true;
  emit apply();
}

void EditStraightenerDialog::on_centerOnBboxPushButton_clicked()
{
  emit begin_action();
  single_slider_action=false;
  if(ui.bboxXSlider->value()==50) emit move_axis_to_bbox(0,0.5f);
  else ui.bboxXSlider->setValue(50);
  
  if(ui.bboxYSlider->value()==50) emit move_axis_to_bbox(1,0.5f);
  else ui.bboxYSlider->setValue(50);
  
  if(ui.bboxZSlider->value()==50) emit move_axis_to_bbox(2,0.5f);
  else ui.bboxZSlider->setValue(50);
  single_slider_action=true;
  emit apply(); 
}

void EditStraightenerDialog::on_centerOnTrackballPushButton_clicked()
{
  emit center_on_trackball();
}

void EditStraightenerDialog::on_freehandAxisDraggingPushButton_toggled(bool checked)
{
  if(special_mode_active==checked)
    return;
  special_mode_active=checked;
  emit freehand_axis_dragging(checked);
  updateEnabled();
  ui.freehandAxisGroupBox->setEnabled(true);
  ui.freehandAxisDraggingPushButton->setEnabled(true);
  ui.snapRotationLabel->setEnabled(checked);
  ui.snapRotationDoubleSpinBox->setEnabled(checked);
}

void EditStraightenerDialog::on_snapRotationDoubleSpinBox_valueChanged(double value)
{
  emit set_snap(float(value));
}

void EditStraightenerDialog::on_freehandMeshDraggingPushButton_toggled(bool checked)
{
  if(special_mode_active==checked)
    return;
  special_mode_active=checked;
  emit freehand_mesh_dragging(checked);
  updateEnabled();
  ui.freehandMeshDraggingPushButton->setEnabled(true);
}

void EditStraightenerDialog::on_drawXAxisOnMeshPushButton_toggled(bool checked)
{
  draw_toggled(checked,'X',' ',ui.drawXAxisOnMeshPushButton);
}

void EditStraightenerDialog::on_drawYAxisOnMeshPushButton_toggled(bool checked)
{
  draw_toggled(checked,'Y',' ',ui.drawYAxisOnMeshPushButton);
}

void EditStraightenerDialog::on_drawZAxisOnMeshPushButton_toggled(bool checked)
{
  draw_toggled(checked,'Z',' ',ui.drawZAxisOnMeshPushButton);
}

void EditStraightenerDialog::on_drawXAndYAxesOnMeshPushButton_toggled(bool checked)  
{
  draw_toggled(checked,'X','Y',ui.drawXAndYAxesOnMeshPushButton);
}

void EditStraightenerDialog::on_drawYAndZAxesOnMeshPushButton_toggled(bool checked)  
{
  draw_toggled(checked,'Y','Z',ui.drawYAndZAxesOnMeshPushButton);
}

void EditStraightenerDialog::on_drawZAndXAxesOnMeshPushButton_toggled(bool checked)   
{
  draw_toggled(checked,'Z','X',ui.drawZAndXAxesOnMeshPushButton);
}

void EditStraightenerDialog::on_getXYPlaneFromSelectionPushButton_clicked()
{
  emit get_plane_from_selection('Z','X');
}

void EditStraightenerDialog::on_getYZPlaneFromSelectionPushButton_clicked()
{
  emit get_plane_from_selection('X','Y');
}

void EditStraightenerDialog::on_getZXPlaneFromSelectionPushButton_clicked()
{
  emit get_plane_from_selection('Y','Z');
}

void EditStraightenerDialog::on_specialBboxCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_baseShowAxesCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_baseShowLabelsCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_baseShowValuesCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_currentShowAxesCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_currentShowLabelsCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_currentShowValuesCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_moveShowCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_rotationShowCheckBox_clicked()
{
  shoutShow();
}

void EditStraightenerDialog::on_freezePushButton_clicked()
{
  emit freeze();
}

void EditStraightenerDialog::on_undoPushButton_clicked()
{
  emit undo();
}
