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
 Revision 1.40  2006/01/13 14:18:36  cignoni
 Added initialization of mask to zero

 Revision 1.39  2006/01/10 16:52:19  fmazzant
 update ply::PlyMask -> io::Mask

 Revision 1.38  2006/01/10 00:36:48  buzzelli
 adding first rough implementation of 3ds file importer

 Revision 1.37  2006/01/04 15:27:30  alemochi
 Renamed property of Format struct, and changed plugin dialog

 Revision 1.36  2005/12/23 00:56:42  buzzelli
 Face normals computed also for STL and OFF files.
 Solved (hopefully) bug with progressbar.

 Revision 1.35  2005/12/22 23:39:21  buzzelli
 UpdateNormals is now called only when needed

 Revision 1.34  2005/12/22 21:05:43  cignoni
 Removed Optional Face Normal and added some initalization after opening

 Revision 1.33  2005/12/21 23:28:56  buzzelli
 code cleaning

 Revision 1.32  2005/12/21 14:22:58  buzzelli
 First steps in STL and OFF files importing

 Revision 1.31  2005/12/21 01:17:05  buzzelli
 Better handling of errors residing inside opened file

 Revision 1.30  2005/12/16 17:14:42  fmazzant
 added control file's extension

 Revision 1.29  2005/12/16 00:37:30  fmazzant
 update base export_3ds.h + callback

 Revision 1.28  2005/12/15 12:27:57  fmazzant
 first commit 3ds

 Revision 1.27  2005/12/15 09:50:53  fmazzant
 3ds

 Revision 1.26  2005/12/15 09:24:23  fmazzant
 added base support (very base) 3ds & cleaned code

 Revision 1.25  2005/12/15 08:10:51  fmazzant
 added OFF & STL in formats(...)

 Revision 1.24  2005/12/15 01:20:28  buzzelli
 formats method adapted in order to fit with recent changes in MeshIOInterface

 Revision 1.23  2005/12/14 18:08:24  fmazzant
 added generic save of all type define obj, ply, off, stl

 Revision 1.22  2005/12/14 00:11:33  fmazzant
 update method SaveAs  for a using generic (not still optimized)

 Revision 1.21  2005/12/13 14:02:50  fmazzant
 added the rescue of the materials of the obj

 Revision 1.20  2005/12/09 18:16:14  fmazzant
 added generic obj save with plugin arch.

 Revision 1.19  2005/12/09 16:37:20  fmazzant
 maskobj for select element to save

 Revision 1.18  2005/12/09 00:34:31  buzzelli
 io importing mechanism adapted in order to be fully transparent towards the user

 Revision 1.17  2005/12/07 07:52:25  fmazzant
 export obj generic(base)

 Revision 1.16  2005/12/07 00:56:40  fmazzant
 added support for exporter generic obj file (level base)

 Revision 1.15  2005/12/06 05:16:54  buzzelli
 added code to grant that material files will be searched into the right directory

 Revision 1.14  2005/12/03 23:46:56  cignoni
 Adapted to the new plugin interface in a more standard way

 Revision 1.13  2005/12/03 09:45:42  fmazzant
 adding to mask how much we save a obj file format. the mask taken from the dialogue window SaveMaskDialog.

 Revision 1.12  2005/12/02 23:36:52  fmazzant
 update to the new interface of MeshIOInterface

 Revision 1.11  2005/12/02 17:41:33  fmazzant
 added support obj dialog exporter

 Revision 1.10  2005/11/30 16:26:56  cignoni
 All the modification, restructuring seen during the 30/12 lesson...

 Revision 1.9  2005/11/30 08:58:57  mariolatronico
 temporary remove of ExportOBJ (in save)

 Revision 1.8  2005/11/30 08:34:33  cignoni
 Removed spurious 'i' before the comment header.
 Corrected small error in save.

 Revision 1.7  2005/11/30 01:06:59  fmazzant
 added support Export OBJ(base)
 added comment line history
 deleted bug-fix in meshio.pro in unix{ ... }

*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "meshio.h"
#include "savemaskdialog.h"

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
	string filename = fileName.toUtf8().data();
	string ex = formatName.toUtf8().data();

	if(formatName.toUpper() == tr("OBJ"))
	{	
		bool result = vcg::tri::io::ExporterOBJ<CMeshO>::Save(m.cm,filename.c_str(),false,mask,cb);//salva escusivamente in formato ASCII
		if(!result)
			QMessageBox::warning(parent, ex.c_str(), "File not saved!");
		return result;
	}

	if(formatName.toUpper() == tr("PLY")|formatName.toUpper() == tr("OFF")|formatName.toUpper() == tr("STL"))
	{
		bool result = vcg::tri::io::Exporter<CMeshO>::Save(m.cm,filename.c_str(),cb);
		if(!result)
			QMessageBox::warning(parent, ex.c_str(), "File not saved!");
		return result;
	}

	if(formatName.toUpper() == tr("3DS"))
	{
		bool result = vcg::tri::io::Exporter3DS<CMeshO>::Save(m.cm,filename.c_str(),true,cb);
		if(!result)
			QMessageBox::warning(parent, ex.c_str(), "File not saved!");
		return result;
	}

	QMessageBox::warning(parent, "Unknow type", "file's extension not supported!!!");

	return false;
}

QList<MeshIOInterface::Format> ExtraMeshIOPlugin::formats() const
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
};


Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)