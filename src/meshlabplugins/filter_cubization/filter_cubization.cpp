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

#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "filter_cubization.h"

#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/smooth.h>

#include <CubicStylizationFiles/cubic_stylizing.h>
#include <CubicStylizationFiles/src/cube_style_data.h>

#include <vcg/complex/algorithms/local_optimization.h>
#include <vcg/complex/algorithms/local_optimization/tri_edge_flip.h>

#include "curvedgeflip.h"
#include "curvdata.h"

using namespace vcg;

// forward declarations
class MeanCEFlip;
typedef Histogram<Scalarm> Histogramm;

class MeanCEFlip : public vcg::tri::CurvEdgeFlip<CMeshO, MeanCEFlip, MeanCEval >
{
public:
  MeanCEFlip(PosType pos, int mark,BaseParameterClass *_pp) :
    vcg::tri::CurvEdgeFlip<CMeshO, MeanCEFlip, MeanCEval >(pos, mark,_pp) {}
};


// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to
//    your filtering actions you can do here by construction the QActions accordingly
CubizationPlugin::CubizationPlugin()
{
    typeList = {
            FP_CUBIZATION,
            FP_CUBIZATION_COLORIZE
    };

    for(ActionIDType tt: types())
        actionList.push_back(new QAction(filterName(tt), this));

    cubic_ApplyEdgeFlip = false;
}

QString CubizationPlugin::pluginName() const
{
    return "FilterCubization";
}

QString CubizationPlugin::pythonFilterName(ActionIDType f) const
{
    switch (f) {
    case FP_CUBIZATION: return tr("applying_cubic_stylization");
    case FP_CUBIZATION_COLORIZE: return tr("applying_cubization_filter_colorizing_vertices");
    default: assert(0); return QString();
    }
}

QString CubizationPlugin::filterName(ActionIDType filterId) const
{
    switch (filterId) {
    case FP_CUBIZATION: return tr("Cubic stylization");
    case FP_CUBIZATION_COLORIZE: return tr("Cubic stylization and colorize");
    default: assert(0); return QString();
    }
}

FilterPlugin::FilterArity CubizationPlugin::filterArity(const QAction*) const
{
    return SINGLE_MESH;
}


int CubizationPlugin::getPreConditions(const QAction*) const
{
    return MeshModel::MM_VERTCOORD | MeshModel::MM_FACEVERT;
}

int CubizationPlugin::getRequirements(const QAction *action)
{
    switch (ID(action)) {
        case FP_CUBIZATION:
        case FP_CUBIZATION_COLORIZE:
            return MeshModel::MM_VERTCOORD |
                MeshModel::MM_FACEFACETOPO |
                MeshModel::MM_VERTFACETOPO |
                MeshModel::MM_VERTMARK;
    }

    return 0;
}

QString CubizationPlugin::filterInfo(ActionIDType filterId) const
{
    switch(filterId) {
        case FP_CUBIZATION:
            return tr("Cubic stylization of the mesh. For all detailed about cubic stylization see: <br> Hsueh-Ti Derek Liu and Alec Jacobson, 'Cubic Stylization', ACM Transactions on Graphics 2019<br> ");
        case FP_CUBIZATION_COLORIZE:
            return tr("Colorize the vertices of a mesh with cubic stylization. For all detailed about cubic stylization see: <br> Hsueh-Ti Derek Liu and Alec Jacobson, 'Cubic Stylization', ACM Transactions on Graphics 2019<br> ");
        default : assert(0);
    }
    return {};
}

 CubizationPlugin::FilterClass CubizationPlugin::getClass(const QAction *action) const
{
    switch(ID(action)) {
        case FP_CUBIZATION:             return FilterPlugin::Remeshing;
        case FP_CUBIZATION_COLORIZE:             return FilterPlugin::Remeshing;
    }
 return FilterPlugin::Generic;
}

int CubizationPlugin::postCondition(const QAction *a) const
{
    switch(ID(a))
    {
        case FP_CUBIZATION      :   return MeshModel::MM_ALL;
        case FP_CUBIZATION_COLORIZE   : return MeshModel::MM_ALL |
                                      MeshModel::MM_VERTQUALITY;
        default                       : assert(0);
    }
    return {};
}

// This function define the needed parameters for each filter.
// Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
RichParameterList CubizationPlugin::initParameterList(const QAction *action, const MeshModel &m)
{
    RichParameterList parlst;
    if (ID(action) == FP_CUBIZATION) {
        parlst.addParam(RichFloat("lcubeness", 0.2f,
                                            tr("Cubeness parameter (λ)"),
                                            tr("Control the cubeness of the mesh. Generally, the higher the cubeness parameter, the more cubic the mesh is. λ ∈ [0, 1] ")));


        parlst.addParam(RichBool("applyef", cubic_ApplyEdgeFlip, tr("Apply edge flipping"), tr("Apply edge flip optimization on cubic stylization.")));

    }
    if (ID(action) == FP_CUBIZATION_COLORIZE) {
        parlst.addParam(RichFloat("lcubeness", 0.2f,
                                            tr("Cubeness parameter (λ)"),
                                            tr("Control the cubeness of the mesh. Generally, the higher the cubeness parameter, the more cubic the mesh is. λ ∈ [0, 1] ")));


        parlst.addParam(RichBool("applyef", cubic_ApplyEdgeFlip, tr("Apply edge flipping"), tr("Apply edge flip optimization on cubic stylization.")));
    }
    return parlst;
}

