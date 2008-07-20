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

#ifndef FILTERDIRTPLUGIN_H
#define FILTERDIRTPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>


class FilterDirt : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

	public:
		enum {FP_DIRT} ;
		
		FilterDirt();
		~FilterDirt(){};

		virtual const QString filterName(FilterIDType filter);
		virtual const QString filterInfo(FilterIDType filter);
		virtual const PluginInfo &pluginInfo();

		virtual const int getRequirements(QAction *);

		virtual bool autoDialog(QAction *) {return false;}
		virtual void initParameterSet(QAction *,MeshModel &/*m*/, FilterParameterSet & /*parent*/){};
		virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
		virtual float FilterDirt::mediumVertexDistance(CFaceO* face);
		virtual const FilterClass getClass(QAction *);



	/*protected:
		const int defaultDustTon;*/
};


#endif
