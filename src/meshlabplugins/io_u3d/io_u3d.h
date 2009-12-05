/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef U3D_IO_H
#define U3D_IO_H

#include <QObject>
#include <QList>
#include <QString>

#include <common/interfaces.h>
#include <wrap/io_trimesh/export_u3d.h>
#include <wrap/io_trimesh/export_idtf.h>

class U3DIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
 public:
	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;

	U3DIOPlugin();

	virtual void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const;

	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb=0, QWidget *parent= 0);

	void initSaveParameter(const QString &format, MeshModel &/*m*/, RichParameterSet &par);

	private:
	QString computePluginsPath();


	void saveParameters(const RichParameterSet &par);
	vcg::tri::io::u3dparametersclasses::Movie15Parameters _param;
	
};

#endif
