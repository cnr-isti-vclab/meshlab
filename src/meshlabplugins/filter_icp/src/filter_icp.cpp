/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
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

#include "filter_icp.h"

#define PAR_SOURCE_MESH         "SourceMesh"
#define PAR_REFERENCE_MESH      "ReferenceMesh"
#define PAR_SAVE_LAST_ITERATION "SaveLastIteration"

/* Static variables needed by the vcg::AlignPair::align() method */
std::vector<vcg::Point3d> *vcg::PointMatchingScale::fix;
std::vector<vcg::Point3d> *vcg::PointMatchingScale::mov;
vcg::Box3d vcg::PointMatchingScale::b;

/**
 * @brief
 * Constructor usually performs only two simple tasks of filling the two lists
 *  - typeList: with all the possible id of the filtering actions
 *  - actionList with the corresponding actions.
 * If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
 */
FilterIcpPlugin::FilterIcpPlugin() {
    this->typeList = {FP_TWO_MESH_ICP, FP_GLOBAL_MESH_ICP};

    for (const ActionIDType &tt : typeList) {
        actionList.push_back(new QAction(this->filterName(tt), this));
    }
}

QString FilterIcpPlugin::pluginName() const {
    return QString{"FilterIcpPlugin"};
}

/**
 * @brief ST() must return the very short string describing each filtering action
 * (this string is used also to define the menu entry)
 * @param filterId: the id of the filter
 * @return the name of the filter
 */
QString FilterIcpPlugin::filterName(ActionIDType filterId) const {
    switch (filterId) {
        case FP_TWO_MESH_ICP: {
            return "ICP Between Meshes";
        }
        case FP_GLOBAL_MESH_ICP: {
            return "Globally Align Meshes";
        }
        default: {
            assert(0);
            return "";
        }
    }
}


/**
 * @brief // Info() must return the longer string describing each filtering action
 * (this string is used in the About plugin dialog)
 * @param filterId: the id of the filter
 * @return an info string of the filter
 */
QString FilterIcpPlugin::filterInfo(ActionIDType filterId) const {
    switch (filterId) {
        case FP_TWO_MESH_ICP : {
            return tr("Perform the ICP algorithm to minimize the difference between two cloud of points.");
        }
        case FP_GLOBAL_MESH_ICP: {
            return tr("TODO");
        }
        default: {
            assert(0);
            return "Unknown Filter";
        }
    }
}

/**
 * @brief The FilterClass describes in which generic class of filters it fits.
 * This choice affect the submenu in which each filter will be placed
 * More than a single class can be chosen.
 * @param a: the action of the filter
 * @return the class od the filter
 */
FilterIcpPlugin::FilterClass FilterIcpPlugin::getClass(const QAction *action) const {
    switch (ID(action)) {
        case FP_TWO_MESH_ICP:
        case FP_GLOBAL_MESH_ICP:
            return FilterPlugin::Remeshing;
        default:
            assert(0);
            return FilterPlugin::Generic;
    }
}

/**
 * @brief FilterSamplePlugin::filterArity
 * @return
 */
FilterPlugin::FilterArity FilterIcpPlugin::filterArity(const QAction *action) const {
    switch (ID(action)) {
        case FP_TWO_MESH_ICP:
            return FilterArity::FIXED;
        default:
            return FilterArity::VARIABLE;
    }
}

/**
 * @brief FilterSamplePlugin::getPreConditions
 * @return
 */
int FilterIcpPlugin::getPreConditions(const QAction *) const {
    return MeshModel::MM_NONE;
}

/**
 * @brief FilterSamplePlugin::postCondition
 * @return
 */
int FilterIcpPlugin::postCondition(const QAction *) const {
    return MeshModel::MM_NONE;
}

/**
 * @brief This function returns a list of parameters needed by each filter.
 * For each parameter you need to define,
 * - the name of the parameter,
 * - the default value
 * - the string shown in the dialog
 * - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
 * @param action
 * @param md
 */
RichParameterList FilterIcpPlugin::initParameterList(const QAction *action, const MeshDocument &md) {

    RichParameterList parameterList;

    switch (ID(action)) {

        case FP_TWO_MESH_ICP: {
            /* Add the Reference and Source Mesh parameters */
            parameterList.addParam(RichMesh(PAR_REFERENCE_MESH, 0, &md, "Reference Mesh",
                                            "The Reference Mesh is the point cloud kept fixed during the ICP process."));
            parameterList.addParam(RichMesh(PAR_SOURCE_MESH, 1, &md, "Source Mesh",
                                            "The Source Mesh is the point cloud which will be roto-translated to match the Reference Mesh."));
            break;
        }

        case FP_GLOBAL_MESH_ICP: {
            // TODO
            break;
        }

        default: {
            assert(0);
        }
    }

    /* Add default ICP parameters to the parameters List */
    FilterIcpAlignParameter::AlignPairParamToRichParameterSet(this->alignParameters, parameterList);

    /* Add a checkbox to toggle 'Save Last Iteration' */
    parameterList.addParam(RichBool(PAR_SAVE_LAST_ITERATION, false, "Save Last Iteration",
                                    "Toggle this checkbox in order to save the last iteration points in two layers."));

    return parameterList;
}

