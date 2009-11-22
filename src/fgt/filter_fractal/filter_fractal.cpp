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

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/math/base.h>
#include "filter_fractal.h"
#include <vcg/math/perlin_noise.h>

using namespace std;
using namespace vcg;

enum {
    FBM = 0,
    STANDARD_MF = 1,
    HETERO_MF = 2,
    HYBRID_MF = 3,
    RIDGED_MF = 4
};

// --------- constructor and destructor ------------------------------
FilterFractal::FilterFractal()
{
    typeList << CR_FRACTAL_TERRAIN;
    FilterIDType tt;
    foreach(tt , types())
	actionList << new QAction(filterName(tt), this);
}

FilterFractal::~FilterFractal(){}
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
            /*
            QString desc = "Generates a fractal terrain with five different algorithms. ";
            desc += "Detailed descriptions can be found in:<br /><br />";
            desc += "<span style=\"font-variant: small-caps;\">Ebert, D.S., Musgrave, F.K., Peachey, D., Perlin, K., and Worley, S.</span><br />";
            desc += "Texturing and Modeling: A Procedural Approach.<br />";
            desc += "<i>Morgan Kaufmann Publishers Inc., San Francisco, CA, USA, 2002.</i><br /> <br />";
            desc += "Some good parameter values to start with:<br />";
            */
            QString desc;
            QFile f(":/ff_description.txt");
            if (f.open(QFile::ReadOnly))
            {
                QTextStream stream(&f);
                desc = stream.readAll();
                f.close();
            }

            return desc;
        }
        break;
        default:
            assert(0); return QString("error");
            break;
    }
}

const int FilterFractal::getRequirements(QAction*/*action*/)
{	
    return MeshModel::MM_NONE;
}

void FilterFractal::initParameterSet(QAction* filter,MeshModel &/*m*/, RichParameterSet &par)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
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
            break;
    }
    return;
}

bool FilterFractal::applyFilter(QAction* filter, MeshModel &m, RichParameterSet &par, vcg::CallBackPos * /*cb*/)
{
    return false;
}

bool FilterFractal::applyFilter(QAction* filter, MeshDocument &m, RichParameterSet &par, vcg::CallBackPos */*cb*/)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
            return generateTerrain(m.mm()->cm,
                                   par.getInt("steps"),
                                   par.getEnum("algorithm"),
                                   par.getFloat("seed"),
                                   par.getFloat("octaves"),
                                   par.getFloat("lacunarity"),
                                   par.getFloat("fractalIncrement"),
                                   par.getFloat("offset"),
                                   par.getFloat("gain"));
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

bool FilterFractal::autoDialog(QAction *)
{
    return true;
}
// ----------------------------------------------------------------------

// -------------------- Private functions -------------------------------
bool FilterFractal::generateTerrain(CMeshO &m, int subSteps, int algorithm,
    float seed, float octaves, float lacunarity, float fractalIncrement,
    float offset, float gain)
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
    switch(algorithm)
    {
        case FBM:
            createFBMTerrain(m, octaves, seedFactor, lacunarity, fractalIncrement);
            break;
        case STANDARD_MF:
            createMFTerrain(m, octaves, seedFactor, lacunarity, fractalIncrement, offset);
            break;
        case HETERO_MF:
            createHeterogeneousMFTerrain(m, octaves, seedFactor, lacunarity, fractalIncrement, offset);
            break;
        case HYBRID_MF:
            createHybridMFTerrain(m, octaves, seedFactor, lacunarity, fractalIncrement, offset);
            break;
        case RIDGED_MF:
            createRidgedMFTerrain(m, octaves, seedFactor, lacunarity, fractalIncrement, offset, gain);
            break;
        default:
            assert(0); Log("FilterFractal error: algoithm type not recognized");
            break;
    }

    // updating bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(m);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m);
    return true;
}

void FilterFractal::createFBMTerrain(CMeshO &m, float octaves, float seedFactor,
    float lacunarity, float fractalIncrement)
{
    double x=.0, y=.0, z=.0, spectralWeight[(int)octaves+1], frequency=1.0, noise=.0, remainder = octaves - (int)octaves;
    CoordType* point;

    for(int i=0; i<=octaves; i++)
    {
        spectralWeight[i] = pow(frequency, -fractalIncrement);
        frequency *= lacunarity;
    }

    for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        noise = .0;
        point = &((*vi).P());
        x=(*point)[0]+seedFactor; y=(*point)[1]+seedFactor; z=(*point)[2]+seedFactor;
        for(int i=0; i<octaves; i++)
        {
            noise += (math::Perlin::Noise(x, y, z) * spectralWeight[i]);
            x *= lacunarity; y *= lacunarity; z *= lacunarity;
        }

        if(remainder != .0)
            noise += (remainder * math::Perlin::Noise(x, y, z) * spectralWeight[(int)octaves]);

        (*point)[2] += noise;
    }
}

