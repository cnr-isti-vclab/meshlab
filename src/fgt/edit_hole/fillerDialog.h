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
#include <QtGui/QDockWidget>
#include "ui_edit_hole.h"

class FillerDialog : public QDockWidget
{
	Q_OBJECT

public:
	FillerDialog(QWidget *parent = 0);
	~FillerDialog(){};
	
	
Q_SIGNALS:
	void SGN_UpdateHoleSelection();
	void SGN_RefreshHoles();
	void SGN_ProcessFilling(); 
	void SGN_UpdateGLArea();
	void SGN_Closing();
	void SGN_Apply();

public:
	Ui::dialogFillerClass ui;
	QWidget* parent;
	
	
private slots:
	void on_hListW_itemSelectionChanged();
	void on_refreshButton_clicked();
	void on_fillButton_clicked();
	void on_applyButton_clicked();

private:
	virtual void closeEvent( QCloseEvent * event );

};

#endif

