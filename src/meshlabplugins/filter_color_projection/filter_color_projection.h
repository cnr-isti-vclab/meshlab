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

#ifndef FILTER_COLORPROJ_H
#define FILTER_COLORPROJ_H

#include <QObject>
#include <common/plugins/interfaces/filter_plugin_interface.h>

class FilterColorProjectionPlugin : public QObject, public FilterPluginInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_INTERFACE_IID)
	Q_INTERFACES(FilterPluginInterface)
	
	public:
		enum { FP_SINGLEIMAGEPROJ, FP_MULTIIMAGETRIVIALPROJ, FP_MULTIIMAGETRIVIALPROJTEXTURE };
	
	FilterColorProjectionPlugin();
	
	QString pluginName() const;
	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	int postCondition( const QAction* ) const;
	
	FilterClass getClass(const QAction*) const;
	void initParameterList(const QAction*, MeshDocument &/*m*/, RichParameterList & /*parent*/);
	int getRequirements(const QAction*);
	bool requiresGLContext(const QAction* action) const;
	bool applyFilter(const QAction* filter, MeshDocument &md, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList & /*parent*/, vcg::CallBackPos * cb);
	
	FILTER_ARITY filterArity(const QAction *) const {return SINGLE_MESH;}
	
private:
	int calculateNearFarAccurate(MeshDocument &md, std::vector<float> *near, std::vector<float> *far);
};

#endif
