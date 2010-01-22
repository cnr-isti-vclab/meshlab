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
#include <vcg/space/plane3.h>
#include "craters_utils.h"

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
    QString filename, description;
    switch (filterId) {
    case CR_FRACTAL_TERRAIN:
    case FP_FRACTAL_MESH:
        filename = ":/ff_fractal_description.txt";
        break;
    case FP_CRATERS:
        filename = ":/ff_craters_description.txt";
        break;
    default:
        assert(0); return QString("error");
        break;
    }

    QFile f(filename);
    if(f.open(QFile::ReadOnly))
    {
        QTextStream stream(&f);
        description = stream.readAll();
        f.close();
    }
    return description;
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
        initParameterSetForCratersGeneration(md, par);
        break;
    }
}

void FilterFractal::initParameterSetForFractalDisplacement(QAction *filter, MeshDocument &md, RichParameterSet &par)
{
    par.addParam(new RichDynamicFloat("heightFactor", 0.4, -1.0, 1.0, "Height factor:", "Scales the resulting perturbation height of the given value."));

    switch(ID(filter)) {
    case CR_FRACTAL_TERRAIN:
        par.addParam(new RichInt("steps", 8, "Subdivision steps:", "Defines the detail of the generated terrain. Allowed values are in range [2,9]. Use values from 6 to 9 to obtain reasonable results."));
        break;
    case FP_FRACTAL_MESH:
        par.addParam(new RichInt("smoothingSteps", 10, "Normals smoothing steps:", "Face normals will be smoothed to make the perturbation more homogeneous. This parameter represents the number of smoothing steps." ));
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
    case CR_FRACTAL_TERRAIN:
        par.addParam(new RichDynamicFloat("zoom_w_size", 1.0, .0, 10.0, "Zoom window size:", "The zoom window is a square. This parameter is the ratio between the side of the zoom window and the size of the original window. A value larger than one means zoom out; a value smaller than one means zoom in."));
        par.addParam(new RichDynamicFloat("zoom_tx", .0, -10.0, 10.0, "Zoom Origin (x):", "The zoom window will be translated on the x-axis of a quantity equal to the product of this parameter and the side length of the original window."));
        par.addParam(new RichDynamicFloat("zoom_ty", .0, -10.0, 10.0, "Zoom Origin (y):", "The zoom window will be translated on the y-axis of a quantity equal to the product of this parameter and the side length of the original window."));
        break;
    }
    return;
}

void FilterFractal::initParameterSetForCratersGeneration(MeshDocument &md, RichParameterSet &par)
{
    int meshCount = md.meshList.size();
    if(meshCount < 2) return;

    // tries to detect the target mesh
    MeshModel* target = md.mm();
    MeshModel* samples = md.mm();
    MeshModel* tmpMesh;
    if (target->cm.fn == 0){ // this is probably the samples layer
        for(int i=0; i<meshCount; i++)
        {
            tmpMesh = md.meshList.at(i);
            if (tmpMesh->cm.fn > 0)
            {
                target = tmpMesh;
                break;
            }
        }
    }

    par.addParam(new RichMesh("target_mesh", target, &md, "Target mesh:", "The mesh on which craters will be generated."));
    par.addParam(new RichMesh("samples_mesh", samples, &md, "Samples layer:", "The samples that represent the central points of craters."));

    QStringList algList;
    algList << "Gaussian" << "Multiquadric" << "Inverse Multiquadric" << "Cauchy";
    par.addParam(new RichEnum("rbf", 0, algList, "Radial function:", "The radial function used to generate craters."));

    par.addParam(new RichDynamicFloat("min_radius", 0.3, 0, 1, "Min crater radius:", "Defines the minimum radius of craters in range [0, 1]. Values near 0 mean very small craters."));
    par.addParam(new RichDynamicFloat("max_radius", 0.6, 0, 1, "Max crater radius:", "Defines the maximum radius of craters in range [0, 1]. Values near 1 mean very large craters."));
    par.addParam(new RichDynamicFloat("min_depth", 0.3, 0, 1, "Min crater depth:", "Defines the minimum depth of craters in range [0, 1]."));
    par.addParam(new RichDynamicFloat("max_depth", 0.6, 0, 1, "Max crater depth:", "Defines the maximum depth of craters in range [0, 1]. Values near 1 mean very deep craters."));
    par.addParam(new RichDynamicFloat("profile_factor", 2, 0, 10, "Profile factor:", "The profile factor controls the ratio between crater borders and crater effective area. Use [1, 4] values to obtain reasonable results. This parameter is not considered in multiquadric radial functions."));
    par.addParam(new RichDynamicFloat("resolution", 0.5, 0, 1, "Craters resolution:", "This parameter defines the quality of generated craters. 1 means maximum quality."));
    return;
}

