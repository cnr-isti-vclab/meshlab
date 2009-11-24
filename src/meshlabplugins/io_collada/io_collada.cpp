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
 Revision 1.19  2008/04/08 10:16:04  cignoni
 added missing std:: and vcg::

 Revision 1.18  2008/01/30 08:36:33  granzuglia
 temporary colladaio interface...TO BE REPLACED!!!!

 Revision 1.17  2007/11/26 07:35:27  cignoni
 Yet another small cosmetic change to the interface of the io filters.

 Revision 1.16  2007/11/25 09:48:39  cignoni
 Changed the interface of the io filters. Now also a default bit set for the capabilities has to specified

 Revision 1.15  2007/08/25 08:42:45  cignoni
 cleaned include files

 Revision 1.14  2007/04/16 09:25:28  cignoni
 ** big change **
 Added Layers managemnt.
 Interfaces are changing again...

 Revision 1.13  2007/03/20 16:23:08  cignoni
 Big small change in accessing mesh interface. First step toward layers

 Revision 1.12  2007/03/20 15:52:46  cignoni
 Patched issue related to path with non ascii chars

 Revision 1.11  2006/11/29 00:59:16  cignoni
 Cleaned plugins interface; changed useless help class into a plain string

 Revision 1.10  2006/11/27 06:57:19  cignoni
 Wrong way of using the __DATE__ preprocessor symbol

 Revision 1.9  2006/11/09 02:20:36  granzuglia
 cleaner version

 Revision 1.8  2006/11/08 17:57:20  granzuglia
 texture loading

 Revision 1.7  2006/11/07 17:26:01  cignoni
 small gcc compiling issues

 Revision 1.6  2006/11/07 09:26:10  granzuglia
 fixed bug: added mask parameter

 Revision 1.5  2006/11/06 05:30:32  granzuglia
 great changes

 Revision 1.4  2006/11/05 19:21:09  granzuglia
 colladaio updated

 Revision 1.3  2006/09/22 06:08:17  granzuglia
 colladaio.pro updated with support for FCollada 1.13

 Revision 1.2  2006/07/07 06:57:04  granzuglia
 added the save function

 Revision 1.1  2006/06/19 13:42:53  granzuglia
 collada importer

*****************************************************************************/

#include <algorithm>

#include <Qt>
#include <QtGui>

#include "io_collada.h"

//#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/texture.h>
#include <wrap/io_trimesh/export.h>
#include <wrap/io_trimesh/io_mask.h>
#include <wrap/io_trimesh/import_dae.h>
#include <wrap/io_trimesh/export_dae.h>

#include <QMessageBox>
using namespace std;
using namespace vcg;

bool ColladaIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &, CallBackPos *cb, QWidget *parent)
{
	// initializing mask
  mask = 0;
	
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
	string filename = QFile::encodeName(fileName).constData ();
  //std::string filename = fileName.toUtf8().data();

	bool normalsUpdated = false;

	if(formatName.toUpper() == tr("DAE"))
	{
		//m.addinfo = NULL;
        tri::io::InfoDAE  info;
		if (!tri::io::ImporterDAE<CMeshO>::LoadMask(filename.c_str(), info))
			return false;

        m.Enable(info.mask);
	//	for(unsigned int tx = 0; tx < info->texturefile.size();++tx)
	//		m.cm.textures.push_back(info->texturefile[tx].toStdString());
		
		int result = vcg::tri::io::ImporterDAE<CMeshO>::Open(m.cm, filename.c_str(),info);
		
		if (result != vcg::tri::io::ImporterDAE<CMeshO>::E_NOERROR)
		{
			//QMessageBox::critical(parent, tr("DAE Opening Error"), errorMsgFormat.arg(fileName, vcg::tri::io::ImporterDAE<CMeshO>::ErrorMsg(result)));
			qDebug() << "DAE Opening Error" << vcg::tri::io::ImporterDAE<CMeshO>::ErrorMsg(result) << endl;
			return false;
		}
		else _mp.push_back(&m);

        if(info.mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
			normalsUpdated = true;
        mask = info.mask;
	}
	
	vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
	if (!normalsUpdated) 
		vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);		// updates normals

	if (cb != NULL)	(*cb)(99, "Done");

	return true;
}

void ColladaIOPlugin::initPreOpenParameter(const QString &/*format*/, const QString &filename, RichParameterSet & parlst)
{
	QTime t;
	t.start();
	
	QDomDocument* doc = new QDomDocument(filename);
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
				return;
	if (!doc->setContent(&file)) 
	{
				file.close();
				return;
	}
	file.close();
	
	QDomNodeList geomList = doc->elementsByTagName("geometry");
	QStringList idList;
	idList.push_back("Full Scene");
	for(int i=0;i<geomList.size();++i)
	{
		QString idVal = geomList.at(i).toElement().attribute("id");
		idList.push_back(idVal);
		qDebug("Node %i geom id = '%s'",i,qPrintable(idVal));
	}
	parlst.addParam(new RichEnum("geomnode",0, idList, tr("geometry nodes"),  tr("dsasdfads")));
	qDebug("Time elapsed: %d ms", t.elapsed());
}



bool ColladaIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
	string filename = QFile::encodeName(fileName).constData ();
  //std::string filename = fileName.toUtf8().data();
	std::string ex = formatName.toUtf8().data();
	int result;
  tri::Allocator<CMeshO>::CompactVertexVector(m.cm);  
  tri::Allocator<CMeshO>::CompactFaceVector(m.cm);  
	// Collada exporting function do not manage very correctly the case
    // of null texture index faces (e.g. faces that have no texture and have a default -1 tex index.
    // so we convert it to a more standard mesh adding a fake notexture.png texture.
	if(tri::HasPerWedgeTexCoord(m.cm))
			tri::UpdateTexture<CMeshO>::WedgeTexRemoveNull(m.cm,"notexture.png");
	
	//if (std::find(_mp.begin(),_mp.end(),&m) == _mp.end())
		result = vcg::tri::io::ExporterDAE<CMeshO>::Save(m.cm,filename.c_str(),mask);
	//else 
		//result = vcg::tri::io::ExporterDAE<CMeshO>::Save(m.cm,filename.c_str(),m.addinfo,mask);

	if(result!=0)
	{
		//QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(fileName, vcg::tri::io::Exporter<CMeshO>::ErrorMsg(result)));
		qDebug() << "Saving Error" << vcg::tri::io::Exporter<CMeshO>::ErrorMsg(result) << endl;
		return false;
	}
	return true;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> ColladaIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("Collada File Format"	,tr("DAE"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> ColladaIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	formatList << Format("Collada File Format"	,tr("DAE"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void ColladaIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits)  const 
{
	if(format.toUpper() == tr("DAE")){
		capability = defaultBits = vcg::tri::io::ExporterDAE<CMeshO>::GetExportMaskCapability();
		return; 
	}
	assert(0);
}

Q_EXPORT_PLUGIN(ColladaIOPlugin)
