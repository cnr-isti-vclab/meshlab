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

    bDebugEdgeCol->setVisible(false);
    bDebugEdgeSplit->setVisible(false);
    
    setTool(TOOL_BRUSH);
    on_algo_simple_toggled(false);
    
    connect(bDebugEdgeCol,SIGNAL(clicked(bool)),plugin,SLOT(debugEdgeCollapse()));
    connect(bDebugEdgeSplit,SIGNAL(clicked(bool)),plugin,SLOT(debugEdgeSplit()));
    
    connect(bEdgeSplit,SIGNAL(clicked(bool)),plugin,SLOT(edgeSplit()));
    connect(bEdgeCollapse,SIGNAL(clicked(bool)),plugin,SLOT(vertexRemoval()));

    connect(bStart,SIGNAL(clicked(bool)),plugin,SLOT(start()));
    connect(bStop,SIGNAL(clicked(bool)),plugin,SLOT(stop()));
    connect(bStep,SIGNAL(clicked(bool)),plugin,SLOT(step()));
    connect(bPause,SIGNAL(clicked(bool)),plugin,SLOT(pause()));
    connect(bResume,SIGNAL(clicked(bool)),plugin,SLOT(resume()));
    
    connect(bPickEdgeOutside,SIGNAL(clicked(bool)),plugin,SLOT(pickEdgeOutside()));
    connect(bPickEdgeBox,SIGNAL(clicked(bool)),plugin,SLOT(pickEdgeBox()));
    
    connect(bPickFace,SIGNAL(clicked(bool)),plugin,SLOT(pickFace()));
    
}

WidgetRgbT::~WidgetRgbT()
{
}

void WidgetRgbT::setTool(Tool t)
{
    tool = t;
    bSelectionSingle->setChecked(false);
    bSelectionRect->setChecked(false);
    bBrush->setChecked(false);
    bEraser->setChecked(false);

    switch (t) {
        case TOOL_SELECTIONSINGLE:
            bSelectionSingle->setChecked(true);
            break;
        case TOOL_SELECTIONRECT:
            bSelectionRect->setChecked(true);
            break;
        case TOOL_BRUSH:
            bBrush->setChecked(true);
            break;
        case TOOL_ERASER:
            bEraser->setChecked(true);
            break;
        default:
            break;
    }
}

void WidgetRgbT::on_algo_simple_toggled(bool /*b*/)
{
    
    sMaxEdgeLevel->setEnabled(algo_simple->isChecked());
    sMinEdgeLevel->setEnabled(algo_simple->isChecked());
}

void WidgetRgbT::on_bSelectionSingle_clicked(bool /*b*/)
{
    setTool(TOOL_SELECTIONSINGLE);
}

void WidgetRgbT::on_bSelectionRect_clicked(bool /*b*/)
{
    setTool(TOOL_SELECTIONRECT);
}

void WidgetRgbT::on_bBrush_clicked(bool /*b*/)
{
	setTool(TOOL_BRUSH);
}
void WidgetRgbT::on_bEraser_clicked(bool /*b*/)
{
	setTool(TOOL_ERASER);
}
