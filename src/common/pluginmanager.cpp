#include "pluginmanager.h"
#include <QObject>
#include <qapplication.h>
#include <QPluginLoader>
#include <QDebug>
#include <vcg/complex/algorithms/create/platonic.h>

#include "mlexception.h"

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

static QString fileNamePrefixPluginDLLs() {
#if defined(Q_OS_WIN)
	return QString("");
#else //macos and linux
	return QString("lib");
#endif
}

PluginManager::PluginManager()
{
	//pluginsDir=QDir(getPluginDirPath());
	//without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
	//qApp->addLibraryPath(getPluginDirPath());
	//qApp->addLibraryPath(getBaseDirPath());
}

PluginManager::~PluginManager()
{
	ioMeshPlugins.clear();
	meshFilterPlug.clear();
	meshRenderPlug.clear();
	meshDecoratePlug.clear();
	for (PluginInterface* plugin : ownerPlug)
		delete plugin;
	ownerPlug.clear();
	
	for (int ii = 0; ii < meshEditInterfacePlug.size(); ++ii)
		delete meshEditInterfacePlug[ii];
	meshEditInterfacePlug.clear();
}



void PluginManager::loadPlugins(RichParameterList& defaultGlobal)
{
	loadPlugins(defaultGlobal, QDir(getDefaultPluginDirPath()));
}

void PluginManager::loadPlugins(RichParameterList& defaultGlobal, const QDir& pluginsDirectory)
{
	pluginsDir = pluginsDirectory;
	// without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
	qApp->addLibraryPath(getDefaultPluginDirPath());
	qApp->addLibraryPath(getBaseDirPath());
	QStringList nameFiltersPlugins = fileNamePluginDLLs();
	
	//only the file with extension pluginfilters will be listed by function entryList()
	pluginsDir.setNameFilters(nameFiltersPlugins);
	
	qDebug("Current Plugins Dir is: %s ", qUtf8Printable(pluginsDir.absolutePath()));
	for(QString fileName : pluginsDir.entryList(QDir::Files))
	{
		//      qDebug() << fileName;
		QString absfilepath = pluginsDir.absoluteFilePath(fileName);
		QFileInfo fin(absfilepath);
		QPluginLoader loader(absfilepath);
		QObject *plugin = loader.instance();
		if (plugin)
		{
			pluginsLoaded.push_back(fileName);
			PluginInterface *iCommon = nullptr;
			FilterPluginInterface *iFilter = qobject_cast<FilterPluginInterface *>(plugin);
			if (iFilter)
			{
				iCommon = iFilter;
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
						stringFilterMap.insert(filterAction->text(), iFilter);
					}
					meshFilterPlug.push_back(iFilter);
				}
			}
			IOMeshPluginInterface *iIOMesh = qobject_cast<IOMeshPluginInterface *>(plugin);
			if (iIOMesh)
			{
				iCommon = iIOMesh;
				ioMeshPlugins.push_back(iIOMesh);
			}
			
			IORasterPluginInterface* iIORaster = qobject_cast<IORasterPluginInterface*>(plugin);
			if (iIORaster){
				iCommon = iIORaster;
				ioRasterPlugins.push_back(iIORaster);
			}
			
			DecoratePluginInterface *iDecorator = qobject_cast<DecoratePluginInterface *>(plugin);
			if (iDecorator)
			{
				iCommon = iDecorator;
				meshDecoratePlug.push_back(iDecorator);
				for(QAction *decoratorAction : iDecorator->actions())
				{
					decoratorActionList.push_back(decoratorAction);
					iDecorator->initGlobalParameterList(decoratorAction, defaultGlobal);
				}
			}
			
			RenderPluginInterface *iRender = qobject_cast<RenderPluginInterface *>(plugin);
			if (iRender)
			{
				iCommon = iRender;
				meshRenderPlug.push_back(iRender);
			}
			
			EditPluginInterfaceFactory *iEditFactory = qobject_cast<EditPluginInterfaceFactory *>(plugin);
			if (iEditFactory)
			{
				meshEditInterfacePlug.push_back(iEditFactory);
				foreach(QAction* editAction, iEditFactory->actions())
					editActionList.push_back(editAction);
			}
			else if (iCommon)
			{
				ownerPlug.push_back(iCommon);
			} else {
				// qDebug("Plugin %s was loaded, but could not be casted to any known type.", qUtf8Printable(fileName));
			}
		}
		else
			qDebug() << loader.errorString();
	}
	fillKnownIOFormats();
}

int PluginManager::numberIOPlugins() const
{
	return ioMeshPlugins.size();
}

// Search among all the decorator plugins the one that contains a decoration with the given name
DecoratePluginInterface *PluginManager::getDecoratorInterfaceByName(const QString& name)
{
	foreach(DecoratePluginInterface *tt, this->meshDecoratePlugins())
	{
		foreach( QAction *ac, tt->actions())
			if( name == tt->decorationName(ac) ) return tt;
	}
	assert(0);
	return 0;
}

