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

#include "widgetRgbT.h"
WidgetRgbT::WidgetRgbT(QWidget* parent,QObject* plugin) : QWidget(parent)
{
    setupUi(this);

    setTool(TOOL_BRUSH);
    
    connect(bEdgeSplit,SIGNAL(clicked(bool)),plugin,SLOT(edgeSplit()));
    connect(bEdgeCollapse,SIGNAL(clicked(bool)),plugin,SLOT(vertexRemoval()));
}

WidgetRgbT::~WidgetRgbT()
{
}

void WidgetRgbT::setTool(Tool t)
{
    tool = t;
    bSelectionSingle->setChecked(false);
    bBrush->setChecked(false);
    bEraser->setChecked(false);

    switch (t) {
        case TOOL_SELECTIONSINGLE:
            bSelectionSingle->setChecked(true);
            break;
        case TOOL_BRUSH:
            bBrush->setChecked(true);
            sBrushLevel->setValue(2);
            break;
        case TOOL_ERASER:
            bEraser->setChecked(true);
            sBrushLevel->setValue(0);
            break;
        default:
            break;
    }
}

void WidgetRgbT::on_bSelectionSingle_clicked(bool /*b*/)
{
    setTool(TOOL_SELECTIONSINGLE);
}

void WidgetRgbT::on_bBrush_clicked(bool /*b*/)
{
	setTool(TOOL_BRUSH);
}
void WidgetRgbT::on_bEraser_clicked(bool /*b*/)
{
	setTool(TOOL_ERASER);
}
