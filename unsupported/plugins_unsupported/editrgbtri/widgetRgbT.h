#ifndef WIDGETRGBT_H_
#define WIDGETRGBT_H_

/****************************************************************************
* Rgb Triangulations Plugin                                                 *
*                                                                           *
* Author: Daniele Panozzo (daniele.panozzo@gmail.com)                       *
* Copyright(C) 2007                                                         *
* DISI - Department of Computer Science                                     *
* University of Genova                                                      *
*                                                                           *
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
****************************************************************************/


#include <QWidget>
#include "ui_widgetRgbT.h"

//! Tool for selection
enum Tool
{
    TOOL_SELECTIONSINGLE,
    TOOL_BRUSH,
    TOOL_ERASER
};

//! UI for the plugin
class WidgetRgbT : public QWidget, public Ui::WidgetRgbT
{
    Q_OBJECT
public:
	WidgetRgbT(QWidget* parent, QObject* plugin);
	virtual ~WidgetRgbT();
    Tool tool;
	
public slots:	
	void setTool(Tool t);
	
private slots:
    void on_bSelectionSingle_clicked(bool b);
    void on_bBrush_clicked(bool b);
    void on_bEraser_clicked(bool b);
};

#endif /*WIDGETRGBT_H_*/
