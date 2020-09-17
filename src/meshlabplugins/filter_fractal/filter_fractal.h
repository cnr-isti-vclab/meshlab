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

#ifndef FILTERFRACTALPLUGIN_H
#define FILTERFRACTALPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <common/interfaces.h>
#include "craters_utils.h"

class FilterFractal : public QObject, public MeshFilterInterface
{
    Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
    Q_INTERFACES(MeshFilterInterface)

public:
    FilterFractal();
    ~FilterFractal(){}

    QString pluginName() const;
    QString filterName(FilterIDType filter) const;
    QString filterInfo(FilterIDType filter) const;

    int getRequirements(QAction *);
    void initParameterSet(QAction*, MeshModel&, RichParameterList &){assert(0);}
    void initParameterSet(QAction *, MeshDocument &, RichParameterList &);

    bool applyFilter (const QAction* filter, MeshDocument &md, const RichParameterList & par, vcg::CallBackPos *cb);

    int postCondition(const QAction *action) const;
    FilterClass getClass(const QAction*) const;
    FILTER_ARITY filterArity(QAction* act) const;
private:
    void initParameterSetForFractalDisplacement (QAction *, MeshDocument &, RichParameterList &);
    void initParameterSetForCratersGeneration   (MeshDocument &md, RichParameterList &par);

    enum {CR_FRACTAL_TERRAIN, FP_FRACTAL_MESH, FP_CRATERS};
};

#endif
