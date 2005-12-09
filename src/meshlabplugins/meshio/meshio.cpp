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



#include<vcg/complex/trimesh/update/bounding.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;


bool ExtraMeshIOPlugin::open(const QString &format, QString &fileName,MeshModel &m, int& mask,CallBackPos *cb,QWidget *parent)
{
	if (fileName.isEmpty())
		fileName = QFileDialog::getOpenFileName(parent,tr("Open File"),"../sample","Obj files (*.obj)");
	
	if (!fileName.isEmpty())
	{
		// initializing progress bar status
		if (cb != NULL)
		(*cb)(0, "Starting...");

		// this change of dir is needed for subsequent texture/material loading
		QString FileNameDir = fileName.left(fileName.lastIndexOf("/")); 
		QDir::setCurrent(FileNameDir);

		QString errorMsgFormat = "Error encountered while loading file %1: %2";

		if(format.toUpper() == tr("OBJ")) //if (format == tr("Import OBJ"))
		{
			string filename = fileName.toUtf8().data();

			vcg::tri::io::ObjInfo oi;
			vcg::tri::io::ImporterOBJ<CMeshO>::LoadMask(filename.c_str(), mask, oi);
			oi.cb = cb;

			if(mask & vcg::ply::PLYMask::PM_WEDGTEXCOORD) 
			{
				//QMessageBox::information(parent, tr("OBJ Opening"), tr("Model has wedge text coords"));
				m.cm.face.EnableWedgeTex();
			}
			m.cm.face.EnableNormal();

			// load from disk
			int result = vcg::tri::io::ImporterOBJ<CMeshO>::Open(m.cm, filename.c_str(), oi);
			if (result != vcg::tri::io::ImporterOBJ<CMeshO>::E_NOERROR)
			{
				QMessageBox::warning(parent, tr("OBJ Opening"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterOBJ<CMeshO>::ErrorMsg(result)));
				return false;
			}
		}

		// update bounding box
		vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);

		// update normals
		vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);

		return true;
	}

	return false;
}

bool ExtraMeshIOPlugin::save(const QString &format,QString &fileName, MeshModel &m, int &mask, vcg::CallBackPos *cb, QWidget *parent)
{
	if(format.toUpper() == tr("OBJ")) ////if (format == tr("Export OBJ"))
	{
		QStringList sl = fileName.split(".");
		if(!(sl.size() == 2 && sl[1] == "obj"))
			fileName = fileName.append(".obj");
		string filename = fileName.toUtf8().data();
		
		bool result = vcg::tri::io::ExporterOBJ<CMeshO>::Save(m.cm,filename.c_str(),false,mask,cb);//salva escusivamente in formato ASCII
		return result;
	}
	return false;
}

QStringList ExtraMeshIOPlugin::formats(QString & description) const
{
	QStringList formatList;
	formatList << tr("OBJ");
	description = "Alias Wavefront Object";

	return formatList;
};


Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)