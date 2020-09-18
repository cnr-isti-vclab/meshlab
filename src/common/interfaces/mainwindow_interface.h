/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#ifndef MESHLAB_MAINWINDOW_INTERFACE_H
#define MESHLAB_MAINWINDOW_INTERFACE_H

#include <QAction>
#include "../filter_parameter/rich_parameter_list.h"

/** The MainWindowInterface class defines just the executeFilter() callback function
that is invoked by the standard parameter input dialog.
It is used as base class of the MainWindow.
*/
class MainWindowInterface
{
public:
	virtual void executeFilter(QAction *, RichParameterList &, bool = false) {}
	//parexpval is a string map containing the parameter expression values set in the filter's dialog.
	//These parameter expression values will be evaluated when the filter will start.
};

#endif // MESHLAB_MAINWINDOW_INTERFACE_H
