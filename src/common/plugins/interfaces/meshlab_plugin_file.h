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

/**
 * @brief The MeshLabPluginFile class is the base of all MeshLab plugin classes,
 * and represents the library file of the plugin.
 * Each MeshLab plugin can then be a classic plugin (and inherit from the MeshLabPlugin class)
 * or a collection of Edit plugins (and inherit from the EditPluginInterfaceFactory class).
 * Each Plugin (classic or edit) inherits from this class. 
 */
class MeshLabPluginFile
{
public:
	friend class PluginManager;
	
	MeshLabPluginFile() : enabled(true) {};
	virtual ~MeshLabPluginFile() {}
	
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
