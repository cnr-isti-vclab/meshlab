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
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/smooth.h>
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

    typeList << CR_FRACTAL_TERRAIN << FP_FRACTAL_MESH;
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
            return QString("Fractal Terrain");
            break;
        case FP_FRACTAL_MESH:
            return QString("Fractal Displacement");
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
        case FP_FRACTAL_MESH:
        {
            QString desc;
            QFile f(":/ff_description.txt");
            bool opened = f.open(QFile::ReadOnly);
            if (opened)
            {
                QTextStream stream(&f);
                desc = stream.readAll();
                f.close();
            } else {
                desc = "Generates a fractal terrain perturbation";
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
    par.addParam(new RichDynamicFloat("scaleFactor", 0.4, 0, 1.0, "Scale factor:", "Scales down the resulting perturbation of the given value."));

    switch(ID(filter)) {
    case CR_FRACTAL_TERRAIN:
        par.addParam(new RichInt("steps", 8, "Subdivision steps:", "Defines the detail of the generated terrain. Allowed values are in range [2,9]. Use values from 6 to 9 to obtain reasonable results."));
        break;
    case FP_FRACTAL_MESH:
        {
            float diag = md.mm()->cm.bbox.Diag();
            par.addParam(new RichAbsPerc("eThreshold", diag*0.005, 0, diag, "Edge threshold:", "Current mesh will be refined until the length of all edges is below the given threshold."));
            par.addParam(new RichInt("smoothingSteps", 3, "Normals smoothing steps:", "After the subdivision step, face normals will be smoothed to make the perturbation more homogeneous. This parameter represents the number of smoothing steps." ));
        }
        break;
    default: assert(0);
    }

    par.addParam(new RichFloat("seed", 1, "Seed:", "By varying this seed, the terrain morphology will change.\nDon't change the seed if you want to refine the current terrain morphology by changing the other parameters."));

    QStringList algList;
    algList << "fBM (fractal Brownian Motion)" << "Standard multifractal" << "Heterogeneous multifractal" << "Hybrid multifractal terrain" << "Ridged multifractal terrain";
    par.addParam(new RichEnum("algorithm", 4, algList, "Algorithm", "The algorithm with which the fractal terrain will be generated."));

    par.addParam(new RichDynamicFloat("octaves", 8.0, 1.0, 20.0, "Octaves:", "The number of Perlin noise frequencies that will be used to generate the terrain. Reasonable values are in range [2,9]."));
    par.addParam(new RichFloat("lacunarity", 4.0, "Lacunarity:", "The gap between noise frequencies. This parameter is used in conjunction with fractal increment to compute the spectral weights that contribute to the noise in each octave."));
    par.addParam(new RichFloat("fractalIncrement", 0.2, "Fractal increment:", "This parameter defines how rough the generated terrain will be. The range of reasonable values changes according to the used algorithm, however you can choose it in range [0.2, 1.5]."));
    par.addParam(new RichFloat("offset", 0.6, "Offset:", "This parameter controls the multifractality of the generated terrain. If offset is low, then the terrain will be smooth."));
    par.addParam(new RichFloat("gain", 2.5, "Gain:", "Ignored in all the algorithms except the ridged one. This parameter defines how hard the terrain will be."));

    return;
}

bool FilterFractal::applyFilter(QAction* filter, MeshDocument &m, RichParameterSet &par, vcg::CallBackPos* cb)
{
    fArgs[SEED] = par.getFloat("seed");         // read parameters
    fArgs[SCALER] = par.getDynamicFloat("scaleFactor");
    fArgs[OCTAVES] = par.getFloat("octaves");
    fArgs[REMAINDER] = fArgs[OCTAVES] - (int)fArgs[OCTAVES];
    fArgs[L] = par.getFloat("lacunarity");
    fArgs[H] = par.getFloat("fractalIncrement");
    fArgs[OFFSET] = par.getFloat("offset");
    fArgs[GAIN] = par.getFloat("gain");
    int algorithmId = par.getEnum("algorithm");

    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
        {
            int steps = par.getInt("steps");
            if (steps > 9) steps = 9;
            if (steps < 2) steps = 2;
            return generateTerrain(m.mm()->cm, steps, algorithmId, cb);
        }
        break;
        case FP_FRACTAL_MESH:
            fArgs[THRESHOLD] = par.getAbsPerc("eThreshold");
            return generateFractalMesh(*(m.mm()), par.getInt("smoothingSteps"), algorithmId, cb);
        break;
        default: assert(0);
    }
    return false;
}

