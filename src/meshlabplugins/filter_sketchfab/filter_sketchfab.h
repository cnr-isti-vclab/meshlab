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

#include <common/interfaces.h>
#include <QHttpPart>

class FilterSketchFabPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_SKETCHFAB  } ;

	FilterSketchFabPlugin();

	QString pluginName() const;
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	FilterClass getClass(QAction *a);
	FILTER_ARITY filterArity(QAction *a) const;
	int getPreConditions(QAction *) const;
	int postCondition( QAction* ) const;
	void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterList & /*parent*/);
	bool applyFilter(QAction *filter, MeshDocument &md, const RichParameterList & /*parent*/, vcg::CallBackPos * cb) ;

public slots:
	void finished();
	void uploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
	bool sketchfab(MeshDocument &md,
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
