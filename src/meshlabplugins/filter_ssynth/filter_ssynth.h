/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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

#ifndef FILTER_SSYNTH_H
#define FILTER_SSYNTH_H



#include <QObject>
#include <common/plugins/interfaces/filter_plugin.h>
#include <meshlabplugins/io_x3d/io_x3d.h>

class FilterSSynth : public QObject, public IOPlugin, public FilterPlugin{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin IOPlugin)
	public:
		enum {CR_SSYNTH} ;

	FilterSSynth();
	~FilterSSynth(){}
	QString pluginName() const;
	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;
	int getRequirements(const QAction*);
	RichParameterList initParameterList(const QAction*, const MeshDocument &/*m*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);
	FilterClass getClass(const QAction* filter) const;
	void setAttributes(CMeshO::VertexIterator &vi, CMeshO &m);
	static void openX3D(const QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb, QWidget *parent=0);
	int postCondition(const QAction* filter) const;
	std::list<FileFormat> importFormats() const;
	std::list<FileFormat> exportFormats() const;

	void exportMaskCapability(const QString &format, int &capability, int &defaultBits) const;
	RichParameterList initPreOpenParameter(const QString&formatName) const;
	void open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList & par, vcg::CallBackPos *cb);
	void save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterList &, vcg::CallBackPos *cb);
	FilterPlugin::FilterArity filterArity(const QAction *) const {return NONE;}
private:
	QString ssynth(QString grammar,int maxdepth,int seed,vcg::CallBackPos *cb);
	QString GetTemplate(int sphereres);
	void ParseGram(QString* grammar,int max,QString pattern);
	int seed;
	QString renderTemplate;
	QString spheres[6];
};
#endif // FILTER_SSYNTH_H
