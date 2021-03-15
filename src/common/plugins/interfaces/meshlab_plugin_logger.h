/****************************************************************************
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

#ifndef MESHLAB_PLUGIN_LOGGER_H
#define MESHLAB_PLUGIN_LOGGER_H

#include <QAction>

#include "meshlab_plugin.h"
#include "../../GLLogStream.h"
#include "../../parameters/rich_parameter_list.h"
#include "../../globals.h"

/**
 * @brief The MeshLabPluginLogger provides some common log functionalities that are
 * used by MeshLab.
 *
 * Each MeshLab abstract plugin class inherits from this class in order to make
 * available log member functions to plugins.
 * The only exception is the EditPlugin class, since it is not the Edit plugin
 * that should be able to log, but the EditTool class (the EditPlugin class is a
 * "container" of EditTools).
 */
class MeshLabPluginLogger
{
public:
	MeshLabPluginLogger();
	virtual ~MeshLabPluginLogger() {}

	/// Standard stuff that usually should not be redefined.
	void setLog(GLLogStream* log);

	// This function must be used to communicate useful information collected in the parsing/saving of the files.
	// NEVER EVER use a msgbox to say something to the user.
	template <typename... Ts>
	void log(const char* f, Ts&&... ts);
	template <typename... Ts>
	void log(const std::string& s, Ts&&... ts);

	void log(const char* s);
	void log(const std::string& s);

	template <typename... Ts>
	void log(GLLogStream::Levels level, const char* f, Ts&&... ts);

	void log(GLLogStream::Levels level, const char* s);

	void log(GLLogStream::Levels  level, const std::string& s);

	void realTimeLog(QString Id, const QString& meshName, const char* f);

	template <typename... Ts>
	void realTimeLog(QString Id, const QString &meshName, const char * f, Ts&&... ts );

private:
	GLLogStream *logstream;
};

/************************
 * Template definitions *
 ************************/

template<typename... Ts>
void MeshLabPluginLogger::log(const char* f, Ts&&... ts)
{
	if(logstream != nullptr) {
		logstream->logf(GLLogStream::FILTER, f, std::forward<Ts>(ts)...);
	}
}

template<typename... Ts>
void MeshLabPluginLogger::log(const std::string& s, Ts&&... ts)
{
	if(logstream != nullptr) {
		logstream->logf(GLLogStream::FILTER, s.c_str(), std::forward<Ts>(ts)...);
	}
}

template <typename... Ts>
void MeshLabPluginLogger::log(GLLogStream::Levels level, const char* f, Ts&&... ts)
{
	if(logstream != nullptr) {
		logstream->logf(level, f, std::forward<Ts>(ts)...);
	}
}

template <typename... Ts>
void MeshLabPluginLogger::realTimeLog(QString id, const QString& meshName, const char* f, Ts&&... ts)
{
	if(logstream != nullptr) {
		logstream->realTimeLogf(id, meshName, f, std::forward<Ts>(ts)...);
	}
}

#endif // MESHLAB_PLUGIN_LOGGER_H