/**
 * @brief The Real Core Function doing the actual mesh processing.
 * @param action
 * @param md: an object containing all the meshes and rasters of MeshLab
 * @param par: the set of parameters of each filter, with the values set by the user
 * @param cb: callback object to tell MeshLab the percentage of execution of the filter
 * @return true if the filter has been applied correctly, false otherwise
 */
std::map<std::string, QVariant> FilterIcpPlugin::applyFilter(
        const QAction *action,
        const RichParameterList &par,
        MeshDocument &md,
        unsigned int & /*postConditionMask*/,
        vcg::CallBackPos *cb) {

    // Set the align parameters from the RichParameterList
    FilterIcpAlignParameter::RichParameterSetToAlignPairParam(par, this->alignParameters);

    switch (ID(action)) {

        case FP_TWO_MESH_ICP: {
            return applyIcpTwoMeshes(md, par);
        }
        case FP_GLOBAL_MESH_ICP: {
            // TODO: Globally align meshes
            break;
        }
        default: {
            wrongActionCalled(action);
        }
    }

    return std::map<std::string, QVariant>();
}


std::map<std::string, QVariant> FilterIcpPlugin::applyIcpTwoMeshes(MeshDocument &meshDocument, const RichParameterList &par) {

    vcg::AlignPair aligner;

    vcg::AlignPair::A2Mesh fix;

    vcg::AlignPair::A2Grid UG;
    vcg::AlignPair::A2GridVert VG;

    vcg::AlignPair::Result alignerResult;

    std::vector<vcg::AlignPair::A2Vertex> tempMoving;

    MeshModel *fixedMesh = meshDocument.getMesh(par.getMeshId(PAR_REFERENCE_MESH));
    MeshModel *movingMesh = meshDocument.getMesh(par.getMeshId(PAR_SOURCE_MESH));

    vcg::Matrix44d inputMatrix = vcg::Matrix44d::Identity();

    bool saveLastIterationFlag = par.getBool(PAR_SAVE_LAST_ITERATION);

    if (fixedMesh == movingMesh) {
        throw MLException{"Cannot apply ICP on the same mesh!"};
    }

    qDebug("Fixed Mesh: %s\nMoving Mesh: %s\n",
           qUtf8Printable(fixedMesh->fullName()), qUtf8Printable(movingMesh->fullName()));

    // 1) Convert fixed mesh and put it into the grid.
    fixedMesh->updateDataMask(MeshModel::MM_FACEMARK);
    aligner.convertMesh<CMeshO>(fixedMesh->cm, fix);

    if (fixedMesh->cm.fn == 0 || this->alignParameters.UseVertexOnly) {
        fix.initVert(vcg::Matrix44d::Identity());
        vcg::AlignPair::InitFixVert(&fix, this->alignParameters, VG);
    } else {
        fix.init(vcg::Matrix44d::Identity());
        vcg::AlignPair::initFix(&fix, this->alignParameters, UG);
    }

    // 2) Convert the second mesh and sample a <alignParameters.SampleNum> points on it.
    movingMesh->updateDataMask(MeshModel::MM_FACEMARK);

    aligner.convertVertex(movingMesh->cm.vert, tempMoving);
    aligner.sampleMovVert(tempMoving, this->alignParameters.SampleNum, this->alignParameters.SampleMode);

    aligner.mov = &tempMoving;
    aligner.fix = &fix;

    // Set user's parameters to the aligner
    aligner.ap = this->alignParameters;

    // 3) Execute the ICP algorithm
    bool success = aligner.align(inputMatrix, UG, VG, alignerResult);
    if (!success) {
        throw MLException{vcg::AlignPair::errorMsg(alignerResult.status)};
    }

    alignerResult.FixName = static_cast<int>(par.getMeshId(PAR_REFERENCE_MESH));
    alignerResult.MovName = static_cast<int>(par.getMeshId(PAR_SOURCE_MESH));

    if (saveLastIterationFlag) {
        saveLastIterationPoints(meshDocument, alignerResult);
    }

    // Prints out the log
    std::vector<vcg::AlignPair::Stat::IterInfo> &I = alignerResult.as.I;

    std::list<double> minDistAbs(I.size());
    std::list<double> pcl50(I.size());
    std::list<double> sampleTested(I.size());
    std::list<double> sampleUsed(I.size());
    std::list<double> distancedDiscarded(I.size());
    std::list<double> borderDiscarded(I.size());
    std::list<double> angleDiscarded(I.size());

    // Print the header
    log("Iter | MinD | Error | Sample | Used | DistR | BordR | AnglR");
    // Print the IterInfos
    for (size_t qi = 0; qi < I.size(); ++qi) {

        // Add values inside the vector
        minDistAbs.push_back(I[qi].MinDistAbs);
        pcl50.push_back(I[qi].pcl50);
        sampleTested.push_back(I[qi].SampleTested);
        sampleUsed.push_back(I[qi].SampleUsed);
        distancedDiscarded.push_back(I[qi].DistanceDiscarded);
        borderDiscarded.push_back(I[qi].BorderDiscarded);
        angleDiscarded.push_back(I[qi].AngleDiscarded);

        log("%04zu | %6.2f | %7.4f | %05i | %05i | %5i | %5i | %5i",
                    qi,
                    I[qi].MinDistAbs,
                    I[qi].pcl50,
                    I[qi].SampleTested,
                    I[qi].SampleUsed,
                    I[qi].DistanceDiscarded,
                    I[qi].BorderDiscarded,
                    I[qi].AngleDiscarded);
    }

    // Apply the obtained transformation matrix to the moving mesh
    movingMesh->cm.Tr.FromMatrix(alignerResult.Tr);

    return std::map<std::string, QVariant> {
            {"min_dist_abs",        QVariant::fromValue(minDistAbs)},
            {"pcl_50",              QVariant::fromValue(pcl50)},
            {"sample_tested",       QVariant::fromValue(sampleTested)},
            {"sample_used",         QVariant::fromValue(sampleUsed)},
            {"distance_discarded",  QVariant::fromValue(distancedDiscarded)},
            {"border_discarded",    QVariant::fromValue(borderDiscarded)},
            {"angle_discarded",     QVariant::fromValue(angleDiscarded)},
    };
}

