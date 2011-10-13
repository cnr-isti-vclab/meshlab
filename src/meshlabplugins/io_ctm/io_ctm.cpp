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
#include <QtGui>

#include "io_ctm.h"

#include <vcg/complex/algorithms/polygon_support.h>
#include <wrap/io_trimesh/import_ctm.h>
#include <wrap/io_trimesh/export_ctm.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

bool IOMPlugin::open(const QString &/*formatName*/, const QString &fileName, MeshModel &m, int& mask,const RichParameterSet & /*par*/,  CallBackPos *cb, QWidget */*parent*/)
{
    QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
    int result = tri::io::ImporterCTM<CMeshO>::Open(m.cm, qPrintable(fileName), mask, cb);
    if (result != 0) // all the importers return 0 on success
    {
      errorMessage = errorMsgFormat.arg(fileName, tri::io::ImporterCTM<CMeshO>::ErrorMsg(result));
      return false;
    }
    return true;
}

bool IOMPlugin::save(const QString &/*formatName*/, const QString &fileName, MeshModel &m, const int mask,const RichParameterSet & par,  vcg::CallBackPos *cb, QWidget *parent)
{
    bool lossLessFlag = par.findParameter("LossLess")->val->getBool();
    float relativePrecisionParam = par.findParameter("relativePrecisionParam")->val->getFloat();
    int result = vcg::tri::io::ExporterCTM<CMeshO>::Save(m.cm,qPrintable(fileName),mask,lossLessFlag,relativePrecisionParam);
    if(result!=0)
    {
        QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
        QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(qPrintable(fileName), vcg::tri::io::ExporterCTM<CMeshO>::ErrorMsg(result)));
        return false;
    }
    return true;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> IOMPlugin::importFormats() const
{
	QList<Format> formatList;
  formatList << Format("OpenCTM compressed format"	,tr("CTM"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> IOMPlugin::exportFormats() const
{
	QList<Format> formatList;
    formatList << Format("OpenCTM compressed format"	,tr("CTM"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void IOMPlugin::GetExportMaskCapability(QString &/*format*/, int &capability, int &defaultBits) const
{
  capability=defaultBits=vcg::tri::io::ExporterCTM<CMeshO>::GetExportMaskCapability();
	return;
}
void IOMPlugin::initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterSet & par)
{
  par.addParam(new RichBool("LossLess",false, "LossLess compression",
                              "If true it does not apply any lossy compression technique."));
  par.addParam(new RichFloat("relativePrecisionParam",0.0001, "Relative Coord Precision",
                             "When using a lossy compression this number control the introduced error and hence the compression factor."
                             "It is a number relative to the average edge lenght. (e.g. the default means that the error should be roughly 1/10000 of the average edge lenght)"));
}
Q_EXPORT_PLUGIN(IOMPlugin)
