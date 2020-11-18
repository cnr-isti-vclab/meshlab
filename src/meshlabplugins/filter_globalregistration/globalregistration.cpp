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

#include "globalregistration.h"
#include <gr/algorithms/match4pcsBase.h>
#include <gr/algorithms/Functor4pcs.h>
#include <gr/algorithms/FunctorSuper4pcs.h>
#include <gr/algorithms/PointPairFilter.h>
//#include <QtScript>

using PointType = gr::Point3D<MESHLAB_SCALAR>;

GlobalRegistrationPlugin::GlobalRegistrationPlugin()
{
    typeList << FP_GLOBAL_REGISTRATION;

  foreach(FilterIDType tt , types())
      actionList << new QAction(filterName(tt), this);
}

QString GlobalRegistrationPlugin::pluginName() const
{
    return "FilterGlobalRegistration";
}

QString GlobalRegistrationPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
        case FP_GLOBAL_REGISTRATION :  return QString("Global registration");
        default : assert(0);
    }
  return QString();
}

 QString GlobalRegistrationPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
        case FP_GLOBAL_REGISTRATION :  return QString("Compute the rigid transformation aligning two 3d objects.");
        default : assert(0);
    }
    return QString("Unknown Filter");
}

GlobalRegistrationPlugin::FilterClass GlobalRegistrationPlugin::getClass(const QAction *a) const
{
  switch(ID(a))
    {
        case FP_GLOBAL_REGISTRATION :  return FilterPluginInterface::PointSet;
        default : assert(0);
    }
    return FilterPluginInterface::Generic;
}

void GlobalRegistrationPlugin::initParameterList(const QAction *action,MeshDocument &md, RichParameterList & parlst)
{

     switch(ID(action))	 {
        case FP_GLOBAL_REGISTRATION :

         parlst.addParam(RichMesh ("refMesh",md.mm(),&md, "Reference Mesh",	"Reference point-cloud or mesh"));
         parlst.addParam(RichMesh ("targetMesh",md.mm(),&md, "Target Mesh",	"Point-cloud or mesh to be aligned to the reference"));
         parlst.addParam(RichAbsPerc("overlap", 50, 0, 100, "Overlap Ratio", "Overlap ratio between the two clouds (command line option: -o)"));
         parlst.addParam(RichFloat("delta",   0.1, "Registration tolerance", "Tolerance value for the congruent set exploration and LCP computation (command line option: -d)"));
         parlst.addParam(RichInt("nbSamples", 200, "Number of samples", "Number of samples used in each mesh (command line option: -n)"));
         parlst.addParam(RichFloat("norm_diff", -1, "Filter: difference of normal (degrees)", "Allowed difference of normals allowed between corresponding pairs of points(command line option: -a)"));
         parlst.addParam(RichFloat("color_diff", -1, "Filter: difference color", "Allowed difference of colors allowed between corresponding pairs of points(command line option: -c)"));
         parlst.addParam(RichInt("max_time_seconds", 10000, "Max. Computation time, in seconds", "Stop the computation before the end of the exploration (command line option: -t)"));
         parlst.addParam(RichBool("useSuper4PCS", true, "Use Super4PCS", "When disable, use 4PCS algorithm (command line option: -x"));

         break;
     default : assert(0);
    }
}


using MatrixType = Eigen::Matrix<MESHLAB_SCALAR, 4, 4>;

struct RealTimeTransformVisitor {
    CMeshO* mesh = nullptr;
    GlobalRegistrationPlugin* plugin;
    inline void operator() (
            float /*fraction*/,
			float /*best_LCP*/,
            Eigen::Ref<MatrixType> mat) {
        //plugin->Log("Found new configuration. LCP = %f", best_LCP);

        mesh->Tr.FromEigenMatrix(mat);
    }
	bool needsGlobalTransformation() const { return true; }
};

struct TransformVisitor {
    CMeshO* mesh = nullptr;
    GlobalRegistrationPlugin* plugin;
    inline void operator() (
            float /*fraction*/,
			float /*best_LCP*/,
            Eigen::Ref<MatrixType> /*mat*/) const {
        //plugin->Log("Found new configuration. LCP = %f", best_LCP);
    }
	bool needsGlobalTransformation() const { return false; }
};

// init Super4PCS point cloud internal structure
auto fillPointSet = [] (const CMeshO& m, std::vector<gr::Point3D<CMeshO::ScalarType>>& out) {
    using gr::Point3D;
    Point3D<CMeshO::ScalarType> p;
    out.clear();
    out.reserve(m.vert.size());

    // TODO: copy other point-wise information, if any
    for(size_t i = 0; i< m.vert.size(); i++){
        const auto& vertex = m.vert[i];
        vertex.P().ToEigenVector(p.pos());
        out.push_back(p);
    }
};

template <typename MatcherType>
float align ( CMeshO* refMesh, CMeshO* trgMesh,
              const RichParameterList & par,
              MatrixType & mat,
              typename MatcherType::TransformVisitor & v) {

    using SamplerType   = gr::UniformDistSampler<gr::Point3D<CMeshO::ScalarType>>;
    using OptionType    = typename MatcherType::OptionsType;

    OptionType opt;
    opt.configureOverlap(par.getAbsPerc("overlap")/100.f);
    opt.delta                 = par.getFloat("delta");
    opt.sample_size           = par.getInt("nbSamples");
    opt.max_normal_difference = par.getFloat("norm_diff");
    opt.max_color_distance    = par.getFloat("color_diff");
    opt.max_time_seconds      = par.getInt("max_time_seconds");

    std::vector<gr::Point3D<CMeshO::ScalarType>> set1, set2;
    fillPointSet(*refMesh, set1);
    fillPointSet(*trgMesh, set2);

    gr::Utils::Logger logger (gr::Utils::LogLevel::NoLog);
    SamplerType sampler;
    MatcherType matcher (opt, logger);

    return matcher.ComputeTransformation(set1, set2, mat, sampler, v);
}

// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool GlobalRegistrationPlugin::applyFilter(
		const QAction* /*filter*/,
		MeshDocument& /*md*/,
		std::map<std::string, QVariant>&,
		unsigned int& /*postConditionMask*/,
		const RichParameterList& par,
		vcg::CallBackPos* /*cb*/)
{

    MeshModel *mmref = par.getMesh("refMesh");
    MeshModel *mmtrg = par.getMesh("targetMesh");
    CMeshO *refMesh=&mmref->cm;
    CMeshO *trgMesh=&mmtrg->cm;

    bool useSuper4PCS         = par.getBool("useSuper4PCS");

    MatrixType mat;
    float score = -1;

    TransformVisitor v;
    v.mesh = trgMesh;
    v.plugin = this;

    if (useSuper4PCS) {
        using MatcherType = gr::Match4pcsBase<gr::FunctorSuper4PCS, PointType, TransformVisitor, gr::AdaptivePointFilter, gr::AdaptivePointFilter::Options>;
        score = align< MatcherType >(refMesh, trgMesh, par, mat, v);
    } else {
        using MatcherType = gr::Match4pcsBase<gr::Functor4PCS, PointType, TransformVisitor, gr::AdaptivePointFilter, gr::AdaptivePointFilter::Options>;
        score = align< MatcherType >(refMesh, trgMesh, par, mat, v);
    }

    // run
    log("Final LCP = %f", score);
    v.mesh->Tr.FromEigenMatrix(mat);

    return true;
}

MESHLAB_PLUGIN_NAME_EXPORTER(GlobalRegistrationPlugin)
