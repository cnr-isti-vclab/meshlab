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
	void loadPlugins(RichParameterList& defaultGlobal);
	void loadPlugins(RichParameterList& defaultGlobal, const QDir& pluginsDirectory);
	QString pluginsCode() const;
	
	int numberIOPlugins() const;
	unsigned int size() const;
	inline QVector<FilterPluginInterface*>& meshFilterPlugins()  {return meshFilterPlug;}
	inline QVector<RenderPluginInterface*>& meshRenderPlugins()  {return meshRenderPlug;}
	inline QVector<DecoratePluginInterface*>& meshDecoratePlugins()  {return meshDecoratePlug;}
	inline QVector<EditPluginInterfaceFactory*>& meshEditFactoryPlugins()  {return meshEditInterfacePlug;}
	
	static QString getDefaultPluginDirPath();
	static QString getBaseDirPath();
	
	QMap<QString,RichParameterList> generateFilterParameterMap();
	
	DecoratePluginInterface* getDecoratorInterfaceByName(const QString& name);
	
	class PluginRangeIterator 
	{
		friend class PluginManager;
	public:
		QVector<PluginInterface*>::iterator begin() {return pm->ownerPlug.begin();}
		QVector<PluginInterface*>::iterator end() {return pm->ownerPlug.end();}
	private:
		PluginRangeIterator(PluginManager* pm) : pm(pm) {}
		PluginManager* pm;
	};
	
	PluginRangeIterator pluginIterator();
	
	static QString osIndependentPluginName(const QString& plname);
	
	QMap<QString, QAction*> actionFilterMap;
	QMap<QString, FilterPluginInterface*> stringFilterMap;
	QMap<QString,IOMeshPluginInterface*> allKnowInputMeshFormats;
	QMap<QString,IOMeshPluginInterface*> allKnowOutputFormats;
	QMap<QString, IORasterPluginInterface*> allKnownInputRasterFormats;
	QStringList inpMeshFilters;
	QStringList inpRasterFilters;
	QStringList outFilters;
	
	
	QVector<FilterPluginInterface*> meshFilterPlug;
	QVector<RenderPluginInterface*> meshRenderPlug;
	QVector<DecoratePluginInterface*> meshDecoratePlug;
	QVector<EditPluginInterfaceFactory*> meshEditInterfacePlug;
	QVector<QAction *> editActionList;
	QVector<QAction *> decoratorActionList;
	// Used for unique destruction - this "owns" all IO, Filter, Render, and Decorate plugins
	
	QStringList pluginsLoaded;
	
	
	
private:
	QVector<PluginInterface *> ownerPlug;
	QVector<IOMeshPluginInterface*> ioMeshPlugins;
	QVector<IORasterPluginInterface*> ioRasterPlugins;
	QDir pluginsDir;
	
	
	void fillKnownIOFormats();
	
	QString addPluginRasterFormats(
			QMap<QString, IORasterPluginInterface*>& map, 
			QStringList& formatFilters, 
			IORasterPluginInterface* pRasterIOPlugin,
			const QList<FileFormat>& format);
	
	QString addPluginMeshFormats(
			QMap<QString, IOMeshPluginInterface*>& map, 
			QStringList& formatFilters, 
			IOMeshPluginInterface* pMeshIOPlugin,
			const QList<FileFormat>& format);
};

#endif // MESHLAB_PLUGIN_MANAGER_H
