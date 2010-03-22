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
Revision 1.8  2008/03/08 17:22:57  cignoni
added plugins path settings for macs

Revision 1.7  2008/02/24 18:08:50  cignoni
added -h and --help standard options

Revision 1.6  2007/03/27 12:20:09  cignoni
Revamped logging iterface, changed function names in automatic parameters, better selection handling

Revision 1.5  2006/11/08 01:04:48  cignoni
First version with http communications

Revision 1.4  2006/02/01 12:45:29  glvertex
- Solved openig bug when running by command line

Revision 1.3  2005/12/01 02:24:50  davide_portelli
Mainwindow Splitted----->[ mainwindow_Init.cpp ]&&[ mainwindow_RunTime.cpp ]

Revision 1.2  2005/11/21 12:12:54  cignoni
Added copyright info

****************************************************************************/

#include <QApplication>
#include <QMessageBox>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
#if defined(Q_OS_MAC)
	QDir dir(QApplication::applicationDirPath());
	dir.cdUp();
	dir.cd("plugins");
	QApplication::setLibraryPaths(QStringList(dir.absolutePath()));
#endif
	QCoreApplication::setOrganizationName("VCG");
  QCoreApplication::setApplicationName("MeshLab");
	
		if(argc>1)	
		{
			QString helpOpt1="-h";
			QString helpOpt2="--help";
			if( (helpOpt1==argv[1]) || (helpOpt2==argv[1]) )
			{
				printf("\n\n"
							 "    MeshLab: an open source mesh processing system\n"
							 "          Paolo Cignoni (and many many others) \n"
							 "              Visual Computing Lab\n"
							 "                  ISTI - CNR \n\n"
							 "usage:\n\n"
							 "    meshlab [meshfile] \n\n"
							 "Look at --- http://meshlab.sourceforge.net/wiki --- for a longer documentation\n\n"
							 );
			exit(-1);
			}
		}
	
  MainWindow window;
  window.showMaximized();

  // This filter is installed to intercept the open events sent directly by the Operative System. 
	FileOpenEater *filterObj=new FileOpenEater();
  filterObj->mainWindow=&window;
	app.installEventFilter(filterObj);
	app.processEvents();
	if(argc>1)	
	{
		QString helpOpt1="-h";
		QString helpOpt2="--help";
		if( (helpOpt1==argv[1]) || (helpOpt2==argv[1]) )
		printf(
			"usage:\n"
			"meshlab <meshfile>\n"
			"Look at http://meshlab.sourceforge.net/wiki\n"
			"for a longer documentation\n"
		);
		window.open(argv[1]);
	}
	else 	if(filterObj->noEvent) window.open();
 
	return app.exec();
}
