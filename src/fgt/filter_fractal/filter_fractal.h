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

        private:
            bool generateTerrain(CMeshO &m, int subSteps, int algorithm,
                float seed, float octaves, float lacunarity, float fractalIncrement,
                float offset);
            void createFBMTerrain(CMeshO &m, float octaves, float seedFactor,
                float lacunarity, float fractalIncrement);
            void createMFTerrain(CMeshO &m, float octaves, float seedFactor,
                float lacunarity, float fractalIncrement, float offset);
            void createHeterogeneousMFTerrain(CMeshO &m, float octaves, float seedFactor,
                float lacunarity, float fractalIncrement, float offset);

	public:
            enum {CR_FRACTAL_TERRAIN} ;
		
            FilterFractal();
            ~FilterFractal();

            virtual const QString filterName(FilterIDType filter) const;
            virtual const QString filterInfo(FilterIDType filter) const;

            virtual const int getRequirements(QAction *);
            virtual bool autoDialog(QAction *);
            virtual void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*par*/);
            virtual bool applyFilter(QAction * /*filter*/, MeshModel &/*m*/, RichParameterSet & /* par */, vcg::CallBackPos * /*cb*/);
            virtual bool applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb);
            virtual const FilterClass getClass(QAction *);
};

#endif
