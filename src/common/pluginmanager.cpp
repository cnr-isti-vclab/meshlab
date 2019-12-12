#include "pluginmanager.h"
#include <QObject>
#include <QtScript/QtScript>
#include <vcg/complex/algorithms/create/platonic.h>

#include "scriptinterface.h"
#include "mlexception.h"


static QString DLLExtension() {
#if defined(Q_OS_WIN)
  return QString("dll");
#elif defined(Q_OS_MAC)
  return QString("dylib");
#else
  return QString("so");
#endif
  assert(0 && "Unknown Operative System. Please Define the appropriate dynamic library extension");
  return QString();
}

static QString DLLFileNamePreamble() {
#if defined(Q_OS_WIN)
  return QString("");
#elif defined(Q_OS_MAC)
  return QString("lib");
#else
  return QString("lib");
#endif
  assert(0 && "Unknown Operative System. Please Define the appropriate dynamic library preamble");
  return QString();
}

PluginManager::PluginManager()
:/*currentDocInterface(NULL),*/scriptplugcode()
{
  //pluginsDir=QDir(getPluginDirPath());
  // without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
  //qApp->addLibraryPath(getPluginDirPath());
  //qApp->addLibraryPath(getBaseDirPath());
}

PluginManager::~PluginManager()
{
	meshIOPlug.clear();
	meshFilterPlug.clear();
	meshRenderPlug.clear();
	meshDecoratePlug.clear();
	for (MeshCommonInterface* plugin : ownerPlug)
		delete plugin;
	ownerPlug.clear();

	for (int ii = 0; ii < meshEditInterfacePlug.size(); ++ii)
		delete meshEditInterfacePlug[ii];
	meshEditInterfacePlug.clear();

	for (int ii = 0; ii < meshlabXMLFilterPlug.size(); ++ii)
		delete meshlabXMLFilterPlug[ii];
	meshlabXMLFilterPlug.clear();

	for (int ii = 0; ii < xmlpluginfo.size(); ++ii)
		MLXMLPluginInfo::destroyXMLPluginInfo(xmlpluginfo[ii]);
	xmlpluginfo.clear();
}



