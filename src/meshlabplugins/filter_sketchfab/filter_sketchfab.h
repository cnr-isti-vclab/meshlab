/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2013                                                \/)\/    *
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

#ifndef FILTERSKETCHFAB_H
#define FILTERSKETCHFAB_H

#include <common/plugins/interfaces/filter_plugin.h>

class QHttpPart;

class FilterSketchFabPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

public:
	enum { FP_SKETCHFAB  } ;

	FilterSketchFabPlugin();

	QString pluginName() const;
	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;
	FilterClass getClass(const QAction* a) const;
	FILTER_ARITY filterArity(const QAction* a) const;
	int getPreConditions(const QAction*) const;
	int postCondition(const QAction* ) const;
	void initParameterList(const QAction*, MeshModel &/*m*/, RichParameterList & /*parent*/);
	std::map<std::string, QVariant> applyFilter(
			const QAction* action,
			const RichParameterList & parameters,
			MeshDocument &md,
			unsigned int& postConditionMask,
			vcg::CallBackPos * cb);
public slots:
	void finished();
	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
	std::string sketchfab(MeshDocument &md,
			vcg::CallBackPos* cb,
			const QString& apiToken,
			const QString&,
			const QString&,
			const QString&,
			bool,
			bool,
			bool,
			bool);

	bool upload(
			const QString& zipFileName,
			const QString& apiToken,
			const QString& name,
			const QString& description,
			const QString& tags,
			const QString& isPrivate,
			const QString& isPublished,
			std::string& urlModel);

	QHttpPart part_parameter(QString key, QString value);

	int saveMeshZip(
			const std::string& fileName,
			const std::string& internalName,
			const std::string& zipName);

	bool uploadCompleteFlag;
	vcg::CallBackPos * fcb;
	const QString DEFAULT_API = "00000000";

};

#endif //FILTERSKETCHFAB_H
