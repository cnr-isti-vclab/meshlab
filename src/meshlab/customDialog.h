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
Revision 1.3  2005/12/03 22:49:46  cignoni
Added copyright info

Revision 1.3  2005/11/24 01:38:36  cignoni
Added new plugins intefaces, tested with shownormal render mode

Revision 1.2  2005/11/21 12:12:54  cignoni
Added copyright info

****************************************************************************/

#include "ui_custom.h"
#include <QColor>
#include <QDialog>
#include <QColorDialog>
#include <wrap/gl/trimesh.h>

using namespace std;
using namespace vcg;

class CustomDialog : public QDialog
{
Q_OBJECT
public:
	CustomDialog(QWidget *parent = 0);

	void	 LoadCurrentSetting(const Color4b& bb,const Color4b& bt,const Color4b& l);
  Color4b GetBackgroundBottomColor(){ return cBackgroundBottom; };
	Color4b GetBackgroundTopColor(){ return cBackgroundTop; };
	Color4b GetLogColor(){ return cLog; };

private slots:
	void SetBackgroundBottomColor();
	void SetLogColor();
	void SetBackgroundTopColor();
		
private:
	Ui::Dialog ui;
	Color4b cBackgroundBottom;
	Color4b cBackgroundTop;
	Color4b cLog;
};
