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

#include "globals.h"

#include <QDir>
#include <qapplication.h>

#include "../parameters/rich_parameter_list.h"

RichParameterList& meshlab::defaultGlobalParameterList()
{
	static RichParameterList globalRPS;
	return globalRPS;
}

QString basePath()
{
	QDir baseDir(qApp->applicationDirPath());
	
#if defined(Q_OS_WIN)
	// Windows:
	// during development with visual studio binary could be in the debug/release subdir.
	// once deployed plugins dir is in the application directory, so
	if (baseDir.dirName() == "debug" || baseDir.dirName() == "release")		baseDir.cdUp(); 
#endif
	
#if defined(Q_OS_MAC)
	// Mac: during developmentwith xcode  and well deployed the binary is well buried.
	for(int i=0;i<6;++i){
		if(baseDir.exists("plugins")) break;
		baseDir.cdUp();
	}
	qDebug("The base dir is %s", qUtf8Printable(baseDir.absolutePath()));
#endif
	return baseDir.absolutePath();
}

QString meshlab::defaultPluginPath()
{
	QDir pluginsDir(basePath());
#if defined(Q_OS_WIN)
	QString d = pluginsDir.dirName();
	QString dLower = d.toLower();
	if (dLower == "release" || dLower == "relwithdebinfo" || dLower == "debug" ||
			dLower == "minsizerel") {
		// This is a configuration directory for MS Visual Studio.
		pluginsDir.cdUp();
	} else {
		d.clear();
	}
#endif
	if (pluginsDir.exists("plugins")) {
		pluginsDir.cd("plugins");
		
#if defined(Q_OS_WIN)
		// Re-apply the configuration dir, if any.
		if (!d.isEmpty() && pluginsDir.exists(d)) {
			pluginsDir.cd(d);
		}
#endif
		
		return pluginsDir.absolutePath();
	}
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)
	else if (pluginsDir.dirName() == "bin") {
		pluginsDir.cdUp();
		pluginsDir.cd("lib");
		pluginsDir.cd("meshlab");
		if (pluginsDir.exists("plugins")) {
			pluginsDir.cd("plugins");
			return pluginsDir.absolutePath();
		}
	}
#endif
	//QMessageBox::warning(0,"Meshlab Initialization","Serious error. Unable to find the plugins directory.");
	qDebug("Meshlab Initialization: Serious error. Unable to find the plugins directory.");
	return {};
}

QString meshlab::defaultShadersPath()
{
	return basePath() + "/shaders";
}
