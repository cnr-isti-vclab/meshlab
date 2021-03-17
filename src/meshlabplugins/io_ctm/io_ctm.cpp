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
 $Log: meshio.cpp,v $
*****************************************************************************/
#include <Qt>

#include "io_ctm.h"

#include <vcg/complex/algorithms/polygon_support.h>
#include <wrap/io_trimesh/import_ctm.h>
#include <wrap/io_trimesh/export_ctm.h>

#include <QFileDialog>

using namespace vcg;

bool IOMPlugin::open(const QString & /*formatName*/, const QString &fileName, MeshModel &m, int& mask,const RichParameterList & /*par*/,  CallBackPos *cb, QWidget * /*parent*/)
{
    QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
    int result = tri::io::ImporterCTM<CMeshO>::Open(m.cm, qUtf8Printable(fileName), mask, cb);
    if (result != 0) // all the importers return 0 on success
    {
      errorMessage = errorMsgFormat.arg(fileName, tri::io::ImporterCTM<CMeshO>::ErrorMsg(result));
      return false;
    }
    return true;
}

bool IOMPlugin::save(const QString & /*formatName*/, const QString &fileName, MeshModel &m, const int mask,const RichParameterList & par,  vcg::CallBackPos * /*cb*/, QWidget *parent)
{
	bool lossLessFlag = par.getBool("LossLess");
	Scalarm relativePrecisionParam = par.getFloat("relativePrecisionParam");
	int result = vcg::tri::io::ExporterCTM<CMeshO>::Save(m.cm,qUtf8Printable(fileName),mask,lossLessFlag,relativePrecisionParam);
	if(result!=0)
	{
		QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
		errorMessage = "Saving Error: " + errorMsgFormat.arg(qUtf8Printable(fileName), vcg::tri::io::ExporterCTM<CMeshO>::ErrorMsg(result));
		return false;
	}
	return true;
}

/*
	returns the list of the file's type which can be imported
*/
QString IOMPlugin::pluginName() const
{
	return "IOCTM";
}

std::list<FileFormat> IOMPlugin::importFormats() const
{
	return { FileFormat("OpenCTM compressed format"	,tr("CTM"))};
}

/*
	returns the list of the file's type which can be exported
*/
std::list<FileFormat> IOMPlugin::exportFormats() const
{
	return {FileFormat("OpenCTM compressed format" ,tr("CTM"))};
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void IOMPlugin::exportMaskCapability(const QString &/*format*/, int &capability, int &defaultBits) const
{
  capability=defaultBits=vcg::tri::io::ExporterCTM<CMeshO>::GetExportMaskCapability();
	return;
}
void IOMPlugin::initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterList & par)
{
  par.addParam(RichBool("LossLess",false, "LossLess compression",
                              "If true it does not apply any lossy compression technique."));
  par.addParam(RichFloat("relativePrecisionParam",0.0001f, "Relative Coord Precision",
                             "When using a lossy compression this number control the introduced error and hence the compression factor."
                             "It is a number relative to the average edge length. (e.g. the default means that the error should be roughly 1/10000 of the average edge length)"));
}
MESHLAB_PLUGIN_NAME_EXPORTER(IOMPlugin)
