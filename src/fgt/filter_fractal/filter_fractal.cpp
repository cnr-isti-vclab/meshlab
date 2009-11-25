/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <Qt>
#include <QtGui>

#include "filter_fractal.h"
#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/math/base.h>
#include <vcg/math/perlin_noise.h>

using namespace std;
using namespace vcg;

// --------- constructor and destructor ------------------------------
FilterFractal::FilterFractal()
{
    vertexDisp[0] = &FilterFractal::fBM;         // populate the vertexDisp array
    vertexDisp[1] = &FilterFractal::StandardMF;
    vertexDisp[2] = &FilterFractal::HeteroMF;
    vertexDisp[3] = &FilterFractal::HybridMF;
    vertexDisp[4] = &FilterFractal::RidgedMF;

    typeList << CR_FRACTAL_TERRAIN;
    FilterIDType tt;
    foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}
// -------------------------------------------------------------------

// ------- MeshFilterInterface implementation ------------------------
const QString FilterFractal::filterName(FilterIDType filterId) const
{
    switch (filterId) {
        case CR_FRACTAL_TERRAIN:
            return QString("Fractal terrain");
            break;
        default:
            assert(0); return QString("error");
            break;
    }
}

const QString FilterFractal::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
        case CR_FRACTAL_TERRAIN:
        {
            QString desc;
            QFile f(":/ff_description.txt");
            if (f.open(QFile::ReadOnly))
            {
                QTextStream stream(&f);
                desc = stream.readAll();
                f.close();
            } else {
                desc = "Generates a fractal terrain.";
            }
            return desc;
        }
        break;
        default:
            assert(0); return QString("error");
            break;
    }
}

void FilterFractal::initParameterSet(QAction* filter,MeshDocument &md, RichParameterSet &par)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
        {
            par.addParam(new RichInt("steps", 8, "Subdivision steps:", "Defines the detail of the generated terrain. Allowed values are in range [2,9]. Use values from 6 to 9 to obtain reasonable results."));
            par.addParam(new RichFloat("seed", 2, "Seed:", "By varying this seed, the terrain morphology will change.\nDon't change the seed if you want to refine the current terrain morphology by changing the other parameters."));

            QStringList algList;
            algList << "fBM (fractal Brownian Motion)" << "Standard multifractal" << "Heterogeneous multifractal" << "Hybrid multifractal terrain" << "Ridged multifractal terrain";
            par.addParam(new RichEnum("algorithm", 4, algList, "Algorithm", "The algorithm with which the fractal terrain will be generated."));

            par.addParam(new RichFloat("octaves", 8.0, "Octaves:", "The number of Perlin noise frequencies that will be used to generate the terrain. Reasonable values are in range [2,9]. Float values are allowed."));
            par.addParam(new RichFloat("lacunarity", 4.0, "Lacunarity:", "The gap between noise frequencies. This parameter is used in conjunction with fractal increment to compute the spectral weights that contribute to the noise in each octave."));
            par.addParam(new RichFloat("fractalIncrement", 0.23, "Fractal increment:", "This parameter defines how rough the generated terrain will be. The range of reasonable values changes according to the used algorithm, however you can choose it in range [0.2, 1.5]."));
            par.addParam(new RichFloat("offset", 0.6, "Offset:", "This parameter controls the multifractality of the generated terrain. If offset is low, then the terrain will be smooth."));
            par.addParam(new RichFloat("gain", 2.0, "Gain:", "Ignored in all the algorithms except the ridged one. This parameter defines how hard the terrain will be."));
        }
        break;
        default: assert(0);
    }
    return;
}

