/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "interfaces.h"

/**
  \brief This class provides the basic tools for managing all the plugins. It is used by both meshlab and meshlab server.
  */
class PluginManager
{
public:
  PluginManager();
  enum TypeIO{IMPORT,EXPORT};
  void loadPlugins(RichParameterSet& defaultGlobal);

	inline QVector<MeshIOInterface*>& meshIOPlugins()  {return meshIOPlug;}
	inline QVector<MeshFilterInterface*>& meshFilterPlugins()  {return meshFilterPlug;}
	inline QVector<MeshRenderInterface*>& meshRenderPlugins()  {return meshRenderPlug;}
	inline QVector<MeshDecorateInterface*>& meshDecoratePlugins()  {return meshDecoratePlug;}
	inline QVector<MeshEditInterfaceFactory*>& meshEditFactoryPlugins()  {return meshEditInterfacePlug;}
	void LoadFormats(QStringList &filters, QHash<QString, MeshIOInterface*> &allKnownFormats, int type);
	static QString getPluginDirPath();
  static QString getBaseDirPath();

	QDir pluginsDir;
  QMap<QString, QAction*> actionFilterMap;
  QMap<QString, MeshFilterInterface*> stringFilterMap;
	QVector<MeshIOInterface*> meshIOPlug;
	QVector<MeshFilterInterface*> meshFilterPlug;
	QVector<MeshRenderInterface*> meshRenderPlug;
	QVector<MeshDecorateInterface*> meshDecoratePlug;
	QVector<MeshEditInterfaceFactory*> meshEditInterfacePlug;
	QVector<QAction *> editActionList;
	QVector<QAction *> decoratorActionList;

  QStringList pluginsLoaded;
};

#endif // PLUGINMANAGER_H