void PluginManager::loadPlugins(RichParameterSet& defaultGlobal)
{
	pluginsDir = QDir(getDefaultPluginDirPath());
	// without adding the correct library path in the mac the loading of jpg (done via qt plugins) fails
	qApp->addLibraryPath(getDefaultPluginDirPath());
	qApp->addLibraryPath(getBaseDirPath());
	QStringList pluginfilters;

	pluginfilters << QString("*." + DLLExtension());
	pluginfilters << "*.xml";

	//only the file with extension pluginfilters will be listed by function entryList()
	pluginsDir.setNameFilters(pluginfilters);

	qDebug("Current Plugins Dir is: %s ", qUtf8Printable(pluginsDir.absolutePath()));
	scriptplugcode = "";
	ScriptAdapterGenerator gen;
	scriptplugcode += gen.mergeOptParamsCodeGenerator() + "\n";
	scriptplugcode += MLXMLUtilityFunctions::pluginsNameSpace() + " = { };\n";
	foreach(QString fileName, pluginsDir.entryList(QDir::Files))
	{
		//      qDebug() << fileName;
		QString absfilepath = pluginsDir.absoluteFilePath(fileName);
		QFileInfo fin(absfilepath);
		if (fin.suffix() == "xml")
		{
			try
			{
				MLXMLPluginInfo* pluginfo = loadXMLPlugin(fileName);
                if (pluginfo == NULL)
                {
                    QString err;
                    err = "WARNING! " + fileName + " xml-based plugin was not found!";
                    throw MLException(err);
                }
//				//WARNING!!!! Why this piece of code is not inside the loadXMLPlugin function?!?!?!?!?!?!
//				//Because one day, hopefully, the RichParameterSet class (the old parameters system) will disappear and we don't want to spread it inside the XML functions

//				if (pluginfo != NULL)
//				{
//					QString pluginprogname = pluginfo->pluginScriptName();
//					foreach(QString filtername, pluginfo->filterNames())
//					{
//						QString filterprogname = pluginfo->filterAttribute(filtername, MLXMLElNames::filterScriptFunctName);
//						foreach(MLXMLPluginInfo::XMLMap params, pluginfo->filterParametersExtendedInfo(filtername))
//						{
//							if (params[MLXMLElNames::paramIsPersistent] == QString("true"))
//							{
//                                //RichParameter* rp = NULL;
//								QString completepreamble = MLXMLUtilityFunctions::completeFilterProgrammingName(MLXMLUtilityFunctions::pluginsNameSpace(), pluginprogname, filterprogname);
//								/*RichParameterAdapter::create(completepreamble,params, &rp);
//								if (rp != NULL)
//								defaultGlobal.addParam(rp);*/
//							}
//						}
//					}
//				}
			}
			catch (MeshLabXMLParsingException& e)
			{
				qDebug() << e.what();
			}
    }
		else
		{
			QPluginLoader loader(absfilepath);
			QObject *plugin = loader.instance();
			if (plugin)
			{
				pluginsLoaded.push_back(fileName);
				MeshCommonInterface *iCommon = nullptr;
				MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
				if (iFilter)
				{
					iCommon = iFilter;
					meshFilterPlug.push_back(iFilter);
					foreach(QAction *filterAction, iFilter->actions())
					{
						filterAction->setData(QVariant(fileName));
						actionFilterMap.insert(filterAction->text(), filterAction);
						stringFilterMap.insert(filterAction->text(), iFilter);
						iFilter->initGlobalParameterSet(filterAction, defaultGlobal);
                        if(iFilter->getClass(filterAction)==MeshFilterInterface::Generic)     
                          throw MLException("Missing class for "        +fileName+filterAction->text());
                        if(iFilter->getRequirements(filterAction) == int(MeshModel::MM_UNKNOWN))   
                          throw MLException("Missing requirements for " +fileName+filterAction->text());
                        if(iFilter->getPreConditions(filterAction) == int(MeshModel::MM_UNKNOWN))  
                          throw MLException("Missing preconditions for "+fileName+filterAction->text());
                        if(iFilter->postCondition(filterAction) == int(MeshModel::MM_UNKNOWN ))    
                          throw MLException("Missing postcondition for "+fileName+filterAction->text());
                        if(iFilter->filterArity(filterAction) == MeshFilterInterface::UNKNOWN_ARITY )    
                          throw MLException("Missing Arity for "        +fileName+filterAction->text());
					}
				}
				MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
				if (iIO)
				{
					iCommon = iIO;
					meshIOPlug.push_back(iIO);
					iIO->initGlobalParameterSet(NULL, defaultGlobal);
				}

				MeshDecorateInterface *iDecorator = qobject_cast<MeshDecorateInterface *>(plugin);
				if (iDecorator)
				{
					iCommon = iDecorator;
					meshDecoratePlug.push_back(iDecorator);
					foreach(QAction *decoratorAction, iDecorator->actions())
					{
						decoratorActionList.push_back(decoratorAction);
						iDecorator->initGlobalParameterSet(decoratorAction, defaultGlobal);
					}
				}

				MeshRenderInterface *iRender = qobject_cast<MeshRenderInterface *>(plugin);
				if (iRender)
				{
					iCommon = iRender;
					meshRenderPlug.push_back(iRender);
				}

				MeshEditInterfaceFactory *iEditFactory = qobject_cast<MeshEditInterfaceFactory *>(plugin);
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
	}
	knownIOFormats();
}

// Search among all the decorator plugins the one that contains a decoration with the given name
MeshDecorateInterface *PluginManager::getDecoratorInterfaceByName(const QString& name)
{
  foreach(MeshDecorateInterface *tt, this->meshDecoratePlugins())
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
QMap<QString, RichParameterSet> PluginManager::generateFilterParameterMap()
{
	QMap<QString, RichParameterSet> FPM;
	MeshDocument md;
	MeshModel* mm = md.addNewMesh("", "dummy", true);
	vcg::tri::Tetrahedron<CMeshO>(mm->cm);
	mm->updateDataMask(MeshModel::MM_ALL);
	QMap<QString, QAction*>::iterator ai;
	for (ai = this->actionFilterMap.begin(); ai != this->actionFilterMap.end(); ++ai)
	{
		QString filterName = ai.key();//  ->filterName();
									  //QAction act(filterName,NULL);
		RichParameterSet rp;
		stringFilterMap[filterName]->initParameterSet(ai.value(), md, rp);
		FPM[filterName] = rp;
	}
	return FPM;
}

QString PluginManager::osDependentFileBaseName(const QString &plname)
{
	return (DLLFileNamePreamble() + plname + "." + DLLExtension());
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
	if (!pluginsDir.exists("plugins"))
		//QMessageBox::warning(0,"Meshlab Initialization","Serious error. Unable to find the plugins directory.");
		qDebug("Meshlab Initialization: Serious error. Unable to find the plugins directory.");
	pluginsDir.cd("plugins");
	return pluginsDir.absolutePath();
}



void PluginManager::knownIOFormats()
{
	for (int inpOut = 0; inpOut < 2; ++inpOut)
	{
		QStringList* formatFilters = NULL;
		QString allKnownFormatsFilter = QObject::tr("All known formats (");
		for (QVector<MeshIOInterface*>::iterator itIOPlugin = meshIOPlug.begin(); itIOPlugin != meshIOPlug.end(); ++itIOPlugin)
		{
			MeshIOInterface* pMeshIOPlugin = *itIOPlugin;
			QList<MeshIOInterface::Format> format;
			QMap<QString, MeshIOInterface*>* map = NULL;
			if (inpOut == int(IMPORT))
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
			for (QList<MeshIOInterface::Format>::iterator itf = format.begin(); itf != format.end(); ++itf)
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
		if (formatFilters != NULL)
			formatFilters->push_front(allKnownFormatsFilter);
	}
}

//void PluginManager::updateDocumentScriptBindings(MeshDocument& doc )
//{
//	//WARNING!
//	//all the currentDocInterface created will be destroyed by QT when the MeshDocument destructor has been called
//	currentDocInterface = new MeshDocumentSI(&doc);
//	QScriptValue val = env.newQObject(currentDocInterface);
//	env.globalObject().setProperty(ScriptAdapterGenerator::meshDocVarName(),val);
//}

QString PluginManager::pluginsCode() const
{
  return scriptplugcode;
}

MLXMLPluginInfo* PluginManager::loadXMLPlugin(const QString& fileName)
{
	ScriptAdapterGenerator gen;
	QString absfilepath = pluginsDir.absoluteFilePath(fileName);
	QFileInfo fin(absfilepath);
	if (fin.suffix() == "xml")
	{

		QString dllfile = osDependentFileBaseName(fin.completeBaseName());

		MeshLabXMLFilterContainer fc;
		//fc.filterInterface = NULL;
		XMLMessageHandler xmlErr;
		MLXMLPluginInfo* pluginfo = MLXMLPluginInfo::createXMLPluginInfo(absfilepath, MLXMLUtilityFunctions::xmlSchemaFile(), xmlErr);
		if (pluginfo != NULL)
		{
			xmlpluginfo << pluginfo;
			fc.xmlInfo = xmlpluginfo[xmlpluginfo.size() - 1];
			//      QStringList fn = fc.xmlInfo->filterNames();
			QObject* par = NULL;
			if (pluginsDir.exists(dllfile))
			{
				QPluginLoader loader(fin.absoluteDir().absolutePath() + "/" + dllfile);
				QObject* plugin = loader.instance();
				MeshLabFilterInterface* iXMLfilter = qobject_cast<MeshLabFilterInterface *>(plugin);
				if (iXMLfilter != NULL)
				{
					meshlabXMLFilterPlug << iXMLfilter;
					fc.filterInterface = meshlabXMLFilterPlug[meshlabXMLFilterPlug.size() - 1];
					par = plugin;
				}
			}
			else
			{
				// we have loaded an xml without the corresponding dll. Let's check that it is a pure javascript plugin
				bool foundANonJavaScriptFilter = false;
				foreach(QString filterName, pluginfo->filterNames())
				{
					if (pluginfo->filterElement(filterName, MLXMLElNames::filterJSCodeTag).isEmpty())
						foundANonJavaScriptFilter = true;
				}
				if (foundANonJavaScriptFilter)
				{
					throw(MeshLabXMLParsingException("We are trying to load a xml file that does not correspond to any dll or javascript code; please delete all the spurious xml files"));
				}
				par = new QObject();
			}
			QString pname = pluginfo->pluginScriptName();
			if (pname != "")
			{
				QString plugnamespace = MLXMLUtilityFunctions::pluginsNameSpace() + "." + pname;
				//pluginnamespaces << plugnamespace;
				scriptplugcode += plugnamespace + " = { };\n";
				QStringList filters = pluginfo->filterNames();
				foreach(QString filter, filters)
				{
					MLXMLPluginInfo::XMLMapList paramsinfo = pluginfo->filterParametersExtendedInfo(filter);
					QString completename = plugnamespace;
					fc.act = new QAction(filter, par);
					fc.act->setData(QVariant(dllfile));
					stringXMLFilterMap.insert(filter, fc);
					QString filterFunction = pluginfo->filterScriptCode(filter);
					if (filterFunction == "")
						filterFunction = gen.funCodeGenerator(filter, *pluginfo);
					else
						filterFunction = "{" + filterFunction + "};";
					QString jname = pluginfo->filterAttribute(filter, MLXMLElNames::filterScriptFunctName);
					completename += "." + jname;
					//filterscriptnames << completename;
					scriptplugcode += completename + " = " + filterFunction + "\n";
					completename += ".";
					//for (MLXMLPluginInfo::XMLMap& paraminfo : paramsinfo)
					//{
					//	if (paraminfo[MLXMLElNames::paramIsPersistent] == "true")
					//	{	
					//		QString fullvarname = completename + paraminfo[MLXMLElNames::paramName];
					//		//scriptplugcode += namespaceVariableAssignment(fullvarname, paraminfo[MLXMLElNames::paramDefExpr]);
					//		paraminfo[MLXMLElNames::paramDefExpr] = fullvarname;
					//	}
					//}

					completename += "(" + gen.parNames(filter, *pluginfo) + ")";
					LibraryElementInfo li;
					li.completename = completename;
					li.help = pluginfo->filterHelp(filter);
					libinfolist << li;
				}
			}
			return pluginfo;
		}
		else
		{
			QString err = xmlErr.statusMessage();
			qDebug("Error in XMLFile: %s - line: %d, column: %d - %s", qUtf8Printable(fileName), xmlErr.line(), xmlErr.column(), qUtf8Printable(err));
		}
	}
	return nullptr;
}

//MLXMLPluginInfo* PluginManager::getXMLPluginInfo( const QString& plugname )
//{
//	for(int ii = 0;ii < xmlpluginfo.size();++ii)
//		if (xmlpluginfo[ii]->pluginFilePath() != plugname)
//			return xmlpluginfo[ii];
//	return NULL;
//}

void PluginManager::deleteXMLPlugin(const QString& plugscriptname)
{
	bool found = false;
	int ii = 0;
	while ((ii < xmlpluginfo.size()) && !found)
	{
		if (xmlpluginfo[ii]->pluginScriptName() == plugscriptname)
			found = true;
		else
			++ii;
	}
	if (found)
	{
		QStringList removefilters;
		QSet<MeshLabFilterInterface*> tobedeleted;
		for (QMap<QString, MeshLabXMLFilterContainer>::iterator it = stringXMLFilterMap.begin(); it != stringXMLFilterMap.end();)
		{
			if (xmlpluginfo[ii] == it.value().xmlInfo)
			{
				QString rem = it.key();
				if (it.value().filterInterface != NULL)
					tobedeleted.insert(it.value().filterInterface);
				++it;
				stringXMLFilterMap.remove(rem);
			}
			else
				++it;
		}
		MLXMLPluginInfo* tmp = xmlpluginfo[ii];
		xmlpluginfo.remove(ii);
		MLXMLPluginInfo::destroyXMLPluginInfo(tmp);
		for (QSet<MeshLabFilterInterface*>::iterator it = tobedeleted.begin(); it != tobedeleted.end(); ++it)
		{
			int ii = meshlabXMLfilterPlugins().indexOf(*it);
			MeshLabFilterInterface* fi = meshlabXMLfilterPlugins()[ii];
			meshlabXMLfilterPlugins().remove(ii);
			delete fi;
		}
	}
}

QString PluginManager::osIndependentPluginName(const QString& plname)
{
	QFileInfo fi(plname);
	QString res = fi.baseName();
	QString pref = DLLFileNamePreamble();
	return res.remove(0, pref.size());
}

