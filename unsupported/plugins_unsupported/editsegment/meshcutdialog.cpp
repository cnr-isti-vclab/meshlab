/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#include "meshcutdialog.h"

MeshCutDialog::MeshCutDialog(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.backColorPicker->setColor(Qt::blue);
	ui.foreColorPicker->setColor(Qt::red);
	ui.foreRadioButton->setChecked(true);
}

MeshCutDialog::~MeshCutDialog()
{

}

void MeshCutDialog::on_meshSegmentButton_clicked() {
	emit meshCutSignal();
}

void MeshCutDialog::on_updateCurvatureButton_clicked() {
	emit updateCurvatureSignal();
}

void MeshCutDialog::on_foreRadioButton_clicked() {
	emit selectForegroundSignal(true);
}


void MeshCutDialog::on_backRadioButton_clicked() {
	emit selectForegroundSignal(false);
}

void MeshCutDialog::on_refineCheckBox_clicked() {
	emit doRefineSignal(ui.refineCheckBox->isChecked());
}

void MeshCutDialog::on_normalWeightSlider_valueChanged() {
	emit normalWeightSignal(ui.normalWeightSlider->value());
}
void MeshCutDialog::on_curvatureWeightSlider_valueChanged() {
	emit curvatureWeightSignal(ui.curvatureWeightSlider->value());
}		

void MeshCutDialog::on_penRadiusSlider_valueChanged() {
	emit penRadiusSignal(ui.penRadiusSlider->value());
}

//debug function
void MeshCutDialog::on_gaussianButton_clicked() {
	emit colorizeGaussianSignal();
}

void MeshCutDialog::on_meanButton_clicked() {
	emit colorizeMeanSignal();
}

void MeshCutDialog::on_resetButton_clicked() {
	emit resetSignal();
}