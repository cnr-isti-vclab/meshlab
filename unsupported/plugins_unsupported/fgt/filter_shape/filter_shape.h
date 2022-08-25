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

#ifndef FilterSolidShapesPLUGIN_H
#define FilterSolidShapesPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <common/interfaces.h>

class FilterSolidShapes : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

public:
    enum { FSS_PLATONIC, FSS_ARCHIMEDEAN, FSS_MISC};
    enum { CR_TETRAHEDRON, CR_HEXAHEDRON, CR_OCTAHEDRON, CR_DODECAHEDRON, CR_ICOSAHEDRON };
    enum { CR_TT, CR_COH, CR_TC, CR_TO, CR_RCOH, CR_TCOH, CR_SC, CR_ISDH, CR_TIS };
    enum { CR_TORUS23, CR_TORUS38 };

    FilterSolidShapes();
    ~FilterSolidShapes(){};

    virtual QString filterName(FilterIDType filter) const;
    virtual QString filterInfo(FilterIDType filter) const;

    virtual int getRequirements(QAction *);

    virtual bool autoDialog(QAction *) {return true;}
    virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
//  virtual bool applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
    virtual bool applyFilter(QAction * /*filter */, MeshModel &, RichParameterSet & /*parent*/, vcg::CallBackPos *);

    virtual FilterClass getClass(QAction *);

};


#endif