void FilterIcpPlugin::saveLastIterationPoints(MeshDocument &meshDocument, vcg::AlignPair::Result &alignerResult) const {

    /* Save the last points iteration */
    MeshModel *chosenMovingPointsMesh = meshDocument.addNewMesh("", "Chosen Source Points", false);
    MeshModel *correspondingFixedPointsMesh = meshDocument.addNewMesh("", "Corresponding Reference Points", false);

    std::vector<vcg::Point3d> &movingPoints = alignerResult.Pmov;
    std::vector<vcg::Point3d> &movingNormals = alignerResult.Nmov;

    std::vector<vcg::Point3d> &fixedPoints = alignerResult.Pfix;
    std::vector<vcg::Point3d> &fixedNormals = alignerResult.Nmov;

    auto viMoving = vcg::tri::Allocator<CMeshO>::AddVertices(chosenMovingPointsMesh->cm, movingPoints.size());
    auto viFixed = vcg::tri::Allocator<CMeshO>::AddVertices(correspondingFixedPointsMesh->cm, fixedPoints.size());

    // Load the moving points inside the mesh chosenMovingPointsMesh
    for (size_t i = 0; i < movingPoints.size(); i++, viMoving++) {
        (*viMoving).P() = movingPoints[i];
        (*viMoving).N() = movingNormals[i];
        (*viMoving).C() = vcg::Color4b::Green;
    }

    // Load the fixed points inside the mesh correspondingFixedPointsMesh
    for (size_t i = 0; i < fixedPoints.size(); i++, viFixed++) {
        (*viFixed).P() = fixedPoints[i];
        (*viFixed).N() = fixedPoints[i];
        (*viFixed).C() = vcg::Color4b::Red;
    }

    // Apply the result transformation matrix to the chosen points
    chosenMovingPointsMesh->cm.Tr.FromMatrix(alignerResult.Tr);

    // Update the data masks for the new meshes
    chosenMovingPointsMesh->updateDataMask(MeshModel::MM_VERTCOLOR);
    correspondingFixedPointsMesh->updateDataMask(MeshModel::MM_VERTCOLOR);

    // Update the bounding box of the new meshes
    vcg::tri::UpdateBounding<CMeshO>::Box(chosenMovingPointsMesh->cm);
    vcg::tri::UpdateBounding<CMeshO>::Box(correspondingFixedPointsMesh->cm);

}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterSamplePlugin)
