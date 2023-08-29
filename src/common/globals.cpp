/*****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2005-2021                                           \/)\/    *
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

#include <QDateTime>
#include <QDir>
#include <qapplication.h>

#include "parameters/rich_parameter_list.h"
#include "plugins/plugin_manager.h"
#include "plugins/action_searcher.h"
#include "python/function_set.h"

QString basePath()
{
	QDir baseDir(qApp->applicationDirPath());

#ifdef Q_OS_WIN
	// Windows:
	// during development with visual studio binary could be in the debug/release subdir.
	// once deployed plugins dir is in the application directory, so
	if (baseDir.dirName() == "debug" || baseDir.dirName() == "release")
		baseDir.cdUp();
#endif

#ifdef Q_OS_MAC
	// Mac: during developmentwith xcode  and well deployed the binary is well buried.
	for (int i = 0; i < 6; ++i) {
		if (baseDir.exists("plugins") || baseDir.exists("PlugIns"))
			break;
		baseDir.cdUp();
	}
#endif
	return baseDir.absolutePath();
}

QString meshlab::defaultPluginPath()
{
	QDir pluginsDir(basePath());
#ifdef Q_OS_WIN
	QString d      = pluginsDir.dirName();
	QString dLower = d.toLower();
	if (dLower == "release" || dLower == "relwithdebinfo" || dLower == "debug" ||
		dLower == "minsizerel") {
		// This is a configuration directory for MS Visual Studio.
		pluginsDir.cdUp();
	}
	else {
		d.clear();
	}
#endif
	if (pluginsDir.exists("PlugIns")) {
		pluginsDir.cd("PlugIns");
		return pluginsDir.absolutePath();
	}

	if (pluginsDir.exists("plugins")) {
		pluginsDir.cd("plugins");

#ifdef Q_OS_WIN
		// Re-apply the configuration dir, if any.
		if (!d.isEmpty() && pluginsDir.exists(d)) {
			pluginsDir.cd(d);
		}
#endif

		return pluginsDir.absolutePath();
	}
#ifdef Q_OS_LINUX
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
	qDebug("Meshlab Initialization: Serious error. Unable to find the plugins directory.");
	return {};
}

QString meshlab::defaultShadersPath()
{
	QDir dir(basePath());
#ifdef Q_OS_MAC // TODO: check that this works as expected
	return dir.path() + "/shaders";
#endif
#ifdef Q_OS_LINUX
	dir.cdUp();
	bool res = dir.cd("share/meshlab/shaders");
	if (res) {
		return dir.path();
	}
	else { // reset and return default
		dir.setPath(basePath());
	}
#endif
	return dir.path() + "/shaders";
}

QString meshlab::logDebugFileName()
{
	static QString filename = QDir::homePath() + "/MeshLab" +
							  QString::fromStdString(meshlab::meshlabCompleteVersion()) + " " +
							  QDateTime::currentDateTime().toString() + ".log";
	return filename;
}

RichParameterList& meshlab::defaultGlobalParameterList()
{
	static RichParameterList globalRPS;
	return globalRPS;
}

PluginManager& meshlab::pluginManagerInstance()
{
	static PluginManager pm;
	return pm;
}

ActionSearcher& meshlab::actionSearcherInstance()
{
	static ActionSearcher as;
	return as;
}

std::string meshlab::meshlabCompleteVersion()
{
	std::string ver = meshlabVersion();
#ifdef MESHLAB_IS_NIGHTLY
	QFile f(":/resources/git_sha.txt");
	if (f.open(QFile::ReadOnly | QFile::Text)) {
		QTextStream in(&f);
		ver += "_nightly_" + in.readAll().toStdString();
		f.close();
	}
#endif
	return ver;
}

pymeshlab::FunctionSet& pymeshlab::functionSetInstance()
{
	static FunctionSet fs(meshlab::pluginManagerInstance());
	return fs;
}
