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

#include "io_m.h"

#include <wrap/io_trimesh/export_smf.h>
#include <wrap/io_trimesh/import_smf.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/polygon_support.h>
#include "export_m.h"

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

bool IOMPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,const RichParameterSet & par,  CallBackPos *cb, QWidget *parent)
{
    assert(0);
	return true;
}

bool IOMPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask,const RichParameterSet & par,  vcg::CallBackPos *cb, QWidget *parent)
{
	QString errorMsgFormat = "Error encountered while exportering file %1:\n%2";
    m.updateDataMask(MeshModel::MM_FACEFACETOPO);
    int result = vcg::tri::io::ExporterM<CMeshO>::Save(m.cm,qPrintable(fileName),mask);
    if(par.getBool("HtmlSnippet"))
    {
        vcg::tri::io::ExporterM<CMeshO>::WriteHtmlSnippet(qPrintable(fileName),qPrintable(QString(fileName)+".html"));
    }
	if(result!=0)
	{
        QMessageBox::warning(parent, tr("Saving Error"), errorMsgFormat.arg(qPrintable(fileName), vcg::tri::io::ExporterM<CMeshO>::ErrorMsg(result)));
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
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> IOMPlugin::exportFormats() const
{
	QList<Format> formatList;
    formatList << Format("LiveGraphics3D applet"	,tr("M"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void IOMPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
  capability=defaultBits=vcg::tri::io::ExporterM<CMeshO>::GetExportMaskCapability();
	return;
}
void IOMPlugin::initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterSet & par)
{
    par.addParam(new RichBool("HtmlSnippet",true, "HTML Snippet",
                                "If true save an HTML snippet ready to be included in a web page"));
}
Q_EXPORT_PLUGIN(IOMPlugin)
