/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2020                                           \/)\/    *
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

#include "io_3mf.h"

Lib3MFPlugin::Lib3MFPlugin()
{
}

QString Lib3MFPlugin::pluginName() const
{
  return "3MF importer and exporter";
}

std::list<FileFormat> Lib3MFPlugin::importFormats() const
{
  return{FileFormat{"3MF", tr("3MF")}};
}

std::list<FileFormat> Lib3MFPlugin::exportFormats() const
{
  return{FileFormat{"3MF", tr("3MF")}};
}

void Lib3MFPlugin::open(
                        const QString& formatName,
                        const QString& fileName,
                        MeshModel& m,
                        int& mask,
                        const RichParameterList& par,
                        vcg::CallBackPos* cb)
{
}

void Lib3MFPlugin::save(
                        const QString &formatName,
                        const QString &fileName,
                        MeshModel &m,
                        const int mask,
                        const RichParameterList& par,
                        vcg::CallBackPos* cb)
{
}

void Lib3MFPlugin::exportMaskCapability(
                                        const QString &format,
                                        int& capability,
                                        int& defaultBits) const 
{
}

MESHLAB_PLUGIN_NAME_EXPORTER(Lib3MFPlugin)
