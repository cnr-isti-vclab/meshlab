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

#ifndef FILTERCSGPLUGIN_H
#define FILTERCSGPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <common/meshmodel.h>
#include <common/interfaces.h>


class FilterCSG : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

    enum {
        CSG_OPERATION_INTERSECTION  = 0,
        CSG_OPERATION_UNION         = 1,
        CSG_OPERATION_DIFFERENCE    = 2
    };

public:
    enum {FP_CSG};

    FilterCSG();
    ~FilterCSG() {};

    virtual QString filterName(FilterIDType filter) const;
    virtual QString filterInfo(FilterIDType filter) const;

    virtual bool autoDialog(QAction *) { return true; }

    virtual void initParameterSet(QAction *, MeshDocument &, RichParameterSet &);
    virtual void initParameterSet(QAction *, MeshModel &, RichParameterSet &) { assert(0); }

    virtual bool applyFilter(QAction *, MeshDocument &, RichParameterSet &, vcg::CallBackPos *);
    virtual bool applyFilter(QAction *, MeshModel &, RichParameterSet &, vcg::CallBackPos *) { assert(0); return false; }

    virtual FilterClass getClass(QAction *) { return MeshFilterInterface::Layer; }
};


#endif
