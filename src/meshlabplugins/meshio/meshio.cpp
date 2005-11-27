/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
** information about Qt Commercial License Agreements.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#include <QtGui>

#include "meshio.h"
// temporaneamente prendo la versione corrente dalla cartella test
#include "../../test/io/import_obj.h"
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
      QString &format,
			QString fileName,
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
			string filename = fileName.toUtf8();

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
			if (result != vcg::tri::io::ImporterOBJ<CMeshO>::OBJError::E_NOERROR)
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

bool ExtraMeshIOPlugin::save(
      QString &format,
			QString fileName,
      MeshModel &m, 
      int mask,
      vcg::CallBackPos *cb,
      QWidget *parent)
{
  return false;
}

Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)

