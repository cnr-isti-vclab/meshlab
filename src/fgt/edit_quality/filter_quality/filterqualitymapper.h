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
  History
$Log$
Revision 1.2  2008/02/20 14:52:57  fbellucci
Refactoring of method necessary ti FilterQualityMapper

Revision 1.1  2008/02/18 18:05:20  amaione
UPDATED PERSONAL PROJECTS FILES
AND
ADDED PROJECT AND SRC FILES FOR FILTER QUALITY

Revision 1.1  2008/02/18 17:14:41  amaione
ADDED SKELETON FILES FOR QUALITY MAPPER FILTER

Revision 1.2  2006/11/29 00:59:21  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.1  2006/09/25 09:24:39  e_cerisoli
add sampleplugins

****************************************************************************/

#ifndef _FILTER_QUALITY_MAPPER_H_
#define _FILTER_QUALITY_MAPPER_H_

#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

#include "../transferfunction.h"
#include "../qualitymapperdialog.h"


class QualityMapperFilter : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum { FP_QUALITY_MAPPER  } ;

	QualityMapperFilter();
	
	virtual const QString filterName(FilterIDType filter);
	virtual const QString filterInfo(FilterIDType filter);
	virtual const PluginInfo &pluginInfo();
	virtual bool autoDialog(QAction *) {return true;}
	virtual void initParameterSet(QAction *,MeshModel &/*m*/, FilterParameterSet & /*parent*/);
	virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
};

#endif