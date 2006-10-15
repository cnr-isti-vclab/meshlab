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
#include "../meshlab/plugin_support.h"

QMap<QString, QAction *> filterMap; // a map to retrieve an action from a name. Used for playing filter scripts.
std::vector<MeshIOInterface*> meshIOPlugins;
 

void loadPlugins()
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
			//MeshColorizeInterface *iColor = qobject_cast<MeshColorizeInterface *>(plugin);
						
		  MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(plugin);
			if (iFilter)
      { 
        QAction *filterAction;
        foreach(filterAction, iFilter->actions())
          filterMap[filterAction->text()]=filterAction;
        printf("Loaded %i filtering actions form %s\n",filterMap.size(),qPrintable(fileName));
       }
		  MeshIOInterface *iIO = qobject_cast<MeshIOInterface *>(plugin);
			if (iIO)	meshIOPlugins.push_back(iIO);
      

//	    pluginfileNames += fileName;
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
	if (!pCurrentIOPlugin->open(extension, fileName, mm ,mask,0,0/*gla*/))
  {
    printf("Failed loading\n");
    return false;
  }
  return true;
}
/*
 sintassi 
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
	if (!pCurrentIOPlugin->save(extension, fileName, mm ,mask,0,0/*gla*/))
  {
    printf("Failed saving\n");
  }
}


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);  
  loadPlugins();
  MeshModel mm;
	if(argc>1)	
  {
    Open(mm,argv[1]);
    printf("Mesh loaded is %i vn %i fn\n",mm.cm.vn,mm.cm.fn);
    Save(mm,argv[2]);    
  }
  else exit(-1);

	//return app.exec();
}

