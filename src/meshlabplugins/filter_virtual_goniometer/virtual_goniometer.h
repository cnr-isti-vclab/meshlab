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

#ifndef FILTER_SELECT_H
#define FILTER_SELECT_H

#include <QObject>
#include <common/interfaces.h>


class VirtualGoniometerFilterPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)
		
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
	QString filterInfo(FilterIDType filter) const;
   QString filterName(FilterIDType filter) const;
   FilterClass getClass(QAction *);

   void initParameterSet(QAction *action, MeshModel &m, RichParameterList &parlst);
   int getPreConditions(QAction *) const;
   int postCondition( QAction* ) const;
   int getRequirements(QAction *);
   bool applyFilter(QAction* filter, MeshDocument& md, const RichParameterList& par, vcg::CallBackPos* cb) ;
   FILTER_ARITY filterArity(QAction *) const {return SINGLE_MESH;}
};

#endif
