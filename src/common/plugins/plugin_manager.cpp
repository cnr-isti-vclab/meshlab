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
 * @brief Checks if the given file is a valid MeshLab plugin.
 * It does not add the plugin to the plugin manger.
 * 
 * Note: this function is called automatically before loading a plugin.
 * 
 * Throws a MLException if the file is not a valid MeshLab plugin.
 */
void PluginManager::checkPlugin(const QString& filename)
{
	QFileInfo fin(filename);
	if (!fin.exists()){
		throw MLException(filename + " does not exists.");
	}
	QPluginLoader loader(fin.absoluteFilePath());
	QObject *plugin = loader.instance();
	if (!plugin) {
		throw MLException(loader.errorString());
	}

	PluginFileInterface* ifp = dynamic_cast<PluginFileInterface *>(plugin);
	if (!ifp){
		throw MLException(fin.fileName() + " is not a MeshLab plugin.");
	}

	//check floating point precision of the plugin
	if (ifp->getMLVersion().second != MeshLabScalarTest<Scalarm>::doublePrecision()) {
		throw MLException(fin.fileName() + " has different floating point precision from the running MeshLab version.");
	}

	//check version of the plugin
	// - needs to be the same major version and 
	//   <= minor version wrt MeshLab version

	// example: 2021.01 plugin can run on 2021.03 MeshLab
	//          2021.03 plugin cannot run on 2021.01 MeshLab
	//          2021.12 plugin cannot run on 2022.01 MeshLab
	std::string mlVersionPlug = ifp->getMLVersion().first;
	std::string majorVersionPlug = mlVersionPlug.substr(0, 4); //4 is the position of '.' in meshlab version
	std::string majorVersionML = meshlab::meshlabVersion().substr(0, 4);
	if (majorVersionML != majorVersionPlug){
		throw MLException(fin.fileName() + " has different major version from the running MeshLab version.");
	}
	std::string minorVersionPlug = mlVersionPlug.substr(5, mlVersionPlug.size());
	std::string minorVersionML = meshlab::meshlabVersion().substr(5, meshlab::meshlabVersion().size());
	if (std::stoi(minorVersionPlug) > std::stoi(minorVersionML)){
		throw MLException(fin.fileName() + " has greater version from the running MeshLab version. Please update MeshLab to use it.");
	}
	
	MeshLabPluginType type(ifp);
	
	if (!type.isValid()){
		throw MLException(fin.fileName() + " has none of the known plugin types known by this MeshLab version.");
	}
	
	//ToDo: proper checks also for other plugin types...
	if (type.isFilterPlugin()){
		checkFilterPlugin(qobject_cast<FilterPluginInterface *>(plugin));
	}
}

/**
 * @brief Loads the plugins contained in the default meshlab plugin directory.
 * 
 * If at least one plugin fails to be loaded, a MLException is thrown.
 * In any case, all the other valid plugins contained in the directory are loaded.
 */
void PluginManager::loadPlugins()
{
	// without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
	// ToDo: get rid of any qApp here
	qApp->addLibraryPath(meshlab::defaultPluginPath());
	loadPlugins(QDir(meshlab::defaultPluginPath()));
}

/**
 * @brief Loads the plugins contained in the given directory.
 * 
 * If at least one plugin fails to be loaded, a MLException is thrown.
 * In any case, all the other valid plugins contained in the directory are loaded.
 */
void PluginManager::loadPlugins(QDir pluginsDirectory)
{
	if (pluginsDirectory.exists()){
		QStringList nameFiltersPlugins = fileNamePluginDLLs();
		
		//only the file with extension pluginfilters will be listed by function entryList()
		pluginsDirectory.setNameFilters(nameFiltersPlugins);
		
		//qDebug("Current Plugins Dir is: %s ", qUtf8Printable(pluginsDirectory.absolutePath()));
		std::list<std::pair<QString, QString>> errors;
		for(QString fileName : pluginsDirectory.entryList(QDir::Files)) {
			try {
				loadPlugin(pluginsDirectory.absoluteFilePath(fileName));
			}
			catch(const MLException& e){
				errors.push_back(std::make_pair(fileName, e.what()));
			}
		}
		if (errors.size() > 0){
			QString singleError = "Unable to load the following plugins:\n\n";
			for (const auto& p : errors){
				singleError += "\t" + p.first + ": " + p.second + "\n";
			}
			throw MLException(singleError);
		}
	}
}

