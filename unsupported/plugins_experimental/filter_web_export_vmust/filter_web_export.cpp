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

#include <QtScript>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/inertia.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/update/selection.h>
#include <vcg/complex/append.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/complex/algorithms/bitquad_support.h>
#include <vcg/complex/algorithms/bitquad_optimization.h>

#include "filter_web_export.h"

using namespace std;
using namespace vcg;

// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterWebExportVMustPlugin::FilterWebExportVMustPlugin() 
{ 
	typeList << FP_WEB_EXPORT;
  
  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
QString FilterWebExportVMustPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) 
	{
		case FP_WEB_EXPORT :  return QString("Web Export"); 
	
		default : assert(0); 
	}

  return QString();
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString FilterWebExportVMustPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) 
	{
		case FP_WEB_EXPORT : return QString("This filter prepares a Web version of the input 3D model according to a chosen pre-defined template. It is based on the CIF API of the V-MUST CIF PIPELINE."); 
		default : assert(0); 
	}

	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
FilterWebExportVMustPlugin::FilterClass FilterWebExportVMustPlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
		case FP_WEB_EXPORT :  return MeshFilterInterface::Layer; 
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
void FilterWebExportVMustPlugin::initParameterSet(QAction *action, MeshDocument &md, RichParameterSet & parlst) 
{
	int meshCount = md.meshList.size();

	// tries to detect the target mesh
	MeshModel* target = md.mm();
  
	parlst.addParam(new RichMesh("target_mesh", target, &md, "Target mesh:", "The mesh to export."));

	templateNamesHR << "Basic Viewer" << "Standard Viewer" << "Fullsize Viewer" << "Radiance Scaling" <<
		"Walk Through" << "POP Geometry" << "Nexus conversion";
	parlst.addParam(new RichEnum("template", 1, templateNamesHR, "Web Template:", "Web template to use."));
	parlst.addParam(new RichString("notification_email", "youremail@domain.com", "Notification email:", 
		"A link to download the exported model will be send at this email address."));

	// names to be used with the CIF API
	templateNames << "basic" << "standard" << "fullsize" << "radianceScaling" << "walkthrough" 
		<< "pop" << "nexus";

	return;
}

QString FilterWebExportVMustPlugin::filterScriptFunctionName( FilterIDType filterID )
{
	switch(filterID) 
	{
		case FP_WEB_EXPORT :  return QString("Web Export"); 

		default : assert(0); 
	}
	return QString();
}

// Core 
bool FilterWebExportVMustPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb)
{
	if (ID(filter) == FP_WEB_EXPORT)
	{
		CMeshO &m=md.mm()->cm;

		QNetworkAccessManager NAManager;

		// STEP 1: check if the server works (by requesting the list of application templates)
		// (note that this list is not used)
		QUrl urlTest ("http://pipeline.v-must.net/api/v1/bundles");

		QNetworkRequest request1(urlTest);

		QNetworkReply *reply1 = NAManager.get(request1);
		QTimer timer;
		timer.setSingleShot(true);
		timer.start(5000);

		QEventLoop eventLoop;
		connect(reply1, SIGNAL(finished()), &eventLoop, SLOT(quit()));
		connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
		eventLoop.exec();   // block the http request for 5 seconds

		delete reply1;

		if (timer.isActive())
		{
			timer.stop();

			// STEP 2: preparing the bucket for the processing

			QUrl urlBucket("http://pipeline.v-must.net/api/v1/buckets");
			QNetworkRequest request2(urlBucket);
			request2.setRawHeader("Accept", "application/json");
			request2.setRawHeader("Accept-Encoding", "gzip, deflate, compress");
			request2.setRawHeader("Content-Type", "application/octet-stream");
			request2.setRawHeader("Host", "pipelineserver.ltd");
			request2.setRawHeader("X-Filename", "test.ply");

			QNetworkReply *reply2 = NAManager.get(request2);
			connect(reply2, SIGNAL(finished()), &eventLoop, SLOT(quit()));
			eventLoop.exec();   // block the http request

			// STEP 3: launch the processing according to the selected template
		}
		else
		{
			QMessageBox::warning(0, tr("V-Must CIF API"), tr("Server is time out. Please, re-try later."));
		}



	}

	return true;
}

Q_EXPORT_PLUGIN(FilterWebExportVMustPlugin)


