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

#include "sample_pclfilter.h"
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

SamplePCLFilterPlugin::SamplePCLFilterPlugin()
:MeshFilterInterface()
{
	typeList << FP_PCL_SAMPLE;

	foreach(FilterIDType tt , types())
		actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString SamplePCLFilterPlugin::filterName(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_PCL_SAMPLE :  return QString("Point Cloud Library Filter Example");
		default : assert(0);
	}
	return QString();
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString SamplePCLFilterPlugin::filterInfo(FilterIDType filterId) const
{
	switch(filterId) {
		case FP_PCL_SAMPLE :  return QString("Small useless filter added only to show how to work with a Point Cloud Library with MeshLab.");
		default : assert(0);
	}
	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits.
// This choice affect the submenu in which each filter will be placed
// More than a single class can be chosen.
SamplePCLFilterPlugin::FilterClass SamplePCLFilterPlugin::getClass(QAction *a)
{
	switch(ID(a))
	{
	case FP_PCL_SAMPLE :  return MeshFilterInterface::Generic;
	default : assert(0);
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void SamplePCLFilterPlugin::initParameterSet(QAction * action, MeshModel & m, RichParameterSet & parlst)
{
	(void)m;

	switch(ID(action))
	{
	case FP_PCL_SAMPLE :
		{
			break;
		}
	default : assert(0);
	}
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool SamplePCLFilterPlugin::applyFilter(QAction * a, MeshDocument & md , RichParameterSet & par, vcg::CallBackPos * /*cb*/)
{
	switch(ID(a))
	{
	case FP_PCL_SAMPLE:
		{
			pcl::PointCloud<pcl::PointXYZ> cloud;

			// Fill in the cloud data
			cloud.width    = 5;
			cloud.height   = 1;
			cloud.is_dense = false;
			cloud.points.resize (cloud.width * cloud.height);

			for (size_t i = 0; i < cloud.points.size (); ++i)
			{
				cloud.points[i].x = 1024 * rand () / (RAND_MAX + 1.0f);
				cloud.points[i].y = 1024 * rand () / (RAND_MAX + 1.0f);
				cloud.points[i].z = 1024 * rand () / (RAND_MAX + 1.0f);
			}

			pcl::io::savePCDFileASCII ("test_pcd.pcd", cloud);
			std::cerr << "Saved " << cloud.points.size () << " data points to test_pcd.pcd." << std::endl;

			for (size_t i = 0; i < cloud.points.size (); ++i)
				std::cerr << "    " << cloud.points[i].x << " " << cloud.points[i].y << " " << cloud.points[i].z << std::endl;
			break;
		}
	}
	return true;
}

MESHLAB_PLUGIN_NAME_EXPORTER(SamplePCLFilterPlugin)