/**
 * @brief Loads the plugin specified in the given file and adds the plugin into the
 * PluginManager.
 * 
 * Note: better to give the absolute path of the plugin file.
 * 
 * Throws a MLException if the load of the plugin fails.
 */
void PluginManager::loadPlugin(const QString& fileName)
{
	checkPlugin(fileName);

	//load the plugin depending on the type (can be more than one type!)
	QFileInfo fin(fileName);
	QPluginLoader loader(fin.absoluteFilePath());
	QObject *plugin = loader.instance();
	PluginFileInterface* ifp = dynamic_cast<PluginFileInterface *>(plugin);
	MeshLabPluginType type(ifp);
	
	if (type.isDecoratePlugin()){
		loadDecoratePlugin(qobject_cast<DecoratePluginInterface *>(plugin));
	}
	if (type.isEditPlugin()){
		loadEditPlugin(qobject_cast<EditPluginInterfaceFactory *>(plugin));
	}
	if (type.isFilterPlugin()){
		loadFilterPlugin(qobject_cast<FilterPluginInterface *>(plugin));
	}
	if (type.isIOMeshPlugin()){
		loadIOMeshPlugin(qobject_cast<IOMeshPluginInterface *>(plugin));
	}
	if (type.isIORasterPlugin()){
		loadIORasterPlugin(qobject_cast<IORasterPluginInterface*>(plugin));
	}
	if (type.isRenderPlugin()){
		loadRenderPlugin(qobject_cast<RenderPluginInterface *>(plugin));
	}

	//set the QFileInfo to the plugin, and add it to the continer
	//of all plugins
	ifp->plugFileInfo = fin;
	allPlugins.push_back(ifp);
}

void PluginManager::unloadPlugin(PluginFileInterface* ifp)
{
	auto it = std::find(allPlugins.begin(), allPlugins.end(), ifp);
	if (it != allPlugins.end()){
		MeshLabPluginType type(ifp);
		if (type.isDecoratePlugin()){
			unloadDecoratePlugin(dynamic_cast<DecoratePluginInterface *>(ifp));
		}
		if (type.isEditPlugin()){
			unloadEditPlugin(dynamic_cast<EditPluginInterfaceFactory *>(ifp));
		}
		if (type.isFilterPlugin()){
			unloadFilterPlugin(dynamic_cast<FilterPluginInterface *>(ifp));
		}
		if (type.isIOMeshPlugin()){
			unloadIOMeshPlugin(dynamic_cast<IOMeshPluginInterface *>(ifp));
		}
		if (type.isIORasterPlugin()){
			unloadIORasterPlugin(dynamic_cast<IORasterPluginInterface*>(ifp));
		}
		if (type.isRenderPlugin()){
			unloadRenderPlugin(dynamic_cast<RenderPluginInterface *>(ifp));
		}
		allPlugins.erase(it);
		delete ifp;
	}
}

void PluginManager::enablePlugin(PluginFileInterface* ifp)
{
	auto it = std::find(allPlugins.begin(), allPlugins.end(), ifp);
	if (it  != allPlugins.end() && !ifp->isEnabled()){
		ifp->enable();
		//ToDo other checks...
	}
}

