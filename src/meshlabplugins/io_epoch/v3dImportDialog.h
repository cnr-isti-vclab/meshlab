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

#ifndef V3DIMPORTER_DIALOG_H
#define V3DIMPORTER_DIALOG_H

// for options on decimator
#include <QDialog>
#include "ui_v3dImportDialog.h"
#include "epoch_reconstruction.h"

class v3dImportDialog : public QDialog, public Ui::v3dImportDialog {

Q_OBJECT

public:
  v3dImportDialog() : QDialog()
  {
    setupUi( this );
    subsampleSpinBox->setValue(2);
    minCountSpinBox->setValue(3);

		// connections
		connect(dilationSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(dilationSizeChanged(int)));
		connect(erosionSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(erosionSizeChanged(int)));

    er=0;
		exportToPLY=false;
	}

public:
  void setEpochReconstruction(EpochReconstruction *_er,vcg::CallBackPos *cb);
	bool exportToPLY;  /// when true all the selected range maps are exported as separated ply

public slots:
void on_selectButton_clicked();
void on_imageTableWidget_itemClicked(QTableWidgetItem * item );
void on_imageTableWidget_itemSelectionChanged();
void on_imageTableWidget_itemDoubleClicked(QTableWidgetItem * item );
void on_plyButton_clicked();
private:
EpochReconstruction *er;
QString erCreated;
QSize imgSize;


private slots:
  void on_mergeResolutionSpinBox_valueChanged(int);
  void on_subsampleSpinBox_valueChanged(int);
  void on_minCountSlider_valueChanged(int);
  void on_minCountSpinBox_valueChanged(int);
	void dilationSizeChanged(int);
	void erosionSizeChanged(int);
};

#endif 
