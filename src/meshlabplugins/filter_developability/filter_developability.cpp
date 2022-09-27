/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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

#include "filter_developability.h"
#include "remeshing.h"
#include "opt.h"
#include "energy.h"

FilterDevelopabilityPlugin::FilterDevelopabilityPlugin() 
{ 
    typeList = {FP_MAKE_DEVELOPABLE};

    for(ActionIDType tt : types())
        actionList.push_back(new QAction(filterName(tt), this));
}

FilterDevelopabilityPlugin::~FilterDevelopabilityPlugin()
{
}

QString FilterDevelopabilityPlugin::pluginName() const
{
    return "FilterDevelopability";
}

QString FilterDevelopabilityPlugin::filterName(ActionIDType filterId) const
{
    switch(filterId) {
    case FP_MAKE_DEVELOPABLE :
        return "Make mesh developable";
    default :
        assert(0);
        return QString();
    }
}

QString FilterDevelopabilityPlugin::pythonFilterName(ActionIDType f) const
{
    switch(f) {
    case FP_MAKE_DEVELOPABLE :
        return "apply_coord_developability_of_mesh";
    default :
        assert(0);
        return QString();
    }
}

 QString FilterDevelopabilityPlugin::filterInfo(ActionIDType filterId) const
{
    switch(filterId) {
    case FP_MAKE_DEVELOPABLE :
        return "The filter improves the developability of the current two-manifold triangular mesh "
               "by applying an optimization process that encourages each vertex star to form an hinge or a flat piece. "
               "The resulting mesh is similar to the initial, but it is comprised of one or more developable pieces held toghether by highly regular seam curves, "
               "i.e. path of edges which vertex stars did not form an hinge or a flat spot.<br>"
               "Since small interior angles can have a negative impact on the outcome, an automatic remeshing that runs along the optimization can be enabled.<br>"
               "When the obtained design is satisfactory, one may want to refine the quality of the seams and the developability of the surfaces "
               "by alternating between regular midpoint subdivisions and further optimization rounds.<br>"
               "For more details see:<br>"
               "<b>Oded Stein, Eitan Grinspun and Keenan Crane</b><br>"
               "<a href=\"https://doi.org/10.1145/3197517.3201303\">'Developability of triangle meshes'</a><br>"
               "ACM Transactions on Graphics, Volume 37, Issue 4";
    default :
        assert(0);
        return "Unknown Filter";
    }
}

FilterDevelopabilityPlugin::FilterClass FilterDevelopabilityPlugin::getClass(const QAction *a) const
{
    switch(ID(a)) {
    case FP_MAKE_DEVELOPABLE :
        return FilterPlugin::Remeshing;
    default :
        assert(0);
        return FilterPlugin::Generic;
    }
}

FilterPlugin::FilterArity FilterDevelopabilityPlugin::filterArity(const QAction*) const
{
    return SINGLE_MESH;
}

int FilterDevelopabilityPlugin::getPreConditions(const QAction*) const
{
    return MeshModel::MM_NONE;
}

int FilterDevelopabilityPlugin::postCondition(const QAction*) const
{
    return MeshModel::MM_ALL;
}

RichParameterList FilterDevelopabilityPlugin::initParameterList(const QAction *action,const MeshModel &m)
{
    RichParameterList parlst;
    QList<QString> optMethodsList{ "[F] Fixed stepsize", "[B] Backtracking line search" };
    switch(ID(action)) {
    case FP_MAKE_DEVELOPABLE :

        parlst.addParam(RichEnum("OptMethod", 1, optMethodsList, "Gradient method", "The gradient method optimization algorithm to use"));
        parlst.addParam(RichInt("MaxFunEvals", 400, "Max function evaluations", "The maximum number of function evaluation. Once reached, the optimization stops"));
        parlst.addParam(RichFloat("Eps", 1e-5, "Stop threshold", "Optimization stops when the squared norm of the gradient is less than or equal to the accuracy"));
        parlst.addParam(RichFloat("StepSize", 0.01, "Initial step size", "The initial step size of the opt method, fixed when using [F] optimizer"));
        parlst.addParam(RichFloat("MinStepSize", 1e-10, "Min step size (B only)", "The minimum step size for the backtracking line search opt method"));
        parlst.addParam(RichFloat("Tau", 0.8, "Tau (B only)", "Scaling factor of the step size for the backtracking line search opt method"));
        parlst.addParam(RichFloat("M1", 1e-4, "Armijo constant (B only)", "The constant of the Armijo condition of the backtracking line search opt method"));
        parlst.addParam(RichBool("EdgeFlips", true, "Apply edge flips", "Whether or not to apply edge flips when necessary during optimization"));
        parlst.addParam(RichBool("EdgeCollapses", true, "Apply edge collapses", "Whether or not to apply edge collapses when necessary during optimization"));
        parlst.addParam(RichFloat("AngleThreshold", 18, "Post-processing angle threshold (deg)", "The maximum angle under which an edge flip or an edge collapse must be performed during optimization"));
        break;
    default :
        assert(0);
    }
    return parlst;
}

