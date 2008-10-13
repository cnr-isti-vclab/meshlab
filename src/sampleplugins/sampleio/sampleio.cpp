/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
 $Log: meshio.cpp,v $
*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "sampleio.h"

#include <wrap/io_trimesh/export_smf.h>
#include <wrap/io_trimesh/import_smf.h>
#include <wrap/io_trimesh/export.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

bool SampleIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, CallBackPos *cb, QWidget *parent)
{
	int result = vcg::tri::io::ImporterSMF<CMeshO>::Open(m.cm, qPrintable(fileName));
	if (result != vcg::tri::io::ImporterSMF<CMeshO>::E_NOERROR)
	{
		return false;
	}
	return true;
}

bool SampleIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";

	int result = vcg::tri::io::ExporterSMF<CMeshO>::Save(m.cm,qPrintable(fileName),mask);
	if(result!=0)
	{
		QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter<CMeshO>::ErrorMsg(result)));
		return false;
	}
	return true;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> SampleIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("Simple Model Format", tr("SMF"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> SampleIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("Simple Model Format"	,tr("SMF"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void SampleIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
  capability=defaultBits=0;
	return;
}

Q_EXPORT_PLUGIN(SampleIOPlugin)
