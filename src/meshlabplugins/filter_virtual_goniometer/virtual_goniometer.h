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

#ifndef VIRTUAL_GONIOMETER_FILLTER_H
#define VIRTUAL_GONIOMETER_FILLTER_H

#include <QObject>
#include <common/interfaces/filter_plugin_interface.h>

class VirtualGoniometerFilterPlugin : public QObject, public FilterPluginInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_INTERFACE_IID)
	Q_INTERFACES(FilterPluginInterface)
		
		public:
	/* naming convention : 
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum { 
      FP_QUALITY_VIRTUAL_GONIOMETER,
      FP_QUALITY_VIRTUAL_GONIOMETER_NEXT,
      FP_QUALITY_VIRTUAL_GONIOMETER_UNDO,
      FP_QUALITY_VIRTUAL_GONIOMETER_RESET
	} ;

	VirtualGoniometerFilterPlugin();
  //~VirtualGoniometerFilterPlugin();
	
	QString pluginName() const;
	
	QString filterInfo(FilterIDType filter) const;
   QString filterName(FilterIDType filter) const;
   FilterClass getClass(const QAction *) const;

   void initParameterSet(QAction *action, MeshModel &m, RichParameterList &parlst);
   int getPreConditions(const QAction *) const;
   int postCondition(const QAction* ) const;
   int getRequirements(const QAction *);
   bool applyFilter(const QAction* filter, MeshDocument& md, std::map<std::string, QVariant>&, unsigned int&, const RichParameterList& par, vcg::CallBackPos* cb) ;
   FILTER_ARITY filterArity(const QAction *) const {return SINGLE_MESH;}
};

#endif //VIRTUAL_GONIOMETER_FILLTER_H
