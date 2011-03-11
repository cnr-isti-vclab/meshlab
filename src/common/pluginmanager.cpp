#include "pluginmanager.h"
#include <QObject>
#include <QtScript/QtScript>
#include <vcg/complex/trimesh/create/platonic.h>

#include "scriptinterface.h"


PluginManager::PluginManager()
:currentDocInterface(NULL),env()
{
    //pluginsDir=QDir(getPluginDirPath());
	// without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
    //qApp->addLibraryPath(getPluginDirPath());
    //qApp->addLibraryPath(getBaseDirPath());
}

void PluginManager::loadPlugins(RichParameterSet& defaultGlobal)
{
    pluginsDir=QDir(getPluginDirPath());
  // without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
    qApp->addLibraryPath(getPluginDirPath());
    qApp->addLibraryPath(getBaseDirPath());
	QStringList pluginfilters;
#if defined(Q_OS_WIN)
	pluginfilters << "*.dll";		
#elif defined(Q_OS_MAC)
	pluginfilters << "*.dylib";		
#else
#endif
	//only the file with extension pluginfilters will be listed by function entryList()
	pluginsDir.setNameFilters(pluginfilters);
	
  qDebug( "Current Plugins Dir is: %s ",qPrintable(pluginsDir.absolutePath()));
	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) 
	{
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		if (plugin) 
		{
      pluginsLoaded.push_back(fileName);
      MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter)
			{
        meshFilterPlug.push_back(iFilter);
        foreach(QAction *filterAction, iFilter->actions())
				{
					actionFilterMap.insert(filterAction->text(),filterAction);
          stringFilterMap.insert(filterAction->text(),iFilter);
					iFilter->initGlobalParameterSet(filterAction,defaultGlobal);
				}
			}

			MeshLabFilterInterface* iXMLfilter = qobject_cast<MeshLabFilterInterface *>(plugin);
			if (iXMLfilter)
			{
				MeshLabXMLFilterContainer fc;
				fc.filterInterface = iXMLfilter;
				int res = fileName.lastIndexOf(QObject::tr("."));
				QString withoutext = fileName.left(res); 
				QString xmlFile = getPluginDirPath() + "/" + withoutext + QObject::tr(".xml");
				qDebug("Loading XMLFile: %s",qPrintable(xmlFile));
        if(!QFileInfo(xmlFile).exists())
        {
          qDebug("Error XMLFile: %s does not exist",qPrintable(xmlFile));
        }
        else
        {
          XMLMessageHandler xmlErr;
          fc.xmlInfo = XMLFilterInfo::createXMLFileInfo(xmlFile,xmlSchemaFile(),xmlErr);
          if (fc.xmlInfo != NULL)
          {
            QStringList fn = fc.xmlInfo->filterNames();
            foreach(QString filtName,fn)
            {
              fc.act = new QAction(filtName,plugin);
              stringXMLFilterMap.insert(filtName,fc);

              //SHOULD INITIALIZE GLOBALS FOR FILTERS

            }
          }
          else
          {
            QString err = xmlErr.statusMessage();
            qDebug("Error in XMLFile: %s - line: %d, column: %d - %s",qPrintable(xmlFile),xmlErr.line(),xmlErr.column(),qPrintable(err));
          }
        }
			}

			MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			if (iIO) 
				meshIOPlug.push_back(iIO);

			MeshDecorateInterface *iDecorator = qobject_cast<MeshDecorateInterface *>(plugin);
			if (iDecorator)
			{
				meshDecoratePlug.push_back(iDecorator);
        foreach(QAction *decoratorAction, iDecorator->actions())
				{
					editActionList.push_back(decoratorAction);
					iDecorator->initGlobalParameterSet(decoratorAction,defaultGlobal);
				}
			}

			MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
			if (iRender)
				meshRenderPlug.push_back(iRender);
			
			MeshEditInterfaceFactory *iEditFactory = qobject_cast<MeshEditInterfaceFactory *>(plugin);
			if(iEditFactory)
			{
				meshEditInterfacePlug.push_back(iEditFactory);
				foreach(QAction* editAction, iEditFactory->actions())
					editActionList.push_back(editAction);
			}
		}
	}
	knownIOFormats();
	
	/*******************************************/

	/*QString code = "";
	code += "Plugins = { };\n";
	QMap<QString,RichParameterSet> FPM = generateFilterParameterMap();
	foreach(MeshFilterInterface* mi,this->meshFilterPlug)
	{
		QString pname = mi->pluginName();
		if (pname != "")
		{
			code += "Plugins." + pname + " = { };\n";
			foreach(MeshFilterInterface::FilterIDType tt,mi->types())
			{
				QString filterName = mi->filterName(tt);
				QString filterFunction = mi->filterScriptFunctionName(tt);
				if (filterFunction != "")
				{
					ScriptAdapterGenerator gen;
					QString gencode = gen.funCodeGenerator(filterName,FPM[filterName]);
					code += "Plugins." + pname + "." + filterFunction + " = " + gencode + "\n";
				}
			}
		}
	}

	QScriptValue initFun  = env.newFunction(PluginInterfaceInit,  this);
	env.globalObject().setProperty("_initParameterSet", initFun);

	QScriptValue applyFun = env.newFunction(PluginInterfaceApply, this);
	env.globalObject().setProperty("_applyFilter", applyFun);

	env.evaluate(code);
*/
	QString code = "";
	code += "Plugins = { };\n";
	//QMap<QString,RichParameterSet> FPM = generateFilterParameterMap();
	foreach(MeshLabXMLFilterContainer mi,stringXMLFilterMap)
	{
		QString pname = mi.xmlInfo->pluginName();
		if (pname != "")
		{
			code += "Plugins." + pname + " = { };\n";
			foreach(QString filterName,mi.xmlInfo->filterNames())
			{
				QString filterFunction = mi.xmlInfo->filterAttribute(filterName,MLXMLElNames::filterScriptFunctName);
				if (filterFunction != "")
				{
					ScriptAdapterGenerator gen;
					QString gencode = gen.funCodeGenerator(filterName,*mi.xmlInfo);
					code += "Plugins." + pname + "." + filterFunction + " = " + gencode + "\n";
				}
			}
		}
	}

	//QScriptValue initFun  = env.newFunction(PluginInterfaceInit,  this);
	//env.globalObject().setProperty("_initParameterSet", initFun);

	QScriptValue applyFun = env.newFunction(PluginInterfaceApplyXML, this);
	env.globalObject().setProperty("_applyFilter", applyFun);

	QScriptValue res = env.evaluate(code);
	qDebug("Code:\n %s",qPrintable(code));
	if (env.hasUncaughtException())
		qDebug() << "JavaScript Interpreter Error: " << res.toString() << "\n";
	
}
/*
 This function create a map from filtername to dummy RichParameterSet.
 containing for each filtername the set of parameter that it uses.
 */
