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
