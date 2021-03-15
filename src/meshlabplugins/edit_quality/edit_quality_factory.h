/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2008                                           \/)\/    *
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


#ifndef QualityMapperFactoryPLUGIN_H
#define QualityMapperFactoryPLUGIN_H

#include <QObject>
#include <common/plugins/interfaces/edit_plugin.h>

class QualityMapperFactory : public QObject, public EditPluginFactory
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(EDIT_PLUGIN_FACTORY_IID)
	Q_INTERFACES(EditPluginFactory)

public:
	QualityMapperFactory();
	virtual ~QualityMapperFactory() { delete editQuality; }

	virtual QString pluginName() const;

	//get the edit tool for the given action
	virtual EditTool* getEditTool(const QAction*);

	//get the description for the given action
	virtual QString getEditToolDescription(const QAction*);
private:
	QAction *editQuality;
};

#endif

