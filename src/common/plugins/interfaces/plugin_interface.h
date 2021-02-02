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

#ifndef MESHLAB_PLUGIN_INTERFACE_H
#define MESHLAB_PLUGIN_INTERFACE_H

#include <QAction>


#include "../../GLLogStream.h"
#include "../../parameters/rich_parameter_list.h"
#include "../../globals.h"

/**
 * \brief The PluginInterface class is the base of all the plugin interfaces.
 *
 * The main idea common to all the framework is that each plugin export a set of actions,
 * internally each action is associated to a FilterIDType, and for each action a name and a formatted INFO is defined.
 *
 * For coding easyness ID are more practical (you can use them in switches).
 * Using action on the other hand is practical because it simplify their management in menus/toolbars and it allows to define icons and other things in a automatic way.
 * Moreover ID are UNSAFE (different plugin can have same id) so they should be used only INTERNALLY
 *
 * \todo There is inconsistency in the usage of ID and actions for retrieving particular filters. Remove.
 */
class PluginInterface
{
public:
	typedef int FilterIDType;

	/** the type used to identify plugin actions; there is a one-to-one relation between an ID and an Action.
	\todo To be renamed as ActionIDType
	*/
	PluginInterface();
	virtual ~PluginInterface() {}

	/** 
	 * This function will be automatically defined in your plugin class
	 * when you use the MESHLAB_PLUGIN_IID_EXPORTER macro.
	 * The only exception is for the Edit plugins (not EditFactory!):
	 * in this case, this function is defined by the macro
	 * MESHLAB_EDIT_PLUGIN
	 **/
	virtual std::pair<std::string, bool> getMLVersion() const  = 0;
	
	/**
	 * @brief This functions returns the name of the current plugin.
	 * Must be implemented in every plugin.
	 * @return
	 */
	virtual QString pluginName() const = 0;

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
	void log(GLLogStream::Levels Level, const char* f, Ts&&... ts);

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
void PluginInterface::log(const char* f, Ts&&... ts)
{
	if(logstream != nullptr) {
		logstream->Logf(GLLogStream::FILTER, f, std::forward<Ts>(ts)...);
	}
}

template<typename... Ts>
void PluginInterface::log(const std::string& s, Ts&&... ts)
{
	if(logstream != nullptr) {
		logstream->Logf(GLLogStream::FILTER, s.c_str(), std::forward<Ts>(ts)...);
	}
}

template <typename... Ts>
void PluginInterface::log(GLLogStream::Levels Level, const char* f, Ts&&... ts)
{
	if(logstream != nullptr) {
		logstream->Logf(Level, f, std::forward<Ts>(ts)...);
	}
}

template <typename... Ts>
void PluginInterface::realTimeLog(QString Id, const QString& meshName, const char* f, Ts&&... ts)
{
	if(logstream != nullptr) {
		logstream->RealTimeLogf(Id, meshName, f, std::forward<Ts>(ts)...);
	}
}

#endif // MESHLAB_PLUGIN_INTERFACE_H
