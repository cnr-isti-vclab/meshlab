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

#include "plugin_manager_iterators.h" //includes plugin_manager.h
#include "meshlab_plugin_type.h"
#include <QObject>
#include <qapplication.h>
#include <QPluginLoader>
#include <QDebug>
#include <vcg/complex/algorithms/create/platonic.h>

#include "../mlexception.h"
#include "../globals.h"

static QStringList fileNamePluginDLLs() {
	QStringList l;
#if defined(Q_OS_WIN)
	l.push_back("*.dll");
	return l;
#elif defined(Q_OS_MAC)
	l.push_back("*.dylib");
	l.push_back("*.so");
	return l;
#else //linux
	l.push_back("*.so");
	return l;
#endif
}

PluginManager::PluginManager()
{
}

PluginManager::~PluginManager()
{
	ioMeshPlugins.clear();
	filterPlugins.clear();
	renderPlugins.clear();
	decoratePlugins.clear();
	editPlugins.clear();
	for (auto& plugin : allPlugins)
		delete plugin.second;
	allPlugins.clear();
}

void PluginManager::loadPlugins()
{
	loadPlugins(QDir(meshlab::defaultPluginPath()));
}

void PluginManager::loadPlugins(const QDir& pluginsDirectory)
{
	pluginsDir = pluginsDirectory;
	// without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
	// ToDo: get rid of any qApp here
	qApp->addLibraryPath(meshlab::defaultPluginPath());
	QStringList nameFiltersPlugins = fileNamePluginDLLs();
	
	//only the file with extension pluginfilters will be listed by function entryList()
	pluginsDir.setNameFilters(nameFiltersPlugins);
	qDebug("Current Plugins Dir is: %s ", qUtf8Printable(pluginsDir.absolutePath()));
	for(QString fileName : pluginsDir.entryList(QDir::Files)) {
		loadPlugin(fileName);
	}
	fillKnownIOFormats();
}

unsigned int PluginManager::size() const
{
	return allPlugins.size();
}

int PluginManager::numberIOPlugins() const
{
	return ioMeshPlugins.size();
}

// Search among all the decorator plugins the one that contains a decoration with the given name
DecoratePluginInterface *PluginManager::getDecoratePlugin(const QString& name)
{
	for(DecoratePluginInterface *tt : decoratePlugins) {
		for( QAction *ac: tt->actions())
			if( name == tt->decorationName(ac) ) return tt;
	}
	assert(0);
	return 0;
}

QAction* PluginManager::filterAction(const QString& name)
{
	auto it = actionFilterMap.find(name);
	if (it != actionFilterMap.end())
		return it.value();
	else
		return nullptr;
}

