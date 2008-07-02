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
 Revision 1.11  2008/04/04 10:07:14  cignoni
 Solved namespace ambiguities caused by the removal of a silly 'using namespace' in meshmodel.h

 Revision 1.10  2007/11/26 07:35:25  cignoni
 Yet another small cosmetic change to the interface of the io filters.

 Revision 1.9  2007/11/25 09:48:38  cignoni
 Changed the interface of the io filters. Now also a default bit set for the capabilities has to specified

 Revision 1.8  2007/07/12 23:13:30  ggangemi
 changed "tri::io::Mask::Mask::IOM_FACECOLOR" to "tri::io::Mask::IOM_FACECOLOR"

 Revision 1.7  2007/07/10 06:52:47  cignoni
 small patch to allow the loading of per wedge color into faces.

 Revision 1.6  2007/04/16 09:25:28  cignoni
 ** big change **
 Added Layers managemnt.
 Interfaces are changing again...

 Revision 1.5  2007/03/20 16:23:07  cignoni
 Big small change in accessing mesh interface. First step toward layers

 Revision 1.4  2007/03/20 15:52:45  cignoni
 Patched issue related to path with non ascii chars

 Revision 1.3  2007/03/20 10:51:26  cignoni
 attempting to solve unicode bug in filenames

 Revision 1.2  2007/01/19 00:51:59  cignoni
 Now meshlab ask for automatic cleaning of stl files

 Revision 1.1  2006/11/30 22:55:06  cignoni
 Separated very basic io filters to the more advanced one into two different plugins baseio and meshio

*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "baseio.h"

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/import_stl.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/export_stl.h>

#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/clean.h>

using namespace std;
using namespace vcg;

bool BaseMeshIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, CallBackPos *cb, QWidget *parent)
{
	// initializing mask
  mask = 0;
	
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

	//string filename = fileName.toUtf8().data();
	string filename = QFile::encodeName(fileName).constData ();
  
  if (formatName.toUpper() == tr("PLY"))
	{
		vcg::tri::io::ImporterPLY<CMeshO>::LoadMask(filename.c_str(), mask); 
		// small patch to allow the loading of per wedge color into faces.  
		if(mask & tri::io::Mask::IOM_WEDGCOLOR) mask |= tri::io::Mask::IOM_FACECOLOR;
		m.Enable(mask);

		 
		int result = vcg::tri::io::ImporterPLY<CMeshO>::Open(m.cm, filename.c_str(), mask, cb);
		if (result != 0) // all the importers return 0 on success
		{
			//QMessageBox::warning(parent, tr("PLY Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterPLY<CMeshO>::ErrorMsg(result)));
			errorMessage = errorMsgFormat.arg(fileName, vcg::tri::io::ImporterPLY<CMeshO>::ErrorMsg(result));
			return false;
		}
	}
	else if (formatName.toUpper() == tr("STL"))
	{
		int result = vcg::tri::io::ImporterSTL<CMeshO>::Open(m.cm, filename.c_str(), cb);
		if (result != 0) // all the importers return 0 on success
		{
			QMessageBox::warning(parent, tr("STL Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterSTL<CMeshO>::ErrorMsg(result)));
			errorMessage = errorMsgFormat.arg(fileName, vcg::tri::io::ImporterSTL<CMeshO>::ErrorMsg(result));
			return false;
		}
    //int retVal=QMessageBox::question ( parent, tr("STL File Importing"),tr("Do you want to unify duplicated vertices?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No );
    //if(retVal==QMessageBox::Yes )
    //  tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
	}
  else
  {
    assert(0); // Unknown File type
    return false;
  }

	// verify if texture files are present
	QString missingTextureFilesMsg = "The following texture files were not found:\n";
	bool someTextureNotFound = false;
	for ( unsigned textureIdx = 0; textureIdx < m.cm.textures.size(); ++textureIdx)
	{
    if (!QFile::exists(m.cm.textures[textureIdx].c_str()))
		{
			missingTextureFilesMsg.append("\n");
			missingTextureFilesMsg.append(m.cm.textures[textureIdx].c_str());
			someTextureNotFound = true;
		}
	}
	if (someTextureNotFound)
		QMessageBox::warning(parent, tr("Missing texture files"), missingTextureFilesMsg);
	
	if (cb != NULL)	(*cb)(99, "Done");

	return true;
}

bool BaseMeshIOPlugin::save(const QString &formatName,const QString &fileName, MeshModel &m, const int mask, const FilterParameterSet & par, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
  string filename = QFile::encodeName(fileName).constData ();
  //string filename = fileName.toUtf8().data();
	string ex = formatName.toUtf8().data();
	bool binaryFlag = par.getBool("Binary");
	if(formatName.toUpper() == tr("PLY"))
	{
		int result = vcg::tri::io::ExporterPLY<CMeshO>::Save(m.cm,filename.c_str(),mask,binaryFlag,cb);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterPLY<CMeshO>::ErrorMsg(result)));
			return false;
		}
		return true;
	}
	if(formatName.toUpper() == tr("STL"))
	{
		int result = vcg::tri::io::ExporterSTL<CMeshO>::Save(m.cm,filename.c_str(),binaryFlag);
		if(result!=0)
		{
			QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ExporterSTL<CMeshO>::ErrorMsg(result)));
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
QList<MeshIOInterface::Format> BaseMeshIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("Stanford Polygon File Format"	,tr("PLY"));
	formatList << Format("STL File Format"				      ,tr("STL"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> BaseMeshIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("Stanford Polygon File Format"	,tr("PLY"));
	formatList << Format("STL File Format"				      ,tr("STL"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void BaseMeshIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
	if(format.toUpper() == tr("PLY")){
		capability = vcg::tri::io::ExporterPLY<CMeshO>::GetExportMaskCapability();
		// For the default bits of the ply format disable flags and normals that usually are not useful.
		defaultBits=capability;
		defaultBits &= (~MeshModel::IOM_FLAGS);
		defaultBits &= (~MeshModel::IOM_VERTNORMAL);
	}
	if(format.toUpper() == tr("STL")){
		capability = vcg::tri::io::ExporterSTL<CMeshO>::GetExportMaskCapability();
		defaultBits=capability;
	}
}

void BaseMeshIOPlugin::initOpenParameter(const QString &format, MeshModel &/*m*/, FilterParameterSet &par) 
{
	if(format.toUpper() == tr("STL"))
		par.addBool("Unify",true, "Unify Duplicated Vertices",
								"The STL format is not an vertex-indexed format. Each triangle is composed by independent vertices, so, usually, duplicated vertices should be unified");		
}
void BaseMeshIOPlugin::initSaveParameter(const QString &format, MeshModel &/*m*/, FilterParameterSet &par) 
{
	if(format.toUpper() == tr("STL") || format.toUpper() == tr("PLY"))
		par.addBool("Binary",true, "Binary encoding",
								"Save the mesh using a binary encoding. If false the mesh is saved in a plain, readable ascii format");		
}
void BaseMeshIOPlugin::applyOpenParameter(const QString &format, MeshModel &m, const FilterParameterSet &par) 
{
	if(format.toUpper() == tr("STL"))
		if(par.getBool("Unify"))
			tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
}


const PluginInfo &BaseMeshIOPlugin::Info()
{
	static PluginInfo ai;
	ai.Date=tr("January 2006");
	ai.Version = tr("1.0");
	ai.Author = ("Paolo Cignoni");
	return ai;
 }

Q_EXPORT_PLUGIN(BaseMeshIOPlugin)