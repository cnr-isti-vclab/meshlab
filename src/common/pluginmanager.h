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

#include<QMap>
#include<QObject>

#include "interfaces.h"
#include "xmlfilterinfo.h"
#include "scriptinterface.h"
#include "scriptsyntax.h"

class QScriptEngine;
/**
\brief This class provides the basic tools for managing all the plugins. It is used by both meshlab and meshlab server.
*/
class PluginManager
{
private:
	MeshDocumentSI* currentDocInterface;
public:
	PluginManager();
	~PluginManager();
	enum TypeIO{IMPORT,EXPORT};
	void loadPlugins(RichParameterSet& defaultGlobal);
	QString pluginsCode() const;
	void loadXMLPlugin(const QString& filename);
	MLXMLPluginInfo* getXMLPluginInfo( const QString& plugname );
	void deleteXMLPlugin(const QString& plugscriptname);

	inline QVector<MeshIOInterface*>& meshIOPlugins()  {return meshIOPlug;}
	inline QVector<MeshFilterInterface*>& meshFilterPlugins()  {return meshFilterPlug;}
	inline QVector<MeshRenderInterface*>& meshRenderPlugins()  {return meshRenderPlug;}
	inline QVector<MeshDecorateInterface*>& meshDecoratePlugins()  {return meshDecoratePlug;}
	inline QVector<MeshEditInterfaceFactory*>& meshEditFactoryPlugins()  {return meshEditInterfacePlug;}
	inline QVector<MeshLabFilterInterface*>& meshlabXMLfilterPlugins()  {return meshlabXMLFilterPlug;}
	void knownIOFormats();
	static QString getPluginDirPath();
	static QString getBaseDirPath();

	QMap<QString,RichParameterSet> generateFilterParameterMap();

	QDir pluginsDir;
	QMap<QString, QAction*> actionFilterMap;
	QMap<QString, MeshFilterInterface*> stringFilterMap;

	//QMap<QString, QAction*> xmlActionFilterMap;
	QMap<QString,	MeshLabXMLFilterContainer> stringXMLFilterMap;
	QMap<QString,MeshIOInterface*> allKnowInputFormats;
	QMap<QString,MeshIOInterface*> allKnowOutputFormats;
	QStringList inpFilters;
	QStringList outFilters;
	//QMap<QString, XMLFilterInfo*> filterXMLFileMap;


	QVector<MeshIOInterface*> meshIOPlug;
	QVector<MeshFilterInterface*> meshFilterPlug;
	QVector<MeshRenderInterface*> meshRenderPlug;
	QVector<MeshDecorateInterface*> meshDecoratePlug;
	QVector<MeshEditInterfaceFactory*> meshEditInterfacePlug;
	//only useful for an easy destruction of the MLXMLPluginInfos
	QVector<MLXMLPluginInfo*> xmlpluginfo;
	QVector<MeshLabFilterInterface*> meshlabXMLFilterPlug;
	QVector<QAction *> editActionList;
	QVector<QAction *> decoratorActionList;

	QStringList pluginsLoaded;

	static QString pluginNameSpace();
	//highlight and autocomplete
	/*QStringList pluginnamespaces;
	QStringList filterscriptnames;*/
	QList<LibraryElementInfo> libinfolist;
	//Env env;

	//void updateDocumentScriptBindings(MeshDocument& doc);
private:
	/*void loadPluginsCode();*/
	QString scriptplugcode;

};

#endif // PLUGINMANAGER_H
