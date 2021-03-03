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

#include "plugin_manager.h"
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
	for (auto& plugin : allPlugins)
		delete plugin;
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
		throw MLException(fin.fileName() + " does not seem to be a Qt Plugin.\n\n" + loader.errorString());
	}

	MeshLabPluginFile* ifp = dynamic_cast<MeshLabPluginFile *>(plugin);
	if (!ifp){
		throw MLException(fin.fileName() + " is not a MeshLab plugin.");
	}

	//check floating point precision of the plugin
	if (ifp->getMLVersion().second != MeshLabScalarTest<Scalarm>::doublePrecision()) {
		throw MLException(fin.fileName() + " has different floating point precision from the running MeshLab version.");
	}

	std::string mlVersionPlug = ifp->getMLVersion().first;
	/** Rules for the future: **/
	//  check version of the plugin
	//   - needs to be the same major version and
	//     <= minor version wrt MeshLab version
	//   example: 2021.01 plugin can run on 2021.03 MeshLab
	//          2021.03 plugin cannot run on 2021.01 MeshLab
	//          2021.12 plugin cannot run on 2022.01 MeshLab
//	std::string majorVersionPlug = mlVersionPlug.substr(0, 4); //4 is the position of '.' in meshlab version
//	std::string majorVersionML = meshlab::meshlabVersion().substr(0, 4);
//	if (majorVersionML != majorVersionPlug){
//		throw MLException(fin.fileName() + " has different major version from the running MeshLab version.");
//	}
//	std::string minorVersionPlug = mlVersionPlug.substr(5, mlVersionPlug.size());
//	std::string minorVersionML = meshlab::meshlabVersion().substr(5, meshlab::meshlabVersion().size());
//	if (std::stoi(minorVersionPlug) > std::stoi(minorVersionML)){
//		throw MLException(fin.fileName() + " has greater version from the running MeshLab version. Please update MeshLab to use it.");
//	}

	/** Rules for now: plugin needs to have same version of meshlab **/
	if (mlVersionPlug != meshlab::meshlabVersion()){
		throw MLException(fin.fileName() + " has different version from the running MeshLab version.");
	}
	
	MeshLabPluginType type(ifp);
	
	if (!type.isValid()){
		throw MLException(fin.fileName() + " has none of the known plugin types known by this MeshLab version.");
	}
	
	//ToDo: proper checks also for other plugin types...
	if (type.isFilterPlugin()){
		checkFilterPlugin(qobject_cast<FilterPlugin *>(plugin));
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
	QFileInfo fin(fileName);
	if (pluginFiles.find(fin.absoluteFilePath()) != pluginFiles.end())
		throw MLException(fin.fileName() + " has been already loaded.");

	checkPlugin(fileName);

	//load the plugin depending on the type (can be more than one type!)
	QPluginLoader loader(fin.absoluteFilePath());
	QObject *plugin = loader.instance();
	MeshLabPluginFile* ifp = dynamic_cast<MeshLabPluginFile *>(plugin);
	MeshLabPluginType type(ifp);
	
	if (type.isDecoratePlugin()){
		decoratePlugins.pushDecoratePlugin(qobject_cast<DecoratePlugin *>(plugin));
	}
	if (type.isEditPlugin()){
		editPlugins.pushEditPlugin(qobject_cast<EditPluginFactory *>(plugin));
	}
	if (type.isFilterPlugin()){
		filterPlugins.pushFilterPlugin(qobject_cast<FilterPlugin *>(plugin));
	}
	if (type.isIOMeshPlugin()){
		ioMeshPlugins.pushIOMeshPlugin(qobject_cast<IOMeshPlugin *>(plugin));
	}
	if (type.isIORasterPlugin()){
		ioRasterPlugins.pushIORasterPlugin(qobject_cast<IORasterPlugin*>(plugin));
	}
	if (type.isRenderPlugin()){
		renderPlugins.pushRenderPlugin(qobject_cast<RenderPlugin *>(plugin));
	}

	//set the QFileInfo to the plugin, and add it to the continer
	//of all plugins
	ifp->plugFileInfo = fin;
	allPlugins.push_back(ifp);
	pluginFiles.insert(fin.absoluteFilePath());
}

void PluginManager::unloadPlugin(MeshLabPluginFile* ifp)
{
	auto it = std::find(allPlugins.begin(), allPlugins.end(), ifp);
	if (it != allPlugins.end()){
		MeshLabPluginType type(ifp);
		if (type.isDecoratePlugin()){
			decoratePlugins.eraseDecoratePlugin(dynamic_cast<DecoratePlugin *>(ifp));
		}
		if (type.isEditPlugin()){
			editPlugins.eraseEditPlugin(dynamic_cast<EditPluginFactory *>(ifp));
		}
		if (type.isFilterPlugin()){
			filterPlugins.eraseFilterPlugin(dynamic_cast<FilterPlugin *>(ifp));
		}
		if (type.isIOMeshPlugin()){
			ioMeshPlugins.eraseIOMeshPlugin(dynamic_cast<IOMeshPlugin *>(ifp));
		}
		if (type.isIORasterPlugin()){
			ioRasterPlugins.eraseIORasterPlugin(dynamic_cast<IORasterPlugin*>(ifp));
		}
		if (type.isRenderPlugin()){
			renderPlugins.eraseRenderPlugin(dynamic_cast<RenderPlugin *>(ifp));
		}
		allPlugins.erase(it);
		delete ifp;
	}
}

