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

#include <common/plugins/interfaces/io_plugin.h>
#include <common/ml_document/mesh_model.h>

class PDBIOPlugin : public QObject, public IOPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(IO_PLUGIN_IID)
	Q_INTERFACES(IOPlugin)

  
public:
	QString pluginName() const;

	std::list<FileFormat> importFormats() const;
	std::list<FileFormat> exportFormats() const;

	void exportMaskCapability(const QString &format, int &capability, int &defaultBits) const;

	void open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,const RichParameterList & par, vcg::CallBackPos *cb=0);
	void save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterList & par, vcg::CallBackPos *cb=0);
	void initPreOpenParameter(const QString &formatName, RichParameterList &parlst);

	virtual void initSaveParameter(const QString &format, const MeshModel &/*m*/, RichParameterList & par);

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
