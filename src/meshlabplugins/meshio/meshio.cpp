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
 Revision 1.53  2006/01/19 09:36:28  fmazzant
 cleaned up history log

 Revision 1.52  2006/01/17 23:46:36  cignoni
 Moved some include from meshmodel.h to here

 Revision 1.51  2006/01/17 13:47:45  fmazzant
 update interface meshio : formats -> importFormats() & exportFormts

 Revision 1.50  2006/01/16 23:53:22  fmazzant
 bux-fix MeshModel &m -> MeshModel *m

 Revision 1.49  2006/01/16 15:30:26  fmazzant
 added rename texture dialog for exporter
 removed old maskobj

*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "meshio.h"
#include "savemaskdialog.h"
#include "savemaskexporter.h"

// temporaneamente prendo la versione corrente dalla cartella test
#include "../../test/io/import_obj.h"
#include "../../test/io/export_obj.h"

#include "../../test/io/import_3ds.h"
#include "../../test/io/export_3ds.h"

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

		QString errorMsgFormat = "Error encountered while loading file %1:\n%2";
		string filename = fileName.toUtf8().data();

		bool bUpdatedNormals = false;

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
				QMessageBox::warning(parent, tr("OBJ Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterOBJ<CMeshO>::ErrorMsg(result)));
				return false;
			}

			if(mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
				bUpdatedNormals = true;
		}
		else if (formatName.toUpper() == tr("PLY"))
		{
			vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(filename.c_str(), mask); 
		  
			if(mask&vcg::tri::io::Mask::IOM_VERTQUALITY) qDebug("Has Vertex Quality\n");
			if(mask&vcg::tri::io::Mask::IOM_FACEQUALITY) qDebug("Has Face Quality\n");
			if(mask&vcg::tri::io::Mask::IOM_FACECOLOR)		qDebug("Has Face Color\n");
			if(mask&vcg::tri::io::Mask::IOM_VERTCOLOR)		qDebug("Has Vertex Color\n");
			if(mask&vcg::tri::io::Mask::IOM_WEDGTEXCOORD) 
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
			// TODO: decomment this code when implementation of loadmask will be done
			/*vcg::tri::io::Importer3DS<CMeshO>::LoadMask(filename.c_str(), mask, info);

			if(mask & vcg::ply::PLYMask::PM_WEDGTEXCOORD) 
			{
				qDebug("Has Wedge Text Coords\n");
				m.cm.face.EnableWedgeTex();
			}*/

			int result = vcg::tri::io::Importer3DS<CMeshO>::Open(m.cm, filename.c_str(), info);
			if (result != vcg::tri::io::Importer3DS<CMeshO>::E_NOERROR)
			{
				QMessageBox::warning(parent, tr("3DS Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Importer3DS<CMeshO>::ErrorMsg(result)));
				return false;
			}

			if(mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
				bUpdatedNormals = true;
		}

		vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
		if (!bUpdatedNormals) 
			vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);		// updates normals

		if (cb != NULL)
			(*cb)(100, "Done");

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
		int newmask = vcg::tri::io::SaveMaskToExporter::GetMaskToExporter(&m,vcg::tri::io::SaveMaskToExporter::_OBJ,vcg::tri::io::ExporterOBJ<CMeshO>::GetExportMaskCapability());
		if( newmask == 0 )return false;

		int result = vcg::tri::io::ExporterOBJ<CMeshO>::Save(m.cm,filename.c_str(),false,newmask,cb);//salva esclusivamente in formato ASCII
		if(result != vcg::tri::io::ExporterOBJ<CMeshO>::E_NOERROR )
		{
			QMessageBox::warning(parent, tr("OBJ Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterOBJ<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}

	if(formatName.toUpper() == tr("PLY"))
	{
		int newmask = vcg::tri::io::SaveMaskToExporter::GetMaskToExporter(&m,vcg::tri::io::SaveMaskToExporter::_PLY,0);//aggiungere la capability
		if( newmask == 0 )return false;
		
		int result = vcg::tri::io::Exporter<CMeshO>::Save(m.cm,filename.c_str(),newmask,cb);
		if(result != 0)
		{
			QMessageBox::warning(parent, tr("PLY Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}

	if(formatName.toUpper() == tr("OFF"))
	{
		//int newmask = vcg::tri::io::SaveMaskToExporter::GetMaskToExporter(&m,vcg::tri::io::SaveMaskToExporter::_OFF,0);//aggiungere la capability
		//if( newmask == 0 )return false;
		
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
		int newmask = vcg::tri::io::SaveMaskToExporter::GetMaskToExporter(&m,vcg::tri::io::SaveMaskToExporter::_STL,0);//aggiungere la capability
		if( newmask == 0 )return false;

		int result = vcg::tri::io::Exporter<CMeshO>::Save(m.cm,filename.c_str(),newmask,cb);
		if(result != 0)
		{
			QMessageBox::warning(parent, tr("Mesh Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;

	}

	if(formatName.toUpper() == tr("3DS"))
	{	
		int newmask = vcg::tri::io::SaveMaskToExporter::GetMaskToExporter(&m,vcg::tri::io::SaveMaskToExporter::_3DS,vcg::tri::io::Exporter3DS<CMeshO>::GetExportMaskCapability());
		if( newmask == 0 )return false;
		
		int result = vcg::tri::io::Exporter3DS<CMeshO>::Save(m.cm,filename.c_str(),true,newmask,cb);//salva esclusivamente in formato binario
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

QList<MeshIOInterface::Format> ExtraMeshIOPlugin::importFormats() const
{
	QList<Format> formatList;

	Format ply;
	ply.description = "Stanford Polygon File Format";
	ply.extensions <<  tr("PLY");
	
	Format obj;
	obj.description = "Alias Wavefront Object";
	obj.extensions <<  tr("OBJ");
	
	Format off;
	off.description = "Object File Format";
	off.extensions << tr("OFF");

	Format stl;
	stl.description = "STL File Format";
	stl.extensions << tr("STL");

	Format _3ds;
	_3ds.description = "3D-Studio File Format";
	_3ds.extensions << tr("3DS");

	formatList << ply;
	formatList << obj;
	formatList << off;
	formatList << stl;
	formatList << _3ds;
	

	return formatList;
}

QList<MeshIOInterface::Format> ExtraMeshIOPlugin::exportFormats() const
{
	QList<Format> formatList;

	Format ply;
	ply.description = "Stanford Polygon File Format";
	ply.extensions <<  tr("PLY");
	
	Format obj;
	obj.description = "Alias Wavefront Object";
	obj.extensions <<  tr("OBJ");
	
	Format off;
	off.description = "Object File Format";
	off.extensions << tr("OFF");

	Format stl;
	stl.description = "STL File Format";
	stl.extensions << tr("STL");

	Format _3ds;
	_3ds.description = "3D-Studio File Format";
	_3ds.extensions << tr("3DS");

	formatList << ply;
	formatList << obj;
	formatList << off;
	formatList << stl;
	formatList << _3ds;

	return formatList;
}

Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)