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
Revision 1.4  2006/06/08 11:55:07  zifnab1974
Do not use classname in class definition

Revision 1.3  2006/01/02 17:19:19  glvertex
Changed include directive to new .ui filenames

Revision 1.2  2005/12/06 10:42:03  vannini
Snapshot dialog now works

Revision 1.1  2005/12/05 18:15:27  vannini
Added snapshot save dialog (not used yet)


****************************************************************************/

#include "glarea.h"
#include <QDialog>
#include <QFileDialog>

namespace Ui 
{
	class SSDialog;
} 

class SaveSnapshotDialog: public QDialog
{
Q_OBJECT

public:
	SaveSnapshotDialog(QWidget *parent = 0);
	~SaveSnapshotDialog();
	void setValues(const SnapshotSetting &ss);
	SnapshotSetting getValues();
	
private slots:
	void browseDir();

private:
	Ui::SSDialog* ui;
	SnapshotSetting settings;
};
