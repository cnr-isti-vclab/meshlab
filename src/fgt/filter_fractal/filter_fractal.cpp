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
    typeList << CR_FRACTAL_TERRAIN << FP_FRACTAL_MESH << FP_CRATERS;
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);

    cache = new FractalMeshCache<float>();
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

    if(filterId == FP_FRACTAL_MESH)
    {
        description += "<br /><br />Hint: search a good compromise between offset and height factor parameter.";
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
    par.addParam(new RichFloat("heightFactor", 0.4, "Height factor:", "Scales the resulting perturbation height of the given value."));

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
    par.addParam(new RichInt("seed", 0, "Seed:", "The seed with which the random number generator is initialized. The random generator generates radius and depth for each crater into the given range."));

    QStringList algList;
    algList << "Gaussian" << "Multiquadric" << "Inverse Multiquadric" << "Cauchy";
    par.addParam(new RichEnum("rbf", 1, algList, "Radial function:", "The radial function used to generate craters."));

    par.addParam(new RichDynamicFloat("min_radius", 0.3, 0, 1, "Min crater radius:", "Defines the minimum radius of craters in range [0, 1]. Values near 0 mean very small craters."));
    par.addParam(new RichDynamicFloat("max_radius", 0.6, 0, 1, "Max crater radius:", "Defines the maximum radius of craters in range [0, 1]. Values near 1 mean very large craters."));
    par.addParam(new RichDynamicFloat("min_depth", 0.3, 0, 1, "Min crater depth:", "Defines the minimum depth of craters in range [0, 1]."));
    par.addParam(new RichDynamicFloat("max_depth", 0.6, 0, 1, "Max crater depth:", "Defines the maximum depth of craters in range [0, 1]. Values near 1 mean very deep craters."));
    par.addParam(new RichDynamicFloat("profile_factor", 2, 1, 10, "Profile factor:", "The profile factor controls the ratio between crater borders and crater effective area. Use [1, 4] values to obtain reasonable results. This parameter is not considered in multiquadric radial functions."));
    par.addParam(new RichBool("ppNoise", false, "Postprocessing noise", "Slightly perturbates the craters with a noise function."));
    par.addParam(new RichBool("invert", false, "Invert perturbation", "If checked, inverts the sign of radial perturbation to create bumps instead of craters."));
    par.addParam(new RichBool("save_as_quality", false, "Save as vertex quality", "Saves the perturbation as vertex quality."));
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
    FractalArgs<float> args(md.mm(), par.getEnum("algorithm"),par.getFloat("seed"), par.getFloat("octaves"),
                           par.getFloat("lacunarity"), par.getFloat("fractalIncrement"), par.getFloat("offset"),
                           par.getFloat("gain"), par.getFloat("heightFactor"));

    switch(ID(filter)) {
    case CR_FRACTAL_TERRAIN:
        {
            int steps = par.getInt("steps");
            args.subdivisionSteps = (steps>2)? steps : 2;
            args.zoom_window_side = par.getDynamicFloat("zoom_w_size");
            args.zoom_org_x = par.getDynamicFloat("zoom_tx");
            args.zoom_org_y = par.getDynamicFloat("zoom_ty");
            return generateTerrain(*(md.mm()), args, cb);
        }
        break;
    case FP_FRACTAL_MESH:
        args.smoothingSteps =  par.getInt("smoothingSteps");
        args.saveAsQuality = par.getBool("saveAsQuality");
        return generateFractalMesh(*(md.mm()), args, cb);
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

    CMeshO* samples = &(par.getMesh("samples_mesh")->cm);
    if (samples->face.size() > 0)
    {
        Log(GLLogStream::FILTER, "The sample layer selected is not a sample layer.");
        return false;
    }

    float minRadius = par.getDynamicFloat("min_radius");
    float maxRadius = par.getDynamicFloat("max_radius");
    if (maxRadius <= minRadius)
    {
        Log(GLLogStream::FILTER, "Min radius is greater than max radius.");
    }

    float minDepth = par.getDynamicFloat("min_depth");
    float maxDepth = par.getDynamicFloat("max_depth");
    if (maxDepth <= minDepth)
    {
        Log(GLLogStream::FILTER, "Min depth is greater than max depth.");
    }

    // reads parameters
    CratersArgs<float> args(par.getMesh("target_mesh"), par.getMesh("samples_mesh"), par.getEnum("rbf"),
                     par.getInt("seed"), minRadius, maxRadius, minDepth, maxDepth,
                     par.getDynamicFloat("profile_factor"), par.getBool("save_as_quality"),
                     par.getBool("invert"), par.getBool("ppNoise"));

    return generateCraters(args, cb);
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
    case FP_CRATERS:
        return MeshModel::MM_VERTCOORD | MeshModel::MM_VERTNORMAL | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTQUALITY;
        break;
    default: assert(0);
    }
}
// ----------------------------------------------------------------------