bool FilterFractal::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet &par, vcg::CallBackPos* cb)
{
    switch(ID(filter))
    {
    case CR_FRACTAL_TERRAIN:
    case FP_FRACTAL_MESH:
        return applyFractalDisplacementFilter(filter, md, par, cb);
        break;
    case FP_CRATERS:
        return applyCratersGenerationFilter(md, par, cb);
        break;
    }
    return false;
}

bool FilterFractal::applyFractalDisplacementFilter (QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    fractalArgs->setFields(par.getEnum("algorithm"),par.getFloat("seed"), par.getFloat("octaves"),
                           par.getFloat("lacunarity"), par.getFloat("fractalIncrement"), par.getFloat("offset"),
                           par.getFloat("gain"), par.getDynamicFloat("heightFactor"));

    switch(ID(filter)) {
    case CR_FRACTAL_TERRAIN:
        {
            int steps = par.getInt("steps");
            fractalArgs->subdivisionSteps = (steps>2)? steps : 2;
            fractalArgs->zoom_window_side = par.getDynamicFloat("zoom_w_size");
            fractalArgs->zoom_org_x = par.getDynamicFloat("zoom_tx");
            fractalArgs->zoom_org_y = par.getDynamicFloat("zoom_ty");
            return generateTerrain(*(md.mm()), cb);
        }
        break;
    case FP_FRACTAL_MESH:
        fractalArgs->smoothingSteps =  par.getInt("smoothingSteps");
        fractalArgs->saveAsQuality = par.getBool("saveAsQuality");
        return generateFractalMesh(*(md.mm()), cb);
        break;
    default: assert(0);
    }
    return false;
}

bool FilterFractal::applyCratersGenerationFilter(MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    if (md.meshList.size() < 2)
    {
        Log(GLLogStream::FILTER, "There must be at least two layers to apply the craters generation filter.");
        return false;
    }

    // reads parameters
    CratersArgs args(par.getMesh("target_mesh"), par.getMesh("samples_mesh"), par.getEnum("rbf"),
                     par.getDynamicFloat("min_radius"), par.getDynamicFloat("max_radius"),
                     par.getDynamicFloat("min_depth"), par.getDynamicFloat("max_depth"),
                     par.getDynamicFloat("profile_factor"), par.getDynamicFloat("resolution"));
    return generateCraters(md, args, cb);
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
        return MeshModel::MM_FACEFACETOPO;
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
        return MeshModel::MM_VERTCOORD | MeshModel::MM_VERTNORMAL | MeshModel::MM_VERTQUALITY;
        break;
    case FP_CRATERS:
        return MeshModel::MM_NONE;
        break;
    default: assert(0);
    }
}
// ----------------------------------------------------------------------

