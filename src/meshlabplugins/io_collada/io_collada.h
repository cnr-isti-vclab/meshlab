/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
 Revision 1.5  2007/11/26 07:35:27  cignoni
 Yet another small cosmetic change to the interface of the io filters.

 Revision 1.4  2007/11/25 09:48:39  cignoni
 Changed the interface of the io filters. Now also a default bit set for the capabilities has to specified

 Revision 1.3  2006/11/29 00:59:16  cignoni
 Cleaned plugins interface; changed useless help class into a plain string

 Revision 1.2  2006/09/22 06:08:17  granzuglia
 colladaio.pro updated with support for FCollada 1.13

 Revision 1.1  2006/06/19 13:42:53  granzuglia
 collada importer

 *****************************************************************************/
#ifndef COLLADAIOPLUGIN_H
#define COLLADAIOPLUGIN_H

#include <QObject>
#include <common/interfaces.h>

class ColladaIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
 public:
	 //std::map<MeshModel*,typename vcg::tri::io::InfoDAE*> _mp;
	 std::vector<MeshModel*> _mp;

	QList<Format> importFormats() const;
  QList<Format> exportFormats() const;

	void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const;
	void initPreOpenParameter(const QString &/*format*/, const QString &/*fileName*/, RichParameterSet & /*par*/);
	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &par, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb=0, QWidget *parent= 0);
};

#endif
