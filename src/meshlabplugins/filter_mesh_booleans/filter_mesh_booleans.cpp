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

#include "filter_mesh_booleans.h"

#include <igl/copyleft/cgal/CSGTree.h>

/**
 * @brief
 * Constructor usually performs only two simple tasks of filling the two lists
 *  - typeList: with all the possible id of the filtering actions
 *  - actionList with the corresponding actions.
 * If you want to add icons to your filtering actions you can do here by construction the QActions accordingly
 */
FilterMeshBooleans::FilterMeshBooleans()
{
	typeList = {
		MESH_INTERSECTION,
		MESH_UNION,
		MESH_DIFFERENCE,
		MESH_XOR};

	for(const ActionIDType& tt : typeList)
		actionList.push_back(new QAction(filterName(tt), this));
}

QString FilterMeshBooleans::pluginName() const
{
	return "FilterMeshBoolean";
}

QString FilterMeshBooleans::vendor() const
{
	return "CNR-ISTI-VCLab";
}

/**
 * @brief ST() must return the very short string describing each filtering action
 * (this string is used also to define the menu entry)
 * @param filterId: the id of the filter
 * @return the name of the filter
 */
QString FilterMeshBooleans::filterName(ActionIDType filterId) const
{
	switch(filterId) {
	case MESH_INTERSECTION :
		return "Mesh Boolean: Intersection";
	case MESH_UNION:
		return "Mesh Boolean: Union";
	case MESH_DIFFERENCE:
		return "Mesh Boolean: Difference";
	case MESH_XOR:
		return "Mesh Boolean: Symmetric Difference (XOR)";
	default :
		assert(0);
		return "";
	}
}


/**
 * @brief // Info() must return the longer string describing each filtering action
 * (this string is used in the About plugin dialog)
 * @param filterId: the id of the filter
 * @return an info string of the filter
 */
