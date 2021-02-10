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
		delete plugin;
	allPlugins.clear();
}

/**
 * @brief Loads the plugins contained in the default meshlab plugin directory.
 * 
 * If at least one plugin fails to be loaded, a MLException is thrown.
 * In any case, all the other valid plugins contained in the directory are loaded.
 */
void PluginManager::loadPlugins()
{
	loadPlugins(QDir(meshlab::defaultPluginPath()));
}

/**
 * @brief Loads the plugins contained in the given directory.
 * 
 * If at least one plugin fails to be loaded, a MLException is thrown.
 * In any case, all the other valid plugins contained in the directory are loaded.
 */
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
	std::list<std::pair<QString, QString>> errors;
	for(QString fileName : pluginsDir.entryList(QDir::Files)) {
		try {
			loadPlugin(fileName);
		}
		catch(const MLException& e){
			errors.push_back(std::make_pair(fileName, e.what()));
		}
	}
	fillKnownIOFormats();
	if (errors.size() > 0){
		QString singleError = "Unable to load the following plugins:\n\n";
		for (const auto& p : errors){
			singleError += "\t" + p.first + ": " + p.second + "\n";
		}
		throw MLException(singleError);
	}
}

/**
 * @brief Loads the plugin specified in the given file and adds the plugin into the
 * PluginManager.
 * 
 * Throws a MLException if the load of the plugin fails.
 */
void PluginManager::loadPlugin(const QString& fileName)
{
	QString absfilepath = pluginsDir.absoluteFilePath(fileName);
	QFileInfo fin(absfilepath);
	QPluginLoader loader(absfilepath);
	QObject *plugin = loader.instance();
	if (!plugin) {
		throw MLException(loader.errorString());
	}
	
	PluginFileInterface* ifp = dynamic_cast<PluginFileInterface *>(plugin);
	if (!ifp){
		throw MLException(fin.fileName() + " is not a MeshLab plugin.");
	}
	if (ifp && ifp->getMLVersion().second != MeshLabScalarTest<Scalarm>::doublePrecision()) {
		throw MLException(fin.fileName() + " has different floating point precision from the running MeshLab version.");
	}
	
	//TODO: check in some way also the meshlab version of the plugin
	
	MeshLabPluginType type(ifp);
	
	if (type.isDecoratePlugin()){
		loadDecoratePlugin(qobject_cast<DecoratePluginInterface *>(plugin), fileName);
	}
	if (type.isEditPlugin()){
		loadEditPlugin(qobject_cast<EditPluginInterfaceFactory *>(plugin), fileName);
	}
	if (type.isFilterPlugin()){
		loadFilterPlugin(qobject_cast<FilterPluginInterface *>(plugin), fileName);
	}
	if (type.isIOMeshPlugin()){
		loadIOMeshPlugin(qobject_cast<IOMeshPluginInterface *>(plugin), fileName);
	}
	if (type.isIORasterPlugin()){
		loadIORasterPlugin(qobject_cast<IORasterPluginInterface*>(plugin), fileName);
	}
	if (type.isRenderPlugin()){
		loadRenderPlugin(qobject_cast<RenderPluginInterface *>(plugin), fileName);
	}

	ifp->plugFileInfo = fin;
	allPlugins.push_back(ifp);
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

PluginFileInterface* PluginManager::operator[](unsigned int i) const
{
	return allPlugins[i];
}

PluginManager::PluginRangeIterator PluginManager::pluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return PluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

PluginManager::FilterPluginRangeIterator PluginManager::filterPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return FilterPluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

PluginManager::IOMeshPluginIterator PluginManager::ioMeshPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return IOMeshPluginIterator(this, iterateAlsoDisabledPlugins);
}

PluginManager::IORasterPluginIterator PluginManager::ioRasterPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return IORasterPluginIterator(this, iterateAlsoDisabledPlugins);
}

PluginManager::RenderPluginRangeIterator PluginManager::renderPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return RenderPluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

PluginManager::DecoratePluginRangeIterator PluginManager::decoratePluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return DecoratePluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

PluginManager::EditPluginFactoryRangeIterator PluginManager::editPluginFactoryIterator(bool iterateAlsoDisabledPlugins) const
{
	return EditPluginFactoryRangeIterator(this, iterateAlsoDisabledPlugins);
}

void PluginManager::loadFilterPlugin(FilterPluginInterface* iFilter, const QString& fileName)
{
	for(QAction *filterAction : iFilter->actions()) {
		if(iFilter->getClass(filterAction)==FilterPluginInterface::Generic){
			throw MLException("Missing class for " +fileName + " " + filterAction->text());
		}
		if(iFilter->getRequirements(filterAction) == int(MeshModel::MM_UNKNOWN)){
			throw MLException("Missing requirements for " +fileName + " " + filterAction->text());
		}
		if(iFilter->getPreConditions(filterAction) == int(MeshModel::MM_UNKNOWN)){
			throw MLException("Missing preconditions for "+fileName + " " + filterAction->text());
		}
		if(iFilter->postCondition(filterAction) == int(MeshModel::MM_UNKNOWN)) {
			throw MLException("Missing postcondition for "+fileName + " " + filterAction->text());
		}
		if(iFilter->filterArity(filterAction) == FilterPluginInterface::UNKNOWN_ARITY) {
			throw MLException("Missing Arity for " +fileName + " " + filterAction->text());
		}
	}

	for(QAction *filterAction : iFilter->actions()) {
		filterAction->setData(QVariant(fileName));
		actionFilterMap.insert(filterAction->text(), filterAction);
	}
	filterPlugins.push_back(iFilter);
}

void PluginManager::loadIOMeshPlugin(IOMeshPluginInterface* iIOMesh, const QString&)
{
	ioMeshPlugins.push_back(iIOMesh);
}

void PluginManager::loadIORasterPlugin(IORasterPluginInterface* iIORaster, const QString&)
{
	ioRasterPlugins.push_back(iIORaster);
}

void PluginManager::loadDecoratePlugin(DecoratePluginInterface* iDecorate, const QString&)
{
	decoratePlugins.push_back(iDecorate);
	for(QAction *decoratorAction : iDecorate->actions()) {
		iDecorate->initGlobalParameterList(decoratorAction, meshlab::defaultGlobalParameterList());
	}
}

void PluginManager::loadRenderPlugin(RenderPluginInterface* iRender, const QString&)
{
	renderPlugins.push_back(iRender);
}

void PluginManager::loadEditPlugin(EditPluginInterfaceFactory* iEditFactory, const QString&)
{
	editPlugins.push_back(iEditFactory);
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

