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

 $Log$
 Revision 1.10  2006/11/27 06:57:19  cignoni
 Wrong way of using the __DATE__ preprocessor symbol

 Revision 1.9  2006/11/09 02:20:36  granzuglia
 cleaner version

 Revision 1.8  2006/11/08 17:57:20  granzuglia
 texture loading

 Revision 1.7  2006/11/07 17:26:01  cignoni
 small gcc compiling issues

 Revision 1.6  2006/11/07 09:26:10  granzuglia
 fixed bug: added mask parameter

 Revision 1.5  2006/11/06 05:30:32  granzuglia
 great changes

 Revision 1.4  2006/11/05 19:21:09  granzuglia
 colladaio updated

 Revision 1.3  2006/09/22 06:08:17  granzuglia
 colladaio.pro updated with support for FCollada 1.13

 Revision 1.2  2006/07/07 06:57:04  granzuglia
 added the save function

 Revision 1.1  2006/06/19 13:42:53  granzuglia
 collada importer

*****************************************************************************/

#include <algorithm>

#include <Qt>
#include <QtGui>

#include "colladaio.h"

#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>


#include <wrap/io_trimesh/import_dae.h>
#include <wrap/io_trimesh/export_dae.h>



#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

bool ColladaIOPlugin::open(const QString &formatName, QString &fileName, MeshModel &m, int& mask, CallBackPos *cb, QWidget *parent)
{
	if (fileName.isEmpty())
	{
		fileName = QFileDialog::getOpenFileName(parent,tr("Open File"),"../sample","Collada files (*.dae)");
		if (fileName.isEmpty())
			return false;

		QFileInfo fi(fileName);
		// this change of dir is needed for subsequent textures/materials loading
		QDir::setCurrent(fi.absoluteDir().absolutePath());
	}

	// initializing mask
  mask = 0;
	
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
	std::string filename = fileName.toUtf8().data();

	bool normalsUpdated = false;

	if(formatName.toUpper() == tr("DAE"))
	{
		m.addinfo = NULL;
		vcg::tri::io::AdditionalInfoDAE* info = NULL;
		if (!vcg::tri::io::ImporterDAE<CMeshO>::LoadMask(filename.c_str(), info))
			return false;

		m.Enable(info->mask);
		for(unsigned int tx = 0; tx < info->texturefile.size();++tx)
			m.cm.textures.push_back(info->texturefile[tx].toStdString());
		
		int result = vcg::tri::io::ImporterDAE<CMeshO>::Open(m.cm, filename.c_str(),m.addinfo);
		
		

		if (result != vcg::tri::io::ImporterDAE<CMeshO>::E_NOERROR)
		{
			QMessageBox::critical(parent, tr("DAE Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterDAE<CMeshO>::ErrorMsg(result)));
			return false;
		}
		else _mp.push_back(&m);

		if(info->mask & MeshModel::IOM_WEDGNORMAL)
			normalsUpdated = true;
		mask = info->mask;
	}

	
	// verify if texture files are present
	QString missingTextureFilesMsg = "The following texture files were not found:\n";
	bool someTextureNotFound = false;
	for ( unsigned textureIdx = 0; textureIdx < m.cm.textures.size(); ++textureIdx)
	{
		FILE* pFile = fopen (m.cm.textures[textureIdx].c_str(), "r");
		if (pFile == NULL)
		{
			missingTextureFilesMsg.append("\n");
			missingTextureFilesMsg.append(m.cm.textures[textureIdx].c_str());
			someTextureNotFound = true;
		}
		else
			fclose (pFile);
	}
	if (someTextureNotFound)
		QMessageBox::warning(parent, tr("Missing texture files"), missingTextureFilesMsg);

	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
	if (!normalsUpdated) 
		vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);		// updates normals

	if (cb != NULL)	(*cb)(99, "Done");

	return true;
}

bool ColladaIOPlugin::save(const QString &formatName,QString &fileName, MeshModel &m, const int &mask, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
	std::string filename = fileName.toUtf8().data();
	std::string ex = formatName.toUtf8().data();
	int result;
	
	if (std::find(_mp.begin(),_mp.end(),&m) == _mp.end()) 
		result = vcg::tri::io::ExporterDAE<CMeshO>::Save(m.cm,filename.c_str(),mask);
	else 
		result = vcg::tri::io::ExporterDAE<CMeshO>::Save(m.cm,filename.c_str(),m.addinfo,mask);

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
QList<MeshIOInterface::Format> ColladaIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("Collada File Format"	,tr("DAE"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> ColladaIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("Collada File Format"	,tr("DAE"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
int ColladaIOPlugin::GetExportMaskCapability(QString &format) const
{
	if(format.toUpper() == tr("DAE")){return vcg::tri::io::ExporterDAE<CMeshO>::GetExportMaskCapability();}
	return 0;
}

const ActionInfo &ColladaIOPlugin::Info(QAction *action)
{
	static ActionInfo ai;
	return ai;
}

const PluginInfo &ColladaIOPlugin::Info()
{
	static PluginInfo ai;
	ai.Date=tr("June 2006");
	ai.Version = tr("0.1");
	ai.Author = ("Guido Ranzuglia");
	return ai;
 }


Q_EXPORT_PLUGIN(ColladaIOPlugin)