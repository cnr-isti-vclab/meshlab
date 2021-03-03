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

#include <common/plugins/interfaces/iomesh_plugin.h>
#include <common/ml_document/mesh_model.h>
#include <wrap/io_trimesh/export_u3d.h>
#include <wrap/io_trimesh/export_idtf.h>

class U3DIOPlugin : public QObject, public IOMeshPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(IOMESH_PLUGIN_IID)
	Q_INTERFACES(IOMeshPlugin)

public:
	QString pluginName() const;
	QList<FileFormat> importFormats() const;
	QList<FileFormat> exportFormats() const;

	U3DIOPlugin();

	virtual void exportMaskCapability(const QString &format, int &capability, int &defaultBits) const;

	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList &, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterList &, vcg::CallBackPos *cb=0, QWidget *parent= 0);

	void initSaveParameter(const QString &format, MeshModel &/*m*/, RichParameterList &par);

private:
	void saveParameters(const RichParameterList &par);

	void saveLatex(const QString& file,const vcg::tri::io::u3dparametersclasses::Movie15Parameters<CMeshO>& mov_par);
	void substituteChar(QString& st, const QChar& ch_remove, const QString& sub);

	vcg::tri::io::u3dparametersclasses::Movie15Parameters<CMeshO> _param;
};

#endif
