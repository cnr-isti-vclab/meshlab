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
 Revision 1.87  2006/10/10 21:10:33  cignoni
 progress bar bug

 Revision 1.86  2006/06/21 04:25:27  cignoni
 Removed progress bar callback because caused assert under gcc debugged QT libs (to be investigated)

 Revision 1.85  2006/05/25 04:57:45  cignoni
 Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
 Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

 Revision 1.84  2006/03/29 10:05:33  cignoni
 Added missing include export.obj

 Revision 1.83  2006/03/07 13:24:52  cignoni
 moved import_obj to vcg library

 Revision 1.82  2006/03/07 10:47:55  cignoni
 Better mask management during io

 Revision 1.81  2006/02/16 19:29:20  fmazzant
 transfer of Export_3ds.h, Export_obj.h, Io_3ds_obj_material.h from Meshlab to vcg

 Revision 1.80  2006/02/15 23:09:06  fmazzant
 added the part of MeshIO credits

 Revision 1.79  2006/02/09 21:25:38  buzzelli
 making obj loadmask interruptable

 Revision 1.78  2006/02/01 17:47:21  buzzelli
 resolved a platform dependent issue about material and texture files locations

 Revision 1.77  2006/01/31 01:14:11  fmazzant
 standardized call to the export_off function.

 Revision 1.76  2006/01/30 23:02:11  buzzelli
 removed redundant argument in ImporterObj::LoadMask

 Revision 1.75  2006/01/30 22:18:09  buzzelli
 removing unnecessary change mask dialog at import tyme

 Revision 1.74  2006/01/30 22:09:13  buzzelli
 code cleaning

*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "meshio.h"

#include <lib3ds/file.h>
#include "import_3ds.h"
#include <wrap/io_trimesh/export_3ds.h>
#include <wrap/io_trimesh/export_obj.h>

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/import_obj.h>
#include <wrap/io_trimesh/import_off.h>

#include <vcg/complex/trimesh/update/bounding.h>
#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/ply/plylib.h>
#include <vcg/complex/trimesh/update/normal.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

bool ExtraMeshIOPlugin::open(const QString &formatName, QString &fileName, MeshModel &m, int& mask, CallBackPos *cb, QWidget *parent)
{
	if (fileName.isEmpty())
	{
		fileName = QFileDialog::getOpenFileName(parent,tr("Open File"),"../sample","Obj files (*.obj)");
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
	string filename = fileName.toUtf8().data();

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
	else if (formatName.toUpper() == tr("PLY"))
	{
		vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(filename.c_str(), mask); 
    m.Enable(mask);
		
		int result = vcg::tri::io::ImporterPLY<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result != ::vcg::ply::E_NOERROR)
		{
			QMessageBox::warning(parent, tr("PLY Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterPLY<CMeshO>::ErrorMsg(result)));
			return false;
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
	else if (formatName.toUpper() == tr("STL"))
	{
		int result = vcg::tri::io::ImporterSTL<CMeshO>::Open(m.cm, filename.c_str(), cb);
		if (result != vcg::tri::io::ImporterSTL<CMeshO>::E_NOERROR)
		{
			QMessageBox::warning(parent, tr("STL Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterSTL<CMeshO>::ErrorMsg(result)));
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
	string filename = fileName.toUtf8().data();
	string ex = formatName.toUtf8().data();
	
	//START TMP
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
	//END TMP

	int result = vcg::tri::io::Exporter<CMeshO>::Save(m.cm,filename.c_str(),mask,cb);
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
QList<MeshIOInterface::Format> ExtraMeshIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("Stanford Polygon File Format"	,tr("PLY"));
	formatList << Format("Alias Wavefront Object"		,tr("OBJ"));
	formatList << Format("Object File Format"			,tr("OFF"));
	formatList << Format("STL File Format"				,tr("STL"));
	formatList << Format("3D-Studio File Format"		,tr("3DS"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> ExtraMeshIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("Stanford Polygon File Format"	,tr("PLY"));
	formatList << Format("Alias Wavefront Object"		,tr("OBJ"));
	formatList << Format("Object File Format"			,tr("OFF"));
	formatList << Format("STL File Format"				,tr("STL"));
	formatList << Format("3D-Studio File Format"		,tr("3DS"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
int ExtraMeshIOPlugin::GetExportMaskCapability(QString &format) const
{
	if(format.toUpper() == tr("OBJ")){return vcg::tri::io::ExporterOBJ<CMeshO>::GetExportMaskCapability();}
	if(format.toUpper() == tr("PLY")){return vcg::tri::io::ExporterPLY<CMeshO>::GetExportMaskCapability();}
	if(format.toUpper() == tr("OFF")){return vcg::tri::io::ExporterOFF<CMeshO>::GetExportMaskCapability();}
	if(format.toUpper() == tr("STL")){return vcg::tri::io::ExporterSTL<CMeshO>::GetExportMaskCapability();}
	if(format.toUpper() == tr("3DS")){return vcg::tri::io::Exporter3DS<CMeshO>::GetExportMaskCapability();}
	return 0;
}

const ActionInfo &ExtraMeshIOPlugin::Info(QAction *action)
{
	static ActionInfo ai;
	return ai;
}

const PluginInfo &ExtraMeshIOPlugin::Info()
{
	static PluginInfo ai;
	ai.Date=tr("January 2006");
	ai.Version = tr("0.6");
	ai.Author = ("Paolo Cignoni, Andrea Buzzelli, Federico Mazzanti");
	return ai;
 }

Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)