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
    HETERO_MF = 2
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
            return QString("Generates a fractal terrain");
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
            par.addParam(new RichInt("steps", 8, "Subdivision steps:", "Defines the detail of the generated terrain. Allowed values are in range [2,9]. Use values from 6 to 9 to obtain reasonable result."));
            par.addParam(new RichFloat("seed", 1, "Seed:", "By varying this seed, the terrain morphology will change.\nDon't change the seed if you want to refine the current terrain morphology by changing the other parameters."));

            QStringList algList;
            algList << "fBM (fractal Brownian Motion)" << "Standard multifractal" << "Heterogeneous multifractal";
            par.addParam(new RichEnum("algorithm", 0, algList, "Algorithm", "Todo..."));

            par.addParam(new RichFloat("octaves", 10.0, "Octaves:", "The number of Perlin noise frequencies that will be used to generate the resulting terrain. Reasonable values are in range [2,9]."));
            par.addParam(new RichFloat("lacunarity", 2.0, "Lacunarity:", "The gap between Perlin noise frequencies. This parameter is used in different ways by applying different algorithms, but you can always choose values between 2 and 7 to make the generated terrain appear different."));
            par.addParam(new RichFloat("fractalIncrement", 1.2, "Fractal increment:", "This parameter defines how rough the generated terrain will be. Use values in range [1,2] to obtain reasonable results.\nIf the value is near 1, then the terrain will be very rough."));
            par.addParam(new RichFloat("offset", 0.7, "Offset:", "Todo..."));
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
                                   par.getFloat("offset"));
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
    float seed, float octaves, float lacunarity, float fractalIncrement, float offset)
{
    m.Clear();
    int k = pow(2, subSteps), k2 = k+1, vertexCount = k2*k2, faceCount = 2*k*k, i=0, j=0;

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
            noise *= (remainder * math::Perlin::Noise(x, y, z) * spectralWeight[(int)octaves] + offset);

        (*point)[2] += noise;
    }
}

void FilterFractal::createHeterogeneousMFTerrain(CMeshO &m, float octaves,
    float seedFactor, float lacunarity, float fractalIncrement, float offset)
{
    return;
}
// ---------------------------------------------------------------------

Q_EXPORT_PLUGIN(FilterFractal)