IOMeshPluginInterface* PluginManager::inputMeshPlugin(const QString& inputFormat) const
{
	auto it = inputMeshFormatToPluginMap.find(inputFormat.toLower());
	if (it != inputMeshFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IOMeshPluginInterface* PluginManager::outputMeshPlugin(const QString& outputFormat) const
{
	auto it = outputMeshFormatToPluginMap.find(outputFormat.toLower());
	if (it != outputMeshFormatToPluginMap.end())
		return *it;
	return nullptr;
}

IORasterPluginInterface* PluginManager::inputRasterPlugin(const QString inputFormat) const
{
	auto it = inputRasterFormatToPluginMap.find(inputFormat.toLower());
	if (it != inputRasterFormatToPluginMap.end())
		return *it;
	return nullptr;
}

bool PluginManager::isInputMeshFormatSupported(const QString inputFormat) const
{
	return inputMeshFormatToPluginMap.find(inputFormat.toLower()) != inputMeshFormatToPluginMap.end();
}

bool PluginManager::isOutputMeshFormatSupported(const QString outputFormat) const
{
	return outputMeshFormatToPluginMap.find(outputFormat.toLower()) != outputMeshFormatToPluginMap.end();
}

bool PluginManager::isInputRasterFormatSupported(const QString inputFormat) const
{
	return inputRasterFormatToPluginMap.find(inputFormat.toLower()) != inputRasterFormatToPluginMap.end();
}

QStringList PluginManager::inputMeshFormatList() const
{
	return inputMeshFormatToPluginMap.keys();
}

QStringList PluginManager::outputMeshFormatList() const
{
	return outputMeshFormatToPluginMap.keys();
}

QStringList PluginManager::inputRasterFormatList() const
{
	return inputRasterFormatToPluginMap.keys();
}

const QStringList& PluginManager::inputMeshFormatListDialog() const
{
	return inputMeshFormatsDialogStringList;
}

const QStringList& PluginManager::outputMeshFormatListDialog() const
{
	return outputMeshFormatsDialogStringList;
}

const QStringList& PluginManager::inputRasterFormatListDialog() const
{
	return inputRasterFormatsDialogStringList;
}

PluginManager::NamePluginPairRangeIterator PluginManager::namePluginPairIterator() const
{
	return NamePluginPairRangeIterator(this);
}

PluginManager::FilterPluginRangeIterator PluginManager::filterPluginIterator() const
{
	return FilterPluginRangeIterator(this);
}

PluginManager::IOMeshPluginIterator PluginManager::ioMeshPluginIterator() const
{
	return IOMeshPluginIterator(this);
}

PluginManager::IORasterPluginIterator PluginManager::ioRasterPluginIterator() const
{
	return IORasterPluginIterator(this);
}

PluginManager::RenderPluginRangeIterator PluginManager::renderPluginIterator() const
{
	return RenderPluginRangeIterator(this);
}

PluginManager::DecoratePluginRangeIterator PluginManager::decoratePluginIterator() const
{
	return DecoratePluginRangeIterator(this);
}

PluginManager::EditPluginFactoryRangeIterator PluginManager::editPluginFactoryIterator() const
{
	return EditPluginFactoryRangeIterator(this);
}

bool PluginManager::loadPlugin(const QString& fileName)
{
	QString absfilepath = pluginsDir.absoluteFilePath(fileName);
	QFileInfo fin(absfilepath);
	QPluginLoader loader(absfilepath);
	QObject *plugin = loader.instance();
	if (!plugin) {
		qDebug() << loader.errorString();
		return false;
	}
	
	PluginFileInterface* ifp = dynamic_cast<PluginFileInterface *>(plugin);
	if (!ifp){
		qDebug() << fin.fileName() << " is not a MeshLab plugin.";
		return false;
	}
	if (ifp && ifp->getMLVersion().second != MeshLabScalarTest<Scalarm>::doublePrecision()) {
		qDebug() << fin.fileName() << " has different floating point precision from the running MeshLab version.";
		return false;
	}
	
	//TODO: check in some way also the meshlab version of the plugin
	
	MeshLabPluginType type(ifp);
	
	bool loadOk = false;
	if (type.isDecoratePlugin()){
		loadOk = loadDecoratePlugin(qobject_cast<DecoratePluginInterface *>(plugin), fileName);
	}
	if (type.isEditPlugin()){
		loadOk = loadEditPlugin(qobject_cast<EditPluginInterfaceFactory *>(plugin), fileName);
	}
	if (type.isFilterPlugin()){
		loadOk = loadFilterPlugin(qobject_cast<FilterPluginInterface *>(plugin), fileName);
	}
	if (type.isIOMeshPlugin()){
		loadOk = loadIOMeshPlugin(qobject_cast<IOMeshPluginInterface *>(plugin), fileName);
	}
	if (type.isIORasterPlugin()){
		loadOk = loadIORasterPlugin(qobject_cast<IORasterPluginInterface*>(plugin), fileName);
	}
	if (type.isRenderPlugin()){
		loadOk = loadRenderPlugin(qobject_cast<RenderPluginInterface *>(plugin), fileName);
	}

	if (loadOk){ //If the plugin has been loaded correctly
		if (allPlugins.find(ifp->pluginName()) != allPlugins.end()) {
			qDebug() << "Warning: " << ifp->pluginName() << " has been already loaded.";
		}
		ifp->plugFileInfo = fin;
		allPlugins[ifp->pluginName()] = ifp;
	}

	return loadOk;
}

bool PluginManager::loadFilterPlugin(FilterPluginInterface* iFilter, const QString& fileName )
{
	bool loadFilterOK = true;
	for(QAction *filterAction : iFilter->actions()) {
		if(iFilter->getClass(filterAction)==FilterPluginInterface::Generic){
			qDebug() << "Missing class for " +fileName + " " + filterAction->text();
			loadFilterOK = false;
		}
		if(iFilter->getRequirements(filterAction) == int(MeshModel::MM_UNKNOWN)){
			qDebug() << "Missing requirements for " +fileName + " " + filterAction->text();
			loadFilterOK = false;
		}
		if(iFilter->getPreConditions(filterAction) == int(MeshModel::MM_UNKNOWN)){
			qDebug() << "Missing preconditions for "+fileName + " " + filterAction->text();
			loadFilterOK = false;
		}
		if(iFilter->postCondition(filterAction) == int(MeshModel::MM_UNKNOWN )) {
			qDebug() << "Missing postcondition for "+fileName + " " + filterAction->text();
			loadFilterOK = false;
		}
		if(iFilter->filterArity(filterAction) == FilterPluginInterface::UNKNOWN_ARITY ) {
			qDebug() << "Missing Arity for " +fileName + " " + filterAction->text();
			loadFilterOK = false;
		}
	}
	if (loadFilterOK) {
		for(QAction *filterAction : iFilter->actions()) {
			filterAction->setData(QVariant(fileName));
			actionFilterMap.insert(filterAction->text(), filterAction);
		}
		filterPlugins.push_back(iFilter);
	}
	return loadFilterOK;
}

bool PluginManager::loadIOMeshPlugin(IOMeshPluginInterface* iIOMesh, const QString&)
{
	ioMeshPlugins.push_back(iIOMesh);
	return true;
}

bool PluginManager::loadIORasterPlugin(IORasterPluginInterface* iIORaster, const QString&)
{
	ioRasterPlugins.push_back(iIORaster);
	return true;
}

bool PluginManager::loadDecoratePlugin(DecoratePluginInterface* iDecorate, const QString&)
{
	decoratePlugins.push_back(iDecorate);
	for(QAction *decoratorAction : iDecorate->actions()) {
		iDecorate->initGlobalParameterList(decoratorAction, meshlab::defaultGlobalParameterList());
	}
	return true;
}

bool PluginManager::loadRenderPlugin(RenderPluginInterface* iRender, const QString&)
{
	renderPlugins.push_back(iRender);
	return true;
}

bool PluginManager::loadEditPlugin(EditPluginInterfaceFactory* iEditFactory, const QString&)
{
	editPlugins.push_back(iEditFactory);
	return true;
}

void PluginManager::fillKnownIOFormats()
{
	QString allKnownFormatsFilter = QObject::tr("All known formats (");
	for (IOMeshPluginInterface* pMeshIOPlugin:  ioMeshPlugins) {
		allKnownFormatsFilter += addPluginMeshFormats(inputMeshFormatToPluginMap, inputMeshFormatsDialogStringList, pMeshIOPlugin, pMeshIOPlugin->importFormats());
	}
	allKnownFormatsFilter.append(')');
	inputMeshFormatsDialogStringList.push_front(allKnownFormatsFilter);
	
	for (IOMeshPluginInterface* pMeshIOPlugin:  ioMeshPlugins) {
		addPluginMeshFormats(outputMeshFormatToPluginMap, outputMeshFormatsDialogStringList, pMeshIOPlugin, pMeshIOPlugin->exportFormats());
	}
	
	allKnownFormatsFilter = QObject::tr("All known formats (");
	
	for (IORasterPluginInterface* pRasterIOPlugin : ioRasterPlugins){
		allKnownFormatsFilter += addPluginRasterFormats(inputRasterFormatToPluginMap, inputRasterFormatsDialogStringList, pRasterIOPlugin, pRasterIOPlugin->importFormats());
	}
	
	allKnownFormatsFilter.append(')');
	inputRasterFormatsDialogStringList.push_front(allKnownFormatsFilter);
}

QString PluginManager::addPluginRasterFormats(
		QMap<QString, IORasterPluginInterface*>& map, 
		QStringList& formatFilters, 
		IORasterPluginInterface* pRasterIOPlugin,
		const QList<FileFormat>& format)
{
	QString allKnownFormatsFilter;
	for (const FileFormat& currentFormat : format) {
		QString currentFilterEntry = currentFormat.description + " (";
		
		//a particular file format could be associated with more than one file extension
		QStringListIterator itExtension(currentFormat.extensions);
		for (QString currentExtension : currentFormat.extensions) {
			currentExtension = currentExtension.toLower();
			if (!map.contains(currentExtension)) {
				map.insert(currentExtension, pRasterIOPlugin);
				allKnownFormatsFilter.append(QObject::tr(" *."));
				allKnownFormatsFilter.append(currentExtension);
			}
			currentFilterEntry.append(QObject::tr(" *."));
			currentFilterEntry.append(currentExtension);
		}
		currentFilterEntry.append(')');
		formatFilters.append(currentFilterEntry);
	}
	return allKnownFormatsFilter;
}

QString PluginManager::addPluginMeshFormats(
		QMap<QString, IOMeshPluginInterface*>& map, 
		QStringList& formatFilters, 
		IOMeshPluginInterface* pMeshIOPlugin,
		const QList<FileFormat>& format)
{
	QString allKnownFormatsFilter;
	for (const FileFormat& currentFormat : format) {
		QString currentFilterEntry = currentFormat.description + " (";
		
		//a particular file format could be associated with more than one file extension
		QStringListIterator itExtension(currentFormat.extensions);
		for (QString currentExtension : currentFormat.extensions) {
			currentExtension = currentExtension.toLower();
			if (!map.contains(currentExtension)) {
				map.insert(currentExtension, pMeshIOPlugin);
				allKnownFormatsFilter.append(QObject::tr(" *."));
				allKnownFormatsFilter.append(currentExtension);
			}
			currentFilterEntry.append(QObject::tr(" *."));
			currentFilterEntry.append(currentExtension);
		}
		currentFilterEntry.append(')');
		formatFilters.append(currentFilterEntry);
	}
	return allKnownFormatsFilter;
}

