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
 Revision 1.92  2007/03/20 15:52:47  cignoni
 Patched issue related to path with non ascii chars

 Revision 1.91  2006/12/01 10:41:11  granzuglia
 fixed a little bug: added return true in the off-file

 Revision 1.90  2006/11/30 22:55:05  cignoni
 Separated very basic io filters to the more advanced one into two different plugins baseio and meshio

 Revision 1.89  2006/11/29 00:59:19  cignoni
 Cleaned plugins interface; changed useless help class into a plain string

 Revision 1.88  2006/11/16 11:25:32  e_cerisoli
 Update meshio.cpp: new file I/O

 Revision 1.87  2006/10/10 21:10:33  cignoni
 progress bar bug

*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "meshio.h"

#include <lib3ds/file.h>
#include "import_3ds.h"
#include <wrap/io_trimesh/export_3ds.h>
#include <wrap/io_trimesh/export_obj.h>
#include <wrap/io_trimesh/export_vrml.h>
#include <wrap/io_trimesh/export_dxf.h>

#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/import_off.h>
#include <wrap/io_trimesh/import_ptx.h>

#include <vcg/complex/trimesh/update/bounding.h>
#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <vcg/complex/trimesh/update/normal.h>

#include <QMessageBox>

using namespace vcg;

bool ExtraMeshIOPlugin::open(const QString &formatName, QString &fileName, MeshModel &m, int& mask, CallBackPos *cb, QWidget *parent)
{
	// initializing mask
  mask = 0;
	
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
	QString error_2MsgFormat = "Error encountered while loading file:\n\"%1\"\n\n File with more than a mesh.\n Load only the first!";

	string filename = QFile::encodeName(fileName).constData ();
  //string filename = fileName.toUtf8().data();

	bool normalsUpdated = false;

	if(formatName.toUpper() == tr("OBJ"))
	{
    vcg::tri::io::ImporterOBJ<CMeshO>::Info oi;	
		oi.cb = cb;
		if (!vcg::tri::io::ImporterOBJ<CMeshO>::LoadMask(filename.c_str(), oi))
			return false;
    m.Enable(oi.mask);
		
		int result = vcg::tri::io::ImporterOBJ<CMeshO>::Open(m.cm, filename.c_str(), oi);
		if (result != vcg::tri::io::ImporterOBJ<CMeshO>::E_NOERROR)
		{
			if (result & vcg::tri::io::ImporterOBJ<CMeshO>::E_NON_CRITICAL_ERROR)
				QMessageBox::warning(parent, tr("OBJ Opening Warning"), vcg::tri::io::ImporterOBJ<CMeshO>::ErrorMsg(result));
			else
			{
				QMessageBox::critical(parent, tr("OBJ Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterOBJ<CMeshO>::ErrorMsg(result)));
				return false;
			}
		}

		if(oi.mask & MeshModel::IOM_WEDGNORMAL)
			normalsUpdated = true;

		mask = oi.mask;
	}
	else if (formatName.toUpper() == tr("PTX"))
	{
		int result = vcg::tri::io::ImporterPTX<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result == 1)
		{
			QMessageBox::warning(parent, tr("PTX Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterPTX<CMeshO>::ErrorMsg(result)));
			return false;
		}
		if (result == 2)
		{
			QMessageBox::warning(parent, tr("PTX Opening Error"), error_2MsgFormat.arg(fileName, vcg::tri::io::ImporterPTX<CMeshO>::ErrorMsg(result)));
		}
	}
	else if (formatName.toUpper() == tr("OFF"))
	{
		int result = vcg::tri::io::ImporterOFF<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result != 0)  // OFFCodes enum is protected
		{
			QMessageBox::warning(parent, tr("OFF Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterOFF<CMeshO>::ErrorMsg(result)));
			return false;
		}

		CMeshO::FaceIterator fi = m.cm.face.begin();
		for (; fi != m.cm.face.end(); ++fi)
			face::ComputeNormalizedNormal(*fi);
	}
	else if (formatName.toUpper() == tr("3DS"))
	{
		vcg::tri::io::_3dsInfo info;	
		info.cb = cb;
		Lib3dsFile *file = NULL;
		vcg::tri::io::Importer3DS<CMeshO>::LoadMask(filename.c_str(), file, info);
    m.Enable(info.mask);
		
		int result = vcg::tri::io::Importer3DS<CMeshO>::Open(m.cm, filename.c_str(), file, info);
		if (result != vcg::tri::io::Importer3DS<CMeshO>::E_NOERROR)
		{
			QMessageBox::warning(parent, tr("3DS Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Importer3DS<CMeshO>::ErrorMsg(result)));
			return false;
		}

		if(info.mask & MeshModel::IOM_WEDGNORMAL)
			normalsUpdated = true;

		mask = info.mask;
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

bool ExtraMeshIOPlugin::save(const QString &formatName,QString &fileName, MeshModel &m, const int &mask, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData ();
  //string filename = fileName.toUtf8().data();
	string ex = formatName.toUtf8().data();
	
	if(formatName.toUpper() == tr("3DS"))
	{
		int result = vcg::tri::io::Exporter3DS<CMeshO>::Save(m.cm,filename.c_str(),mask,cb);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter3DS<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}
	else if(formatName.toUpper() == tr("WRL"))
	{
		int result = vcg::tri::io::ExporterWRL<CMeshO>::Save(m.cm,filename.c_str(),mask,cb);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterWRL<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}
	else if( formatName.toUpper() == tr("OFF") || formatName.toUpper() == tr("DXF") || formatName.toUpper() == tr("OBJ") )
  {
    int result = vcg::tri::io::Exporter<CMeshO>::Save(m.cm,filename.c_str(),mask,cb);
  	if(result!=0)
	  {
		  QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter<CMeshO>::ErrorMsg(result)));
		  return false;
	  }
	return true;
  }
  assert(0); // unknown format
  return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> ExtraMeshIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("Alias Wavefront Object"		,tr("OBJ"));
	formatList << Format("Object File Format"			  ,tr("OFF"));
	formatList << Format("3D-Studio File Format"		,tr("3DS"));
	formatList << Format("PTX File Format"      		,tr("PTX"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> ExtraMeshIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("Alias Wavefront Object"		,tr("OBJ"));
	formatList << Format("Object File Format"			  ,tr("OFF"));
	formatList << Format("3D-Studio File Format"		,tr("3DS"));
	formatList << Format("VRML File Format"         ,tr("WRL"));
	formatList << Format("DXF File Format"          ,tr("DXF"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
int ExtraMeshIOPlugin::GetExportMaskCapability(QString &format) const
{
	if(format.toUpper() == tr("OBJ")){return vcg::tri::io::ExporterOBJ<CMeshO>::GetExportMaskCapability();}
	if(format.toUpper() == tr("OFF")){return vcg::tri::io::ExporterOFF<CMeshO>::GetExportMaskCapability();}
	if(format.toUpper() == tr("3DS")){return vcg::tri::io::Exporter3DS<CMeshO>::GetExportMaskCapability();}
	if(format.toUpper() == tr("WRL")){return vcg::tri::io::ExporterWRL<CMeshO>::GetExportMaskCapability();}
	return 0;
}

const PluginInfo &ExtraMeshIOPlugin::Info()
{
	static PluginInfo ai;
	ai.Date=tr("January 2006");
	ai.Version = tr("0.6");
	ai.Author = ("Paolo Cignoni, Andrea Buzzelli, Elisa Cerisoli, Federico Mazzanti");
	return ai;
 }

Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)