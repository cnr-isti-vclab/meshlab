#include "trimesh_heatmethod.h"

#include "filter_heatgeodesic.h"

#include <common/plugins/interfaces/meshlab_plugin_logger.h>

/**
 * @brief Constructor usually performs only two simple tasks of filling the two lists
 *  - typeList: with all the possible id of the filtering actions
 *  - actionList with the corresponding actions. If you want to add icons to
 *  your filtering actions you can do here by construction the QActions accordingly
 */
FilterHeatGeodesicPlugin::FilterHeatGeodesicPlugin()
{
    typeList = {FP_COMPUTE_HEATGEODESIC_FROM_SELECTION};

    for(ActionIDType tt : types())
        actionList.push_back(new QAction(filterName(tt), this));
}

FilterHeatGeodesicPlugin::~FilterHeatGeodesicPlugin()
{
}

QString FilterHeatGeodesicPlugin::pluginName() const
{
    return "FilterHeatGeodesic";
}

/**
 * @brief ST() must return the very short string describing each filtering action
 * (this string is used also to define the menu entry)
 * @param filterId: the id of the filter
 * @return the name of the filter
 */
QString FilterHeatGeodesicPlugin::filterName(ActionIDType filterId) const
{
    switch(filterId) {
    case FP_COMPUTE_HEATGEODESIC_FROM_SELECTION :
        return QString("Compute Geodesic From Selection (Heat)");
    default :
        assert(0);
        return QString();
    }
}

/**
 * @brief FilterHeatGeodesicPlugin::pythonFilterName if you want that your filter should have a different
 * name on pymeshlab, use this function to return its python name.
 * @param f
 * @return
 */
QString FilterHeatGeodesicPlugin::pythonFilterName(ActionIDType f) const
{
    switch(f) {
    case FP_COMPUTE_HEATGEODESIC_FROM_SELECTION :
        return "compute_approximate_geodesic_from_selection";
    default :
        assert(0);
        return QString();
    }
}


/**
 * @brief // Info() must return the longer string describing each filtering action
 * (this string is used in the About plugin dialog)
 * @param filterId: the id of the filter
 * @return an info string of the filter
 */
 QString FilterHeatGeodesicPlugin::filterInfo(ActionIDType filterId) const
{
    switch(filterId) {
    case FP_COMPUTE_HEATGEODESIC_FROM_SELECTION :
        return QString("Computes an approximated geodesic distance from the selected vertices to all others. This algorithm implements the heat method.");
    default :
        assert(0);
        return QString("Unknown Filter");
    }
}

 /**
 * @brief The FilterClass describes in which generic class of filters it fits.
 * This choice affect the submenu in which each filter will be placed
 * More than a single class can be chosen.
 * @param a: the action of the filter
 * @return the class od the filter
 */
FilterHeatGeodesicPlugin::FilterClass FilterHeatGeodesicPlugin::getClass(const QAction *a) const
{
    switch(ID(a)) {
    case FP_COMPUTE_HEATGEODESIC_FROM_SELECTION :
        return FilterPlugin::Measure; // Note: not sure
        // return FilterGeodesic::FilterClass(FilterPlugin::VertexColoring + FilterPlugin::Quality);
    default :
        assert(0);
        return FilterPlugin::Generic;
    }
}

/**
 * @brief FilterSamplePlugin::filterArity
 * @return
 */
FilterPlugin::FilterArity FilterHeatGeodesicPlugin::filterArity(const QAction*) const
{
    return SINGLE_MESH;
}

/**
 * @brief FilterSamplePlugin::getPreConditions
 * @return
 */
int FilterHeatGeodesicPlugin::getPreConditions(const QAction*) const
{
    // NOTE: note sure about these
    return MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTFACETOPO;
}

/**
 * @brief FilterSamplePlugin::postCondition
 * @return
 */
int FilterHeatGeodesicPlugin::postCondition(const QAction*) const
{
    // NOTE: note sure about these
    return MeshModel::MM_VERTQUALITY | MeshModel::MM_VERTCOLOR;
}

/**
 * @brief This function define the needed parameters for each filter. Return true if the filter has some parameters
 * it is called every time, so you can set the default value of parameters according to the mesh
 * For each parameter you need to define,
 * - the name of the parameter,
 * - the default value
 * - the string shown in the dialog
 * - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
 * @param action
 * @param m
 * @param parlst
 */
RichParameterList FilterHeatGeodesicPlugin::initParameterList(const QAction *action,const MeshModel &m)
{
    RichParameterList parlst;
    switch(ID(action)) {
    case FP_COMPUTE_HEATGEODESIC_FROM_SELECTION :
        parlst.addParam(RichFloat("m", 1, "m", "Multiplier used in backward Euler timestep."));
        break;
    default :
        assert(0);
    }
    return parlst;
}

