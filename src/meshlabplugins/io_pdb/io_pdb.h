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

#ifndef PDBIOPLUGIN_H
#define PDBIOPLUGIN_H

#include <common/interfaces/io_plugin_interface.h>
#include <common/mesh_data_structures/meshmodel.h>

class PDBIOPlugin : public QObject, public IOPluginInterface
{
  Q_OBJECT
    MESHLAB_PLUGIN_IID_EXPORTER(IO_PLUGIN_INTERFACE_IID)
  Q_INTERFACES(IOPluginInterface)

  
public:
	QString pluginName() const;

	QList<Format> importFormats() const;
	QList<Format> exportFormats() const;

	void GetExportMaskCapability(const QString &format, int &capability, int &defaultBits) const;

	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,const RichParameterList & par, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterList & par, vcg::CallBackPos *cb=0, QWidget *parent= 0);
  virtual void initOpenParameter(const QString &format, MeshModel &/*m*/, RichParameterList & par);
	virtual void applyOpenParameter(const QString &format, MeshModel &m, const RichParameterList &par);
	void initPreOpenParameter(const QString &formatName, const QString &filename, RichParameterList &parlst);

  virtual void initSaveParameter(const QString &format, MeshModel &/*m*/, RichParameterList & par);

	//---------- PDB READER -----------//
	bool parsePDB(const std::string &filename, CMeshO &m, const RichParameterList &parlst, vcg::CallBackPos *cb=0); 
	void mysscanf(const char* st, float *f);
    void mysscanf(const char* st, double *f);
	float getAtomRadius(const char* atomicElementCharP);
	vcg::Color4b getAtomColor(const char* atomicElementCharP);

private:

	std::vector<std::string > atomDetails;
	std::vector<Point3m> atomPos;
	std::vector<vcg::Color4b> atomCol;
	std::vector<float>        atomRad;


};

#endif
