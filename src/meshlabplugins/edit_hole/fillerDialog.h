/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
#ifndef FILLERDIALOG_H
#define FILLERDIALOG_H

#include <QWidget>
#include <QDockWidget>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include "ui_edit_hole.h"

class FillerDialog : public QDockWidget
{
	Q_OBJECT

public:
	FillerDialog(QWidget *parent = 0);
	~FillerDialog();
	
	void clickStartBridging();
	void clickEndBridging();
	void clickSingleHoleBridgeOpt();
	

Q_SIGNALS:
	void SGN_Closing();
	
public Q_SLOTS:
	void SLOT_ExistBridge(bool exist);

public:
	Ui::FillerWidget ui;
	QWidget* parent;

private:
	virtual void closeEvent( QCloseEvent * event );

private:
	bool someBridge;
};

#endif