/*
This function create a map from filtername to dummy RichParameterSet.
containing for each filtername the set of parameter that it uses.
*/
QMap<QString, RichParameterList> PluginManager::generateFilterParameterMap()
{
	QMap<QString, RichParameterList> FPM;
	MeshDocument md;
	MeshModel* mm = md.addNewMesh("", "dummy", true);
	vcg::tri::Tetrahedron<CMeshO>(mm->cm);
	mm->updateDataMask(MeshModel::MM_ALL);
	QMap<QString, QAction*>::iterator ai;
	for (ai = this->actionFilterMap.begin(); ai != this->actionFilterMap.end(); ++ai)
	{
		QString filterName = ai.key();//  ->filterName();
		//QAction act(filterName,NULL);
		RichParameterList rp;
		stringFilterMap[filterName]->initParameterList(ai.value(), md, rp);
		FPM[filterName] = rp;
	}
	return FPM;
}

QString PluginManager::getBaseDirPath()
{
	QDir baseDir(qApp->applicationDirPath());
	
#if defined(Q_OS_WIN)
	// Windows:
	// during development with visual studio binary could be in the debug/release subdir.
	// once deployed plugins dir is in the application directory, so
	if (baseDir.dirName() == "debug" || baseDir.dirName() == "release")		baseDir.cdUp(); 
#endif
	
#if defined(Q_OS_MAC)
	// Mac: during developmentwith xcode  and well deployed the binary is well buried.
	for(int i=0;i<6;++i){
		if(baseDir.exists("plugins")) break;
		baseDir.cdUp();
	}
	qDebug("The base dir is %s", qUtf8Printable(baseDir.absolutePath()));
#endif
	return baseDir.absolutePath();
}

QString PluginManager::getDefaultPluginDirPath()
{
	QDir pluginsDir(getBaseDirPath());
#if defined(Q_OS_WIN)
	QString d = pluginsDir.dirName();
	QString dLower = d.toLower();
	if (dLower == "release" || dLower == "relwithdebinfo" || dLower == "debug" ||
			dLower == "minsizerel") {
		// This is a configuration directory for MS Visual Studio.
		pluginsDir.cdUp();
	} else {
		d.clear();
	}
#endif
	if (pluginsDir.exists("plugins")) {
		pluginsDir.cd("plugins");
		
#if defined(Q_OS_WIN)
		// Re-apply the configuration dir, if any.
		if (!d.isEmpty() && pluginsDir.exists(d)) {
			pluginsDir.cd(d);
		}
#endif
		
		return pluginsDir.absolutePath();
	}
#if !defined(Q_OS_MAC) && !defined(Q_OS_WIN)
	else if (pluginsDir.dirName() == "bin") {
		pluginsDir.cdUp();
		pluginsDir.cd("lib");
		pluginsDir.cd("meshlab");
		if (pluginsDir.exists("plugins")) {
			pluginsDir.cd("plugins");
			return pluginsDir.absolutePath();
		}
	}
#endif
	//QMessageBox::warning(0,"Meshlab Initialization","Serious error. Unable to find the plugins directory.");
	qDebug("Meshlab Initialization: Serious error. Unable to find the plugins directory.");
	return {};
}



void PluginManager::fillKnownIOFormats()
{
	QString allKnownFormatsFilter = QObject::tr("All known formats (");
	for (IOMeshPluginInterface* pMeshIOPlugin:  ioMeshPlugins) {
		allKnownFormatsFilter += addPluginMeshFormats(allKnowInputMeshFormats, inpMeshFilters, pMeshIOPlugin, pMeshIOPlugin->importFormats());
	}
	allKnownFormatsFilter.append(')');
	inpMeshFilters.push_front(allKnownFormatsFilter);
	
	for (IOMeshPluginInterface* pMeshIOPlugin:  ioMeshPlugins) {
		addPluginMeshFormats(allKnowOutputFormats, outFilters, pMeshIOPlugin, pMeshIOPlugin->exportFormats());
	}
	
	allKnownFormatsFilter = QObject::tr("All known formats (");
	
	for (IORasterPluginInterface* pRasterIOPlugin : ioRasterPlugins){
		addPluginRasterFormats(allKnownInputRasterFormats, inpRasterFilters, pRasterIOPlugin, pRasterIOPlugin->importFormats());
	}
	
	allKnownFormatsFilter.append(')');
	inpRasterFilters.push_front(allKnownFormatsFilter);
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

QString PluginManager::osIndependentPluginName(const QString& plname)
{
	QFileInfo fi(plname);
	QString res = fi.baseName();
	QString pref = fileNamePrefixPluginDLLs();
	return res.remove(0, pref.size());
}

