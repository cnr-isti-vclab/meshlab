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

#ifndef MESHLAB_PLUGIN_FILE_H
#define MESHLAB_PLUGIN_FILE_H

#include <string>
#include <QFileInfo>
#include <QString>
#include "../../mlexception.h"

/**
 * @brief The MeshLabPlugin class is the base of all MeshLab plugin classes,
 * and represents the library file of the plugin.
 *
 * The main idea common to all the framework is that each plugin export a set
 * of actions, internally each action is associated to an ActionIDType, and for
 * each action a name and a formatted INFO is defined.
 *
 * For coding easyness ID are more practical (you can use them in switches).
 * Using action on the other hand is practical because it simplify their
 * management in menus/toolbars and it allows to define icons and other things
 * in a automatic way.
 * Moreover ID are UNSAFE (different plugin can have same id) so they should be
 * used only INTERNALLY.
 */
class MeshLabPluginFile
{
public:
	friend class PluginManager;
	
	MeshLabPluginFile() : enabled(true) {};
	virtual ~MeshLabPluginFile() {}
	
	/**
	 * @brief the type used to identify plugin actions;
	 * there is a one-to-one relation between an ID and an Action.
	 */
	typedef int ActionIDType;

	/** 
	 * This function will be automatically defined in your plugin class
	 * when you use the MESHLAB_PLUGIN_IID_EXPORTER macro.
	 * The only exception is for the Edit plugins (not EditFactory!):
	 * in this case, this function is defined by the macro
	 * MESHLAB_EDIT_PLUGIN
	 **/
	virtual std::pair<std::string, bool> getMLVersion() const  = 0;
	
	/**
	 * @brief This function returns the name of the current plugin.
	 * Must be implemented in every plugin.
	 * @return
	 */
	virtual QString pluginName() const = 0;
	
	/**
	 * @brief This function returns the vendor (developer or organization)
	 * of the plugin. 
	 * @return 
	 */
	virtual QString vendor() const {return "CNR-ISTI VCLab";};
	
	bool isEnabled() const {return enabled;}

	QFileInfo pluginFileInfo() const {return plugFileInfo;}

private:
	void enable() {enabled = true;}
	void disable() {enabled = false;}
	bool enabled;
	QFileInfo plugFileInfo;
};

#define MESHLAB_PLUGIN_IID_EXPORTER(x) \
	Q_PLUGIN_METADATA(IID x) \
	public: \
		virtual std::pair<std::string, bool> getMLVersion() const { \
			return std::make_pair(meshlab::meshlabVersion(), meshlab::builtWithDoublePrecision()); \
		} \
	private: 
#define MESHLAB_PLUGIN_NAME_EXPORTER(x)

#endif // MESHLAB_PLUGIN_FILE_H