void PluginManager::disablePlugin(PluginFileInterface* ifp)
{
	auto it = std::find(allPlugins.begin(), allPlugins.end(), ifp);
	if (it  != allPlugins.end() && ifp->isEnabled()){
		ifp->disable();
		//ToDo other checks...
	}
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

QStringList PluginManager::inputMeshFormatListDialog() const
{
	return inputFormatListDialog(ioMeshPluginIterator());
}

QStringList PluginManager::outputMeshFormatListDialog() const
{
	return outputFormatListDialog(ioMeshPluginIterator());
}

QStringList PluginManager::inputRasterFormatListDialog() const
{
	return inputFormatListDialog(ioRasterPluginIterator());
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

void PluginManager::checkFilterPlugin(FilterPluginInterface* iFilter)
{
	for(QAction *filterAction : iFilter->actions()) {
		if(iFilter->getClass(filterAction)==FilterPluginInterface::Generic){
			throw MLException("Missing class for " + iFilter->pluginName() + " " + filterAction->text());
		}
		if(iFilter->getRequirements(filterAction) == int(MeshModel::MM_UNKNOWN)){
			throw MLException("Missing requirements for " + iFilter->pluginName() + " " + filterAction->text());
		}
		if(iFilter->getPreConditions(filterAction) == int(MeshModel::MM_UNKNOWN)){
			throw MLException("Missing preconditions for "+ iFilter->pluginName() + " " + filterAction->text());
		}
		if(iFilter->postCondition(filterAction) == int(MeshModel::MM_UNKNOWN)) {
			throw MLException("Missing postcondition for "+ iFilter->pluginName() + " " + filterAction->text());
		}
		if(iFilter->filterArity(filterAction) == FilterPluginInterface::UNKNOWN_ARITY) {
			throw MLException("Missing Arity for " + iFilter->pluginName() + " " + filterAction->text());
		}
	}
}

void PluginManager::loadFilterPlugin(FilterPluginInterface* iFilter)
{
	for(QAction *filterAction : iFilter->actions()) {
		filterAction->setData(QVariant(iFilter->pluginName()));
		actionFilterMap.insert(filterAction->text(), filterAction);
	}
	filterPlugins.push_back(iFilter);
}

void PluginManager::loadIOMeshPlugin(IOMeshPluginInterface* iIOMesh)
{
	ioMeshPlugins.push_back(iIOMesh);

	//add input formats to inputFormatMap
	for (const FileFormat& ff : iIOMesh->importFormats()){
		for (QString currentExtension : ff.extensions) {
			if (! inputMeshFormatToPluginMap.contains(currentExtension.toLower())) {
				inputMeshFormatToPluginMap.insert(currentExtension.toLower(), iIOMesh);
			}
		}
	}
	
	//add output formats to outputFormatMap
	for (const FileFormat& ff : iIOMesh->exportFormats()){
		for (QString currentExtension : ff.extensions) {
			if (! outputMeshFormatToPluginMap.contains(currentExtension.toLower())) {
				outputMeshFormatToPluginMap.insert(currentExtension.toLower(), iIOMesh);
			}
		}
	}
}

void PluginManager::loadIORasterPlugin(IORasterPluginInterface* iIORaster)
{
	ioRasterPlugins.push_back(iIORaster);
	
	//add input formats to inputFormatMap
	for (const FileFormat& ff : iIORaster->importFormats()){
		for (QString currentExtension : ff.extensions) {
			if (! inputRasterFormatToPluginMap.contains(currentExtension.toLower())) {
				inputRasterFormatToPluginMap.insert(currentExtension.toLower(), iIORaster);
			}
		}
	}
}

void PluginManager::loadDecoratePlugin(DecoratePluginInterface* iDecorate)
{
	decoratePlugins.push_back(iDecorate);
	for(QAction *decoratorAction : iDecorate->actions()) {
		iDecorate->initGlobalParameterList(decoratorAction, meshlab::defaultGlobalParameterList());
	}
}

void PluginManager::loadRenderPlugin(RenderPluginInterface* iRender)
{
	renderPlugins.push_back(iRender);
}

void PluginManager::loadEditPlugin(EditPluginInterfaceFactory* iEditFactory)
{
	editPlugins.push_back(iEditFactory);
}

void PluginManager::unloadFilterPlugin(FilterPluginInterface* iFilter)
{
	for(QAction *filterAction : iFilter->actions()) {
		actionFilterMap.remove(filterAction->text());
	}
	filterPlugins.erase(std::find(filterPlugins.begin(), filterPlugins.end(), iFilter));
}

void PluginManager::unloadIOMeshPlugin(IOMeshPluginInterface* iIOMesh)
{
	ioMeshPlugins.erase(std::find(ioMeshPlugins.begin(), ioMeshPlugins.end(), iIOMesh));
	for (const FileFormat& ff : iIOMesh->importFormats()){
		for (QString currentExtension : ff.extensions) {
			inputMeshFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
	for (const FileFormat& ff : iIOMesh->exportFormats()){
		for (QString currentExtension : ff.extensions) {
			outputMeshFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
}

void PluginManager::unloadIORasterPlugin(IORasterPluginInterface* iIORaster)
{
	ioRasterPlugins.erase(std::find(ioRasterPlugins.begin(), ioRasterPlugins.end(), iIORaster));
	for (const FileFormat& ff : iIORaster->importFormats()){
		for (QString currentExtension : ff.extensions) {
			inputRasterFormatToPluginMap.remove(currentExtension.toLower());
		}
	}
}

void PluginManager::unloadDecoratePlugin(DecoratePluginInterface* iDecorate)
{
	decoratePlugins.erase(std::find(decoratePlugins.begin(), decoratePlugins.end(), iDecorate));
}

void PluginManager::unloadRenderPlugin(RenderPluginInterface* iRender)
{
	renderPlugins.erase(std::find(renderPlugins.begin(), renderPlugins.end(), iRender));
}

void PluginManager::unloadEditPlugin(EditPluginInterfaceFactory* iEditFactory)
{
	editPlugins.erase(std::find(editPlugins.begin(), editPlugins.end(), iEditFactory));
}

template<typename RangeIterator>
QStringList PluginManager::inputFormatListDialog(RangeIterator iterator)
{
	QString allKnownFormats = QObject::tr("All known formats (");
	QStringList inputRasterFormatsDialogStringList;
	for (auto ioRaster : iterator){
		QString allKnownFormatsFilter;
		for (const FileFormat& currentFormat : ioRaster->importFormats()){
			QString currentFilterEntry = currentFormat.description + " (";
			for (QString currentExtension : currentFormat.extensions) {
				currentExtension = currentExtension.toLower();
				allKnownFormatsFilter.append(QObject::tr(" *."));
				allKnownFormatsFilter.append(currentExtension);
				currentFilterEntry.append(QObject::tr(" *."));
				currentFilterEntry.append(currentExtension);
			}
			currentFilterEntry.append(')');
			inputRasterFormatsDialogStringList.append(currentFilterEntry);
		}
		allKnownFormats += allKnownFormatsFilter;
	}
	allKnownFormats.append(')');
	inputRasterFormatsDialogStringList.push_front(allKnownFormats);
	return inputRasterFormatsDialogStringList;
}

template<typename RangeIterator>
QStringList PluginManager::outputFormatListDialog(RangeIterator iterator)
{
	QStringList inputRasterFormatsDialogStringList;
	for (auto ioRaster : iterator){
		for (const FileFormat& currentFormat : ioRaster->exportFormats()){
			QString currentFilterEntry = currentFormat.description + " (";
			for (QString currentExtension : currentFormat.extensions) {
				currentExtension = currentExtension.toLower();
				currentFilterEntry.append(QObject::tr(" *."));
				currentFilterEntry.append(currentExtension);
			}
			currentFilterEntry.append(')');
			inputRasterFormatsDialogStringList.append(currentFilterEntry);
		}
	}
	return inputRasterFormatsDialogStringList;
}