// -------------------- Private functions -------------------------------
bool FilterFractal::generateCraters(MeshDocument &md, CratersArgs &args, vcg::CallBackPos *cb)
{
    CMeshO::PerVertexAttributeHandle<float> rh = tri::Allocator<CMeshO>::AddPerVertexAttribute<float>(*(args.samples_mesh), std::string("Radius"));
    CMeshO::PerVertexAttributeHandle<float> dh = tri::Allocator<CMeshO>::AddPerVertexAttribute<float>(*(args.samples_mesh), std::string("Depth"));
    CMeshO::PerVertexAttributeHandle<FacePointer> fh = tri::Allocator<CMeshO>::AddPerVertexAttribute<FacePointer>(*(args.samples_mesh), std::string("CentralFace"));

    args.target_model->updateDataMask(MeshModel::MM_FACEFACETOPO);
    args.target_model->updateDataMask(MeshModel::MM_FACECOLOR);
    vcg::tri::UpdateSelection<CMeshO>::ClearFace(*(args.target_mesh));

    CratersUtils<CMeshO>::findSamplesFaces<float>(args.target_mesh, args.samples_mesh, fh);

    // selection of crater faces
    float maxRadius = .0;
    for(VertexIterator vi = args.samples_mesh->vert.begin(); vi != args.samples_mesh->vert.end(); ++vi)
    {
        rh[vi] = args.generateRadius();             // sets the crater radius
        if(rh[vi] > maxRadius) maxRadius = rh[vi];  // updates, if necessary, the maxRadius
        dh[vi] = args.generateDepth();              // sets the crater depth
        CratersUtils<CMeshO>::SelectCraterFaces<float>(args.target_mesh, fh[vi], &*vi, rh[vi]);
    }

    /*
    for(FaceIterator fi=args.target_mesh->face.begin(); fi!=args.target_mesh->face.end(); ++fi)
    {
        if( (*fi).IsS() )
        {
            (*fi).C() = Color4b::Red;
        }
    }
    */

    // refinement of crater faces
    float edgeThreshold = maxRadius * 2 / (50 * args.resolution);
    int maxIterations = 7;
    int iter = 0;
    while(Refine<CMeshO, MidPoint<CMeshO> >
          (*(args.target_mesh), MidPoint<CMeshO>(args.target_mesh), edgeThreshold, true, cb)
          && iter++ < maxIterations);

    // radial function application
    CratersUtils<CMeshO>::findSamplesFaces<float>(args.target_mesh, args.samples_mesh, fh);
    std::vector<FacePointer> craterFaces;
    for(VertexIterator vi = args.samples_mesh->vert.begin(); vi != args.samples_mesh->vert.end(); ++vi)
    {
        CratersUtils<CMeshO>::SelectCraterFaces<float>(args.target_mesh, fh[vi], &*vi, rh[vi], &craterFaces);
        CratersUtils<CMeshO>::applyRadialPerturbation<float>
                (args.target_mesh, args.algorithm, craterFaces, &(*vi), rh[vi], dh[vi], args.profile_factor);
    }

    // deleting attributes
    tri::Allocator<CMeshO>::DeletePerVertexAttribute<FacePointer>(*(args.samples_mesh), fh);
    tri::Allocator<CMeshO>::DeletePerVertexAttribute<float>(*(args.samples_mesh), rh);
    tri::Allocator<CMeshO>::DeletePerVertexAttribute<float>(*(args.samples_mesh), dh);

    // updating bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(*(args.target_mesh));
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(*(args.target_mesh));
    return true;
}

bool FilterFractal::generateTerrain(MeshModel &mm, vcg::CallBackPos* cb)
{
    CMeshO* m = &mm.cm;
    m->Clear();
    int subSteps = fractalArgs->subdivisionSteps;
    int k = (int)(pow(2, subSteps)), k2 = k+1, vertexCount = k2*k2, faceCount = 2*k*k, i=0, j=0;

    double step = 1/(double)k2;
    double new_step = step * fractalArgs->zoom_window_side;
    double tmp = step * k;
    double zoom_tx = tmp * fractalArgs->zoom_org_x;
    double zoom_ty = tmp * fractalArgs->zoom_org_y;
    double x = .0, y = .0;

    // grid generation
    vcg::tri::Allocator<CMeshO>::AddVertices(*m, vertexCount);
    vcg::tri::Allocator<CMeshO>::AddFaces(*m, faceCount);

    VertexIterator vi;
    VertexPointer ivp[vertexCount];
    cb(0, "Grid construction..");
    for(vi = m->vert.begin(); vi!=m->vert.end(); ++vi)
    {
        x = (i%k2) * new_step + zoom_tx;
        y = (i/k2) * new_step + zoom_ty;
        (*vi).P() = CoordType(x, y, .0);
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

    return perturbation;
}

// ---------------------------------------------------------------------

Q_EXPORT_PLUGIN(FilterFractal)

