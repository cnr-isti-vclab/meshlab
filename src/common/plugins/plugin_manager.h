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

#ifndef MESHLAB_PLUGIN_MANAGER_H
#define MESHLAB_PLUGIN_MANAGER_H

#include "interfaces/edit_plugin_interface.h"

#include "containers/decorate_plugin_container.h"
#include "containers/filter_plugin_container.h"
#include "containers/iomesh_plugin_container.h"
#include "containers/ioraster_plugin_container.h"
#include "containers/render_plugin_container.h"

#include<QMap>
#include<QObject>
#include <QDir>

/**
\brief This class provides the basic tools for managing all the plugins. It is used by both meshlab and meshlab server.
*/
class PluginManager
{
public:
	PluginManager();
	~PluginManager();

	/** Iterators (definitions can be found in plugin_manager_iterators.h) **/ 
	class PluginRangeIterator;
	class RenderPluginRangeIterator;
	class EditPluginFactoryRangeIterator;

	/** Member functions **/
	static void checkPlugin(const QString& filename);

	void loadPlugins();
	void loadPlugins(QDir pluginsDirectory);
	void loadPlugin(const QString& filename);
	void unloadPlugin(PluginFileInterface* ifp);

	void enablePlugin(PluginFileInterface* ifp);
	void disablePlugin(PluginFileInterface* ifp);

	QString pluginsCode() const;

	unsigned int size() const;
	int numberIOPlugins() const;

	DecoratePluginInterface* getDecoratePlugin(const QString& name);

	QAction* filterAction(const QString& name);

	IOMeshPluginInterface* inputMeshPlugin(const QString& inputFormat) const;
	IOMeshPluginInterface* outputMeshPlugin(const QString& outputFormat) const;
	IORasterPluginInterface* inputRasterPlugin(const QString inputFormat) const;
	bool isInputMeshFormatSupported(const QString inputFormat) const;
	bool isOutputMeshFormatSupported(const QString outputFormat) const;
	bool isInputRasterFormatSupported(const QString inputFormat) const;
	QStringList inputMeshFormatList() const;
	QStringList outputMeshFormatList() const;
	QStringList inputRasterFormatList() const;
	QStringList inputMeshFormatListDialog() const;
	QStringList outputMeshFormatListDialog() const;
	QStringList inputRasterFormatListDialog() const;
	
	PluginFileInterface* operator [](unsigned int i) const;

	/** Member functions for range iterators **/
	PluginRangeIterator pluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	FilterPluginContainer::FilterPluginRangeIterator filterPluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	IOMeshPluginContainer::IOMeshPluginRangeIterator ioMeshPluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	IORasterPluginContainer::IORasterPluginRangeIterator ioRasterPluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	RenderPluginContainer::RenderPluginRangeIterator renderPluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	DecoratePluginContainer::DecoratePluginRangeIterator decoratePluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	EditPluginFactoryRangeIterator editPluginFactoryIterator(bool iterateAlsoDisabledPlugins = false) const;

private:
	//all plugins
	std::vector<PluginFileInterface*> allPlugins;
	std::set<QString> pluginFiles; //used to check if a plugin file has been already loaded

	//IOMeshPlugins
	IOMeshPluginContainer ioMeshPlugins;

	//IORasterPlugins
	IORasterPluginContainer ioRasterPlugins;

	//Filter Plugins
	FilterPluginContainer filterPlugins;

	//Render Plugins
	RenderPluginContainer renderPlugins;

	//Decorate Plugins
	DecoratePluginContainer decoratePlugins;

	//Edit Plugins
	std::vector<EditPluginInterfaceFactory*> editPlugins;

	static void checkFilterPlugin(FilterPluginInterface* iFilter);

	void loadEditPlugin(EditPluginInterfaceFactory* iEditFactory);

	void unloadEditPlugin(EditPluginInterfaceFactory* iEditFactory);

	template <typename RangeIterator>
	static QStringList inputFormatListDialog(RangeIterator iterator);

	template <typename RangeIterator>
	static QStringList outputFormatListDialog(RangeIterator iterator);
};

class PluginManager::PluginRangeIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator<PluginFileInterface> begin();
	ConstPluginIterator<PluginFileInterface> end();
private:
	PluginRangeIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false);
	const PluginManager* pm;
	bool b;
};

#include "plugin_manager_iterators.h"

#endif // MESHLAB_PLUGIN_MANAGER_H
