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

#include <QApplication>
#include "../meshlab/meshmodel.h"
#include "../meshlab/interfaces.h"
#include "../meshlab/filterScriptDialog.h"
#include "../meshlab/plugin_support.h"
#include <vcg/complex/trimesh/update/bounding.h>

QMap<QString, QAction *> filterMap; // a map to retrieve an action from a name. Used for playing filter scripts.
std::vector<MeshIOInterface*> meshIOPlugins;
 
// Here we need a better way to find the plugins directory. 
// To be implemented:
// use the QSettings togheter with MeshLab. 
// When meshlab starts if he find the plugins write the absolute path of that directory in a persistent qsetting place. 
// Here we use that QSetting. If it is not set we remember to run meshlab first once. 
// in this way it works safely on mac too and allows the user to put the small meshlabserver binary wherever they desire (/usr/local/bin). 

void loadPlugins(FILE *fp=0)
{
	QDir pluginsDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (pluginsDir.dirName() == "debug" || pluginsDir.dirName() == "release")
		pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
	if (pluginsDir.dirName() == "MacOS") {
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
#endif
	pluginsDir.cd("../meshlab/plugins");

	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		QObject *plugin = loader.instance();
		
		if (plugin) {		
			MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter){ 
				QAction *filterAction;
				int oldSize = filterMap.size();
				foreach(filterAction, iFilter->actions())
				{
					filterMap[filterAction->text()]=filterAction;
					 if(fp) fprintf(fp, "*<b><i>%s</i></b> <br>%s\n",qPrintable(filterAction->text()), qPrintable(iFilter->filterInfo(filterAction)));
				}
				//printf("Loaded %i filtering actions form %s\n", filterMap.size() - oldSize, qPrintable(fileName));
			}
			MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			if (iIO)	meshIOPlugins.push_back(iIO);
		}
	}
	printf("Total %i filtering actions\n", filterMap.size());
	printf("Total %i io plugins\n", meshIOPlugins.size());
}

bool Open(MeshModel &mm, QString fileName)
{
	// Opening files in a transparent form (IO plugins contribution is hidden to user)
	QStringList filters;
	
	// HashTable storing all supported formats togheter with
	// the (1-based) index  of first plugin which is able to open it
	QHash<QString, int> allKnownFormats;
	
	LoadKnownFilters(meshIOPlugins, filters, allKnownFormats,IMPORT);

	QFileInfo fi(fileName);
	// this change of dir is needed for subsequent textures/materials loading
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	
	QString extension = fi.suffix();
	
	// retrieving corresponding IO plugin
	int idx = allKnownFormats[extension.toLower()];
	if (idx == 0)
	{	
    printf("Error encountered while opening file: ");
		//QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
		//QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
		return false;
	}
	MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
	
	int mask = 0;
	
	FilterParameterSet prePar;
	pCurrentIOPlugin->initPreOpenParameter(extension, fileName,prePar);
	
	if (!pCurrentIOPlugin->open(extension, fileName, mm ,mask,prePar))
  {
    printf("Failed loading\n");
    return false;
  }
	vcg::tri::UpdateBounding<CMeshO>::Box(mm.cm);

  return true;
}
/*
 meshlab -i mesh -o mesh -f filtro


*/

bool Save(MeshModel *mm, QString fileName)
{
	// Opening files in a transparent form (IO plugins contribution is hidden to user)
	QStringList filters;
	
	// HashTable storing all supported formats togheter with
	// the (1-based) index  of first plugin which is able to open it
	QHash<QString, int> allKnownFormats;
	
	LoadKnownFilters(meshIOPlugins, filters, allKnownFormats,IMPORT);

	QFileInfo fi(fileName);
	// this change of dir is needed for subsequent textures/materials loading
	QDir::setCurrent(fi.absoluteDir().absolutePath());
	
	QString extension = fi.suffix();
	
	// retrieving corresponding IO plugin
	int idx = allKnownFormats[extension.toLower()];
	if (idx == 0)
	{	
    printf("Error encountered while opening file: ");
		//QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
		//QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
		return false;
	}
	MeshIOInterface* pCurrentIOPlugin = meshIOPlugins[idx-1];
	
	int mask = 0;
	
	// optional saving parameters (like ascii/binary encoding)
	FilterParameterSet savePar;
	pCurrentIOPlugin->initSaveParameter(extension, *mm, savePar);
	
	if (!pCurrentIOPlugin->save(extension, fileName, *mm ,mask, savePar))
  {
    printf("Failed saving\n");
    return false;
  }
  return true;
}


