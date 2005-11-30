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
 Revision 1.8  2005/11/30 08:34:33  cignoni
 Removed spurious 'i' before the comment header.
 Corrected small error in save.

 Revision 1.7  2005/11/30 01:06:59  fmazzant
 added support Export OBJ(base)
 added comment line history
 deleted bug-fix in meshio.pro in unix{ ... }

*****************************************************************************/

#include <QtGui>

#include "meshio.h"
// temporaneamente prendo la versione corrente dalla cartella test
#include "../../test/io/import_obj.h"
#include "../../test/io/export_obj.h"

#include<vcg/complex/trimesh/update/bounding.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

QStringList ExtraMeshIOPlugin::formats() const
{
  return QStringList() << tr("Import OBJ")
											 << tr("Export OBJ");
}

bool ExtraMeshIOPlugin::open(
      const QString &format,
			QString &fileName,
      MeshModel &m, 
      int& mask,
      CallBackPos *cb,
			QWidget *parent)
{
	if (fileName.isEmpty())
		fileName = QFileDialog::getOpenFileName(parent,tr("Open File"),"../sample","Obj files (*.obj)");
	
	if (!fileName.isEmpty())
	{
		QString errorMsgFormat = "Error encountered while loading file %1: %2";

		if (format == tr("Import OBJ"))
		{
			string filename = fileName.toUtf8().data();

			vcg::tri::io::ObjInfo oi;
			vcg::tri::io::ImporterOBJ<CMeshO>::LoadMask(filename.c_str(), mask, oi);
			oi.cb = cb;

			if(mask & vcg::ply::PLYMask::PM_WEDGTEXCOORD) 
			{
				QMessageBox::information(parent, tr("OBJ Opening"), tr("Model has wedge text coords"));
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

bool ExtraMeshIOPlugin::save(const QString &format,QString &fileName, MeshModel &m, int mask, vcg::CallBackPos *cb, QWidget *parent)
{
	//TODO: aggiungere la possibilita' di selezionare la maschera
	//TODO: trattare i casi di errore aprendo una dialog
	//TODO: ed antro ancora.....:)
	if(format == tr("Export OBJ"))
	{
		int result = vcg::tri::io::ExporterOBJ<CMeshO>::Save(m.cm,"La_Prova.obj",false,mask,cb);
    return true;
	}
	return false;
}

Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)

