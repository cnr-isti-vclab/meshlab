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

#include <common/meshmodel.h>
#include <common/interfaces.h>
#include "fractal_perturbation.h"

class FilterFractal : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

        typedef CMeshO::CoordType       CoordType;
        typedef CMeshO::VertexPointer   VertexPointer;
        typedef CMeshO::VertexIterator  VertexIterator;
        typedef CMeshO::FacePointer     FacePointer;
        typedef CMeshO::FaceIterator    FaceIterator;

        public:
            FilterFractal();
            ~FilterFractal(){delete fractalArgs;}

            virtual QString filterName(FilterIDType filter) const;
            virtual QString filterInfo(FilterIDType filter) const;

            virtual int getRequirements(QAction *);
            virtual bool autoDialog(QAction *){return true;}

            virtual void initParameterSet(QAction*, MeshModel&, RichParameterSet &){assert(0);}
            virtual void initParameterSet(QAction *, MeshDocument &, RichParameterSet &);

            virtual bool applyFilter(QAction *, MeshModel&, RichParameterSet &, vcg::CallBackPos *){assert(0); return false;}
            virtual bool applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
            virtual int postCondition(QAction *action) const;

            virtual FilterClass getClass(QAction *);

       private:
            void initParameterSetForFractalDisplacement (QAction *, MeshDocument &, RichParameterSet &);
            void initParameterSetForCratersGeneration   (MeshDocument &md, RichParameterSet &par);
            bool applyFractalDisplacementFilter (QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);

            double computeFractalPerturbation(CoordType &point);
            bool generateTerrain    (MeshModel &mm, vcg::CallBackPos* cb);
            bool generateFractalMesh(MeshModel &mm, vcg::CallBackPos* cb);
            bool generateCraters    (MeshDocument &md, RichParameterSet &par);

            enum {CR_FRACTAL_TERRAIN, FP_FRACTAL_MESH, FP_CRATERS};

            FractalArgs* fractalArgs;
};

#endif