bool FilterFractal::applyFilter(QAction* filter, MeshDocument &m, RichParameterSet &par, vcg::CallBackPos */*cb*/)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
        {
            fArgs[OCTAVES] = par.getFloat("octaves");
            fArgs[REMAINDER] = fArgs[OCTAVES] - (int)fArgs[OCTAVES];
            fArgs[L] = par.getFloat("lacunarity");
            fArgs[H] = par.getFloat("fractalIncrement");
            fArgs[OFFSET] = par.getFloat("offset");
            fArgs[GAIN] = par.getFloat("gain");
            return generateTerrain(m.mm()->cm, par.getInt("steps"),
                par.getEnum("algorithm"), par.getFloat("seed"));
        }
        break;
    }
    return false;
}

const MeshFilterInterface::FilterClass FilterFractal::getClass(QAction* filter)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
            return MeshFilterInterface::MeshCreation;
        break;
        default: assert(0);
            return MeshFilterInterface::Generic;
    }
}
// ----------------------------------------------------------------------

// -------------------- Private functions -------------------------------
bool FilterFractal::generateTerrain(CMeshO &m, int subSteps, int algorithm, float seed)
{
    m.Clear();
    int k = (int)(pow(2, subSteps)), k2 = k+1, vertexCount = k2*k2, faceCount = 2*k*k, i=0, j=0;

    // grid generation
    vcg::tri::Allocator<CMeshO>::AddVertices(m, vertexCount);
    vcg::tri::Allocator<CMeshO>::AddFaces(m, faceCount);

    VertexIterator vi;
    VertexPointer ivp[vertexCount];
    for(vi = m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        (*vi).P() = CoordType((i%k2)/(double)k2, i/((double)vertexCount), .0);
        ivp[i++] = &*vi;
    }

    FaceIterator fi = m.face.begin();
    int evenFace[3] = {0, 1, k2}, oddFace[3] = {1, k2+1, k2};
    for(i=0; i<k; i++)
    {
        for(j=0; j<k; j++)
        {
            (*fi).V(0) = ivp[evenFace[0]++];
            (*fi).V(1) = ivp[evenFace[1]++];
            (*fi).V(2) = ivp[evenFace[2]++];
            ++fi;
            (*fi).V(0) = ivp[oddFace[0]++];
            (*fi).V(1) = ivp[oddFace[1]++];
            (*fi).V(2) = ivp[oddFace[2]++];
            if(fi != m.face.end())++fi;
        }
        evenFace[0]++; evenFace[1]++; evenFace[2]++;
        oddFace[0]++;  oddFace[1]++;  oddFace[2]++;
    }

    // terrain generation
    double seedFactor = (seed*vertexCount)/100;
    CoordType* point;
    double (FilterFractal::*f)() = vertexDisp[algorithm];

    computeSpectralWeights();
    for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        point = &((*vi).P());
        fArgs[X] = (*point)[0] + seedFactor;
        fArgs[Y] = (*point)[1] + seedFactor;
        fArgs[Z] = (*point)[2] + seedFactor;
        (*point)[2] += (this->*f)();
    }

    // updating bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(m);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m);
    return true;
}

double FilterFractal::fBM()
{
    double noise = .0, x = fArgs[X], y = fArgs[Y], z = fArgs[Z];
    for(int i=0; i<(int)fArgs[OCTAVES]; i++)
    {
        noise += (math::Perlin::Noise(x, y, z) * spectralWeight[i]);
        x *= fArgs[L]; y *= fArgs[L]; z *= fArgs[L];
    }

    if(fArgs[REMAINDER] != .0)
        noise += (fArgs[REMAINDER] * math::Perlin::Noise(x, y, z) * spectralWeight[(int)fArgs[OCTAVES]]);

    return noise;
}

double FilterFractal::StandardMF()
{
    double noise = 1.0, x = fArgs[X], y = fArgs[Y], z = fArgs[Z];

    for(int i=0; i<(int)fArgs[OCTAVES]; i++)
    {
        noise *= (fArgs[OFFSET] + math::Perlin::Noise(x, y, z) * spectralWeight[i]);
        x *= fArgs[L]; y *= fArgs[L]; z *= fArgs[L];
    }

    if(fArgs[REMAINDER] != .0)
        noise *= (fArgs[REMAINDER] * (math::Perlin::Noise(x, y, z) *
                  spectralWeight[(int)fArgs[OCTAVES]] + fArgs[OFFSET]));

    return noise;
}

