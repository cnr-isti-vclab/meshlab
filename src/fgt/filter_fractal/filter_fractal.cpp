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
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/refine.h>
#include <vcg/complex/trimesh/smooth.h>
#include <vcg/math/base.h>

using namespace std;
using namespace vcg;

// --------- constructor and destructor ------------------------------
FilterFractal::FilterFractal()
{
    fractalArgs = new FractalArgs();

    typeList << CR_FRACTAL_TERRAIN << FP_FRACTAL_MESH << FP_CRATERS;
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}
// -------------------------------------------------------------------

// ------- MeshFilterInterface implementation ------------------------
QString FilterFractal::filterName(FilterIDType filterId) const
{
    switch (filterId) {
    case CR_FRACTAL_TERRAIN:
        return QString("Fractal Terrain");
        break;
    case FP_FRACTAL_MESH:
        return QString("Fractal Displacement");
        break;
    case FP_CRATERS:
        return QString("Craters Generation");
        break;
    default:
        assert(0); return QString("error");
        break;
    }
}

QString FilterFractal::filterInfo(FilterIDType filterId) const
{
    switch (filterId) {
    case CR_FRACTAL_TERRAIN:
    case FP_FRACTAL_MESH:
        {
            QString desc;
            QFile f(":/ff_description.txt");
            if(f.open(QFile::ReadOnly))
            {
                QTextStream stream(&f);
                desc = stream.readAll();
                f.close();
            }
            return desc;
        }
        break;
    case FP_CRATERS:
        return QString("Add craters onto the mesh.");
        break;
    default:
        assert(0); return QString("error");
        break;
    }
}

void FilterFractal::initParameterSet(QAction* filter,MeshDocument &md, RichParameterSet &par)
{
    switch(ID(filter))
    {
    case CR_FRACTAL_TERRAIN:
    case FP_FRACTAL_MESH:
        initParameterSetForFractalDisplacement(filter, md, par);
        break;
    case FP_CRATERS:
        initParameterSetForCratersGeneration(filter, md, par);
        break;
    }
}

void FilterFractal::initParameterSetForFractalDisplacement(QAction *filter, MeshDocument &md, RichParameterSet &par)
{
    par.addParam(new RichDynamicFloat("scaleFactor", 0.4, 0, 1.0, "Scale factor:", "Scales down the resulting perturbation of the given value."));

    switch(ID(filter)) {
    case CR_FRACTAL_TERRAIN:
        par.addParam(new RichInt("steps", 8, "Subdivision steps:", "Defines the detail of the generated terrain. Allowed values are in range [2,9]. Use values from 6 to 9 to obtain reasonable results."));
        break;
    case FP_FRACTAL_MESH:
        par.addParam(new RichInt("smoothingSteps", 10, "Normals smoothing steps:", "After the subdivision step, face normals will be smoothed to make the perturbation more homogeneous. This parameter represents the number of smoothing steps." ));
        break;
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

    switch(ID(filter))
    {
    case FP_FRACTAL_MESH:
        par.addParam(new RichBool("saveAsQuality", false, "Save as vertex quality", "Saves the perturbation value as vertex quality."));
        break;
    case CR_FRACTAL_TERRAIN: break;
    }
    return;
}

void FilterFractal::initParameterSetForCratersGeneration(QAction *, MeshDocument &, RichParameterSet &)
{
    return;
}

bool FilterFractal::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos* cb)
{
    fractalArgs->setFields(par.getEnum("algorithm"),par.getFloat("seed"), par.getFloat("octaves"),
                           par.getFloat("lacunarity"), par.getFloat("fractalIncrement"), par.getFloat("offset"),
                           par.getFloat("gain"), par.getDynamicFloat("scaleFactor"));

    switch(ID(filter)) {
    case CR_FRACTAL_TERRAIN:
        {
            int steps = par.getInt("steps");
            fractalArgs->subdivisionSteps = (steps>2)? steps : 2;
            return generateTerrain(*(md.mm()), cb);
        }
        break;
    case FP_FRACTAL_MESH:
        fractalArgs->smoothingSteps =  par.getInt("smoothingSteps");
        fractalArgs->saveAsQuality = par.getBool("saveAsQuality");
        return generateFractalMesh(*(md.mm()), cb);
        break;
    case FP_CRATERS:
        return generateCraters(md);
        break;
    default: assert(0);
    }
    return false;
}

MeshFilterInterface::FilterClass FilterFractal::getClass(QAction* filter)
{
    switch(ID(filter)) {
    case CR_FRACTAL_TERRAIN:
        return MeshFilterInterface::MeshCreation;
        break;
    case FP_FRACTAL_MESH:
    case FP_CRATERS:
        return MeshFilterInterface::Remeshing;
        break;
    default: assert(0);
        return MeshFilterInterface::Generic;
    }
}

int FilterFractal::getRequirements(QAction *filter)
{
    switch(ID(filter)) {
    case CR_FRACTAL_TERRAIN:
        return MeshModel::MM_NONE;
        break;
    case FP_FRACTAL_MESH:
        return MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER;
        break;
    case FP_CRATERS:
        return MeshModel::MM_VERTFACETOPO;
        break;
    default: assert(0);
    }
}

