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
 Revision 1.2  2006/09/22 06:08:17  granzuglia
 colladaio.pro updated with support for FCollada 1.13

 Revision 1.1  2006/06/19 13:42:53  granzuglia
 collada importer

 *****************************************************************************/
#ifndef COLLADAIOPLUGIN_H
#define COLLADAIOPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include "../../meshlab/meshmodel.h"
#include "../../meshlab/interfaces.h"

class ColladaIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
 public:
	 //std::map<MeshModel*,typename vcg::tri::io::InfoDAE*> _mp;
	 std::vector<MeshModel*> _mp;

	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;
	const ActionInfo &Info(QAction *);
    const PluginInfo &Info();
	int GetExportMaskCapability(QString &format) const;

	bool open(const QString &formatName, QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, QString &fileName, MeshModel &m, const int& mask, vcg::CallBackPos *cb=0, QWidget *parent= 0);
};

#endif
