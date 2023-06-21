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
    // build boundary conditions
    Eigen::VectorXd boundaryConditions(mesh.VN());
    int selection_count = 0;
    for(int i=0; i < mesh.VN(); i++){
        boundaryConditions(i) = mesh.vert[i].IsS() ? (++selection_count, 1) : 0;
    }
    if (selection_count < 1){
        log("Warning: no vertices are selected! aborting computation.");
        return;
    }

    // update topology and face normals
    vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh);
    vcg::tri::UpdateTopology<CMeshO>::FaceFace(mesh);
    vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mesh);

    // state variables
    Eigen::SparseMatrix<double> massMatrix;
    Eigen::SparseMatrix<double> cotanMatrix;
    double avg_edge_len;

    typedef std::tuple<
        Eigen::SparseMatrix<double>, // system1 fact
        Eigen::SparseMatrix<double>, // system2 fact
        double                       // avg edge len
    > HeatMethodData;

    // recover state if it exists
    if (!vcg::tri::HasPerMeshAttribute(mesh, "HeatMethodData")){
        // current solution saves matrices not factorizations
        // if we save factorization it is best to at least also save cotanMatrix
        // to avoid an expensive reconstruction when parameter("m") changes
        buildMassMatrix(mesh, massMatrix);
        // this step is very slow (95% of total time) hence we pass the callback
        // to update progress and avoid freezes
        buildCotanLowerTriMatrix(mesh, cotanMatrix, cb);
        avg_edge_len = computeAverageEdgeLength(mesh);

        HeatMethodData &HMData = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<HeatMethodData>(mesh, std::string("HeatMethodData"))();
        HMData = HeatMethodData(massMatrix, cotanMatrix, avg_edge_len);
    }
    else {
        HeatMethodData &HMData = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<HeatMethodData>(mesh, std::string("HeatMethodData"))();
        std::tie (massMatrix, cotanMatrix, avg_edge_len) = HMData;
    }

    // cholesky type solver
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;

    // build system 1
    double timestep = m * avg_edge_len * avg_edge_len;
    Eigen::SparseMatrix<double> system1(mesh.VN(), mesh.VN());
    system1 = massMatrix - timestep * cotanMatrix;

    cb(91, "Computing Factorization 1...");

    // factorize system 1
    solver.compute(system1);
    if(solver.info() != Eigen::Success) {
        log("Error: Factorization Failed (Heatflow).");
    }

    cb(93, "Solving System 1...");
    // solve system 1
    Eigen::VectorXd heatflow = solver.solve(boundaryConditions); // (VN)
    if(solver.info() != Eigen::Success) {
        log("Error: Solver Failed (Heatflow).");
    }

    cb(95, "Computing Gradient, VectorField, Divergence...");
    // intermediate steps
    Eigen::MatrixX3d heatGradient = computeVertexGradient(mesh, heatflow); // (FN, 3)
    Eigen::MatrixX3d normalizedVectorField = normalizeVectorField(-heatGradient); // (FN, 3)
    Eigen::VectorXd divergence = computeVertexDivergence(mesh, normalizedVectorField); // (VN)

    // build system 2
    Eigen::SparseMatrix<double> system2(mesh.VN(), mesh.VN());
    system2 = cotanMatrix; //+ 1e-6 * Eigen::Matrix<double,-1,-1>::Identity(mesh.VN(), mesh.VN());

    cb(96, "Computing Factorization 2...");
    // factorize system 2
    solver.compute(system2);
    if(solver.info() != Eigen::Success) {
        log("Error: Factorization Failed (Geodesic).");
    }

    cb(97, "Solving System 2...");
    // solve system 2
    Eigen::VectorXd geodesicDistance = solver.solve(divergence);

    if(solver.info() != Eigen::Success) {
        log("Error: Solver Failed (Geodesic).");
    }

    // shift to impose boundary conditions (dist(d) = 0 \forall d \in init_cond)
    geodesicDistance.array() -= geodesicDistance.minCoeff();

    cb(99, "Updating Quality and Color...");
    // set geodesic distance as quality and color
    for(int i=0; i < mesh.vert.size(); i++){
        mesh.vert[i].Q() = geodesicDistance(i);
    }
    vcg::tri::UpdateColor<CMeshO>::PerVertexQualityRamp(mesh);
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterSamplePlugin)
