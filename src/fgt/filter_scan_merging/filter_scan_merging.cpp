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

#include <QtGui>

#include "filter_scan_merging.h"

using namespace std;
using namespace vcg;

// Constructor
FilterScanMergingPlugin::FilterScanMergingPlugin()
{
}

// ST() return the very short string describing each filtering action
 QString FilterScanMergingPlugin::filterName() const
{
   return QString("Merges different layers into one mesh.");
}

// Info() return the longer string describing each filtering action
 QString FilterScanMergingPlugin::filterInfo() const
{
   return QString("All the layers are merged into one mesh without loss of the high frequencies details.");
}

// This function define the needed parameters for each filter.
void FilterScanMergingPlugin::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet &parlst)
{
}

// Core Function doing the actual mesh processing.
bool FilterScanMergingPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
}

Q_EXPORT_PLUGIN(FilterScanMergingPlugin)
