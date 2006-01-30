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

#ifndef EQUALIZERDIALOG_H
#define EQUALIZERDIALOG_H

#include <QWidget>
#include <QDialog>
#include "ui_equalizerDialog.h"

class EqualizerSettings
{
public:
	int percentile;
  int range;
  float meshMaxQ;
  float meshMinQ;
  float histoMaxQ;
  float histoMinQ;
  float manualMaxQ;
  float manualMinQ;
  bool useManual;
  		
	EqualizerSettings()
	{
    percentile=20;
    range=10000;
    meshMaxQ=meshMinQ=histoMaxQ=histoMinQ=manualMaxQ=manualMinQ=0.0f;
    useManual=false;
	};
};

class EqualizerDialog : public QDialog
{
    Q_OBJECT

private:
    Ui::EqualizerDialogClass ui;
    EqualizerSettings settings;

public:
  EqualizerDialog(QWidget *parent = 0);
  ~EqualizerDialog();
  void EqualizerDialog::setValues(const EqualizerSettings& es);
  EqualizerSettings EqualizerDialog::getValues();

};

#endif // EQUALIZERDIALOG_H
