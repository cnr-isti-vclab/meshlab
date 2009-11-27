#include "pluginmanager.h"

PluginManager::PluginManager()
{
	pluginsDir=QDir(getPluginDirPath());
	// without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
	qApp->addLibraryPath(getPluginDirPath());
	qApp->addLibraryPath(getBaseDirPath());
}

void PluginManager::loadPlugins(RichParameterSet& defaultGlobal)
{
	QStringList pluginfilters;
#if defined(Q_OS_WIN)
	pluginfilters << "*.dll";		
#elif defined(Q_OS_MAC)
	pluginfilters << "*.dylib";		
#else
#endif
	//only the file with extension pluginfilters will be listed by function entryList()
	pluginsDir.setNameFilters(pluginfilters);
	
	qDebug( "Current Plugins Dir: %s ",qPrintable(pluginsDir.absolutePath()));
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
	{
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		if (plugin) 
		{
			MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter)
			{
				QAction *filterAction;
				foreach(filterAction, iFilter->actions())
				{
					actionFilterMap.insert(filterAction->text(),filterAction);
					iFilter->initGlobalParameterSet(filterAction,defaultGlobal);
					//filterAction->setToolTip(iFilter->filterInfo(filterAction));
					//connect(filterAction,SIGNAL(triggered()),this,SLOT(startFilter()));
					// if(fp) 
					//	 fprintf(fp, "*<b><i>%s</i></b> <br>%s<br>\n",qPrintable(filterAction->text()), qPrintable(iFilter->filterInfo(filterAction)));	
				}

				meshFilterPlug.push_back(iFilter);
			}
			

			MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			if (iIO) 
				meshIOPlug.push_back(iIO);

			MeshDecorateInterface *iDecorator = qobject_cast<MeshDecorateInterface *>(plugin);
			if (iDecorator)
			{
				meshDecoratePlug.push_back(iDecorator);

				QAction *decoratorAction;
				foreach(decoratorAction, iDecorator->actions())
				{
					//actionMap.insert(decoratorAction->name(),decoratorAction);
					iDecorator->initGlobalParameterSet(decoratorAction,defaultGlobal);
					//connect(decoratorAction,SIGNAL(triggered()),this,SLOT(applyDecorateMode()));
					//decoratorAction->setToolTip(iDecorator->Info(decoratorAction));
					//renderMenu->addAction(decoratorAction);
				}
			}

			MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
			if (iRender)
				//addToMenu(iRender->actions(), shadersMenu, SLOT(applyRenderMode()));
				meshRenderPlug.push_back(iRender);

			
			MeshEditInterfaceFactory *iEditFactory = qobject_cast<MeshEditInterfaceFactory *>(plugin);
			QAction *editAction = 0;
			if(iEditFactory)
			{
				//qDebug() << "Here with filename:" << fileName;
				meshEditInterfacePlug.push_back(iEditFactory);
				/*foreach(editAction, iEditFactory->actions())
				{
					editMenu->addAction(editAction);
					if(!editAction->icon().isNull())
					{
						editToolBar->addAction(editAction);
					} else qDebug() << "action was null";

					connect(editAction, SIGNAL(triggered()), this, SLOT(applyEditMode()));
					editActionList.push_back(editAction);
				}*/
			}
		}
	}


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
#endif
	return baseDir.absolutePath();
}

QString PluginManager::getPluginDirPath()
{
	QDir pluginsDir(getBaseDirPath());
	if(!pluginsDir.exists("plugins"))
		//QMessageBox::warning(0,"Meshlab Initialization","Serious error. Unable to find the plugins directory.");
		qDebug("Meshlab Initialization: Serious error. Unable to find the plugins directory.");
	pluginsDir.cd("plugins");
	return pluginsDir.absolutePath();
}



void PluginManager::LoadFormats(QStringList &filters, QHash<QString, MeshIOInterface*> &allKnownFormats, int type)
{
	QList<MeshIOInterface::Format> currentFormats;

	QString allKnownFormatsFilter = QObject::tr("All known formats ("); 

	QVector<MeshIOInterface*>::iterator itIOPlugin = meshIOPlug.begin();
	for (int i = 0; itIOPlugin != meshIOPlug.end(); ++itIOPlugin, ++i)  // cycle among loaded IO plugins
	{
		MeshIOInterface* pMeshIOPlugin = *itIOPlugin;

		switch(type){ 
		  case IMPORT :	currentFormats = pMeshIOPlugin->importFormats(); break;
		  case EXPORT : currentFormats = pMeshIOPlugin->exportFormats(); break;
		  default :assert(0); // unknown filter type
		}

		QList<MeshIOInterface::Format>::iterator itFormat = currentFormats.begin();
		while(itFormat != currentFormats.end())
		{
			MeshIOInterface::Format currentFormat = *itFormat;

			QString currentFilterEntry = currentFormat.description + " (";

			QStringListIterator itExtension(currentFormat.extensions);
			while (itExtension.hasNext())
			{
				QString currentExtension = itExtension.next().toLower();
				if (!allKnownFormats.contains(currentExtension))
				{
					allKnownFormats.insert(currentExtension, pMeshIOPlugin);
					allKnownFormatsFilter.append(QObject::tr(" *."));
					allKnownFormatsFilter.append(currentExtension);
				}
				currentFilterEntry.append(QObject::tr(" *."));
				currentFilterEntry.append(currentExtension);
			}
			currentFilterEntry.append(')');
			filters.append(currentFilterEntry);

			++itFormat;
		}
	}
	allKnownFormatsFilter.append(')');
	filters.push_front(allKnownFormatsFilter);
}