QString FilterMeshBooleans::filterInfo(ActionIDType filterId) const
{
	QString description =
		"This filter extecutes an exact boolean %1 between two meshes. <br>"
		"The filter uses the original code provided in the "
		"<a href=\"https://libigl.github.io/\">libigl library</a>.<br>"
		"The implementation refers to the following paper:<br>"
		"<i>Qingnan Zhou, Eitan Grinspun, Denis Zorin, Alec Jacobson</i>,<br>"
		"<b>\"Mesh Arrangements for Solid Geometry\"</b><br>";
	switch(filterId) {
	case MESH_INTERSECTION :
		return description.arg("intersection");
	case MESH_UNION:
		return description.arg("union");
	case MESH_DIFFERENCE:
		return description.arg("difference");
	case MESH_XOR:
		return description.arg("symmetric difference (XOR)");
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
FilterMeshBooleans::FilterClass FilterMeshBooleans::getClass(const QAction *a) const
{
	switch(ID(a)) {
	case MESH_INTERSECTION :
	case MESH_UNION:
	case MESH_DIFFERENCE:
	case MESH_XOR:
		return FilterPlugin::FilterClass(FilterPlugin::FilterClass(FilterPlugin::Layer + FilterPlugin::Remeshing));
	default :
		assert(0);
		return FilterPlugin::Generic;
	}
}

/**
 * @brief FilterSamplePlugin::filterArity
 * @return
 */
FilterPlugin::FilterArity FilterMeshBooleans::filterArity(const QAction*) const
{
	return FIXED;
}

/**
 * @brief FilterSamplePlugin::getPreConditions
 * @return
 */
//int FilterMeshBooleans::getPreConditions(const QAction*) const
//{
//	return MeshModel::MM_NONE;
//}

/**
 * @brief FilterSamplePlugin::postCondition
 * @return
 */
//int FilterMeshBooleans::postCondition(const QAction*) const
//{
//	return MeshModel::MM_VERTCOORD | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTNORMAL;
//}

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
RichParameterList FilterMeshBooleans::initParameterList(
		const QAction *action,
		const MeshDocument& md)
{
	RichParameterList parlst;
	switch(ID(action)) {
	case MESH_INTERSECTION :
	case MESH_UNION:
	case MESH_DIFFERENCE:
	case MESH_XOR:
	{
		const MeshModel *target = md.mm();
		//looking for a second mesh different that the current one
		for (const MeshModel * t : md.meshIterator()){
			if (t != md.mm()) {
				target = t;
				break;
			}
		}

		parlst.addParam(RichMesh(
							"first_mesh", md.mm()->id(), &md, "First Mesh",
							"The first operand of the boolean operation"));
		parlst.addParam(RichMesh(
							"second_mesh", target->id(), &md, "Second Mesh",
							"The second operand of the boolean operation"));

		parlst.addParam(RichBool(
							"transfer_face_color", false,
							"Transfer face color", "Save the color of the birth face to the faces of resulting mesh."));

		parlst.addParam(RichBool(
							"transfer_face_quality", false,
							"Transfer face quality", "Save the quality of the birth face to the faces of resulting mesh."));

		parlst.addParam(RichBool(
							"transfer_vert_color", false,
							"Transfer vertex color",
							"Save the color of the birth vertex to the faces of resulting mesh. For newly created vertices, "
							"a simple average of the neighbours is computed."));

		parlst.addParam(RichBool(
							"transfer_vert_quality", false,
							"Transfer vertex quality",
							"Save the quality of the birth vertex to the faces of resulting mesh.  For newly created vertices, "
							"a simple average of the neighbours is computed."));
	}
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
std::map<std::string, QVariant> FilterMeshBooleans::applyFilter(
		const QAction * action,
		const RichParameterList & par,
		MeshDocument &md,
		unsigned int& /*postConditionMask*/,
		vcg::CallBackPos *)
{
	bool transfFaceQuality = par.getBool("transfer_face_quality");
	bool transfFaceColor = par.getBool("transfer_face_color");
	bool transfVertQuality = par.getBool("transfer_vert_quality");
	bool transfVertColor = par.getBool("transfer_vert_color");


	switch(ID(action)) {
	case MESH_INTERSECTION :
		booleanOperation(
					md,
					*md.getMesh(par.getMeshId("first_mesh")),
					*md.getMesh(par.getMeshId("second_mesh")),
					igl::MESH_BOOLEAN_TYPE_INTERSECT,
					transfFaceQuality,
					transfFaceColor,
					transfVertQuality,
					transfVertColor);
		break;
	case MESH_UNION:
		booleanOperation(
					md,
					*md.getMesh(par.getMeshId("first_mesh")),
					*md.getMesh(par.getMeshId("second_mesh")),
					igl::MESH_BOOLEAN_TYPE_UNION,
					transfFaceQuality,
					transfFaceColor,
					transfVertQuality,
					transfVertColor);
		break;
	case MESH_DIFFERENCE:
		booleanOperation(
					md,
					*md.getMesh(par.getMeshId("first_mesh")),
					*md.getMesh(par.getMeshId("second_mesh")),
					igl::MESH_BOOLEAN_TYPE_MINUS,
					transfFaceQuality,
					transfFaceColor,
					transfVertQuality,
					transfVertColor);
		break;
	case MESH_XOR:
		booleanOperation(
					md,
					*md.getMesh(par.getMeshId("first_mesh")),
					*md.getMesh(par.getMeshId("second_mesh")),
					igl::MESH_BOOLEAN_TYPE_XOR,
					transfFaceQuality,
					transfFaceColor,
					transfVertQuality,
					transfVertColor);
		break;
	default :
		wrongActionCalled(action);
	}
	return std::map<std::string, QVariant>();
}

/**
 * @brief Puts coords and triangle indices of m into V and F matrices
 * @param m
 * @param V
 * @param F
 */
void FilterMeshBooleans::CMeshOToEigen(const CMeshO& m, Eigen::MatrixX3d& V, Eigen::MatrixX3i& F)
{
	vcg::tri::RequireVertexCompactness(m);
	vcg::tri::RequireFaceCompactness(m);
	V.resize(m.VN(), 3);
	for (int i = 0; i < m.VN(); i++){
		for (int j = 0; j < 3; j++){
			V(i,j) = m.vert[i].P()[j];
		}
	}
	F.resize(m.FN(), 3);
	for (int i = 0; i < m.FN(); i++){
		for (int j = 0; j < 3; j++){
			F(i,j) = (int) vcg::tri::Index(m,m.face[i].cV(j));
		}
	}
}

/**
 * @brief Returns a CMeshO containing the triangle mesh contained in V and F
 * @param V
 * @param F
 * @return
 */
CMeshO FilterMeshBooleans::EigenToCMeshO(const Eigen::MatrixX3d& V, const Eigen::MatrixX3i& F)
{
	CMeshO m;
	CMeshO::VertexIterator vi =
			vcg::tri::Allocator<CMeshO>::AddVertices(m, V.rows());
	std::vector<CMeshO::VertexPointer> ivp(V.rows());
	for (unsigned int i = 0; i < V.rows(); ++i, ++vi) {
		ivp[i] = &*vi;
		vi->P() = CMeshO::CoordType(V(i,0), V(i,1), V(i,2));
	}

	CMeshO::FaceIterator fi =
			vcg::tri::Allocator<CMeshO>::AddFaces(m, F.rows());
	for (unsigned int i = 0; i < F.rows(); ++i, ++fi) {
		for (unsigned int j = 0; j < 3; j++){
			if ((unsigned int)F(i,j) >= ivp.size()) {
				throw MLException(
						"Error while creating mesh: bad vertex index " +
						QString::number(F(i,j)) + " in face " +
						QString::number(i) + "; vertex " + QString::number(j) + ".");
			}
		}
		fi->V(0)=ivp[F(i,0)];
		fi->V(1)=ivp[F(i,1)];
		fi->V(2)=ivp[F(i,2)];
	}
	vcg::tri::UpdateNormal<CMeshO>::PerFace(m);
	vcg::tri::UpdateNormal<CMeshO>::PerVertex(m);

	return m;
}

/**
 * @brief Executes the boolean operation between m1 and m2, and puts the result
 * as a new mesh into md.
 * @param md: mesh document
 * @param m1: first mesh
 * @param m2: second mesh
 * @param op: type of boolean operation
 * @param transfQuality: if true, face quality will be transferred in the res mesh
 * @param transfColor: if true, face color will be transferred in the res mesh
 */
void FilterMeshBooleans::booleanOperation(
		MeshDocument& md,
		const MeshModel& m1,
		const MeshModel& m2,
		int op,
		bool transfFaceQuality,
		bool transfFaceColor,
		bool transfVertQuality,
		bool transfVertColor)
{
	QString name;
	switch (op) {
	case igl::MESH_BOOLEAN_TYPE_INTERSECT:
		name = "intersection";
		break;
	case igl::MESH_BOOLEAN_TYPE_MINUS:
		name = "difference";
		break;
	case igl::MESH_BOOLEAN_TYPE_XOR:
		name = "xor";
		break;
	case igl::MESH_BOOLEAN_TYPE_UNION:
		name = "union";
		break;
	default:
		throw MLException("Boolean Operation not found! Please report this issue on https://github.com/cnr-isti-vclab/meshlab/issues");
	}

	Eigen::MatrixX3d V1, V2, VR;
	Eigen::MatrixX3i F1, F2, FR;
	Eigen::VectorXi indices; //mapping indices for birth faces

	//vcg to eigen meshes
	CMeshOToEigen(m1.cm, V1, F1);
	CMeshOToEigen(m2.cm, V2, F2);

	bool result = igl::copyleft::cgal::mesh_boolean(V1, F1, V2, F2, (igl::MeshBooleanType)op, VR, FR, indices);

	if (!result){
		throw MLException(
			"Mesh inputs must induce a piecewise constant winding number field.<br>"
			"Make sure that both the input mesh are watertight (closed).");
	}
	else {
		//everything ok, create new mesh into md
		MeshModel* mesh = md.addNewMesh("", name);
		mesh->cm = EigenToCMeshO(VR, FR);

		//if transfer option enabled
		if (transfFaceColor || transfFaceQuality)
			transferFaceAttributes(*mesh, indices, m1, m2, transfFaceQuality, transfFaceColor);
		if (transfVertColor || transfVertQuality)
			transferVertexAttributes(*mesh, indices, m1, m2, transfVertQuality, transfVertColor);
	}
}

/**
 * @brief Allows to transfer face attributes from m1 and m2 to res, depending on the
 * birth faces indices.
 *
 * If one mesh does not have the required attribute, a default value will be
 * placed in the result:
 * - quality: 0
 * - color: 128,128,128
 *
 * @param res
 * @param indices
 * @param m1
 * @param m2
 * @param quality: if true, face quality will be transferred
 * @param color: if true, face color will be transferred
 */
void FilterMeshBooleans::transferFaceAttributes(
		MeshModel& res,
		const Eigen::VectorXi& faceIndices,
		const MeshModel& m1,
		const MeshModel& m2,
		bool quality,
		bool color)
{
	//checking if m1 and m2 have quality and color
	bool m1HasQuality = true, m1HasColor = true, m2HasQuality = true, m2HasColor = true;
	if (quality){
		res.updateDataMask(MeshModel::MM_FACEQUALITY);
		if (!m1.hasDataMask(MeshModel::MM_FACEQUALITY))
			m1HasQuality = false;
		if (!m2.hasDataMask(MeshModel::MM_FACEQUALITY))
			m2HasQuality = false;
	}
	if (color) {
		res.updateDataMask(MeshModel::MM_FACECOLOR);
		if (!m1.hasDataMask(MeshModel::MM_FACECOLOR))
			m1HasColor = false;
		if (!m2.hasDataMask(MeshModel::MM_FACECOLOR))
			m2HasColor = false;
	}

	//for each index in the birth faces vector
	for (unsigned int i = 0; i < faceIndices.size(); ++i){
		bool fromM1 = true;
		unsigned int mIndex = faceIndices[i];

		//if the index is >= FN of m1, it means that the index is of m2
		if (faceIndices[i] >= m1.cm.FN()){
			fromM1 = false;
			mIndex -= m1.cm.FN();
		}

		//if we need to transfer quality
		if (quality){
			Scalarm q = 0; //default quality value
			if (fromM1 && m1HasQuality)
				q = m1.cm.face[mIndex].Q();
			if (!fromM1 && m2HasQuality)
				q = m2.cm.face[mIndex].Q();
			res.cm.face[i].Q() = q;
		}

		//if we need to transfer color
		if (color) {
			vcg::Color4b c(128, 128, 128, 255); //default color value
			if (fromM1 && m1HasColor)
				c = m1.cm.face[mIndex].C();
			if (!fromM1 && m2HasColor)
				c = m2.cm.face[mIndex].C();
			res.cm.face[i].C() = c;
		}
	}
}

/**
 * @brief Allows to transfer vertex attributes from m1 and m2 to res, depending on the
 * birth faces indices.
 *
 * If one mesh does not have the required attribute, a default value will be
 * placed in the result:
 * - quality: 0
 * - color: 128,128,128
 *
 * @param res
 * @param indices
 * @param m1
 * @param m2
 * @param quality: if true, vertex quality will be transferred
 * @param color: if true, vertex color will be transferred
 */
void FilterMeshBooleans::transferVertexAttributes(
		MeshModel& res,
		const Eigen::VectorXi& faceIndices,
		const MeshModel& m1,
		const MeshModel& m2,
		bool quality,
		bool color)
{
	res.updateDataMask(MeshModel::MM_VERTFACETOPO);
	vcg::tri::UpdateTopology<CMeshO>::VertexFace(res.cm);

	//checking if m1 and m2 have quality and color
	bool m1HasQuality = true, m1HasColor = true, m2HasQuality = true, m2HasColor = true;
	if (quality){
		res.updateDataMask(MeshModel::MM_VERTQUALITY);
		if (!m1.hasDataMask(MeshModel::MM_VERTQUALITY))
			m1HasQuality = false;
		if (!m2.hasDataMask(MeshModel::MM_VERTQUALITY))
			m2HasQuality = false;
	}
	if (color) {
		res.updateDataMask(MeshModel::MM_VERTCOLOR);
		if (!m1.hasDataMask(MeshModel::MM_VERTCOLOR))
			m1HasColor = false;
		if (!m2.hasDataMask(MeshModel::MM_VERTCOLOR))
			m2HasColor = false;
	}

	//vertIndices construction
	Eigen::VectorXi vertIndices(res.cm.VN());
	vertIndices.setConstant(-1);

	for (unsigned int i = 0; i < faceIndices.size(); ++i){
		bool fromM1 = true;
		unsigned int mIndex = faceIndices[i];

		//if the index is >= FN of m1, it means that the index is of m2
		if (faceIndices[i] >= m1.cm.FN()){
			fromM1 = false;
			mIndex -= m1.cm.FN();
		}

		CMeshO::ConstFacePointer fBirth;
		CMeshO::FacePointer fRes = &(res.cm.face[i]);
		if (fromM1)
			fBirth = &(m1.cm.face[mIndex]);
		else
			fBirth = &(m2.cm.face[mIndex]);

		for (unsigned int j = 0; j < 3; ++j){
			CMeshO::VertexPointer vp = fRes->V(j);
			unsigned int vi = vcg::tri::Index(res.cm, vp);
			if (vertIndices[vi] == -1){
				//look if there is an equal vertex in fBirth
				for (unsigned k = 0; k < 3; ++k){
					if (fRes->V(j)->P() == fBirth->V(k)->P()){
						unsigned int birthVertIndex;
						if (fromM1)
							birthVertIndex = vcg::tri::Index(m1.cm, fBirth->V(k));
						else
							birthVertIndex = vcg::tri::Index(m2.cm, fBirth->V(k)) + m1.cm.VN();
						vertIndices[vi] = birthVertIndex;
					}
				}
			}
		}
	}

	//update birth vertices
	for (unsigned int i = 0; i < vertIndices.size(); ++i){
		bool fromM1 = false;
		bool fromM2 = false;

		int mIndex = vertIndices[i];
		if (vertIndices[i] >= m1.cm.VN()){
			fromM2 = true;
			mIndex -= m1.cm.VN();
		}
		else if (vertIndices[i] >= 0) {
			fromM1 = true;
		}

		//if we need to transfer quality
		if (quality){
			Scalarm q = 0; //default quality value
			if (fromM1 && m1HasQuality)
				q = m1.cm.vert[mIndex].Q();
			if (!fromM1 && m2HasQuality)
				q = m2.cm.vert[mIndex].Q();
			res.cm.vert[i].Q() = q;
		}

		//if we need to transfer color
		if (color) {
			vcg::Color4b c(128, 128, 128, 255); //default color value
			if (fromM1 && m1HasColor)
				c = m1.cm.vert[mIndex].C();
			if (fromM2 && m2HasColor)
				c = m2.cm.vert[mIndex].C();
			res.cm.vert[i].C() = c;
		}
	}

	//update newly created vertices
	for (unsigned int i = 0; i < vertIndices.size(); ++i){
		if (vertIndices[i] == -1){
			//base values
			unsigned int avgr=0, avgg=0, avgb=0, avga=0;
			Scalarm avgq=0;
			unsigned int nAdjs = 0;

			CMeshO::VertexPointer vp = &res.cm.vert[i];
			vcg::face::VFIterator<CMeshO::FaceType> fadjit(vp);
			//for each incident face fadj to vp
			for (; !fadjit.End(); ++fadjit){
				for (unsigned int j = 0; j < 3; j++){
					//get each vertex  to f
					CMeshO::VertexPointer vadj = fadjit.F()->V(j);
					unsigned int vi = vcg::tri::Index(res.cm, vadj);
					//if the vertex is not i and it is not newly created
					if (vi != i && vertIndices[vi] != -1) {
						nAdjs++;
						//if we need to transfer color
						if (color) {
							avgr += res.cm.vert[vi].C()[0];
							avgg += res.cm.vert[vi].C()[1];
							avgb += res.cm.vert[vi].C()[2];
							avga += res.cm.vert[vi].C()[3];
						}
						if (quality){
							avgq += res.cm.vert[vi].Q();
						}
					}
				}
			}
			if (nAdjs != 0){
				if (color) {
					res.cm.vert[i].C() = vcg::Color4b(avgr/nAdjs, avgg/nAdjs, avgb/nAdjs, avga/nAdjs);
				}
				if (quality){
					res.cm.vert[i].Q() = avgq / nAdjs;
				}
			}
		}
	}
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterMeshBooleans)
