/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#ifndef IO_NXS_PLUGIN_H
#define IO_NXS_PLUGIN_H

#include <common/plugins/interfaces/io_plugin.h>
#include <common/plugins/interfaces/filter_plugin.h>

class FilterIONXSPlugin : public QObject, public IOPlugin, public FilterPlugin
{ 
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(IO_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin IOPlugin)

public:
	FilterIONXSPlugin();
	~FilterIONXSPlugin() {}
	QString pluginName() const;

	//IOPlugin interface
	std::list<FileFormat> importFormats() const;
	std::list<FileFormat> exportFormats() const;

	void open(
			const QString &format, /// the extension of the format e.g. "PLY"
			const QString &fileName, /// The name of the file to be opened
			MeshModel &m, /// The mesh that is filled with the file content
			int &mask, /// a bit mask that will be filled reporting what kind of data we have found in the file (per vertex color, texture coords etc)
			const RichParameterList & par, /// The parameters that have been set up in the initPreOpenParameter()
			vcg::CallBackPos *cb = nullptr); /// standard callback for reporting progress in the loading

	void exportMaskCapability(
			const QString &format,
			int &capability,
			int &defaultBits) const;

	RichParameterList initSaveParameter(
			const QString &format,
			const MeshModel &/*m*/) const;

	void save(
			const QString &format, // the extension of the format e.g. "PLY"
			const QString &fileName,
			MeshModel &m,
			const int mask,// a bit mask indicating what kind of the data present in the mesh should be saved (e.g. you could not want to save normals in ply files)
			const RichParameterList & par,
			vcg::CallBackPos *cb = 0);

	//FilterPlugin interface
	enum FileterIds {
		FP_NXS_BUILDER,
		FP_NXS_COMPRESS
	};

	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FilterArity filterArity(const QAction*) const;
	int getPreConditions(const QAction *) const;
	int postCondition(const QAction* ) const;
	RichParameterList initParameterList(const QAction*, const MeshModel &/*m*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & params,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);

private:
	RichParameterList nxsParameters() const;
	RichParameterList nxzParameters(bool categorize) const;

	void buildNxs(
			const QString& outputFile,
			const RichParameterList& params,
			const MeshModel* m,
			int mask);

	void compressNxs(
			const QString& inputFile,
			const QString& outputFile,
			const RichParameterList& params);
};

#endif //IO_NXS_PLUGIN_H