bool Script(MeshDocument &meshDocument, QString scriptfile){
	
	MeshModel &mm = *meshDocument.mm();
	
	FilterScript scriptPtr;
	
	//Open/Load FilterScript 
	
	if (scriptfile.isEmpty())	return false;
	scriptPtr.open(scriptfile);
	
	FilterScript::iterator ii;
	for(ii = scriptPtr.actionList.begin();ii!= scriptPtr.actionList.end();++ii){
		FilterParameterSet &par = (*ii).second;
		QString &name = (*ii).first;
		printf("filter: %s\n",qPrintable((*ii).first));
		
		QAction *action = filterMap[ (*ii).first];
		MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
		iFilter->setLog(NULL);
		int req = iFilter->getRequirements(action);
		mm.updateDataMask(req);

		
		
		//make sure the PARMESH parameters are initialized
		FilterParameterSet &parameterSet = (*ii).second;
		for(int i = 0; i < parameterSet.paramList.size(); i++)
		{	
			//get a modifieable reference
			FilterParameter &parameter = parameterSet.paramList[i];
				
			//if this is a mesh paramter and the index is valid
			if(parameter.fieldType == FilterParameter::PARMESH)
			{  
				if(	parameter.fieldVal.toInt() < meshDocument.size() && 
					parameter.fieldVal.toInt() >= 0  )
				{
					parameter.pointerVal = meshDocument.getMesh(parameter.fieldVal.toInt());					
				} else
				{
					printf("Meshes loaded: %i, meshes asked for: %i \n", meshDocument.size(), parameter.fieldVal.toInt() );
					printf("One of the filters in the script needs more meshes than you have loaded.\n");
					exit(-1);
				}
			}
		}
		
		bool ret = iFilter->applyFilter( action, meshDocument, (*ii).second, NULL);
		//iFilter->applyFilter( action, mm, (*ii).second, QCallBack );
		//GLA()->log.Logf(GLLogStream::Info,"Re-Applied filter %s",qPrintable((*ii).first));
		
		if(!ret)
		{
			printf("Problem with filter: %s\n",qPrintable((*ii).first));
			return false;
		}
	}
	
	return true;
}

void Usage()
{
	printf("\nUsage:\n"
		"    meshlabserver arg1 arg2 ...  \n"
		"where args can be: \n"
		" -i [filename...]  mesh(s) that has to be loaded\n" 
		" -o [filename...]  mesh(s) where to write the result(s)\n"
		" -s filename		    script to be applied\n"
		" -d filename       dump on a text file a list of all the filtering fucntion\n" 		 
		"\nNotes:\n\n"
		"There can be multiple meshes loaded and the order they are listed matters because \n"
		"filters that use meshes as parameters choose the mesh based on the order.\n"
		"The number of output meshes must be either one or equal to the number of input meshes.\n"
		"If the number of output meshes is one then only the first mesh in the input list is saved.\n"
		"The format of the output mesh is guessed by the used extension.\n"
		"Script is optional and must be in the format saved by MeshLab.\n"
		);
	
	exit(-1);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);  
	MeshDocument meshDocument;
	QStringList meshNamesIn, meshNamesOut;
	QString scriptName;
	FILE *filterFP=0;
	if(argc < 3) Usage();
	int i = 1;
	while(i < argc)
	{
		if(argv[i][0] != '-') Usage();
		switch(argv[i][1])
		{
			case 'i' :  
				while( ((i+1) < argc) && argv[i+1][0] != '-')
				{
					meshNamesIn << argv[i+1];
					printf("Input mesh  %s\n", qPrintable(meshNamesIn.last() ));
					i++;
				}
				i++; 
				break; 
			case 'o' :  
				while( ((i+1) < argc) && argv[i+1][0] != '-')
				{
					meshNamesOut << argv[i+1];
					printf("output mesh  %s\n", qPrintable(meshNamesOut.last()));
					i++;
				}
				i++; 
				break; 
			case 's' :  
				if( argc <= i+1 ) {
					printf("Missing script name\n");  
					exit(-1);
				}
				scriptName = argv[i+1];
				printf("script %s\n", qPrintable(scriptName));
				i += 2;
				break; 
			case 'd' :
				if( argc <= i+1 ) {
					printf("Missing dump name\n");  
					exit(-1);
				}
				filterFP=fopen(argv[i+1],"w");
			 i+=2;
			 break;
											 
		}
	}
	
	printf("Loading Plugins:\n");
	loadPlugins(filterFP);
	
	
	if(meshNamesIn.isEmpty()) {
		printf("No input mesh\n"); exit(-1);
	} else
	{
		for(int i = 0; i < meshNamesIn.size(); i++)
		{
			MeshModel *mm = new MeshModel( meshNamesIn.at(i).toStdString().c_str() );
			Open(*mm, meshNamesIn.at(i));
			printf("Mesh %s loaded has %i vn %i fn\n", mm->fileName.c_str(), mm->cm.vn, mm->cm.fn);

			//now add it to the document
			meshDocument.addNewMesh(mm->fileName.c_str(), mm);
		}
		//the first mesh is the one the script is applied to
		meshDocument.setCurrentMesh(0);
	}
				
	if(!scriptName.isEmpty())
	{		
		printf("Apply FilterScript:\n");
		bool returnValue = Script(meshDocument, scriptName);
		if(!returnValue)
		{
			printf("Failed to apply FilterScript\n");
			exit(-1);
		}
	} else 
		printf("No Script to apply.");
	
	//if there is not one name or an equal number of in and out names then exit 
	if(meshNamesOut.isEmpty() )
		printf("No output mesh names given."); 
	else if(meshNamesOut.size() != 1 && meshNamesOut.size() != meshNamesIn.size() ) {
		printf("Wrong number of output mesh names given\n"); 
		exit(-1);
	} else 
	{
		for(int i = 0; i < meshNamesOut.size(); i++)
		{
			Save(meshDocument.getMesh(i), meshNamesOut.at(i));
			printf("Mesh %s saved with: %i vn %i fn\n", qPrintable(meshNamesOut.at(i)), meshDocument.mm()->cm.vn, meshDocument.mm()->cm.fn);
		}
	}		
}

