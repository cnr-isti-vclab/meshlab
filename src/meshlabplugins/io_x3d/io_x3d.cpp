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
 Revision 1.11  2008/04/04 14:08:23  cignoni
 Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

 Revision 1.10  2008/02/20 21:59:37  gianpaolopalma
 Added support to file .x3dv and .wrl

 Revision 1.9  2008/02/15 08:27:44  cignoni
 - '>> 'changed into '> >'
 - Used HasPerFaceSomething(M) instead of M.HasPerFaceSomething() that is deprecated.
 - many unsigned warning removed
 - added const to some functions parameters (FindDEF, FindAndReplaceUSE ...)

 Revision 1.8  2008/02/14 13:00:22  gianpaolopalma
 Changed assignment of defaultBits in the method GetExportMaskCapability

 Revision 1.7  2008/02/13 15:18:20  gianpaolopalma
 Updating mesh mask accoding to mesh data supported

 Revision 1.6  2008/02/11 09:28:33  gianpaolopalma
 return error code if file doesn't contain geometry

 Revision 1.5  2008/02/08 17:02:09  gianpaolopalma
 Improved memory management

 Revision 1.4  2008/02/06 13:09:10  gianpaolopalma
 Updated vertexs and faces number in addinfo

 Revision 1.3  2008/02/05 16:38:55  gianpaolopalma
 Added texture file path in the mesh

 Revision 1.2  2008/02/04 13:28:36  gianpaolopalma
 Added management to texture coordinates per vertex

 Revision 1.1  2008/02/02 13:41:08  gianpaolopalma
 First working version

*****************************************************************************/
#include <Qt>
#include <QtGui>
#include <QtXml>


#include "io_x3d.h"
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>

#include "import_x3d.h"
#include "export_x3d.h"


#include <QMessageBox>
#include <QFileDialog>

using namespace std;
using namespace vcg;

bool IoX3DPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &, CallBackPos *cb, QWidget *parent)
{
	// initializing mask
	mask = 0;
	
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nFile: %2\nError details: %3";
	string filename = QFile::encodeName(fileName).constData ();
	bool normalsUpdated = false;
	vcg::tri::io::AdditionalInfoX3D* info = NULL;
	if(formatName.toUpper() == tr("X3D") || formatName.toUpper() == tr("X3DV") || formatName.toUpper() == tr("WRL"))
	{
		int result;
		if (formatName.toUpper() == tr("X3D"))
			result = vcg::tri::io::ImporterX3D<CMeshO>::LoadMask(filename.c_str(), info); 
		else
			result = vcg::tri::io::ImporterX3D<CMeshO>::LoadMaskVrml(filename.c_str(), info);
		if ( result != vcg::tri::io::ImporterX3D<CMeshO>::E_NOERROR)
		{
			QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName, info->filenameStack[info->filenameStack.size()-1], vcg::tri::io::ImporterX3D<CMeshO>::ErrorMsg(result)));
			return false;
		}
		if (info->mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD)
		{
			info->mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
			info->mask &=(~vcg::tri::io::Mask::IOM_VERTTEXCOORD);
		}
		if (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR)
			info->mask &=(~vcg::tri::io::Mask::IOM_WEDGCOLOR);
		if (info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
			info->mask &=(~vcg::tri::io::Mask::IOM_WEDGNORMAL);
		m.Enable(info->mask);
				
		errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nFile: %2\nLine number: %3\nError details: %4";
		result = vcg::tri::io::ImporterX3D<CMeshO>::Open(m.cm, filename.c_str(), info, cb);
		if (result != vcg::tri::io::ImporterX3D<CMeshO>::E_NOERROR)
		{
			QString fileError = info->filenameStack[info->filenameStack.size()-1];
			QString lineError;
			lineError.setNum(info->lineNumberError);
			QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName, fileError, lineError, vcg::tri::io::ImporterX3D<CMeshO>::ErrorMsg(result)));
			return false;
		}
		if (m.cm.vert.size() == 0)
		{
			errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: File without a geometry";
			QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName));
			return false;
		}
		if(info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
			normalsUpdated = true;
		mask = info->mask;
	}
	// verify if texture files are present
	QString missingTextureFilesMsg = "The following texture files were not found:\n";
	bool someTextureNotFound = false;
	for(unsigned int tx = 0; tx < info->textureFile.size(); ++tx)
	{
		FILE* pFile = fopen (info->textureFile[tx].toStdString().c_str(), "r");
		if (pFile == NULL)
		{
			missingTextureFilesMsg.append("\n");
			missingTextureFilesMsg.append(info->textureFile[tx].toStdString().c_str());
			someTextureNotFound = true;
		}
		else
		{
			m.cm.textures.push_back(info->textureFile[tx].toStdString());
			fclose (pFile);
		}
	}	
		
		
	if (someTextureNotFound)
	{
		QMessageBox::warning(parent, tr("Missing texture files"), missingTextureFilesMsg);
	}
	
	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
	if (!normalsUpdated) 
		vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);		// updates normals

	if (cb != NULL)	(*cb)(99, "Done");
	
	delete(info);
	return true;
}


bool IoX3DPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file:\n%1\n\nError details: %2";
	string filename = QFile::encodeName(fileName).constData ();
	if(formatName.toUpper() == tr("X3D"))
	{
		int result = vcg::tri::io::ExporterX3D<CMeshO>::Save(m.cm, filename.c_str(), mask, cb);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterX3D<CMeshO>::ErrorMsg(result)));
			return false;
		}
		if (cb !=NULL) (*cb)(99, "Saving X3D File...");
		return true;
	}
	assert(0);
	return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> IoX3DPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("X3D File Format - XML encoding", tr("X3D"));
	formatList << Format("X3D File Format - VRML encoding", tr("X3DV"));
	formatList << Format("VRML 2.0 File Format", tr("WRL"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> IoX3DPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("X3D File Format", tr("X3D"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void IoX3DPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
	if(format.toUpper() == tr("X3D")){
		capability = defaultBits = vcg::tri::io::ExporterX3D<CMeshO>::GetExportMaskCapability();
		return; 
	}
	assert(0);
}
 
Q_EXPORT_PLUGIN(IoX3DPlugin)
