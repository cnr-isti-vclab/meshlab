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
Revision 1.7  2006/01/06 00:16:57  glvertex
Renamed Dialog to customDialog

Revision 1.6  2006/01/02 19:13:57  glvertex
Multi level logging

Revision 1.5  2006/01/02 17:19:19  glvertex
Changed include directive to new .ui filenames

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

#include <QDialog>
#include <QDockWidget>
#include <QColorDialog>
#include "common/filterparameter.h"
#include "stdpardialog.h"


class SettingDialog : public QDialog
{
Q_OBJECT
public:
	SettingDialog(RichParameter* curPar,RichParameter* defPar,QWidget* parent = 0);
	~SettingDialog();
public slots:
	void save();
	void reset();
	void apply();
	void load();

signals:
	void applySettingSignal();

private:
	StdParFrame frame;
	RichParameter* curPar;
	RichParameter* defPar;
	RichParameter* tmppar;
	QPushButton* savebut;
	QPushButton* applybut;
	QPushButton* resetbut;
	QPushButton* closebut;
	QPushButton* loadbut;
};

class CustomDialog : public QDialog
{
Q_OBJECT
public:
	CustomDialog(RichParameterSet& parset,RichParameterSet& defparset,QWidget *parent = 0);
	~CustomDialog();
	//void loadCurrentSetting(RichParameterSet& parset);

public slots:
	void openSubDialog(QTableWidgetItem* itm);
	void updateSettings();
signals:
	void applyCustomSetting();

private:
	RichParameterSet& curParSet;
	RichParameterSet& defParSet;
	QTableWidget* tw;
	QVector<RichParameter*> vrp; 
	void dispatch(const RichParameter& par);
	QPushButton* closebut;
};
