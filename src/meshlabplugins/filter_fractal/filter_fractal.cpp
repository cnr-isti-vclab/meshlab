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

using namespace std;
using namespace vcg;

// ------- MeshFilterInterface implementation ------------------------
FilterFractal::FilterFractal()
{
    typeList << CR_FRACTAL_TERRAIN << FP_FRACTAL_MESH << FP_CRATERS;
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}

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
    bool terrain_filter = (ID(filter) == CR_FRACTAL_TERRAIN);

    if(terrain_filter) {
        par.addParam(new RichInt("steps", 8, "Subdivision steps:", "Defines the detail of the generated terrain. Allowed values are in range [2,9]. Use values from 6 to 9 to obtain reasonable results."));
        par.addParam(new RichDynamicFloat("maxHeight", 0.2, 0, 1, "Max height:", "Defines the maximum perturbation height as a fraction of the terrain's side."));
    } else {
        float diag = md.mm()->cm.bbox.Diag();
        par.addParam(new RichAbsPerc("maxHeight", 0.02 * diag, 0, 0.5*diag, "Max height:", "Defines the maximum height for the perturbation."));
    }

    par.addParam(new RichDynamicFloat("scale", 1, 0, 10, "Scale factor:", "Scales the fractal perturbation in and out. Values larger than 1 mean zoom out; values smaller than one mean zoom in."));
    if (!terrain_filter)
    {
        par.addParam(new RichInt("smoothingSteps", 5, "Normals smoothing steps:", "Face normals will be smoothed to make the perturbation more homogeneous. This parameter represents the number of smoothing steps." ));
    }
    par.addParam(new RichFloat("seed", 2, "Seed:", "By varying this seed, the terrain morphology will change.\nDon't change the seed if you want to refine the current terrain morphology by changing the other parameters."));

    QStringList algList;
    algList << "fBM (fractal Brownian Motion)" << "Standard multifractal" << "Heterogeneous multifractal" << "Hybrid multifractal terrain" << "Ridged multifractal terrain";
    par.addParam(new RichEnum("algorithm", 4, algList, "Algorithm", "The algorithm with which the fractal terrain will be generated."));
    par.addParam(new RichDynamicFloat("octaves", 8.0, 1.0, 20.0, "Octaves:", "The number of Perlin noise frequencies that will be used to generate the terrain. Reasonable values are in range [2,9]."));
    par.addParam(new RichFloat("lacunarity", 4.0, "Lacunarity:", "The gap between noise frequencies. This parameter is used in conjunction with fractal increment to compute the spectral weights that contribute to the noise in each octave."));
    par.addParam(new RichFloat("fractalIncrement", terrain_filter? 0.5 : 0.2, "Fractal increment:", "This parameter defines how rough the generated terrain will be. The range of reasonable values changes according to the used algorithm, however you can choose it in range [0.2, 1.5]."));
    par.addParam(new RichFloat("offset", 0.9, "Offset:", "This parameter controls the multifractality of the generated terrain. If offset is low, then the terrain will be smooth."));
    par.addParam(new RichFloat("gain", 2.5, "Gain:", "Ignored in all the algorithms except the ridged one. This parameter defines how hard the terrain will be."));
    par.addParam(new RichBool("saveAsQuality", false, "Save as vertex quality", "Saves the perturbation value as vertex quality."));
}

