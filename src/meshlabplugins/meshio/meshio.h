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
 Revision 1.23  2006/11/29 00:59:19  cignoni
 Cleaned plugins interface; changed useless help class into a plain string

 Revision 1.22  2006/03/07 13:30:24  cignoni
 undoed wrong removal...

 Revision 1.20  2006/02/16 19:29:20  fmazzant
 transfer of Export_3ds.h, Export_obj.h, Io_3ds_obj_material.h from Meshlab to vcg

 Revision 1.19  2006/02/15 23:09:06  fmazzant
 added the part of MeshIO credits

 Revision 1.18  2006/01/19 15:59:00  fmazzant
 moved savemaskexporter to mainwindows

 Revision 1.17  2006/01/19 09:36:28  fmazzant
 cleaned up history log

 Revision 1.16  2006/01/17 13:47:45  fmazzant
 update interface meshio : formats -> importFormats() & exportFormts

 Revision 1.15  2005/12/15 01:20:28  buzzelli
 formats method adapted in order to fit with recent changes in MeshIOInterface

 Revision 1.14  2005/12/09 00:34:31  buzzelli
 io importing mechanism adapted in order to be fully transparent towards the user

 Revision 1.13  2005/12/07 07:52:25  fmazzant
 export obj generic(base)

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
	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;
	const PluginInfo &Info();
	int GetExportMaskCapability(QString &format) const;

	bool open(const QString &formatName, QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, QString &fileName, MeshModel &m, const int& mask, vcg::CallBackPos *cb=0, QWidget *parent= 0);
};

#endif
