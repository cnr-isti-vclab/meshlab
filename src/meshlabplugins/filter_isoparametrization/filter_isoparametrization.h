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

#ifndef FILTERISOPARAMPLUGIN_H
#define FILTERISOPARAMPLUGIN_H

#include <QObject>
#include <cassert>
#include <parametrizator.h>
#include <iso_parametrization.h>
#include <diamond_sampler.h>
#include <diam_parametrization.h>
#include <stat_remeshing.h>
#include <common/interfaces.h>

class FilterIsoParametrization : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

	public:
		enum { ISOP_PARAM,
           ISOP_REMESHING,
		   ISOP_DIAMPARAM,
		   ISOP_LOAD,
		   ISOP_SAVE
       };

		FilterIsoParametrization();
		~FilterIsoParametrization();

        virtual FilterClass getClass(QAction *);
		virtual QString filterName(FilterIDType filter) const;
		virtual QString filterInfo(FilterIDType filter) const;

		virtual int getRequirements(QAction *);

        virtual void initParameterSet(QAction *,MeshDocument&, RichParameterSet & /*parent*/);
        virtual bool applyFilter(QAction *filter, MeshDocument&, RichParameterSet & /*parent*/, vcg::CallBackPos * cb);
        int postCondition(QAction* filter) const;
		void PrintStats(CMeshO *mesh);
		AbstractMesh abs_mesh;
		ParamMesh para_mesh;
		//IsoParametrization IsoParam;
		CMeshO::PerMeshAttributeHandle<IsoParametrization> isoPHandle;
		DiamSampler DiamSampl;
    };
#endif
