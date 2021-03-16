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

class FilterSSynth : public QObject, public IOMeshPlugin, public FilterPlugin{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin IOMeshPlugin)
	public:
		enum {CR_SSYNTH} ;

	FilterSSynth();
	~FilterSSynth(){}
	QString pluginName() const;
	virtual QString filterName(ActionIDType filter) const;
	virtual QString filterInfo(ActionIDType filter) const;
	virtual int getRequirements(const QAction*);
	virtual void initParameterList(const QAction* /*filter*/,MeshModel &,RichParameterList &){};
	virtual void initParameterList(const QAction*, MeshDocument &/*m*/, RichParameterList & /*parent*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);
	virtual FilterClass getClass(const QAction* filter) const;
	void setAttributes(CMeshO::VertexIterator &vi, CMeshO &m);
	static void openX3D(const QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb, QWidget *parent=0);
	virtual int postCondition(const QAction* filter) const;
	std::list<FileFormat> importFormats() const;
	std::list<FileFormat> exportFormats() const;

	virtual void exportMaskCapability(const QString &format, int &capability, int &defaultBits) const;
	void initPreOpenParameter(const QString &formatName, const QString &filename, RichParameterList &parlst);
	bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList & par, vcg::CallBackPos *cb=0, QWidget *parent=0);
	bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterList &, vcg::CallBackPos *cb, QWidget *parent);
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