const MeshFilterInterface::FilterClass FilterFractal::getClass(QAction* filter)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
            return MeshFilterInterface::MeshCreation;
        break;
        case FP_FRACTAL_MESH:
            return MeshFilterInterface::Smoothing;
        break;
        default: assert(0);
            return MeshFilterInterface::Generic;
    }
}

const int FilterFractal::getRequirements(QAction *filter)
{
    switch(ID(filter)) {
        case CR_FRACTAL_TERRAIN:
            return MeshModel::MM_NONE;
        break;
        case FP_FRACTAL_MESH:
            return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER;
        break;
        default: assert(0);
    }
}
// ----------------------------------------------------------------------

// -------------------- Private functions -------------------------------
bool FilterFractal::generateTerrain(CMeshO &m, int subSteps, int algorithm, vcg::CallBackPos* cb)
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
    double seedFactor = (fArgs[SEED] * vertexCount)/100;
    double (FilterFractal::*f)() = vertexDisp[algorithm];
    CoordType* point;
    computeSpectralWeights();
    for(VertexIterator vi=m.vert.begin(); vi!=m.vert.end(); ++vi)
    {
        point = &((*vi).P());
        fArgs[X] = (*point)[0] + seedFactor;
        fArgs[Y] = (*point)[1] + seedFactor;
        fArgs[Z] = (*point)[2] + seedFactor;
        (*point)[2] += ((this->*f)() * fArgs[SCALER]);
    }

    // updating bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(m);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m);
    return true;
}

bool FilterFractal::generateFractalMesh(MeshModel &mm, int smoothingSteps, int algorithm, vcg::CallBackPos* cb)
{
    CMeshO* m = &mm.cm;

    // checks 2-manifoldness
    if (!tri::Clean<CMeshO>::IsTwoManifoldFace(*m))
    {
        errorMessage = "There are some not 2-manifold faces. Manifoldness is required in order to apply the filter.";
        return false;
    }

    // refines mesh until a given threshold
    while (Refine<CMeshO, MidPoint<CMeshO> >(*m, MidPoint<CMeshO>(m), fArgs[THRESHOLD], false, cb)){};
    mm.clearDataMask(MeshModel::MM_VERTFACETOPO);
    tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(*m);

    // smoothes face normals
    tri::Smooth<CMeshO>::FaceNormalLaplacianFF(*m, smoothingSteps, false);

    // recomputes vertex normals from face normals
    tri::UpdateNormals<CMeshO>::PerVertexFromCurrentFaceNormal(*m);

    // normalizes vertex normals
    tri::UpdateNormals<CMeshO>::NormalizeVertex(*m);

    double seedFactor = (fArgs[SEED]*((*m).vn))/100;
    double (FilterFractal::*f)() = vertexDisp[algorithm];

    computeSpectralWeights();
    double perturbation = .0;
    for(VertexIterator vi=(*m).vert.begin(); vi!=(*m).vert.end(); ++vi)
    {
        fArgs[X] = (*vi).P()[0] + seedFactor;
        fArgs[Y] = (*vi).P()[1] + seedFactor;
        fArgs[Z] = (*vi).P()[2] + seedFactor;
        perturbation = (this->*f)();
        (*vi).P()[0] += ((*vi).N()[0] * perturbation);
        (*vi).P()[1] += ((*vi).N()[1] * perturbation);
        (*vi).P()[2] += ((*vi).N()[2] * perturbation);
    }

    // updating bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(*m);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(*m);
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

