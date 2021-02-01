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

	class NamePluginPairRangeIterator
	{
		friend class PluginManager;
	public:
		std::map<QString, PluginInterface*>::const_iterator begin() {return pm->allPlugins.begin();}
		std::map<QString, PluginInterface*>::const_iterator end() {return pm->allPlugins.end();}
	private:
		NamePluginPairRangeIterator(const PluginManager* pm) : pm(pm) {}
		const PluginManager* pm;
	};

	class FilterPluginRangeIterator
	{
		friend class PluginManager;
	public:
		QVector<FilterPluginInterface*>::const_iterator begin() {return pm->filterPlugins.begin();}
		QVector<FilterPluginInterface*>::const_iterator end() {return pm->filterPlugins.end();}
	private:
		FilterPluginRangeIterator(const PluginManager* pm) : pm(pm) {}
		const PluginManager* pm;
	};

	class IOMeshPluginIterator
	{
		friend class PluginManager;
	public:
		QVector<IOMeshPluginInterface*>::const_iterator begin() {return pm->ioMeshPlugins.begin();}
		QVector<IOMeshPluginInterface*>::const_iterator end() {return pm->ioMeshPlugins.end();}
	private:
		IOMeshPluginIterator(const PluginManager* pm) : pm(pm) {}
		const PluginManager* pm;
	};

	class IORasterPluginIterator
	{
		friend class PluginManager;
	public:
		QVector<IORasterPluginInterface*>::const_iterator begin() {return pm->ioRasterPlugins.begin();}
		QVector<IORasterPluginInterface*>::const_iterator end() {return pm->ioRasterPlugins.end();}
	private:
		IORasterPluginIterator(const PluginManager* pm) : pm(pm) {}
		const PluginManager* pm;
	};

	class RenderPluginRangeIterator
	{
		friend class PluginManager;
	public:
		QVector<RenderPluginInterface*>::const_iterator begin() {return pm->renderPlugins.begin();}
		QVector<RenderPluginInterface*>::const_iterator end() {return pm->renderPlugins.end();}
	private:
		RenderPluginRangeIterator(const PluginManager* pm) : pm(pm) {}
		const PluginManager* pm;
	};

	class DecoratePluginRangeIterator
	{
		friend class PluginManager;
	public:
		QVector<DecoratePluginInterface*>::const_iterator begin() {return pm->decoratePlugins.begin();}
		QVector<DecoratePluginInterface*>::const_iterator end() {return pm->decoratePlugins.end();}
	private:
		DecoratePluginRangeIterator(const PluginManager* pm) : pm(pm) {}
		const PluginManager* pm;
	};

	class EditPluginFactoryRangeIterator
	{
		friend class PluginManager;
	public:
		QVector<EditPluginInterfaceFactory*>::const_iterator begin() {return pm->editPlugins.begin();}
		QVector<EditPluginInterfaceFactory*>::const_iterator end() {return pm->editPlugins.end();}
	private:
		EditPluginFactoryRangeIterator(const PluginManager* pm) : pm(pm) {}
		const PluginManager* pm;
	};

	NamePluginPairRangeIterator namePluginPairIterator() const;
	FilterPluginRangeIterator filterPluginIterator() const;
	IOMeshPluginIterator ioMeshPluginIterator() const;
	IORasterPluginIterator ioRasterPluginIterator() const;
	RenderPluginRangeIterator renderPluginIterator() const;
	DecoratePluginRangeIterator decoratePluginIterator() const;
	EditPluginFactoryRangeIterator editPluginFactoryIterator() const;

private:
	QDir pluginsDir;

	//all plugins
	std::map<QString, PluginInterface*> allPlugins;

	//IOMeshPlugins
	QVector<IOMeshPluginInterface*> ioMeshPlugins;
	QMap<QString,IOMeshPluginInterface*> inputMeshFormatToPluginMap;
	QMap<QString,IOMeshPluginInterface*> outputMeshFormatToPluginMap;
	QStringList inputMeshFormatsDialogStringList; //todo: remove this
	QStringList outputMeshFormatsDialogStringList; //todo: remove this

	//IORasterPlugins
	QVector<IORasterPluginInterface*> ioRasterPlugins;
	QMap<QString, IORasterPluginInterface*> inputRasterFormatToPluginMap;
	QStringList inputRasterFormatsDialogStringList;

	//Filter Plugins
	QVector<FilterPluginInterface*> filterPlugins;
	QMap<QString, QAction*> actionFilterMap;

	//Render Plugins
	QVector<RenderPluginInterface*> renderPlugins;

	//Decorate Plugins
	QVector<DecoratePluginInterface*> decoratePlugins;

	//Edit Plugins
	QVector<EditPluginInterfaceFactory*> editPlugins;

	//Private member functions
	void loadPlugin(QString& filename);
	
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

#endif // MESHLAB_PLUGIN_MANAGER_H
