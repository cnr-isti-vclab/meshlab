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

class FractalArgs
{
public:
    float octaves, remainder, l, h, offset, gain, seed, scale;
    int subdivisionSteps, algorithmId, smoothingSteps;
    bool saveAsQuality;

    FractalArgs(int algorithmId, float seed, float octaves, float lacunarity, float fractalIncrement,
                float offset, float gain, float scale)
    {
        this->algorithmId = algorithmId;
        this->seed = seed;
        this->octaves = octaves;
        this->remainder = octaves - (int)octaves;
        l = lacunarity;
        h = fractalIncrement;
        this->offset = offset;
        this->gain = gain;
        this->scale = scale;
        this->smoothingSteps = 1;
        this->subdivisionSteps = 2;
        this->saveAsQuality = false;
    }
};

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
            void computeSpectralWeights(FractalArgs &args);
            bool generateTerrain    (MeshModel &mm, FractalArgs &args, vcg::CallBackPos* cb);
            bool generateFractalMesh(MeshModel &mm, FractalArgs &args, vcg::CallBackPos* cb);

            double fBM          (CoordType &point, FractalArgs &args);
            double StandardMF   (CoordType &point, FractalArgs &args);
            double HeteroMF     (CoordType &point, FractalArgs &args);
            double HybridMF     (CoordType &point, FractalArgs &args);
            double RidgedMF     (CoordType &point, FractalArgs &args);

            float spectralWeight[21];
            enum {CR_FRACTAL_TERRAIN, FP_FRACTAL_MESH};
            double (FilterFractal::*vertexDisp[5]) (CoordType &, FractalArgs&);
};

#endif
