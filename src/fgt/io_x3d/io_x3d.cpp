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
 Revision 1.3  2008/02/05 16:38:55  gianpaolopalma
 Added texture file path in the mesh

 Revision 1.2  2008/02/04 13:28:36  gianpaolopalma
 Added management to texture coordinates per vertex

 Revision 1.1  2008/02/02 13:41:08  gianpaolopalma
 First working version

*****************************************************************************/
#include <Qt>
#include <QtGui>
#include <QtXml>

#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>

#include "io_x3d.h"
#include "import_x3d.h"
#include "export_x3d.h"


#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

bool IoX3DPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, CallBackPos *cb, QWidget *parent)
{
	// initializing mask
	mask = 0;
	
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nFile: %2\nError details: %3";
	string filename = QFile::encodeName(fileName).constData ();
  	bool normalsUpdated = false;
	if(formatName.toUpper() == tr("X3D"))
	{
		m.addinfo = NULL;
		vcg::tri::io::AdditionalInfoX3D* info = NULL;
		int result = vcg::tri::io::ImporterX3D<CMeshO>::LoadMask(filename.c_str(), info); 
		if ( result != vcg::tri::io::ImporterX3D<CMeshO>::E_NOERROR)
		{
			QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName, info->filenameStack[info->filenameStack.size()-1], vcg::tri::io::ImporterX3D<CMeshO>::ErrorMsg(result)));
			return false;
		}
		m.addinfo = info;
		if (info->mask & MeshModel::IOM_VERTTEXCOORD)
			info->mask |= MeshModel::IOM_WEDGTEXCOORD;
		m.Enable(info->mask);
		for(unsigned int tx = 0; tx < info->textureFile.size(); ++tx)
			m.cm.textures.push_back(info->textureFile[tx].toStdString());
		
		errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nFile: %2\nLine number: %3\nError details: %4";
		result = vcg::tri::io::ImporterX3D<CMeshO>::Open(m.cm, filename.c_str(), info);
		if (result != vcg::tri::io::ImporterX3D<CMeshO>::E_NOERROR)
		{
			QString fileError = info->filenameStack[info->filenameStack.size()-1];
			QString lineError;
			lineError.setNum(info->lineNumberError);
			QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName, fileError, lineError, vcg::tri::io::ImporterX3D<CMeshO>::ErrorMsg(result)));
			return false;
		}
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

bool IoX3DPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData ();
	if(formatName.toUpper() == tr("3DS"))
	{
		int result = vcg::tri::io::ExporterX3D<CMeshO>::Save(m.cm,filename.c_str(),m.addinfo,mask);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterX3D<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}
	assert(0);
	return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> IoX3DPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("X3D File Format", tr("X3D"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> IoX3DPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("X3D File Format"	,tr("X3D"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void IoX3DPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
	if(format.toUpper() == tr("X3D")){
		capability = defaultBits = vcg::tri::io::ExporterX3D<CMeshO>::GetExportMaskCapability();
		return; 
	}
	assert(0);
}

const PluginInfo &IoX3DPlugin::Info()
{
	static PluginInfo ai;
	ai.Date=tr("January 2008");
	ai.Version = tr("0.1");
	ai.Author = ("Gianpaolo Palma");
	return ai;
 }
 
Q_EXPORT_PLUGIN(IoX3DPlugin)