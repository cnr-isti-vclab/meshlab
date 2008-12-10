/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
Revision 1.2  2006/01/30 17:19:22  vannini
Added manual values to control min and max Q (mapping curvature to color)

Revision 1.1  2006/01/27 18:27:53  vannini
code refactoring for curvature colorize
added colorize equalizer dialog and
"Colorize by Quality" filter
some small bugfixes
removed color_curvature.h in favour of curvature.h


****************************************************************************/
#include "equalizerDialog.h"

EqualizerDialog::EqualizerDialog(QWidget *parent): QDialog(parent)
{
	ui.setupUi(this);
  connect(ui.cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  connect(ui.applyButton, SIGNAL(clicked()), this, SLOT(accept()));
  setFixedSize(338,235);
}

EqualizerDialog::~EqualizerDialog()
{

}

void EqualizerDialog::setValues(const EqualizerSettings& es)
{
	settings=es;
  QString qnum="%1";

  ui.percentileSpinbox->setValue(settings.percentile);
  ui.rangeSpinbox->setValue(settings.range);
  ui.meshMinQlineEdit->setText(qnum.arg(settings.meshMinQ));
  ui.meshMaxQlineEdit->setText(qnum.arg(settings.meshMaxQ));
  ui.histoMinQlineEdit->setText(qnum.arg(settings.histoMinQ));
  ui.histoMaxQlineEdit->setText(qnum.arg(settings.histoMaxQ));
  ui.manualMinQlineEdit->setText(qnum.arg(settings.manualMinQ));
  ui.manualMaxQlineEdit->setText(qnum.arg(settings.manualMaxQ));
  ui.manualCheckBox->setChecked(settings.useManual);  
}

EqualizerSettings EqualizerDialog::getValues()
{
  settings.percentile = ui.percentileSpinbox->value();
  settings.range = ui.rangeSpinbox->value();
  settings.useManual=ui.manualCheckBox->isChecked();
  settings.manualMinQ=ui.manualMinQlineEdit->text().toFloat();
  settings.manualMaxQ=ui.manualMaxQlineEdit->text().toFloat();
	return settings;
}