double FilterFractal::HeteroMF()
{
    double noise = .0, x = fArgs[X], y = fArgs[Y], z = fArgs[Z], increment = .0;
    noise = (fArgs[OFFSET] + math::Perlin::Noise(x, y, z)) * spectralWeight[0];
    x *= fArgs[L]; y *= fArgs[L]; z *= fArgs[L];

    for(int i=1; i<(int)fArgs[OCTAVES]; i++)
    {
        increment = (fArgs[OFFSET] + math::Perlin::Noise(x, y, z)) * spectralWeight[i] * noise;
        noise += increment;
        x *= fArgs[L]; y *= fArgs[L]; z *= fArgs[L];
    }

    if(fArgs[REMAINDER] != .0)
    {
        increment = (math::Perlin::Noise(x, y, z) + fArgs[OFFSET]) * spectralWeight[(int)fArgs[OCTAVES]] * noise;
        increment *= fArgs[REMAINDER];
        noise += increment;
    }
    return noise;
}

double FilterFractal::HybridMF()
{
    double x = fArgs[X], y = fArgs[Y], z = fArgs[Z];
    double noise = (fArgs[OFFSET] + math::Perlin::Noise(x, y, z));
    double weight = noise, signal = .0;
    x *= fArgs[L]; y *= fArgs[L]; z *= fArgs[L];

    for(int i=1; i<(int)fArgs[OCTAVES]; i++)
    {
        if (weight > 1.0) weight = 1.0;
        signal = (fArgs[OFFSET] + math::Perlin::Noise(x, y, z)) * spectralWeight[i];
        noise += (weight * signal);
        weight *= signal;
        x *= fArgs[L]; y *= fArgs[L]; z *= fArgs[L];
    }

    if(fArgs[REMAINDER] != .0)
    {
        signal = (fArgs[OFFSET] + math::Perlin::Noise(x, y, z)) * spectralWeight[(int)fArgs[OCTAVES]];
        noise += (weight * signal * fArgs[REMAINDER]);
    }
    return noise;
}

double FilterFractal::RidgedMF()
{
    double x = fArgs[X], y = fArgs[Y], z = fArgs[Z];
    double signal = pow(fArgs[OFFSET] - fabs(math::Perlin::Noise(x, y, z)), 2);
    double noise = signal, weight = .0;
    x *= fArgs[L]; y *= fArgs[L]; z *= fArgs[L];

    for(int i=1; i<(int)fArgs[OCTAVES]; i++)
    {
        weight = signal * fArgs[GAIN];
        if (weight > 1.0) weight = 1.0;
        if (weight < 0.0) weight = 0.0;
        signal = pow(fArgs[OFFSET] - fabs(math::Perlin::Noise(x, y, z)), 2) * weight * spectralWeight[i];
        noise += signal;
        x *= fArgs[L]; y *= fArgs[L]; z *= fArgs[L];
    }

    if(fArgs[REMAINDER] != .0)
    {
        weight = signal * fArgs[GAIN];
        if (weight > 1.0) weight = 1.0;
        if (weight < 0.0) weight = 0.0;
        signal = pow(fArgs[OFFSET] - fabs(math::Perlin::Noise(x, y, z)), 2) * weight * spectralWeight[(int)fArgs[OCTAVES]];
        signal *= fArgs[REMAINDER];
        noise += signal;
    }
    return noise;
}

void FilterFractal::computeSpectralWeights()
{
    float frequency = 1.0;
    for(int i=0; i<=(int)fArgs[OCTAVES]; i++)
    {
        spectralWeight[i] = pow(frequency, -fArgs[H]);
        frequency *= fArgs[L];
    }
}
// ---------------------------------------------------------------------

Q_EXPORT_PLUGIN(FilterFractal)

