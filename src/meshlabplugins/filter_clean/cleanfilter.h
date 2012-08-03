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

#ifndef __CLEAN_FILTER_H__
#define __CLEAN_FILTER_H__

#include <QObject>
#include <common/interfaces.h>

class CleanFilter : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

		public:
	/* naming convention :
		 - FP -> Filter Plugin
		 - name of the plugin separated by _
	*/
	enum { 
    FP_BALL_PIVOTING, 
    FP_REMOVE_ISOLATED_COMPLEXITY, 
    FP_REMOVE_ISOLATED_DIAMETER, 
    FP_REMOVE_WRT_Q,
    FP_REMOVE_TVERTEX_FLIP,
    FP_SNAP_MISMATCHED_BORDER,
    FP_REMOVE_TVERTEX_COLLAPSE,
    FP_REMOVE_FOLD_FACE,
    FP_REMOVE_DUPLICATE_FACE,
    FP_REMOVE_NON_MANIF_EDGE,
    FP_REMOVE_NON_MANIF_VERT,
		FP_MERGE_CLOSE_VERTEX,
	FP_COMPACT_VERT,
	FP_COMPACT_FACE
  } ;
	
	/* default values for standard parameters' values of the plugin actions */
    float maxDiag1;
    float maxDiag2;
    int minCC;
    float val1;


	CleanFilter();
	~CleanFilter();

	virtual QString filterName(FilterIDType filter) const;
	virtual QString filterInfo(FilterIDType filter) const;

  virtual FilterClass getClass(QAction *);
  virtual int getRequirements(QAction *);

  virtual void initParameterSet(QAction *,MeshDocument &/*m*/, RichParameterSet & /*parent*/);
  virtual bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
	
};






#endif
