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

#define PAR_MOVING_MESH "MovingMesh"
#define PAR_FIXED_MESH "FixedMesh"

/* Static variables needed by the vcg::AlignPair::align() method */
std::vector<vcg::Point3d>* vcg::PointMatchingScale::fix;
std::vector<vcg::Point3d>* vcg::PointMatchingScale::mov;
vcg::Box3d vcg::PointMatchingScale::b;

/**
 * @brief
 * Constructor usually performs only two simple tasks of filling the two lists
 *  - typeList: with all the possible id of the filtering actions
 *  - actionList with the corresponding actions.
 * If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
 */
FilterIcpPlugin::FilterIcpPlugin()
{ 
	this->typeList = { FP_TWO_MESH_ICP, FP_GLOBAL_MESH_ICP };

	for (const ActionIDType& tt : typeList) {
		actionList.push_back(new QAction(this->filterName(tt), this));
	}
}

QString FilterIcpPlugin::pluginName() const
{
	return QString{"FilterIcpPlugin"};
}

QString FilterIcpPlugin::vendor() const
{
	return QString{"Gabriele Pappalardo"};
}

/**
 * @brief ST() must return the very short string describing each filtering action
 * (this string is used also to define the menu entry)
 * @param filterId: the id of the filter
 * @return the name of the filter
 */
QString FilterIcpPlugin::filterName(ActionIDType filterId) const
{
	switch(filterId) {
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
QString FilterIcpPlugin::filterInfo(ActionIDType filterId) const
{
	switch (filterId) {
        case FP_TWO_MESH_ICP :
        case FP_GLOBAL_MESH_ICP:
            return tr("Allows one to align different layers together.");
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
FilterIcpPlugin::FilterClass FilterIcpPlugin::getClass(const QAction *action) const
{
	switch (ID(action)) {
        case FP_TWO_MESH_ICP:
        case FP_GLOBAL_MESH_ICP:
            return FilterPlugin::Remeshing;
        default :
            assert(0);
            return FilterPlugin::Generic;
	}
}

/**
 * @brief FilterSamplePlugin::filterArity
 * @return
 */
FilterPlugin::FilterArity FilterIcpPlugin::filterArity(const QAction* action) const
{
    return VARIABLE;
}

/**
 * @brief FilterSamplePlugin::getPreConditions
 * @return
 */
int FilterIcpPlugin::getPreConditions(const QAction*) const
{
	return MeshModel::MM_NONE;
}

/**
 * @brief FilterSamplePlugin::postCondition
 * @return
 */
int FilterIcpPlugin::postCondition(const QAction*) const
{
	return MeshModel::MM_VERTCOORD | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTNORMAL;
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
RichParameterList FilterIcpPlugin::initParameterList(const QAction* action, const MeshDocument &md)
{

    RichParameterList parameterList;

	switch (ID(action)) {

        case FP_TWO_MESH_ICP: {
            parameterList.addParam(RichMesh(PAR_FIXED_MESH, 0, &md, "The fixed mesh."));
            parameterList.addParam(RichMesh(PAR_MOVING_MESH, 1, &md, "The moving mesh."));
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

    // Add default ICP parameters to the parameters List
    FilterIcpAlignParameter::AlignPairParamToRichParameterSet(this->alignParameters, parameterList);

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
		const QAction * action,
		const RichParameterList & par,
		MeshDocument &md,
		unsigned int& /*postConditionMask*/,
		vcg::CallBackPos *cb)
{

    // Set the align parameters from the RichParameterList
    FilterIcpAlignParameter::RichParameterSetToAlignPairParam(par, this->alignParameters);

    switch (ID(action)) {

        case FP_TWO_MESH_ICP: {
            applyIcpTwoMeshes(md, par);
            break;
        }
        case FP_GLOBAL_MESH_ICP: {
            // TODO: Globally align meshes
            break;
        }
        default :
            wrongActionCalled(action);
	}
	return std::map<std::string, QVariant>();
}


void FilterIcpPlugin::applyIcpTwoMeshes(MeshDocument &meshDocument, const RichParameterList &par) {

    vcg::AlignPair aligner;

    vcg::AlignPair::A2Mesh fix;

    vcg::AlignPair::A2Grid UG;
    vcg::AlignPair::A2GridVert VG;

    vcg::AlignPair::Result alignerResult;

    std::vector<vcg::AlignPair::A2Vertex> tempMoving;

    MeshModel* fixedMesh = meshDocument.getMesh(par.getMeshId(PAR_FIXED_MESH));
    MeshModel* movingMesh = meshDocument.getMesh(par.getMeshId(PAR_MOVING_MESH));

    vcg::Matrix44d inputMatrix = vcg::Matrix44d::Identity();

    if (fixedMesh == movingMesh) {
        throw MLException{"Cannot apply ICP on the same mesh!"};
    }

    qDebug("Fixed Mesh: %s\nMoving Mesh: %s\n",
           fixedMesh->fullName().toStdString().c_str(), movingMesh->fullName().toStdString().c_str());

    // 1) Convert fixed mesh and put it into the grid.
    fixedMesh->updateDataMask(MeshModel::MM_FACEMARK);
    aligner.convertMesh<CMeshO>(fixedMesh->cm, fix);

    if (fixedMesh->cm.fn == 0 || this->alignParameters.UseVertexOnly) {
        fix.initVert(vcg::Matrix44d::Identity());
        vcg::AlignPair::InitFixVert(&fix, this->alignParameters, VG);
    }
    else {
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

    // Execute the ICP algorithm
    bool success = aligner.align(inputMatrix, UG, VG, alignerResult);
    if (!success) {
        throw MLException{vcg::AlignPair::errorMsg(alignerResult.status)};
    }

    alignerResult.FixName = static_cast<int>(par.getMeshId(PAR_FIXED_MESH));
    alignerResult.MovName = static_cast<int>(par.getMeshId(PAR_MOVING_MESH));

    movingMesh->cm.Tr.FromMatrix(alignerResult.Tr);

}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterSamplePlugin)
