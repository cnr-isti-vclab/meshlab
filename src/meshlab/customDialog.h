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
Revision 1.4  2005/12/04 16:50:15  glvertex
Removed [using namespace] directive form .h
Renaming in QT style
Adapted method behavior to the new ui interface

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
//#include <wrap/gl/trimesh.h>
#include <vcg/space/color4.h>

class CustomDialog : public QDialog
{
Q_OBJECT
public:
	CustomDialog(QWidget *parent = 0);

	void loadCurrentSetting(const vcg::Color4b& bkgBottom,const vcg::Color4b& bkgTop,const vcg::Color4b& logArea);
  
	vcg::Color4b getBkgBottomColor()	{ return bkgBottomColor;}
	vcg::Color4b getBkgTopColor()			{ return bkgTopColor;		}
	vcg::Color4b getLogAreaColor()		{ return logAreaColor;	}

private slots:
	void setBkgBottomColor();
	void setBkgTopColor();
	void setLogAreaColor();
		
private:
	Ui::Dialog ui;

	vcg::Color4b bkgBottomColor;
	vcg::Color4b bkgTopColor;
	vcg::Color4b logAreaColor;
};
