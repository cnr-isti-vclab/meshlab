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
/****************************************************************************
History

$Log: FilterCreateIso.h,v $
Revision 1.1  2007/12/02 07:57:48  cignoni
Added the new sample filter plugin that removes border faces


*****************************************************************************/
#ifndef FILTERCREATEISO_PLUGIN_H
#define FILTERCREATEISO_PLUGIN_H

#include <QObject>
#include <common/plugins/interfaces/filter_plugin.h>


class FilterCreateIso : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)

	public:
		/* naming convention :
		- FP -> Filter Plugin
		- name of the plugin separated by _
		*/
		enum {
		FP_CREATEISO,
	} ;

	/* default values for standard parameters' values of the plugin actions */
	FilterCreateIso();
	~FilterCreateIso();

	QString pluginName() const;
	QString filterName(ActionIDType filter) const;
	QString filterInfo(ActionIDType filter) const;

	FilterClass getClass(const QAction*) const;
	int getRequirements(const QAction*);
	RichParameterList initParameterList(const QAction*, const MeshModel &/*m*/);

	std::map<std::string, QVariant> applyFilter(const QAction* action, const RichParameterList & /*parent*/, MeshDocument &md, unsigned int& postConditionMask, vcg::CallBackPos * cb);
	FilterArity filterArity(const QAction*) const {return NONE;}

};


#endif