void PluginManager::enablePlugin(MeshLabPluginFile* ifp)
{
	auto it = std::find(allPlugins.begin(), allPlugins.end(), ifp);
	if (it  != allPlugins.end() && !ifp->isEnabled()){
		ifp->enable();
		//ToDo other checks...
	}
}

void PluginManager::disablePlugin(MeshLabPluginFile* ifp)
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
DecoratePlugin *PluginManager::getDecoratePlugin(const QString& name)
{
	return decoratePlugins.decoratePlugin(name);
}

QAction* PluginManager::filterAction(const QString& name)
{
	return filterPlugins.filterAction(name);
}

IOMeshPlugin* PluginManager::inputMeshPlugin(const QString& inputFormat) const
{
	return ioMeshPlugins.inputMeshPlugin(inputFormat);
}

IOMeshPlugin* PluginManager::outputMeshPlugin(const QString& outputFormat) const
{
	return ioMeshPlugins.outputMeshPlugin(outputFormat);
}

IORasterPlugin* PluginManager::inputRasterPlugin(const QString inputFormat) const
{
	return ioRasterPlugins.inputRasterPlugin(inputFormat);
}

bool PluginManager::isInputMeshFormatSupported(const QString inputFormat) const
{
	return ioMeshPlugins.isInputMeshFormatSupported(inputFormat);
}

bool PluginManager::isOutputMeshFormatSupported(const QString outputFormat) const
{
	return ioMeshPlugins.isOutputMeshFormatSupported(outputFormat);
}

bool PluginManager::isInputRasterFormatSupported(const QString inputFormat) const
{
	return ioRasterPlugins.isInputRasterFormatSupported(inputFormat);
}

QStringList PluginManager::inputMeshFormatList() const
{
	return ioMeshPlugins.inputMeshFormatList();
}

QStringList PluginManager::outputMeshFormatList() const
{
	return ioMeshPlugins.outputMeshFormatList();
}

QStringList PluginManager::inputRasterFormatList() const
{
	return ioRasterPlugins.inputRasterFormatList();
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

MeshLabPluginFile* PluginManager::operator[](unsigned int i) const
{
	return allPlugins[i];
}

PluginManager::PluginRangeIterator PluginManager::pluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return PluginRangeIterator(this, iterateAlsoDisabledPlugins);
}

FilterPluginContainer::FilterPluginRangeIterator PluginManager::filterPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return filterPlugins.filterPluginIterator(iterateAlsoDisabledPlugins);
}

IOMeshPluginContainer::IOMeshPluginRangeIterator PluginManager::ioMeshPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return ioMeshPlugins.ioMeshPluginIterator(iterateAlsoDisabledPlugins);
}

IORasterPluginContainer::IORasterPluginRangeIterator PluginManager::ioRasterPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return ioRasterPlugins.ioRasterPluginIterator(iterateAlsoDisabledPlugins);
}

RenderPluginContainer::RenderPluginRangeIterator PluginManager::renderPluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return renderPlugins.renderPluginIterator(iterateAlsoDisabledPlugins);
}

DecoratePluginContainer::DecoratePluginRangeIterator PluginManager::decoratePluginIterator(bool iterateAlsoDisabledPlugins) const
{
	return decoratePlugins.decoratePluginIterator(iterateAlsoDisabledPlugins);
}

EditPluginContainer::EditPluginFactoryRangeIterator PluginManager::editPluginFactoryIterator(bool iterateAlsoDisabledPlugins) const
{
	return editPlugins.editPluginIterator(iterateAlsoDisabledPlugins);
}

void PluginManager::checkFilterPlugin(FilterPlugin* iFilter)
{
	for(QAction *filterAction : iFilter->actions()) {
		if(iFilter->getClass(filterAction)==FilterPlugin::Generic){
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
		if(iFilter->filterArity(filterAction) == FilterPlugin::UNKNOWN_ARITY) {
			throw MLException("Missing Arity for " + iFilter->pluginName() + " " + filterAction->text());
		}
	}
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

ConstPluginIterator<MeshLabPluginFile> PluginManager::PluginRangeIterator::begin()
{
	return ConstPluginIterator<MeshLabPluginFile>(pm->allPlugins, pm->allPlugins.begin(), b);
}

ConstPluginIterator<MeshLabPluginFile> PluginManager::PluginRangeIterator::end()
{
	return ConstPluginIterator<MeshLabPluginFile>(pm->allPlugins, pm->allPlugins.end(), b);
}

PluginManager::PluginRangeIterator::PluginRangeIterator(const PluginManager* pm, bool iterateAlsoDisabledPlugins) :
	pm(pm), b(iterateAlsoDisabledPlugins)
{
}