// -------------------- Private functions -------------------------------
bool FilterFractal::generateCraters(CratersArgs<float> &args, vcg::CallBackPos *cb)
{
    typedef std::pair<VertexPointer, FacePointer> SampleFace;
    typedef std::vector<SampleFace> SampleFaceVector;

    // finds samples faces
    args.target_model->updateDataMask(MeshModel::MM_FACEFACETOPO);
    SampleFaceVector sfv;
    CratersUtils<CMeshO>::FindSamplesFaces<float>(args.target_mesh, args.samples_mesh, sfv, cb);

    // detectes crater faces and applies the radial perturbation
    int cratersNo = args.samples_mesh->vert.size(), currentCrater = 0;
    char buffer[50];
    SampleFaceVector::iterator sfvi;
    SampleFace p;
    float radius = .0, depth = .0;
    std::vector<FacePointer> craterFaces;
    for(sfvi=sfv.begin(); sfvi!=sfv.end(); ++sfvi)
    {
        p = (*sfvi);
        radius = args.generateRadius();
        depth = args.generateDepth();
        sprintf(buffer, "Generating crater %i...", currentCrater);
        cb(100*(currentCrater++)/cratersNo, buffer);
        CratersUtils<CMeshO>::GetCraterFaces<float>(args.target_mesh, p.second, p.first, radius, craterFaces);
        CratersUtils<CMeshO>::applyRadialPerturbation<float>(args, p.first, craterFaces, radius, depth);
    }

    // updates bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(*(args.target_mesh));
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(*(args.target_mesh));
    return true;
}

bool FilterFractal::generateTerrain(MeshModel &mm, FractalArgs<float> &args, vcg::CallBackPos* cb)
{
    CMeshO* m = &mm.cm;
    m->Clear();
    int subSteps = args.subdivisionSteps;
    int k = (int)(pow(2, subSteps)), k2 = k+1, vertexCount = k2*k2, faceCount = 2*k*k, i=0, j=0;

    double step = 1/(double)k2;
    double new_step = step * args.zoom_window_side;
    double tmp = step * k;
    double zoom_tx = tmp * args.zoom_org_x;
    double zoom_ty = tmp * args.zoom_org_y;
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
    args.seed = args.seed * vertexCount / 100;
    i=0;
    char buffer[50];
    for(VertexIterator vi=m->vert.begin(); vi!=m->vert.end(); ++vi)
    {
        sprintf(buffer, "Computing perturbation on vertex %d..", i++);
        cb(100*i/vertexCount, buffer);
        (*vi).P()[2] += FractalPerturbation<float>::computeFractalPerturbation(args, (*vi).P());
    }

    // updating bounding box and normals
    vcg::tri::UpdateBounding<CMeshO>::Box(*m);
    vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(*m);
    return true;
}

bool FilterFractal::generateFractalMesh(MeshModel &mm, FractalArgs<float> &args, vcg::CallBackPos* cb)
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
    tri::Smooth<CMeshO>::VertexNormalLaplacian(*m, args.smoothingSteps, false);

    // calculates seed factor
    args.seed = args.seed * m->vn / 100;

    // adds per-vertex quality if needed
    if(args.saveAsQuality)
        mm.updateDataMask(MeshModel::MM_VERTQUALITY);

    // updates, if necessary, the cache content with the new projected points
    char buffer[50];          // char buffer used in progress bar handling
    sprintf(buffer, "Updating cache..");
    cb(0, buffer);
    cache->updateCache(args);

    // applies the perturbation
    int i=0, vertexCount = m->vn;
    float perturbation = .0, minPerturbation = .0, pert[vertexCount];
    std::vector< Point3<float> >::iterator pi = cache->projectedPoints->begin();
    for(VertexIterator vi=(*m).vert.begin(); vi!=(*m).vert.end(); ++vi, ++pi, ++i)
    {
        sprintf(buffer, "Calculating perturbation on vertex %d..", i);
        cb(100*i/vertexCount, buffer);

        pert[i] = FractalPerturbation<float>::computeFractalPerturbation(args, (*pi));
        if (minPerturbation == .0)
        {
            minPerturbation = fabs(pert[i]);
        } else
        {
            if (fabs(pert[i]) < minPerturbation)
            {
                minPerturbation = fabs(pert[i]);
            }
        }        
    }

    i=0;
    for(VertexIterator vi=(*m).vert.begin(); vi!=(*m).vert.end(); ++vi, ++i)
    {
        sprintf(buffer, "Applying perturbation on vertex %d..", i);
        cb(100*i/vertexCount, buffer);

        perturbation = pert[i] - minPerturbation;
        if(args.saveAsQuality)
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
// ---------------------------------------------------------------------

Q_EXPORT_PLUGIN(FilterFractal)