void FilterFractal::createMFTerrain(CMeshO &m, float octaves, float seedFactor,
                float lacunarity, float fractalIncrement, float offset)
{
    double x=.0, y=.0, z=.0, spectralWeight[(int)octaves+1], frequency=1.0, noise=.0, remainder = octaves - (int)octaves;
    CoordType* point;

    for(int i=0; i<=octaves; i++)
    {
        spectralWeight[i] = pow(frequency, -fractalIncrement);
        frequency *= lacunarity;
    }

    for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        noise = 1.0;
        point = &((*vi).P());
        x=(*point)[0]+seedFactor; y=(*point)[1]+seedFactor; z=(*point)[2]+seedFactor;
        for(int i=0; i<octaves; i++)
        {
            noise *= (offset + math::Perlin::Noise(x, y, z) * spectralWeight[i]);
            x *= lacunarity; y *= lacunarity; z *= lacunarity;
        }

        if(remainder != .0)
            noise *= (remainder * (math::Perlin::Noise(x, y, z) * spectralWeight[(int)octaves] + offset));

        (*point)[2] += noise;
    }
}

void FilterFractal::createHeterogeneousMFTerrain(CMeshO &m, float octaves,
    float seedFactor, float lacunarity, float fractalIncrement, float offset)
{
    double x=.0, y=.0, z=.0, spectralWeight[(int)octaves+1], frequency=1.0;
    double noise=.0, remainder = octaves - (int)octaves, increment = .0;
    CoordType* point;

    for(int i=0; i<=octaves; i++)
    {
        spectralWeight[i] = pow(frequency, -fractalIncrement);
        frequency *= lacunarity;
    }

    for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        point = &((*vi).P());
        x=(*point)[0]+seedFactor; y=(*point)[1]+seedFactor; z=(*point)[2]+seedFactor;
        noise = (offset + math::Perlin::Noise(x, y, z)) * spectralWeight[0];
        x *= lacunarity; y *= lacunarity; z *= lacunarity;

        for(int i=1; i<octaves; i++)
        {
            increment = (offset + math::Perlin::Noise(x, y, z)) * spectralWeight[i] * noise;
            noise += increment;
            x *= lacunarity; y *= lacunarity; z *= lacunarity;
        }

        if(remainder != .0)
        {
            increment = (math::Perlin::Noise(x, y, z) + offset) * spectralWeight[(int)octaves] * noise;
            increment *= remainder;
            noise += increment;
        }

        (*point)[2] += noise;
    }
}

void FilterFractal::createHybridMFTerrain(CMeshO &m, float octaves, float seedFactor,
                float lacunarity, float fractalIncrement, float offset)
{
    double x=.0, y=.0, z=.0, spectralWeight[(int)octaves+1], frequency=1.0;
    double noise=.0, remainder = octaves - (int)octaves, weight = .0, signal = .0;
    CoordType* point;

    for(int i=0; i<=octaves; i++)
    {
        spectralWeight[i] = pow(frequency, -fractalIncrement);
        frequency *= lacunarity;
    }

    for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        point = &((*vi).P());
        x=(*point)[0]+seedFactor; y=(*point)[1]+seedFactor; z=(*point)[2]+seedFactor;
        noise = (offset + math::Perlin::Noise(x, y, z));
        weight = noise;
        x *= lacunarity; y *= lacunarity; z *= lacunarity;

        for(int i=1; i<octaves; i++)
        {
            if (weight > 1.0) weight = 1.0;
            signal = (offset + math::Perlin::Noise(x, y, z)) * spectralWeight[i];
            noise += (weight * signal);
            weight *= signal;
            x *= lacunarity; y *= lacunarity; z *= lacunarity;
        }

        if(remainder != .0)
        {
            signal = (offset + math::Perlin::Noise(x, y, z)) * spectralWeight[(int)octaves];
            noise += (weight * signal * remainder);
        }

        (*point)[2] += noise;
    }
}

void FilterFractal::createRidgedMFTerrain(CMeshO &m, float octaves, float seedFactor,
    float lacunarity, float fractalIncrement, float offset, float gain)
{
    double x=.0, y=.0, z=.0, spectralWeight[(int)octaves+1], frequency=1.0;
    double noise=.0, remainder = octaves - (int)octaves, weight = .0, signal = .0;
    CoordType* point;

    for(int i=0; i<=octaves; i++)
    {
        spectralWeight[i] = pow(frequency, -fractalIncrement);
        frequency *= lacunarity;
    }

    for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        point = &((*vi).P());
        x=(*point)[0]+seedFactor; y=(*point)[1]+seedFactor; z=(*point)[2]+seedFactor;
        signal = pow(offset - fabs(math::Perlin::Noise(x, y, z)), 2);
        noise = signal;
        x *= lacunarity; y *= lacunarity; z *= lacunarity;

        for(int i=1; i<octaves; i++)
        {
            weight = signal * gain;
            if (weight > 1.0) weight = 1.0;
            if (weight < 0.0) weight = 0.0;
            signal = pow(offset - fabs(math::Perlin::Noise(x, y, z)), 2) * weight * spectralWeight[i];
            noise += signal;
            x *= lacunarity; y *= lacunarity; z *= lacunarity;
        }

        if(remainder != .0)
        {
            weight = signal * gain;
            if (weight > 1.0) weight = 1.0;
            if (weight < 0.0) weight = 0.0;
            signal = pow(offset - fabs(math::Perlin::Noise(x, y, z)), 2) * weight * spectralWeight[(int)octaves];
            signal *= remainder;
            noise += signal;
        }

        (*point)[2] += noise;
    }
}
// ---------------------------------------------------------------------

Q_EXPORT_PLUGIN(FilterFractal)

