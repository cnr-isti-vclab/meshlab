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
 Revision 1.12  2005/12/07 01:11:18  fmazzant
 bug-fix(sorry)

 Revision 1.11  2005/12/03 23:46:56  cignoni
 Adapted to the new plugin interface in a more standard way

 Revision 1.10  2005/12/03 22:50:06  cignoni
 Added copyright info

 Revision 1.9  2005/12/02 23:36:52  fmazzant
 update to the new interface of MeshIOInterface

 Revision 1.8  2005/12/02 17:41:33  fmazzant
 added support obj dialog exporter

 Revision 1.7  2005/11/30 16:26:56  cignoni
 All the modification, restructuring seen during the 30/12 lesson...

 Revision 1.6  2005/11/30 01:06:59  fmazzant
 added support Export OBJ(base)
 added comment line history
 deleted bug-fix in meshio.pro in unix{ ... }

   

 *****************************************************************************/
#ifndef EXTRAIOPLUGIN_H
#define EXTRAIOPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include "../../meshlab/meshmodel.h"
#include "../../meshlab/interfaces.h"

class ExtraMeshIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)

  
public:
	//QStringList formats() const; //obsoleto!!!!!!!
	ExtraMeshIOPlugin();
	
	bool open(QAction *format, QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(QAction *format, QString &fileName, MeshModel &m,int mask,vcg::CallBackPos *cb=0, QWidget *parent= 0);
	QList<QAction *> formats() const;
  protected:
	QList <QAction *> actionList;

};

#endif