int FilterFractal::postCondition(QAction *filter) const
{
    switch(ID(filter))
    {
    case CR_FRACTAL_TERRAIN:
        return MeshModel::MM_UNKNOWN;
        break;
    case FP_FRACTAL_MESH:
    case FP_CRATERS:
        return MeshModel::MM_VERTCOORD | MeshModel::MM_VERTNORMAL | MeshModel::MM_VERTQUALITY;
        break;
    default: assert(0);
    }
}
// ----------------------------------------------------------------------

// -------------------- Private functions -------------------------------
bool FilterFractal::generateCraters(MeshDocument &md)
{
    tri::Clean<CMeshO>::RemoveUnreferencedVertex(md.mm()->cm);
    tri::Allocator<CMeshO>::CompactVertexVector(md.mm()->cm);
    tri::Allocator<CMeshO>::CompactFaceVector(md.mm()->cm);
    return true;
}

bool FilterFractal::generateTerrain(MeshModel &mm, vcg::CallBackPos* cb)
{
    CMeshO* m = &mm.cm;
    m->Clear();
    int subSteps = fractalArgs->subdivisionSteps;
    int k = (int)(pow(2, subSteps)), k2 = k+1, vertexCount = k2*k2, faceCount = 2*k*k, i=0, j=0;

    // grid generation
    vcg::tri::Allocator<CMeshO>::AddVertices(*m, vertexCount);
    vcg::tri::Allocator<CMeshO>::AddFaces(*m, faceCount);

    VertexIterator vi;
    VertexPointer ivp[vertexCount];
    cb(0, "Grid construction..");
    for(vi = m->vert.begin(); vi!=m->vert.end(); ++vi)
    {
        (*vi).P() = CoordType((i%k2)/(double)k2, i/((double)vertexCount), .0);
        ivp[i++] = &*vi;
    }

    FaceIterator fi = m->face.begin();
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
            if(fi != m->face.end())++fi;
        }
        evenFace[0]++; evenFace[1]++; evenFace[2]++;
        oddFace[0]++;  oddFace[1]++;  oddFace[2]++;
    }

    // terrain generation
    fractalArgs->seed = fractalArgs->seed * vertexCount / 100;
    i=0;
    char buffer[50];
    for(VertexIterator vi=m->vert.begin(); vi!=m->vert.end(); ++vi)
    {
        sprintf(buffer, "Computing perturbation on vertex %d..", i++);
        cb(100*i/vertexCount, buffer);
        (*vi).P()[2] += computeFractalPerturbation((*vi).P());
    }

    // updating bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(*m);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(*m);
    return true;
}

bool FilterFractal::generateFractalMesh(MeshModel &mm, vcg::CallBackPos* cb)
{
    CMeshO* m = &mm.cm;

    // checks 2-manifoldness
    if (!tri::Clean<CMeshO>::IsTwoManifoldFace(*m))
    {
        errorMessage = "There are some not 2-manifold faces. Manifoldness is required in order to apply the filter.";
        return false;
    }
    tri::UpdateNormals<CMeshO>::PerVertexNormalized(*m);

    // smoothes vertex normals
    tri::Smooth<CMeshO>::VertexNormalLaplacian(*m, fractalArgs->smoothingSteps, false);

    double perturbation = .0;
    char buffer[50];
    int i=0, vertexCount = m->vn;

    // calculates seed factor and spectral weights
    fractalArgs->seed = fractalArgs->seed * (*m).vn / 100;

    // adds per-vertex quality if needed
    if(fractalArgs->saveAsQuality)
        mm.updateDataMask(MeshModel::MM_VERTQUALITY);

    // computes perturbation
    for(VertexIterator vi=(*m).vert.begin(); vi!=(*m).vert.end(); ++vi)
    {
        sprintf(buffer, "Computing perturbation on vertex %d..", i++);
        cb(100*i/vertexCount, buffer);
        if ((*vi).IsD()) continue;
        perturbation = computeFractalPerturbation((*vi).P());
        if(fractalArgs->saveAsQuality)
        {
            (*vi).Q() = perturbation;
        }else {
            (*vi).P() += ((*vi).N() * perturbation);
        }
    }

    // updates bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(*m);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(*m);
    return true;
}

double FilterFractal::computeFractalPerturbation(CoordType &point)
{
    double perturbation = .0;
    switch(fractalArgs->algorithmId)
    {
    case 0: //fBM
        perturbation = FractalPerturbation<CMeshO>::fBM(point, *fractalArgs);
        break;
    case 1: //standard multifractal
        perturbation = FractalPerturbation<CMeshO>::StandardMF(point, *fractalArgs);
        break;
    case 2: //heterogeneous multifractal
        perturbation = FractalPerturbation<CMeshO>::HeteroMF(point, *fractalArgs);
        break;
    case 3: //hybrid multifractal
        perturbation = FractalPerturbation<CMeshO>::HybridMF(point, *fractalArgs);
        break;
    case 4: //ridged multifractal
        perturbation = FractalPerturbation<CMeshO>::RidgedMF(point, *fractalArgs);
        break;
    }

    return perturbation * fractalArgs->scale;
}
// ---------------------------------------------------------------------

Q_EXPORT_PLUGIN(FilterFractal)

