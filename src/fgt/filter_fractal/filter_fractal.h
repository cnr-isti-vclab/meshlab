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

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

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
            ~FilterFractal(){}

            virtual const QString filterName(FilterIDType filter) const;
            virtual const QString filterInfo(FilterIDType filter) const;

            virtual const int getRequirements(QAction *){return MeshModel::MM_NONE;}
            virtual bool autoDialog(QAction *){return true;}

            virtual void initParameterSet(QAction*, MeshModel&, RichParameterSet &){assert(0);}
            virtual void initParameterSet(QAction *, MeshDocument &, RichParameterSet &);

            virtual bool applyFilter(QAction *, MeshModel&, RichParameterSet &, vcg::CallBackPos *){assert(0); return false;}
            virtual bool applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);

            virtual const FilterClass getClass(QAction *);

       private:
            void computeSpectralWeights();
            bool generateTerrain(CMeshO &m, int subSteps, int algorithm, float seed);

            double fBM(void);
            double StandardMF(void);
            double HeteroMF(void);
            double HybridMF(void);
            double RidgedMF(void);

            float spectralWeight[20];
            float fArgs[9];
            enum {X=0, Y=1, Z=2, OCTAVES=3, REMAINDER=4, L=5, H=6, OFFSET=7, GAIN=8};
            enum {CR_FRACTAL_TERRAIN};
            double (FilterFractal::*vertexDisp[5]) (void);
};

#endif
