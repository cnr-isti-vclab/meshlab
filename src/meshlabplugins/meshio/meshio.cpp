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
 Revision 1.70  2006/01/29 18:33:42  fmazzant
 added some comment to the code

 Revision 1.69  2006/01/29 17:14:20  buzzelli
 files import_obj.h, import_3ds.h, io_3ds.h and io_obj.h have been moved from test/io to meshio

 Revision 1.68  2006/01/29 17:07:16  buzzelli
 missing texture files warning has been added

 Revision 1.67  2006/01/29 16:33:03  fmazzant
 moved export_obj and export_3ds from test/io into meshio/

 Revision 1.66  2006/01/28 20:43:38  buzzelli
 minor changes in Import3ds LoadMask

 Revision 1.65  2006/01/27 17:02:51  buzzelli
 solved a small bug with progress bar

 Revision 1.64  2006/01/27 01:13:27  buzzelli
 Added a better distinction beetween critical and non critical error messages

 Revision 1.63  2006/01/26 22:51:57  fmazzant
 removed last links to the Exporter mask

*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "meshio.h"

// temporaneamente prendo la versione corrente dalla cartella test
#include "import_obj.h"
#include "export_obj.h"

#include <lib3ds/file.h>
#include "import_3ds.h"
#include "export_3ds.h"

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>

#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/import_off.h>

#include<vcg/complex/trimesh/update/bounding.h>
#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/ply/plylib.h>
#include <vcg/complex/trimesh/update/normal.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;


bool ExtraMeshIOPlugin::open(const QString &formatName, QString &fileName,MeshModel &m, int& mask,CallBackPos *cb,QWidget *parent)
{
	if (fileName.isEmpty())
		fileName = QFileDialog::getOpenFileName(parent,tr("Open File"),"../sample","Obj files (*.obj)");
	
  mask=0; // just to be sure...
	
	if (!fileName.isEmpty())
	{
		// initializing progress bar status
		if (cb != NULL)
			(*cb)(0, "Loading...");

		// this change of dir is needed for subsequent texture/material loading
		QString FileNameDir = fileName.left(fileName.lastIndexOf("/")); 
		QDir::setCurrent(FileNameDir);

		QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
		string filename = fileName.toUtf8().data();

		bool normalsUpdated = false;

		if(formatName.toUpper() == tr("OBJ"))
		{
			vcg::tri::io::ObjInfo oi;	
			oi.cb = cb;
			vcg::tri::io::ImporterOBJ<CMeshO>::LoadMask(filename.c_str(), mask, oi);

			if(mask & vcg::tri::io::Mask::IOM_WEDGTEXCOORD) 
			{
				qDebug("Has Wedge Text Coords\n");
				m.cm.face.EnableWedgeTex();
			}

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

			if(mask & MeshModel::IOM_WEDGNORMAL)
				normalsUpdated = true;
		}
		else if (formatName.toUpper() == tr("PLY"))
		{
			vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(filename.c_str(), mask); 
		  
			if(mask&MeshModel::IOM_VERTQUALITY) qDebug("Has Vertex Quality\n");
			if(mask&MeshModel::IOM_FACEQUALITY) qDebug("Has Face Quality\n");
			if(mask&MeshModel::IOM_FACECOLOR)		qDebug("Has Face Color\n");
			if(mask&MeshModel::IOM_VERTCOLOR)		qDebug("Has Vertex Color\n");
			if(mask&MeshModel::IOM_WEDGTEXCOORD) 
			{
				qDebug("Has Wedge Text Coords\n");
				m.cm.face.EnableWedgeTex();
			}
			
			int result = vcg::tri::io::ImporterPLY<CMeshO>::Open(m.cm, filename.c_str(), cb);
			if (result != ::vcg::ply::E_NOERROR)
			{
				QMessageBox::warning(parent, tr("PLY Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterPLY<CMeshO>::ErrorMsg(result)));
				return false;
			}
		}
		else if (formatName.toUpper() == tr("OFF"))
		{
			int result = vcg::tri::io::ImporterOFF<CMeshO>::Open(m.cm, filename.c_str());
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

			if(mask & MeshModel::IOM_WEDGTEXCOORD) 
			{
				qDebug("Has Wedge Text Coords\n");
				m.cm.face.EnableWedgeTex();
			}
			
			int result = vcg::tri::io::Importer3DS<CMeshO>::Open(m.cm, filename.c_str(), file, info);
			if (result != vcg::tri::io::Importer3DS<CMeshO>::E_NOERROR)
			{
				QMessageBox::warning(parent, tr("3DS Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Importer3DS<CMeshO>::ErrorMsg(result)));
				return false;
			}

			if(mask & MeshModel::IOM_WEDGNORMAL)
				normalsUpdated = true;
		}

		// verify if texture files apre present
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

		if (cb != NULL)
			(*cb)(99, "Done");

		m.storeVertexColor();
		
		m.mask = mask;

		return true;
	}

	return false;
}

bool ExtraMeshIOPlugin::save(const QString &formatName,QString &fileName, MeshModel &m, int &mask, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
	string filename = fileName.toUtf8().data();
	string ex = formatName.toUtf8().data();

	if(formatName.toUpper() == tr("OBJ"))
	{	
		int result = vcg::tri::io::ExporterOBJ<CMeshO>::Save(m.cm,filename.c_str(),false,mask,cb);//only ASCII format
		if(result != vcg::tri::io::ExporterOBJ<CMeshO>::E_NOERROR )
		{
			QMessageBox::warning(parent, tr("OBJ Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterOBJ<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}

	if(formatName.toUpper() == tr("PLY"))
	{		
		int result = vcg::tri::io::Exporter<CMeshO>::Save(m.cm,filename.c_str(),mask,cb);
		if(result != 0)
		{
			QMessageBox::warning(parent, tr("PLY Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}

	if(formatName.toUpper() == tr("OFF"))
	{
		int result = vcg::tri::io::Exporter<CMeshO>::Save(m.cm,filename.c_str(),cb);
		if(result != 0)
		{
			//QMessageBox::warning(parent, tr("OFF Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterOFF<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}

	if(formatName.toUpper() == tr("STL"))
	{
		int result = vcg::tri::io::Exporter<CMeshO>::Save(m.cm,filename.c_str(),mask,cb);
		if(result != 0)
		{
			QMessageBox::warning(parent, tr("Mesh Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;

	}

	if(formatName.toUpper() == tr("3DS"))
	{	
		int result = vcg::tri::io::Exporter3DS<CMeshO>::Save(m.cm,filename.c_str(),true,mask,cb);//only binary format
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("3DS Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter3DS<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}

	QMessageBox::warning(parent, "Unknow type", "file's extension not supported!!!");

	return false;
}

/*
	returns the list of the file's type which can be importered
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
	if(format.toUpper() == tr("PLY")){return MeshModel::IOM_ALL;}
	if(format.toUpper() == tr("OFF")){return MeshModel::IOM_ALL;}
	if(format.toUpper() == tr("STL")){return MeshModel::IOM_ALL;}
	if(format.toUpper() == tr("3DS")){return vcg::tri::io::Exporter3DS<CMeshO>::GetExportMaskCapability();}
	return 0;
}

Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)