void FilterFractal::initParameterSetForCratersGeneration(MeshDocument &md, RichParameterSet &par)
{
    int meshCount = md.meshList.size();

    // tries to detect the target mesh
    MeshModel* target = md.mm();
    MeshModel* samples = md.mm();
    MeshModel* tmpMesh;
    if (samples->cm.fn != 0){ // this is probably not the samples layer
        for(int i=0; i<meshCount; i++)
        {
            tmpMesh = md.meshList.at(i);
            if (tmpMesh->cm.fn == 0)
            {
                samples = tmpMesh;
                break;
            }
        }
    }

    par.addParam(new RichMesh("target_mesh", target, &md, "Target mesh:", "The mesh on which craters will be generated."));
    par.addParam(new RichMesh("samples_mesh", samples, &md, "Samples layer:", "The samples that represent the central points of craters."));
    par.addParam(new RichInt("seed", 0, "Seed:", "The seed with which the random number generator is initialized. The random generator generates radius and depth for each crater into the given range."));
    par.addParam(new RichInt("smoothingSteps", 5, "Normals smoothing steps:", "Vertex normals are smoothed this number of times before generating craters."));

    QStringList algList;
    algList << "f1 (Gaussian)" << "f2 (Multiquadric)" << "f3";
    par.addParam(new RichEnum("rbf", 1, algList, "Radial function:", "The radial function used to generate craters."));

    par.addParam(new RichDynamicFloat("min_radius", 0.1, 0, 1, "Min crater radius:", "Defines the minimum radius of craters in range [0, 1]. Values near 0 mean very small craters."));
    par.addParam(new RichDynamicFloat("max_radius", 0.35, 0, 1, "Max crater radius:", "Defines the maximum radius of craters in range [0, 1]. Values near 1 mean very large craters."));
    par.addParam(new RichDynamicFloat("min_depth", 0.05, 0, 1, "Min crater depth:", "Defines the minimum depth of craters in range [0, 1]."));
    par.addParam(new RichDynamicFloat("max_depth", 0.15, 0, 1, "Max crater depth:", "Defines the maximum depth of craters in range [0, 1]. Values near 1 mean very deep craters."));
    par.addParam(new RichDynamicFloat("elevation", 0.4, 0, 1, "Elevation:", "Defines how much the crater rise itself from the mesh surface, giving an \"impact-effect\"."));

    QStringList blendList;
    blendList << "Exponential blending" << "Linear blending" << "Gaussian blending" << "f3 blending";
    par.addParam(new RichEnum("blend", 3, blendList, "Blending algorithm:", "The algorithm that is used to blend the perturbation towards the mesh surface."));
    par.addParam(new RichDynamicFloat("blendThreshold", 0.8, 0, 1, "Blending threshold:", "The fraction of craters radius beyond which the radial function is replaced with the blending function."));
    par.addParam(new RichBool("successiveImpacts", true, "Successive impacts", "If not checked, the impact-effects of generated craters will be superimposed with each other."));
    par.addParam(new RichBool("ppNoise", true, "Postprocessing noise", "Slightly perturbates the craters with a noise function."));
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
        {
            MeshModel* mm = md.mm();
            float maxHeight = .0;
            int smoothingSteps = 0;

            if(ID(filter) == CR_FRACTAL_TERRAIN)
            {
                int steps = par.getInt("steps");
                steps = ((steps<2)? 2: steps);
                float gridSide = .0;
                FractalUtils<CMeshO>::GenerateGrid(mm->cm, steps, gridSide);
                maxHeight = par.getDynamicFloat("maxHeight") * gridSide;
            } else {
                maxHeight = par.getAbsPerc("maxHeight");
                smoothingSteps = par.getInt("smoothingSteps");
            }

            FractalUtils<CMeshO>::FractalArgs args
               (mm, par.getEnum("algorithm"),par.getFloat("seed"),
                par.getFloat("octaves"), par.getFloat("lacunarity"),
                par.getFloat("fractalIncrement"), par.getFloat("offset"), par.getFloat("gain"),
                maxHeight, par.getDynamicFloat("scale"), smoothingSteps, par.getBool("saveAsQuality"));

            if(args.saveAsQuality)
                mm->updateDataMask(MeshModel::MM_VERTQUALITY);
            return FractalUtils<CMeshO>::ComputeFractalPerturbation(mm->cm, args, cb);
        }
        break;
    case FP_CRATERS:
        {
            if (md.meshList.size() < 2) {
                errorMessage = "There must be at least two layers to apply the craters generation filter.";
                return false;
            }

            CMeshO* samples = &(par.getMesh("samples_mesh")->cm);
            if (samples->face.size() > 0) {
                errorMessage = "The sample layer selected is not a sample layer.";
                return false;
            }
            CMeshO* target = &(par.getMesh("target_mesh")->cm);
            if (samples == target) {
                errorMessage = "The sample layer and the target layer must be different.";
                return false;
            }

            float minRadius = par.getDynamicFloat("min_radius");
            float maxRadius = par.getDynamicFloat("max_radius");
            if (maxRadius <= minRadius)  {
                errorMessage =  "Min radius is greater than max radius.";
                return false;
            }

            float minDepth = par.getDynamicFloat("min_depth");
            float maxDepth = par.getDynamicFloat("max_depth");
            if (maxDepth <= minDepth) {
                errorMessage = "Min depth is greater than max depth.";
                return false;
            }

            // reads parameters
            CratersUtils<CMeshO>::CratersArgs args(par.getMesh("target_mesh"), par.getMesh("samples_mesh"), par.getEnum("rbf"),
                                    par.getInt("seed"), minRadius, maxRadius, minDepth, maxDepth,
                                    par.getInt("smoothingSteps"), par.getBool("save_as_quality"), par.getBool("invert"),
                                    par.getBool("ppNoise"), par.getBool("successiveImpacts"),
                                    par.getDynamicFloat("elevation"), par.getEnum("blend"),
                                    par.getDynamicFloat("blendThreshold"));

            return CratersUtils<CMeshO>::GenerateCraters(args, cb);
        }
        break;
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
        return MeshFilterInterface::Smoothing;
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
    case FP_FRACTAL_MESH:
    case FP_CRATERS:
        return MeshModel::MM_VERTCOORD | MeshModel::MM_VERTNORMAL | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEFLAG;
        break;
    default: assert(0);
    }
}
// ----------------------------------------------------------------------
Q_EXPORT_PLUGIN(FilterFractal)

