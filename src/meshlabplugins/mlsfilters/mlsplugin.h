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

#ifndef MLSPLUGIN_H
#define MLSPLUGIN_H

#include <QObject>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

class MlsPlugin : public QObject, public MeshFilterInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshFilterInterface)

public:

  enum {
		_RIMLS_ 			= 0x1,
		_APSS_				= 0x2,
		_PROJECTION_ 	= 0x1000,
		_AFRONT_			= 0X2000,
		_MCUBE_				= 0X4000,

		FP_RIMLS_PROJECTION = _RIMLS_ | _PROJECTION_,
		FP_APSS_PROJECTION	= _APSS_  | _PROJECTION_,

		FP_RIMLS_AFRONT = _RIMLS_ | _AFRONT_,
		FP_APSS_AFRONT	= _APSS_  | _AFRONT_,

		FP_RIMLS_MCUBE	= _RIMLS_ | _MCUBE_,
		FP_APSS_MCUBE		= _APSS_  | _MCUBE_
	};

  MlsPlugin();

	//virtual const QString filterName(FilterIDType filter) {return filterNames(filter);}
  virtual const QString filterName(FilterIDType filter);
  virtual const QString filterInfo(FilterIDType filter);

  virtual bool autoDialog(QAction *) {return true;}
  virtual void initParameterSet(QAction *,MeshDocument &md, FilterParameterSet &parent);
  virtual const int getRequirements(QAction *action);
  virtual bool applyFilter(QAction *filter, MeshDocument &m, FilterParameterSet &parent, vcg::CallBackPos *cb) ;
  virtual bool applyFilter(QAction *filter, MeshModel &m, FilterParameterSet &parent, vcg::CallBackPos *cb)
  {assert(0);}
};

#endif
