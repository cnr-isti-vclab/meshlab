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

#include "interfaces/filter_plugin_interface.h"
#include "interfaces/iomesh_plugin_interface.h"
#include "interfaces/ioraster_plugin_interface.h"
#include "interfaces/render_plugin_interface.h"
#include "interfaces/decorate_plugin_interface.h"
#include "interfaces/edit_plugin_interface.h"

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
	class FilterPluginRangeIterator;
	class IOMeshPluginIterator;
	class IORasterPluginIterator;
	class RenderPluginRangeIterator;
	class DecoratePluginRangeIterator;
	class EditPluginFactoryRangeIterator;

	/** Member functions **/
	void loadPlugins();
	void loadPlugins(const QDir& pluginsDirectory);
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
	const QStringList& inputMeshFormatListDialog() const;
	const QStringList& outputMeshFormatListDialog() const;
	const QStringList& inputRasterFormatListDialog() const;

	/** Member functions for range iterators **/
	PluginRangeIterator namePluginPairIterator() const;
	FilterPluginRangeIterator filterPluginIterator() const;
	IOMeshPluginIterator ioMeshPluginIterator() const;
	IORasterPluginIterator ioRasterPluginIterator() const;
	RenderPluginRangeIterator renderPluginIterator() const;
	DecoratePluginRangeIterator decoratePluginIterator() const;
	EditPluginFactoryRangeIterator editPluginFactoryIterator() const;

private:
	QDir pluginsDir;

	//all plugins (except Edit plugins)
	std::vector<PluginFileInterface*> allPlugins;

	//IOMeshPlugins
	std::vector<IOMeshPluginInterface*> ioMeshPlugins;
	QMap<QString,IOMeshPluginInterface*> inputMeshFormatToPluginMap;
	QMap<QString,IOMeshPluginInterface*> outputMeshFormatToPluginMap;
	QStringList inputMeshFormatsDialogStringList; //todo: remove this
	QStringList outputMeshFormatsDialogStringList; //todo: remove this

	//IORasterPlugins
	std::vector<IORasterPluginInterface*> ioRasterPlugins;
	QMap<QString, IORasterPluginInterface*> inputRasterFormatToPluginMap;
	QStringList inputRasterFormatsDialogStringList;

	//Filter Plugins
	std::vector<FilterPluginInterface*> filterPlugins;
	QMap<QString, QAction*> actionFilterMap;

	//Render Plugins
	std::vector<RenderPluginInterface*> renderPlugins;

	//Decorate Plugins
	std::vector<DecoratePluginInterface*> decoratePlugins;

	//Edit Plugins
	std::vector<EditPluginInterfaceFactory*> editPlugins;

	//Private member functions
	bool loadPlugin(const QString& filename);
	
	bool loadFilterPlugin(FilterPluginInterface* iFilter, const QString& fileName);
	bool loadIOMeshPlugin(IOMeshPluginInterface* iIOMesh, const QString& fileName);
	bool loadIORasterPlugin(IORasterPluginInterface* iIORaster, const QString& fileName);
	bool loadDecoratePlugin(DecoratePluginInterface* iDecorate, const QString& fileName);
	bool loadRenderPlugin(RenderPluginInterface* iRender, const QString& fileName);
	bool loadEditPlugin(EditPluginInterfaceFactory* iEditFactory, const QString& fileName);
	
	void fillKnownIOFormats();

	static QString addPluginRasterFormats(
			QMap<QString, IORasterPluginInterface*>& map, 
			QStringList& formatFilters, 
			IORasterPluginInterface* pRasterIOPlugin,
			const QList<FileFormat>& format);

	static QString addPluginMeshFormats(
			QMap<QString, IOMeshPluginInterface*>& map, 
			QStringList& formatFilters, 
			IOMeshPluginInterface* pMeshIOPlugin,
			const QList<FileFormat>& format);
};

#include "plugin_manager_iterators.h"

#endif // MESHLAB_PLUGIN_MANAGER_H
