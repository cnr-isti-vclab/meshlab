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
/****************************************************************************
  History
$Log$
Revision 1.8  2008/02/10 15:22:57  cignoni
slightly revised the way of parameter passing

Revision 1.7  2008/01/22 14:18:27  sherholz
Added support for .mlx filter scripts

Revision 1.6  2007/04/16 10:02:15  cignoni
Again on the cm() ->cm issue

Revision 1.5  2007/03/20 16:22:35  cignoni
Big small change in accessing mesh interface. First step toward layers

Revision 1.4  2006/12/05 15:19:18  cignoni
added a missing return

Revision 1.3  2006/10/15 20:31:38  cignoni
Added saving of the mesh

Revision 1.2  2006/06/27 08:08:12  cignoni
First working version. now it loads all the needed plugins and a mesh!

Revision 1.1  2006/06/19 15:11:50  cignoni
Initial Rel

Revision 1.4  2006/02/01 12:45:29  glvertex
- Solved openig bug when running by command line

Revision 1.3  2005/12/01 02:24:50  davide_portelli
Mainwindow Splitted----->[ mainwindow_Init.cpp ]&&[ mainwindow_RunTime.cpp ]

Revision 1.2  2005/11/21 12:12:54  cignoni
Added copyright info

****************************************************************************/

#include <QApplication>
#include "../meshlab/meshmodel.h"
#include "../meshlab/interfaces.h"
#include "../meshlab/filterScriptDialog.h"
#include "../meshlab/plugin_support.h"

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
	        foreach(filterAction, iFilter->actions())
						{
							filterMap[filterAction->text()]=filterAction;
							//if(fp)
							 printf( "*'''%s''': %s\n",qPrintable(filterAction->text()), qPrintable(iFilter->filterInfo(filterAction)));
						}
	        printf("Loaded %i filtering actions form %s\n",filterMap.size(),qPrintable(fileName));
	       }
		  MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
		  if (iIO)	meshIOPlugins.push_back(iIO);
		}
	}
   printf("Total %i filtering actions\n",filterMap.size());
   printf("Total %i io plugins\n",meshIOPlugins.size());
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
  return true;
}
/*
 meshlab -i mesh -o mesh -f filtro


*/

bool Save(MeshModel &mm, QString fileName)
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
	FilterParameterSet savingPar;
	if (!pCurrentIOPlugin->save(extension, fileName, mm ,mask, savingPar,0,0/*gla*/))
  {
    printf("Failed saving\n");
    return false;
  }
  return true;
}


bool Script(MeshModel& mm, QString scriptfile){
	
	FilterScript scriptPtr;
	
	//Open/Load FilterScript 
	
	if (scriptfile.isEmpty())	return false;
	scriptPtr.open(scriptfile);
	
	FilterScript::iterator ii;
	for(ii = scriptPtr.actionList.begin();ii!= scriptPtr.actionList.end();++ii){
		FilterParameterSet &par=(*ii).second;
		QString &name = (*ii).first;
		printf("filter: %s\n",qPrintable((*ii).first));
		
		QAction *action = filterMap[ (*ii).first];
		MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
		iFilter->setLog(NULL);
		int req=iFilter->getRequirements(action);
		mm.updateDataMask(req);

		bool ret = iFilter->applyFilter( action, mm, (*ii).second, NULL);

		//iFilter->applyFilter( action, mm, (*ii).second, QCallBack );
		//GLA()->log.Logf(GLLogStream::Info,"Re-Applied filter %s",qPrintable((*ii).first));

	}
	
}

void Usage()
{
  printf(
		"\nUsage:\n"
		"         meshlabserver arg1 arg2 ...  \n"
		"where args can be: \n"
		" -i filename  mesh that has to be loaded\n" 
		" -o filename  mesh where to write the result\n"
	  " -s filename  script to be applied\n"
		"\nNotes:\n\n"
		"There must be exactly one input mesh and at most one output mesh.\n"
		"Script is optional and must be in the format saved by MeshLab.\n"
		"The format of the output mesh is guessed by the used extension.\n"
		);
	exit(-1);

}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);  
	loadPlugins();
	MeshModel mm;
	QString meshNameIn,meshNameOut,scriptName;
	if(argc<3) Usage();
	int i=1;
	while(i<argc)
	{
		if(argv[i][0] !='-') Usage();
		switch(argv[i][1])
			{
        case 'i' :  meshNameIn= argv[i+1];printf("Input mesh  %s\n",qPrintable(meshNameIn)); i+=2; break; 
        case 'o' :  meshNameOut=argv[i+1];printf("output mesh  %s\n",qPrintable(meshNameOut));i+=2; break; 
        case 's' :  scriptName= argv[i+1];printf("script %s\n",qPrintable(scriptName));i+=2; break; 
			}
	}
		if(meshNameIn.isEmpty()) {
			printf("No input mesh\n"); exit(-1);
		}
		if(meshNameOut.isEmpty()) {
			printf("No output mesh\n"); exit(-1);
		}
		
		Open(mm,meshNameIn);
		printf("Mesh loaded is %i vn %i fn\n",mm.cm.vn,mm.cm.fn);
		
		if(!scriptName.isEmpty())
		{
			printf("Apply FilterScript:\n");
			Script(mm,scriptName);
		}
		
		Save(mm,meshNameOut);    
		printf("Mesh saved is %i vn %i fn\n",mm.cm.vn,mm.cm.fn);
}