QMap<QString, RichParameterSet> PluginManager::generateFilterParameterMap()
{
  QMap<QString,RichParameterSet> FPM;
  MeshDocument md;
  MeshModel* mm = md.addNewMesh("","dummy",true);
  vcg::tri::Tetrahedron<CMeshO>(mm->cm);
  mm->updateDataMask(MeshModel::MM_ALL);
  QMap<QString, QAction*>::iterator ai;
  for(ai=this->actionFilterMap.begin(); ai !=this->actionFilterMap.end();++ai)
  {
      QString filterName = ai.key();//  ->filterName();
      //QAction act(filterName,NULL);
      RichParameterSet rp;
	  stringFilterMap[filterName]->initParameterSet(ai.value(),md,rp);
	  FPM[filterName]=rp;
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
    qDebug("The base dir is %s",qPrintable(baseDir.absolutePath()));
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



void PluginManager::knownIOFormats()
{
	for(int inpOut = 0;inpOut < 2;++inpOut)
	{
		QStringList* formatFilters = NULL;
		QString allKnownFormatsFilter = QObject::tr("All known formats (");
		for(QVector<MeshIOInterface*>::iterator itIOPlugin = meshIOPlug.begin();itIOPlugin != meshIOPlug.end();++itIOPlugin)
		{
			MeshIOInterface* pMeshIOPlugin = *itIOPlugin;	
			QList<MeshIOInterface::Format> format;
			QMap<QString,MeshIOInterface*>* map = NULL;
			if(inpOut== int(IMPORT))
			{
				map = &allKnowInputFormats;
				formatFilters = &inpFilters;
				format = pMeshIOPlugin->importFormats();
			}
			else
			{
				map = &allKnowOutputFormats;
				formatFilters = &outFilters;
				format = pMeshIOPlugin->exportFormats();
			}
			for(QList<MeshIOInterface::Format>::iterator itf = format.begin();itf != format.end();++itf)
			{
				MeshIOInterface::Format currentFormat = *itf;

				QString currentFilterEntry = currentFormat.description + " (";

				//a particular file format could be associated with more than one file extension
				QStringListIterator itExtension(currentFormat.extensions);
				while (itExtension.hasNext())
				{
					QString currentExtension = itExtension.next().toLower();
					if (!map->contains(currentExtension))
					{
						map->insert(currentExtension, pMeshIOPlugin);
						allKnownFormatsFilter.append(QObject::tr(" *."));
						allKnownFormatsFilter.append(currentExtension);
					}
					currentFilterEntry.append(QObject::tr(" *."));
					currentFilterEntry.append(currentExtension);
				}
				currentFilterEntry.append(')');				
				formatFilters->append(currentFilterEntry);
			}

		}
		allKnownFormatsFilter.append(')');
		formatFilters->push_front(allKnownFormatsFilter);
	}
}

void PluginManager::updateDocumentScriptBindings(MeshDocument& doc )
{ 
	//WARNING!
	//all the currentDocInterface created will be destroyed by QT when the MeshDocument destructor has been called
	currentDocInterface = new MeshDocumentScriptInterface(&doc);
	QScriptValue val = env.newQObject(currentDocInterface);
	env.globalObject().setProperty(meshDocVarName(),val); 
}
