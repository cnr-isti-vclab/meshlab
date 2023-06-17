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
        return "Compute Geodesic From Selection (Heat)";
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
        return "compute_heat_geodesic_from_selection";
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
        return "Computes an approximated geodesic distance from the selected vertices to all others. This algorithm implements the heat method.";
    default :
        assert(0);
        return "Unknown Filter";
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
        return FilterPlugin::Measure;
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
    return MeshModel::MM_VERTCOORD | MeshModel::MM_VERTQUALITY |
           MeshModel::MM_FACEFACETOPO | MeshModel::MM_VERTFACETOPO |
           MeshModel::MM_FACEQUALITY | MeshModel::MM_FACENORMAL |
           MeshModel::MM_VERTFLAGSELECT;
}

/**
 * @brief FilterSamplePlugin::postCondition
 * @return
 */
int FilterHeatGeodesicPlugin::postCondition(const QAction*) const
{
    // NOTE: note sure about these
    return MeshModel::MM_VERTQUALITY | MeshModel::MM_FACEQUALITY;
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
    switch(ID(action)) {
    case FP_COMPUTE_HEATGEODESIC_FROM_SELECTION :
    {
        CMeshO &mesh = md.mm()->cm;
        cb(100*0/10, "Computing Boundary Conditions...");
        Eigen::VectorXd initialConditions(mesh.VN());
        for(int i=0; i < mesh.vert.size(); i++){
            initialConditions[i] = mesh.vert[i].IsS() ? 1 : 0;
        }
        cb(100*1/10, "Updating Topology and Computing Face Normals...");
        vcg::tri::UpdateTopology<CMeshO>::VertexFace(mesh);
        vcg::tri::UpdateTopology<CMeshO>::FaceFace(mesh);
        vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mesh);

        cb(100*2/10, "Building Linear System (Heatflow)...");
        Eigen::SparseMatrix<double> mass(mesh.VN(), mesh.VN());
        buildMassMatrix(mesh, mass);

        Eigen::SparseMatrix<double> cotanOperator(mesh.VN(), mesh.VN());
        buildCotanMatrix(mesh, cotanOperator);

        double avg_edge_len = computeAverageEdgeLength(mesh);
        double timestep = parameters.getFloat("m") * avg_edge_len * avg_edge_len;
        Eigen::SparseMatrix<double> system1(mesh.VN(), mesh.VN());
        system1 = mass - timestep * cotanOperator;

        Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;

        cb(100*3/10, "Computing Matrix Factorization (Heatflow)...");
        solver.compute(system1);
        if(solver.info() != Eigen::Success) {
            log("Error: Factorization Failed (Heatflow).");
        }
        cb(100*4/10, "Solving Linear System (Heatflow)...");
        Eigen::VectorXd heatflow = solver.solve(initialConditions); // (VN)
        if(solver.info() != Eigen::Success) {
            log("Error: Solver Failed (Heatflow).");
        }
        cb(100*5/10, "Computing Heat Gradient...");
        Eigen::MatrixX3d heatGradient = computeVertexGradient(mesh, heatflow); // (FN, 3)

        Eigen::MatrixX3d normalizedVectorField = normalizeVectorField(-heatGradient); // (FN, 3)
        cb(100*6/10, "Computing Divergence...");
        Eigen::VectorXd divergence = computeVertexDivergence(mesh, normalizedVectorField); // (VN)

        cb(100*7/10, "Building Linear System (Geodesic)...");
        Eigen::SparseMatrix<double> system2(mesh.VN(), mesh.VN());
        system2 = cotanOperator; //+ 1e-6 * Eigen::Matrix<double,-1,-1>::Identity(mesh.VN(), mesh.VN());

        cb(100*8/10, "Computing Matrix Factorization (Geodesic)...");
        solver.compute(system2);
        if(solver.info() != Eigen::Success) {
            log("Error: Factorization Failed (Geodesic).");
        }
        cb(100*9/10, "Solving Linear System (Geodesic)...");
        Eigen::VectorXd geodesicDistance = solver.solve(divergence);
        if(solver.info() != Eigen::Success) {
            log("Error: Solver Failed (Geodesic).");
        }
        cb(100*10/10, "Saving Geodesic Distance...");
        // invert and shift
        geodesicDistance.array() *= -1; // no clue as to why this needs to be here
        geodesicDistance.array() -= geodesicDistance.minCoeff();

        // set geodesic distance as quality
        for(int i=0; i < mesh.vert.size(); i++){
            mesh.vert[i].Q() = geodesicDistance(0);
        }
    }
        break;
    default :
        wrongActionCalled(action);
    }
    return std::map<std::string, QVariant>();
}

void computeHeatGeodesicFromSelection(MeshDocument &md, vcg::CallBackPos *cb, float m){

}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterSamplePlugin)