std::map<std::string, QVariant> FilterDevelopabilityPlugin::applyFilter(const QAction * action, const RichParameterList & parameters, MeshDocument &md, unsigned int& /*postConditionMask*/, vcg::CallBackPos *cb)
{
    switch(ID(action)) {
    case FP_MAKE_DEVELOPABLE :
        makeDevelopable(md,
                        cb,
                        parameters.getEnum("OptMethod"),
                        parameters.getInt("MaxFunEvals"),
                        parameters.getFloat("Eps"),
                        parameters.getFloat("StepSize"),
                        parameters.getFloat("MinStepSize"),
                        parameters.getFloat("Tau"),
                        parameters.getFloat("M1"),
                        parameters.getBool("EdgeFlips"),
                        parameters.getBool("EdgeCollapses"),
                        parameters.getFloat("AngleThreshold"));
        break;
    default :
        wrongActionCalled(action);
    }
    return std::map<std::string, QVariant>();
}

void FilterDevelopabilityPlugin::makeDevelopable(MeshDocument &md,
                                                   vcg::CallBackPos *cb,
                                                   int optMethod,
                                                   int maxFunEval,
                                                   double eps,
                                                   double initialStepSize,
                                                   double minStepSize,
                                                   double tau,
                                                   double m1,
                                                   bool doEdgeFlip,
                                                   bool doEdgeCollapse,
                                                   double angleThreshold)
{
    // Get the current mesh, update FF topology and check 2-manifold precondition
    
    md.mm()->updateDataMask(MeshModel::MM_FACEFACETOPO);
    CMeshO &m = md.mm()->cm;

    if(vcg::tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m) > 0)
        throw MLException("non possible developability optimization because of non manifold edges");
    if(vcg::tri::Clean<CMeshO>::CountNonManifoldVertexFF(m) > 0)
        throw MLException("non possible developability optimization because of non manifold verties");

    // Apply an initial remeshing if necessary

    MeshPostProcessing<CMeshO> postProcessing(doEdgeFlip, doEdgeCollapse, vcg::math::ToRad(angleThreshold));
    if(postProcessing.process(m))
        log("An initial remeshing has been applied");

    // Temporarily move the mesh to origin and scale the diagonal of the bounding box to unit

    double boundingBoxDiag;
    vcg::Point3d boundingBoxCenter;

    vcg::tri::UpdateBounding<CMeshO>::Box(m);
    boundingBoxDiag = m.bbox.Diag();
    boundingBoxCenter = m.bbox.Center();
    vcg::tri::UpdatePosition<CMeshO>::Translate(m, -boundingBoxCenter);
    vcg::tri::UpdatePosition<CMeshO>::Scale(m, 1.0 / boundingBoxDiag);

    // Proceed to the actual optimization

    vcg::tri::UpdateFlags<CMeshO>::VertexBorderFromFaceAdj(m);
    Optimizer<CMeshO>* opt;

    if(optMethod == 0)
        opt = new FixedStepOpt<CMeshO>(m, maxFunEval, eps, initialStepSize);
    else
        opt = new BacktrackingOpt<CMeshO>(m, maxFunEval, eps, initialStepSize, minStepSize, tau, m1);

    while(opt->step())
    {
        if(optMethod == 0)
            log("[F] nFunEvals:%d gradSqNorm:%f energy:%f", opt->getNFunEval(), opt->getGradientSqNorm(), opt->getEnergy());
        else
            log("[B] nFunEvals:%d stepSize:%f gradSqNorm:%f energy:%f", opt->getNFunEval(), opt->getStepSize(), opt->getGradientSqNorm(), opt->getEnergy());

        if(optMethod == 1 || (opt->getNFunEval() % 10 == 0))
            cb(100 * opt->getNFunEval() / maxFunEval, "Optimizing developability energy...");
                
        if(postProcessing.process(m))
        {
            log("Remeshing applied");
            opt->reset();
        }
    }

    delete opt;

    // Reset original position and scale of the mesh

    vcg::tri::UpdatePosition<CMeshO>::Scale(m, boundingBoxDiag);
    vcg::tri::UpdatePosition<CMeshO>::Translate(m, boundingBoxCenter);

    md.mm()->updateBoxAndNormals();
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterDevelopabilityPlugin)
