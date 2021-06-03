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

#include "containers/decorate_plugin_container.h"
#include "containers/edit_plugin_container.h"
#include "containers/filter_plugin_container.h"
#include "containers/io_plugin_container.h"
#include "containers/render_plugin_container.h"
#include "meshlab_plugin_type.h"

#include <QPluginLoader>
#include <QObject>

/**
 * @brief The PluginManager class provides the basic tools for managing all the plugins.
 */
class PluginManager
{
public:
	PluginManager();
	~PluginManager();

	class PluginRangeIterator;

	/** Member functions **/
	static MeshLabPluginType checkPlugin(const QString& filename);

	void loadPlugins();
	void loadPlugins(QDir pluginsDirectory);
	MeshLabPlugin* loadPlugin(const QString& filename);
	void unloadPlugin(MeshLabPlugin* ifp);

	void enablePlugin(MeshLabPlugin* ifp);
	void disablePlugin(MeshLabPlugin* ifp);

	unsigned int size() const;
	int numberIOPlugins() const;

	DecoratePlugin* getDecoratePlugin(const QString& name);

	QAction* filterAction(const QString& name);

	IOPlugin* inputMeshPlugin(const QString& inputFormat) const;
	IOPlugin* outputMeshPlugin(const QString& outputFormat) const;
	IOPlugin* inputImagePlugin(const QString inputFormat) const;
	IOPlugin* outputImagePlugin(const QString& outputFormat) const;
	IOPlugin* inputProjectPlugin(const QString& inputFormat) const;
	IOPlugin* outputProjectPlugin(const QString& outputFormat) const;
	bool isInputMeshFormatSupported(const QString inputFormat) const;
	bool isOutputMeshFormatSupported(const QString outputFormat) const;
	bool isInputImageFormatSupported(const QString inputFormat) const;
	bool isOutputImageFormatSupported(const QString outputFormat) const;
	bool isInputProjectFormatSupported(const QString inputFormat) const;
	bool isOutputProjectFormatSupported(const QString outputFormat) const;
	QStringList inputMeshFormatList() const;
	QStringList outputMeshFormatList() const;
	QStringList inputImageFormatList() const;
	QStringList outputImageFormatList() const;
	QStringList inputProjectFormatList() const;
	QStringList outputProjectFormatList() const;
	QStringList inputMeshFormatListDialog() const;
	QStringList outputMeshFormatListDialog() const;
	QStringList inputImageFormatListDialog() const;
	QStringList inputProjectFormatListDialog() const;
	QStringList outputProjectFormatListDialog() const;
	
	MeshLabPlugin* operator [](unsigned int i) const;

	/** Member functions for range iterators **/
	PluginRangeIterator pluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	FilterPluginContainer::FilterPluginRangeIterator filterPluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	IOPluginContainer::IOPluginRangeIterator ioPluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	RenderPluginContainer::RenderPluginRangeIterator renderPluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	DecoratePluginContainer::DecoratePluginRangeIterator decoratePluginIterator(bool iterateAlsoDisabledPlugins = false) const;
	EditPluginContainer::EditPluginFactoryRangeIterator editPluginFactoryIterator(bool iterateAlsoDisabledPlugins = false) const;

private:
	//all plugins
	std::vector<MeshLabPlugin*> allPlugins;
	std::vector<QPluginLoader*> allPluginLoaders;
	std::set<QString> pluginFiles; //used to check if a plugin file has been already loaded

	//Plugin containers: used for better organization of each type of plugin
	// note: these containers do not own any plugin. Plugins are owned by the PluginManager
	IOPluginContainer ioPlugins;
	FilterPluginContainer filterPlugins;
	RenderPluginContainer renderPlugins;
	DecoratePluginContainer decoratePlugins;
	EditPluginContainer editPlugins;

	static void checkFilterPlugin(FilterPlugin* iFilter);

	template <typename RangeIterator>
	static QStringList inputFormatListDialog(RangeIterator iterator);

	template <typename RangeIterator>
	static QStringList outputFormatListDialog(RangeIterator iterator);

	template <typename RangeIterator>
	static QStringList inputImageFormatListDialog(RangeIterator iterator);

	template <typename RangeIterator>
	static QStringList inputProjectFormatListDialog(RangeIterator iterator);

	template <typename RangeIterator>
	static QStringList outputProjectFormatListDialog(RangeIterator iterator);
};

class PluginManager::PluginRangeIterator
{
	friend class PluginManager;
public:
	ConstPluginIterator<MeshLabPlugin> begin();
	ConstPluginIterator<MeshLabPlugin> end();
private:
	PluginRangeIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins = false);
	const PluginManager* pm;
	bool b;
};

#endif // MESHLAB_PLUGIN_MANAGER_H
