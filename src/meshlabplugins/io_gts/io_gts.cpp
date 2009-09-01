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

#include <Qt>
#include <QtGui>

#include "io_gts.h"

#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>

#include "import_gts.h"
#include "export_gts.h"

#include <QMessageBox>

using namespace std;
using namespace vcg;


// initialize importing parameters
// void GtsIOPlugin::initPreOpenParameter(const QString &formatName, const QString &filename, RichParameterSet &parlst)
// {
// 	parlst.addBool("pointsonly",false,"Keep only points","Just import points, without triangulation");
// 	parlst.addBool("flipfaces",false,"Flip all faces","Flip the orientation of all the triangles");
// }


bool GtsIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &parlst, CallBackPos *cb, QWidget *parent)
{
	// initializing mask
	mask = 0;

	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
	QString error_2MsgFormat = "Error encountered while loading file:\n\"%1\"\n\n File with more than a mesh.\n Load only the first!";

	string filename = QFile::encodeName(fileName).constData ();

	bool normalsUpdated = false;

	if (formatName.toUpper() == tr("GTS"))
	{
		int loadMask;
		if (!vcg::tri::io::ImporterGTS<CMeshO>::LoadMask(filename.c_str(),loadMask))
			return false;
		m.Enable(loadMask);

		vcg::tri::io::ImporterGTS<CMeshO>::Options opt;
		opt.flipFaces = true;

		int result = vcg::tri::io::ImporterGTS<CMeshO>::Open(m.cm, filename.c_str(), mask, opt, cb);
		if (result != 0)
		{
			QMessageBox::warning(parent, tr("GTS Opening Error"),
													 errorMsgFormat.arg(fileName, vcg::tri::io::ImporterGTS<CMeshO>::ErrorMsg(result)));
			return false;
		}

		CMeshO::FaceIterator fi = m.cm.face.begin();
		for (; fi != m.cm.face.end(); ++fi)
			face::ComputeNormalizedNormal(*fi);
	}

	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
	if (!normalsUpdated)
		vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);		// updates normals

	if (cb != NULL)
		(*cb)(99, "Done");

	return true;
}

bool GtsIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exporting file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData ();
	string ex = formatName.toUtf8().data();

	if( formatName.toUpper() == tr("GTS") )
	{
		int result = vcg::tri::io::ExporterGTS<CMeshO>::Save(m.cm,filename.c_str(),mask);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterGTS<CMeshO>::ErrorMsg(result)));
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
QList<MeshIOInterface::Format> GtsIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("GNU Triangulated Surface"		,tr("GTS"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> GtsIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("GNU Triangulated Surface"		,tr("GTS"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type.
	otherwise it returns 0 if the file format is unknown
*/
void GtsIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
	if(format.toUpper() == tr("GTS")){capability=defaultBits= vcg::tri::io::ExporterGTS<CMeshO>::GetExportMaskCapability();}
	return;
}

Q_EXPORT_PLUGIN(GtsIOPlugin)
