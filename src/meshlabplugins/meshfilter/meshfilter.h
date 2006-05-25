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
/****************************************************************************
/* History
$Log$
Revision 1.29  2006/05/25 04:57:45  cignoni
Major 0.7 release. A lot of things changed. Colorize interface gone away, Editing and selection start to work.
Optional data really working. Clustering decimation totally rewrote. History start to work. Filters organized in classes.

Revision 1.28  2006/04/12 15:12:18  cignoni
Added Filter classes (cleaning, meshing etc)

Revision 1.27  2006/02/21 15:24:47  mariolatronico
case typo on #include

Revision 1.26  2006/02/20 21:11:36  giec
bugfix

Revision 1.25  2006/02/20 20:52:37  giec
replace refine and detacher dialog whit GnericELDialog

Revision 1.24  2006/02/06 10:18:27  mariolatronico
reverted to old behaviour, removed QEdgeLength

Revision 1.23  2006/02/05 11:22:40  mariolatronico
changed spinbox to QEdgeLength widget

Revision 1.22  2006/01/31 14:40:40  mariolatronico
removed unused variable ActionInfo *ai, added Log history

****************************************************************************/

#ifndef EXTRAFILTERSPLUGIN_H
#define EXTRAFILTERSPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QList>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include "decimatorDialog.h"
#include "transformDialog.h"
#include "../../meshlab/GenericELDialog.h"
#include "../../meshlab/GLLogStream.h"

class ExtraMeshFilterPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

		public:
	/* naming convention :
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum {  FP_LOOP_SS, 
          FP_BUTTERFLY_SS, 
          FP_REMOVE_UNREFERENCED_VERTEX,
				  FP_REMOVE_DUPLICATED_VERTEX, 
          FP_REMOVE_FACES_BY_AREA,
				  FP_REMOVE_FACES_BY_EDGE,
          FP_LAPLACIAN_SMOOTH, 
          FP_DECIMATOR, 
          FP_MIDPOINT, 
          FP_REORIENT ,
          FP_INVERT_FACES,
				  FP_TRANSFORM
          } ;

	 
  
	ExtraMeshFilterPlugin();
	~ExtraMeshFilterPlugin();
	virtual const QString ST(FilterType filter);
  virtual const ActionInfo &Info(QAction *);
	virtual const PluginInfo &Info();
  virtual const FilterClass getClass(QAction *);
  virtual bool getParameters(QAction *, QWidget *, MeshModel &m, FilterParameter &par);
  virtual const int getRequirements(QAction *);


	virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameter & /*parent*/, vcg::CallBackPos * cb) ;
protected:


//	RefineDialog *refineDialog;
	DecimatorDialog *decimatorDialog;
	TransformDialog *transformDialog;
	GenericELDialog *genericELD;
};

#endif
