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
 Revision 1.99  2008/04/15 10:45:51  cignoni
 Added support of color in off files

 Revision 1.98  2008/04/04 10:03:53  cignoni
 Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

 Revision 1.97  2007/11/26 07:35:26  cignoni
 Yet another small cosmetic change to the interface of the io filters.

 Revision 1.96  2007/11/25 09:48:39  cignoni
 Changed the interface of the io filters. Now also a default bit set for the capabilities has to specified

 Revision 1.95  2007/10/17 21:24:36  cignoni
 corrected orthographic error in report string

 Revision 1.94  2007/04/16 09:25:30  cignoni
 ** big change **
 Added Layers managemnt.
 Interfaces are changing again...

 Revision 1.93  2007/03/20 16:23:10  cignoni
 Big small change in accessing mesh interface. First step toward layers

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

#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include <QMessageBox>

using namespace std;
using namespace vcg;




bool ExtraMeshIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &, CallBackPos *cb, QWidget *parent)
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

	if (formatName.toUpper() == tr("3DS"))
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

		if(info.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
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

bool ExtraMeshIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exporting file %1:\n%2";
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
	else 
  assert(0); // unknown format
  return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> ExtraMeshIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("3D-Studio File Format"		,tr("3DS"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> ExtraMeshIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("3D-Studio File Format"		,tr("3DS"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void ExtraMeshIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
	if(format.toUpper() == tr("3DS")){capability=defaultBits= vcg::tri::io::Exporter3DS<CMeshO>::GetExportMaskCapability();}
	return;
}

Q_EXPORT_PLUGIN(ExtraMeshIOPlugin)