/**
 * @brief The Real Core Function doing the actual mesh processing.
 * @param action
 * @param md: an object containing all the meshes and rasters of MeshLab
 * @param par: the set of parameters of each filter
 * @param cb: callback object to tell MeshLab the percentage of execution of the filter
 * @return true if the filter has been applied correctly, false otherwise
 */
std::map<std::string, QVariant> FilterHeatGeodesicPlugin::applyFilter(const QAction * action, const RichParameterList & parameters, MeshDocument &md, unsigned int& /*postConditionMask*/, vcg::CallBackPos *cb)
{
    MeshModel &mm=*(md.mm());
    switch(ID(action)) {
    case FP_COMPUTE_HEATGEODESIC_FROM_SELECTION :
    {
        mm.updateDataMask(MeshModel::MM_FACEFACETOPO);
        mm.updateDataMask(MeshModel::MM_VERTFACETOPO);
        mm.updateDataMask(MeshModel::MM_FACEQUALITY);
        mm.updateDataMask(MeshModel::MM_FACENORMAL);
        mm.updateDataMask(MeshModel::MM_VERTQUALITY);
        mm.updateDataMask(MeshModel::MM_VERTCOLOR);
        CMeshO &mesh = mm.cm;
        // TODO: compact all vectors
        computeHeatGeodesicFromSelection(mesh, cb, parameters.getFloat("m"));
    }
        break;
    default :
        wrongActionCalled(action);
    }
    return std::map<std::string, QVariant>();
}

inline void FilterHeatGeodesicPlugin::computeHeatGeodesicFromSelection(CMeshO& mesh, vcg::CallBackPos* cb, float m){
    vcg::tri::Allocator<CMeshO>::CompactEveryVector(mesh);

    // build boundary conditions
    Eigen::VectorXd sourcePoints(mesh.VN());
    int selection_count = 0;
    for(int i=0; i < mesh.VN(); i++){
        sourcePoints(i) = mesh.vert[i].IsS() ? (++selection_count, 1) : 0;
    }
    if (selection_count < 1){
        log("Warning: no vertices are selected! aborting computation.");
        return;
    }

    std::shared_ptr<HeatMethodSolver> HMSolver;

    // delete state if the mesh has changed
    bool meshHasChanged = false; // Update with appropriate function
    if (meshHasChanged) {
        auto handle = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::shared_ptr<HeatMethodSolver>>(mesh, std::string("HeatMethodSolver"));
        vcg::tri::Allocator<CMeshO>::DeletePerMeshAttribute<std::shared_ptr<HeatMethodSolver>>(mesh, handle);
    }

    // recover state if it exists
    if (!vcg::tri::HasPerMeshAttribute(mesh, "HeatMethodSolver")){

        // update topology and face normals
        vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh);
        vcg::tri::UpdateTopology<CMeshO>::FaceFace(mesh);
        vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mesh);

        // state variables
        Eigen::SparseMatrix<double> massMatrix;
        Eigen::SparseMatrix<double> cotanMatrix;
        double averageEdgeLength;

        buildMassMatrix(mesh, massMatrix);
        // this step is very slow (95% of total time) hence we pass the callback
        // to update progress and avoid freezes
        buildCotanLowerTriMatrix(mesh, cotanMatrix, cb);
        averageEdgeLength = computeAverageEdgeLength(mesh);

        cb(91, "Matrix Factorization...");
        HMSolver = std::make_shared<HeatMethodSolver>(HeatMethodSolver(std::move(massMatrix), std::move(cotanMatrix), averageEdgeLength, m));
        if (HMSolver->factorization_failed())
            log("Warning: factorization has failed. The mesh is non-manifold or badly conditioned (angles ~ 0deg)");
        auto HMSolverHandle = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::shared_ptr<HeatMethodSolver>>(mesh, std::string("HeatMethodSolver"));
        HMSolverHandle() = HMSolver;
    }
    else {
        // recover solver
        HMSolver = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<std::shared_ptr<HeatMethodSolver>>(mesh, std::string("HeatMethodSolver"))();
        // if m has changed rebuild factorizations from existing matrices
        if (HMSolver->get_m() != m){
            cb(91, "Rebuilding Factorization...");
            HMSolver->rebuildFactorization(m);
        }
    }

    cb(95, "Computing Geodesic Distance...");
    Eigen::VectorXd geodesicDistance = HMSolver->solve(mesh, sourcePoints);
    if (HMSolver->solver_failed())
        log("Warning: solver has failed.");

    cb(99, "Updating Mesh...");
    // set geodesic distance as quality and color
    for(int i=0; i < mesh.VN(); i++){
        mesh.vert[i].Q() = geodesicDistance(i);
    }
    vcg::tri::UpdateColor<CMeshO>::PerVertexQualityRamp(mesh);
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterSamplePlugin)
