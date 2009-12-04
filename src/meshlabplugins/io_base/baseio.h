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
 Revision 1.3  2007/11/26 07:35:25  cignoni
 Yet another small cosmetic change to the interface of the io filters.

 Revision 1.2  2007/11/25 09:48:38  cignoni
 Changed the interface of the io filters. Now also a default bit set for the capabilities has to specified

 Revision 1.1  2006/11/30 22:55:06  cignoni
 Separated very basic io filters to the more advanced one into two different plugins baseio and meshio

 *****************************************************************************/
#ifndef BASEIOPLUGIN_H
#define BASEIOPLUGIN_H

#include <QString>

#include <common/interfaces.h>

class BaseMeshIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)

  
public:
	
	BaseMeshIOPlugin() : MeshIOInterface() {}
	
	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;

	void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const;

	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,const RichParameterSet & par, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet & par, vcg::CallBackPos *cb=0, QWidget *parent= 0);
  virtual void initOpenParameter(const QString &format, MeshModel &/*m*/, RichParameterSet & par);
	virtual void applyOpenParameter(const QString &format, MeshModel &m, const RichParameterSet &par);
	void initPreOpenParameter(const QString &formatName, const QString &filename, RichParameterSet &parlst);

  virtual void initSaveParameter(const QString &format, MeshModel &/*m*/, RichParameterSet & par);
	//const QString filterInfo(FilterIDType filter);
	//const QString filterName(FilterIDType filter);
};

#endif