// The Real Core Function doing the actual mesh processing.
// Run mesh optimization
std::map<std::string, QVariant> CubizationPlugin::applyFilter(
        const QAction *filter,
        const RichParameterList & par,
        MeshDocument &md,
        unsigned int& /*postConditionMask*/,
        vcg::CallBackPos *cb)
{

    MeshModel &m=*(md.mm());
    double energyTotal = 0.f;
    time_t start = clock();

    if (ID(filter) == FP_CUBIZATION) {
         m = ComputeCubicStylization(md, par, energyTotal);
    }

    if (ID(filter) == FP_CUBIZATION_COLORIZE) {
        m = ComputeCubicStylization(md, par, energyTotal, true);

        m.updateDataMask(MeshModel::MM_FACEFACETOPO);
        m.updateDataMask(MeshModel::MM_VERTCOLOR);
        m.updateDataMask(MeshModel::MM_VERTQUALITY);

        Histogramm H;
        vcg::tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm, H);
        vcg::tri::UpdateColor<CMeshO>::PerVertexQualityRamp(
            m.cm, H.Percentile(0.01f), H.Percentile(0.99f));
    }

    m.updateBoxAndNormals();

    log( "cubic stylization performed in %.2f sec. with cubic energy equal to %.5f", (clock() - start) / (float) CLOCKS_PER_SEC, energyTotal);
    return std::map<std::string, QVariant>();
}

MeshModel CubizationPlugin::ComputeCubicStylization(
        MeshDocument&                md,
        const RichParameterList&     par,
        double& totalEnergy,
        bool isColorizing){

    MeshModel &m=*(md.mm());
    float limit = -std::numeric_limits<float>::epsilon();

    int delvert = tri::Clean<CMeshO>::RemoveUnreferencedVertex(m.cm);
    if (delvert)
        log( "Pre-Curvature Cleaning: Removed %d unreferenced vertices",
                    delvert);


    tri::Allocator<CMeshO>::CompactVertexVector(m.cm);
    tri::Allocator<CMeshO>::CompactFaceVector(m.cm);
    m.updateDataMask(MeshModel::MM_FACEFACETOPO);
    vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromFF(m.cm);

    bool isApplyEdgeFlip = par.getBool("applyef");

    if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(m.cm) >0 && isApplyEdgeFlip) {
        throw MLException("Mesh has some not 2-manifold faces, edge flips requires manifoldness");
    }

    Eigen::MatrixXd verts;
    Eigen::MatrixXi faces;
    Eigen::MatrixXd u_verts;
    Eigen::VectorXd energy_verts;

    float lambda = par.getFloat("lcubeness");
    cube_style_data data;

    data.lambda = lambda;

    vcg::tri::Cubization<CMeshO>::Init(m.cm, verts, u_verts, faces, data);

    // apply cubic stylization
    int maxIter = 1000;
    double stopReldV = 1e-3; // stopping criteria for relative displacement
    double reldV = 0;

    for (int iter=0; iter<maxIter; iter++)
    {
        reldV = vcg::tri::Cubization<CMeshO>::Stylize(verts, u_verts, faces, data, energy_verts, totalEnergy);

        //apply Edge Flips
        if((iter%30 == 0 || reldV < stopReldV) && isApplyEdgeFlip){
            Matrix2Mesh(m.cm, u_verts, faces);

            vcg::tri::PlanarEdgeFlipParameter pp;
            float limit = -std::numeric_limits<float>::epsilon();

            vcg::LocalOptimization<CMeshO> optimiz(m.cm,&pp);
            float pthr = 10;

            // VF adjacency needed for edge flips based on vertex curvature
            vcg::tri::UpdateTopology<CMeshO>::VertexFace(m.cm);
            vcg::tri::UpdateTopology<CMeshO>::TestVertexFace(m.cm);

            pp.CoplanarAngleThresholdDeg = pthr;
            optimiz.Init<MeanCEFlip>();

            // stop when flips become harmful
            optimiz.SetTargetMetric(limit);
            optimiz.DoOptimization();
            optimiz.h.clear();

            Mesh2Matrix(m.cm, u_verts, faces);
        }

        if (reldV < stopReldV) break;
    }

    Matrix2Mesh(m.cm, u_verts, faces);

    if(isColorizing){
        assert(energy_verts.size() == m.cm.vert.size());
        for(int i = 0; i < energy_verts.size(); i++){
            m.cm.vert[i].Q() = energy_verts[i];
        }

    }

    return m;
}

MESHLAB_PLUGIN_NAME_EXPORTER(CubizationPlugin)
