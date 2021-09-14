/*****************************************************************************
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

#include "eigen_mesh_conversions.h"
#include "../mlexception.h"
#include <vcg/complex/algorithms/polygon_support.h>

namespace vcg {
class PEdge;
class PFace;
class PVertex;
struct PUsedTypes :
		public UsedTypes<Use<PVertex>::AsVertexType, Use<PEdge>::AsEdgeType, Use<PFace>::AsFaceType>
{
};

class PVertex :
		public Vertex<
			PUsedTypes,
			vertex::Coord3f,
			vertex::Normal3f,
			vertex::Qualityf,
			vertex::Color4b,
			vertex::BitFlags>
{
};
class PEdge : public Edge<PUsedTypes, edge::VertexRef, edge::BitFlags>
{
};
class PFace :
		public vcg::Face<
			PUsedTypes,
			face::PolyInfo, // this is necessary  if you use component in
							// vcg/simplex/face/component_polygon.h
			face::PFVAdj, // Pointer to the vertices (just like FVAdj )
			face::Qualityf,
			face::Color4b,
			face::BitFlags, // bit flags
			face::Normal3f, // normal
			face::WedgeTexCoord2f>
{
};

} // namespace vcg

class PolyMesh :
		public vcg::tri::
			TriMesh<std::vector<vcg::PVertex>, std::vector<vcg::PEdge>, std::vector<vcg::PFace>>
{
};

/**
 * @brief Creates a CMeshO mesh from the data contained in the given matrices.
 * The only matrix required to be non-empty is the 'vertices' matrix.
 * Other matrices may be empty.
 *
 * If normals and quality matrices are give, their sizes must be coherent with
 * the sizes of vertex and face matrices. If this requirement is not satisfied,
 * a MLException will be thrown.
 *
 * @param vertices: #V*3 matrix of scalars (vertex coordinates)
 * @param faces: #F*3 matrix of integers (vertex indices composing the faces)
 * @param vertexNormals: #V*3 matrix of scalars (vertex normals)
 * @param faceNormals: #F*3 matrix of scalars (face normals)
 * @param vertexQuality: #V vector of scalars (vertex quality)
 * @param faceQuality: #F vector of scalars (face quality)
 * @param vertexColor: #V*4 vector of scalars (RGBA vertex colors in interval [0-1])
 * @param faceColor: #F*4 vector of scalars (RGBA face colors in interval [0-1])
 * @return a CMeshO made of the given components
 */
CMeshO meshlab::meshFromMatrices(
	const EigenMatrixX3m&   vertices,
	const Eigen::MatrixX3i& faces,
	const EigenMatrixX3m&   vertexNormals,
	const EigenMatrixX3m&   faceNormals,
	const EigenVectorXm&    vertexQuality,
	const EigenVectorXm&    faceQuality,
	const EigenMatrixX4m&   vertexColor,
	const EigenMatrixX4m&   faceColor)
{
	CMeshO m;
	if (vertices.rows() > 0) {
		// add vertices and their associated normals and quality if any
		std::vector<CMeshO::VertexPointer> ivp(vertices.rows());

		bool hasVNormals = vertexNormals.rows() > 0;
		bool hasVQuality = vertexQuality.rows() > 0;
		bool hasVColors  = vertexColor.rows() > 0;
		if (hasVNormals && (vertices.rows() != vertexNormals.rows())) {
			throw MLException(
				"Error while creating mesh: the number of vertex normals "
				"is different from the number of vertices.");
		}
		if (hasVQuality && (vertices.rows() != vertexQuality.size())) {
			throw MLException(
				"Error while creating mesh: the number of vertex quality "
				"values is different from the number of vertices.");
		}
		if (hasVColors && (vertices.rows() != vertexColor.rows())) {
			throw MLException(
				"Error while creating mesh: the number of vertex colors "
				"is different from the number of vertices.");
		}
		CMeshO::VertexIterator vi = vcg::tri::Allocator<CMeshO>::AddVertices(m, vertices.rows());
		for (unsigned int i = 0; i < vertices.rows(); ++i, ++vi) {
			ivp[i]  = &*vi;
			vi->P() = CMeshO::CoordType(vertices(i, 0), vertices(i, 1), vertices(i, 2));
			if (hasVNormals) {
				vi->N() = CMeshO::CoordType(
					vertexNormals(i, 0), vertexNormals(i, 1), vertexNormals(i, 2));
			}
			if (hasVQuality) {
				vi->Q() = vertexQuality(i);
			}
			if (hasVColors) {
				vi->C() = CMeshO::VertexType::ColorType(
					vertexColor(i, 0) * 255,
					vertexColor(i, 1) * 255,
					vertexColor(i, 2) * 255,
					vertexColor(i, 3) * 255);
			}
		}

		// add faces and their associated normals and quality if any

		bool hasFNormals = faceNormals.rows() > 0;
		bool hasFQuality = faceQuality.rows() > 0;
		bool hasFColors  = faceColor.rows() > 0;
		if (hasFNormals && (faces.rows() != faceNormals.rows())) {
			throw MLException(
				"Error while creating mesh: the number of face normals "
				"is different from the number of faces.");
		}
		if (hasFQuality) {
			if (faces.rows() != faceQuality.size()) {
				throw MLException(
					"Error while creating mesh: the number of face quality "
					"values is different from the number of faces.");
			}
			m.face.EnableQuality();
		}
		if (hasFColors) {
			if (faces.rows() != faceColor.rows()) {
				throw MLException(
					"Error while creating mesh: the number of face colors "
					"is different from the number of faces.");
			}
			m.face.EnableColor();
		}
		CMeshO::FaceIterator fi = vcg::tri::Allocator<CMeshO>::AddFaces(m, faces.rows());
		for (unsigned int i = 0; i < faces.rows(); ++i, ++fi) {
			for (unsigned int j = 0; j < 3; j++) {
				if ((unsigned int) faces(i, j) >= ivp.size()) {
					throw MLException(
						"Error while creating mesh: bad vertex index " +
						QString::number(faces(i, j)) + " in face " + QString::number(i) +
						"; vertex " + QString::number(j) + ".");
				}
			}
			fi->V(0) = ivp[faces(i, 0)];
			fi->V(1) = ivp[faces(i, 1)];
			fi->V(2) = ivp[faces(i, 2)];

			if (hasFNormals) {
				fi->N() =
					CMeshO::CoordType(faceNormals(i, 0), faceNormals(i, 1), faceNormals(i, 2));
			}
			if (hasFQuality) {
				fi->Q() = faceQuality(i);
			}
			if (hasFColors) {
				fi->C() = CMeshO::FaceType::ColorType(
					faceColor(i, 0) * 255,
					faceColor(i, 1) * 255,
					faceColor(i, 2) * 255,
					faceColor(i, 3) * 255);
			}
		}
		if (!hasFNormals) {
			vcg::tri::UpdateNormal<CMeshO>::PerFace(m);
		}
		if (!hasVNormals) {
			vcg::tri::UpdateNormal<CMeshO>::PerVertex(m);
		}
	}
	else {
		throw MLException("Error while creating mesh: Vertex matrix is empty.");
	}

	return m;
}

/**
 * @brief Creates a CMeshO mesh from the data contained in the given matrices,
 * which may describe also a polygonal mesh.
 * At least vertices and faces parameters are required.
 *
 * If normals and quality matrices are give, their sizes must be coherent with
 * the sizes of vertex and face matrix/list. If this requirement is not satisfied,
 * a MLException will be thrown.
 *
 * This function stores in the returned mesh a custom per face scalar attribute
 * called 'poly_birth_faces', which stores for each triangle of the mesh, the id
 * of its polygonal birth face of the polygon mesh given in input.
 *
 * @param vertices: #V*3 matrix of scalars (vertex coordinates)
 * @param faces: #F list of vector of integers (vertex indices composing the faces)
 * @param vertexNormals: #V*3 matrix of scalars (vertex normals)
 * @param faceNormals: #F*3 matrix of scalars (face normals)
 * @param vertexQuality: #V vector of scalars (vertex quality)
 * @param faceQuality: #F vector of scalars (face quality)
 * @param vertexColor: #V*4 vector of scalars (RGBA vertex colors in interval [0-1])
 * @param faceColor: #F*4 vector of scalars (RGBA face colors in interval [0-1])
 * @return a CMeshO made of the given components
 */
CMeshO meshlab::polyMeshFromMatrices(
	const EigenMatrixX3m&            vertices,
	const std::list<EigenVectorXui>& faces,
	const EigenMatrixX3m&            vertexNormals,
	const EigenMatrixX3m&            faceNormals,
	const EigenVectorXm&             vertexQuality,
	const EigenVectorXm&             faceQuality,
	const EigenMatrixX4m&            vertexColor,
	const EigenMatrixX4m&            faceColor)
{
	PolyMesh pm;
	CMeshO   m;
	if (vertices.rows() > 0) {
		// add vertices and their associated normals and quality if any
		std::vector<PolyMesh::VertexPointer> ivp(vertices.rows());

		bool hasVNormals = vertexNormals.rows() > 0;
		bool hasVQuality = vertexQuality.rows() > 0;
		bool hasVColors  = vertexColor.rows() > 0;
		if (hasVNormals && (vertices.rows() != vertexNormals.rows())) {
			throw MLException(
				"Error while creating mesh: the number of vertex normals "
				"is different from the number of vertices.");
		}
		if (hasVQuality && (vertices.rows() != vertexQuality.size())) {
			throw MLException(
				"Error while creating mesh: the number of vertex quality "
				"values is different from the number of vertices.");
		}
		if (hasVColors && (vertices.rows() != vertexColor.rows())) {
			throw MLException(
				"Error while creating mesh: the number of vertex colors "
				"is different from the number of vertices.");
		}
		PolyMesh::VertexIterator vi =
			vcg::tri::Allocator<PolyMesh>::AddVertices(pm, vertices.rows());
		for (unsigned int i = 0; i < vertices.rows(); ++i, ++vi) {
			ivp[i]  = &*vi;
			vi->P() = PolyMesh::CoordType(vertices(i, 0), vertices(i, 1), vertices(i, 2));
			if (hasVNormals) {
				vi->N() = PolyMesh::CoordType(
					vertexNormals(i, 0), vertexNormals(i, 1), vertexNormals(i, 2));
			}
			if (hasVQuality) {
				vi->Q() = vertexQuality(i);
			}
			if (hasVColors) {
				vi->C() = CMeshO::VertexType::ColorType(
					vertexColor(i, 0) * 255,
					vertexColor(i, 1) * 255,
					vertexColor(i, 2) * 255,
					vertexColor(i, 3) * 255);
			}
		}

		// add faces and their associated normals and quality if any

		bool hasFNormals = faceNormals.rows() > 0;
		bool hasFQuality = faceQuality.rows() > 0;
		bool hasFColors  = faceColor.rows() > 0;
		if (hasFNormals && (faces.size() != (size_t) faceNormals.rows())) {
			throw MLException(
				"Error while creating mesh: the number of face normals "
				"is different from the number of faces.");
		}
		if (hasFQuality) {
			if (faces.size() != (size_t) faceQuality.size()) {
				throw MLException(
					"Error while creating mesh: the number of face quality "
					"values is different from the number of faces.");
			}
			m.face.EnableQuality();
		}
		if (hasFColors) {
			if (faces.size() != (size_t) faceColor.rows()) {
				throw MLException(
					"Error while creating mesh: the number of face colors "
					"is different from the number of faces.");
			}
			m.face.EnableColor();
		}
		PolyMesh::FaceIterator fi = vcg::tri::Allocator<PolyMesh>::AddFaces(pm, faces.size());
		auto                   it = faces.begin();
		for (unsigned int i = 0; i < faces.size(); ++i, ++fi, ++it) {
			fi->Alloc(it->size());
			const EigenVectorXui& iface = *it;
			for (unsigned int j = 0; j < iface.size(); ++j) {
				if ((unsigned int) iface(j) >= ivp.size()) {
					throw MLException(
						"Error while creating mesh: bad vertex index " + QString::number(iface(j)) +
						" in face " + QString::number(i) + "; vertex " + QString::number(j) + ".");
				}
				fi->V(j) = ivp[iface(j)];
			}
			if (hasFNormals) {
				fi->N() =
					PolyMesh::CoordType(faceNormals(i, 0), faceNormals(i, 1), faceNormals(i, 2));
			}
			if (hasFQuality) {
				fi->Q() = faceQuality(i);
			}
			if (hasFColors) {
				fi->C() = CMeshO::FaceType::ColorType(
					faceColor(i, 0) * 255,
					faceColor(i, 1) * 255,
					faceColor(i, 2) * 255,
					faceColor(i, 3) * 255);
			}
		}

		std::vector<unsigned int> birthFaces;
		vcg::tri::PolygonSupport<CMeshO, PolyMesh>::ImportFromPolyMesh(m, pm, birthFaces);

		auto h = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<Scalarm>(m, "poly_birth_faces");
		for (unsigned int i = 0; i < m.face.size(); ++i)
			h[i] = birthFaces[i];

		if (!hasFNormals) {
			vcg::tri::UpdateNormal<CMeshO>::PerFace(m);
		}
		if (!hasVNormals) {
			vcg::tri::UpdateNormal<CMeshO>::PerVertex(m);
		}
	}
	else {
		throw MLException("Error while creating mesh: Vertex matrix is empty.");
	}

	return m;
}

/**
 * @brief Adds a new custom vertex attribute of scalars to the given mesh.
 *
 * The vector of values must have the same size of the number of vertices of the
 * mesh, and if an attribute with the give name already exists, a MLException
 * will be raised.
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: the input mesh
 * @param attributeValues: #V vector of values
 * @param attributeName: the name of the new attribute
 */
void meshlab::addVertexScalarAttribute(
	CMeshO&              mesh,
	const EigenVectorXm& attributeValues,
	const std::string&   attributeName)
{
	if (mesh.VN() != attributeValues.size())
		throw MLException(
			"The given vector has different number of elements than the number of vertices of the "
			"mesh.");
	auto h = vcg::tri::Allocator<CMeshO>::FindPerVertexAttribute<Scalarm>(mesh, attributeName);
	if (vcg::tri::Allocator<CMeshO>::IsValidHandle(mesh, h)) {
		throw MLException(
			"The mesh already has a custom attribute with the name " +
			QString::fromStdString(attributeName));
	}
	h = vcg::tri::Allocator<CMeshO>::AddPerVertexAttribute<Scalarm>(mesh, attributeName);
	for (unsigned int i = 0; i < attributeValues.size(); ++i) {
		h[i] = attributeValues(i);
	}
}

/**
 * @brief Adds a new custom face attribute of scalars to the given mesh.
 *
 * The vector of values must have the same size of the number of faces of the
 * mesh, and if an attribute with the give name already exists, a MLException
 * will be raised.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: the input mesh
 * @param attributeValues: #F vector of values
 * @param attributeName: the name of the new attribute
 */
void meshlab::addFaceScalarAttribute(
	CMeshO&              mesh,
	const EigenVectorXm& attributeValues,
	const std::string&   attributeName)
{
	if (mesh.FN() != attributeValues.size())
		throw MLException(
			"The given vector has different number of elements than the number of faces of the "
			"mesh.");
	auto h = vcg::tri::Allocator<CMeshO>::FindPerFaceAttribute<Scalarm>(mesh, attributeName);
	if (vcg::tri::Allocator<CMeshO>::IsValidHandle(mesh, h)) {
		throw MLException(
			"The mesh already has a custom attribute with the name " +
			QString::fromStdString(attributeName));
	}
	h = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<Scalarm>(mesh, attributeName);
	for (unsigned int i = 0; i < attributeValues.size(); ++i) {
		h[i] = attributeValues(i);
	}
}

/**
 * @brief Adds a new custom vertex attribute of vectors to the given mesh.
 *
 * The matrix of values must have the same number of rows of the number of vertices of the
 * mesh (and 3 columns), and if an attribute with the give name already exists, a MLException
 * will be raised.
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: the input mesh
 * @param attributeValues: #V*3 matrix of values
 * @param attributeName: the name of the new attribute
 */
void meshlab::addVertexVectorAttribute(
	CMeshO&               mesh,
	const EigenMatrixX3m& attributeValues,
	const std::string&    attributeName)
{
	if (mesh.VN() != attributeValues.rows())
		throw MLException(
			"The given vector has different number of rows than the number of vertices of the "
			"mesh.");
	auto h = vcg::tri::Allocator<CMeshO>::FindPerVertexAttribute<Point3m>(mesh, attributeName);
	if (vcg::tri::Allocator<CMeshO>::IsValidHandle(mesh, h)) {
		throw MLException(
			"The mesh already has a custom attribute with the name " +
			QString::fromStdString(attributeName));
	}
	h = vcg::tri::Allocator<CMeshO>::AddPerVertexAttribute<Point3m>(mesh, attributeName);
	for (unsigned int i = 0; i < attributeValues.size(); ++i) {
		h[i][0] = attributeValues(i, 0);
		h[i][1] = attributeValues(i, 1);
		h[i][2] = attributeValues(i, 2);
	}
}

/**
 * @brief Adds a new custom faces attribute of vectors to the given mesh.
 *
 * The matrix of values must have the same number of rows of the number of faces of the
 * mesh (and 3 columns), and if an attribute with the give name already exists, a MLException
 * will be raised.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: the input mesh
 * @param attributeValues: #F*3 matrix of values
 * @param attributeName: the name of the new attribute
 */
void meshlab::addFaceVectorAttribute(
	CMeshO&               mesh,
	const EigenMatrixX3m& attributeValues,
	const std::string&    attributeName)
{
	if (mesh.FN() != attributeValues.rows())
		throw MLException(
			"The given vector has different number of rows than the number of faces of the mesh.");
	auto h = vcg::tri::Allocator<CMeshO>::FindPerFaceAttribute<Point3m>(mesh, attributeName);
	if (vcg::tri::Allocator<CMeshO>::IsValidHandle(mesh, h)) {
		throw MLException(
			"The mesh already has a custom attribute with the name " +
			QString::fromStdString(attributeName));
	}
	h = vcg::tri::Allocator<CMeshO>::AddPerFaceAttribute<Point3m>(mesh, attributeName);
	for (unsigned int i = 0; i < attributeValues.size(); ++i) {
		h[i][0] = attributeValues(i, 0);
		h[i][1] = attributeValues(i, 1);
		h[i][2] = attributeValues(i, 2);
	}
}

/**
 * @brief Get a #V*3 Eigen matrix of scalars containing the coordinates of the
 * vertices of a CMeshO.
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V*3 matrix of scalars (vertex coordinates)
 */
EigenMatrixX3m meshlab::vertexMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);

	// create eigen matrix of vertices
	EigenMatrixX3m vert(mesh.VN(), 3);

	// copy vertices
	for (int i = 0; i < mesh.VN(); i++) {
		for (int j = 0; j < 3; j++) {
			vert(i, j) = mesh.vert[i].P()[j];
		}
	}

	return vert;
}

/**
 * @brief Get a #F*3 Eigen matrix of integers containing the vertex indices of
 * a CMeshO.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F*3 matrix of integers (vertex indices composing the faces)
 */
Eigen::MatrixX3i meshlab::faceMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);

	// create eigen matrix of faces
	Eigen::MatrixXi faces(mesh.FN(), 3);

	// copy faces
	for (int i = 0; i < mesh.FN(); i++) {
		for (int j = 0; j < 3; j++) {
			faces(i, j) = (int) vcg::tri::Index(mesh, mesh.face[i].V(j));
		}
	}

	return faces;
}

/**
 * @brief Get a #V*3 Eigen matrix of scalars containing the values of the
 * vertex normals of a CMeshO.
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V*3 matrix of scalars (vertex normals)
 */
EigenMatrixX3m meshlab::vertexNormalMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);

	// create eigen matrix of vertex normals
	EigenMatrixX3m vertexNormals(mesh.VN(), 3);

	// per vertices normals
	for (int i = 0; i < mesh.VN(); i++) {
		for (int j = 0; j < 3; j++) {
			vertexNormals(i, j) = mesh.vert[i].N()[j];
		}
	}

	return vertexNormals;
}

/**
 * @brief Get a #F*3 Eigen matrix of scalars containing the values of the
 * face normals of a CMeshO.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F*3 matrix of scalars (face normals)
 */
EigenMatrixX3m meshlab::faceNormalMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);

	// create eigen matrix of face normals
	EigenMatrixX3m faceNormals(mesh.FN(), 3);

	// per face normals
	for (int i = 0; i < mesh.FN(); i++) {
		for (int j = 0; j < 3; j++) {
			faceNormals(i, j) = mesh.face[i].N()[j];
		}
	}

	return faceNormals;
}

/**
 * @brief Get a #V*4 Eigen matrix of scalars containing the values of the
 * vertex colors of a CMeshO, each value in an interval [0, 1].
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V*4 matrix of scalars (vertex colors)
 */
EigenMatrixX4m meshlab::vertexColorMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);
	EigenMatrixX4m vertexColors(mesh.VN(), 4);

	for (int i = 0; i < mesh.VN(); i++) {
		for (int j = 0; j < 4; j++) {
			vertexColors(i, j) = mesh.vert[i].C()[j] / 255.0;
		}
	}

	return vertexColors;
}

/**
 * @brief Get a #F*4 Eigen matrix of scalars containing the values of the
 * face colors of a CMeshO, each value in an interval [0, 1].
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F*4 matrix of scalars (face colors)
 */
EigenMatrixX4m meshlab::faceColorMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequirePerFaceColor(mesh);

	EigenMatrixX4m faceColors(mesh.FN(), 4);

	for (int i = 0; i < mesh.FN(); i++) {
		for (int j = 0; j < 4; j++) {
			faceColors(i, j) = mesh.face[i].C()[j] / 255.0;
		}
	}

	return faceColors;
}

/**
 * @brief Get a #V Eigen vector of unsigned integers containing the values of the
 * vertex colors of a CMeshO. Each value is a packed ARGB color in a 32 bit
 * unsigned int (8 bits per component).
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V vector of unsigned integers (vertex colors)
 */
EigenVectorXui meshlab::vertexColorArray(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);
	EigenVectorXui vertexColors(mesh.VN());

	for (int i = 0; i < mesh.VN(); i++) {
		vertexColors(i) = vcg::Color4<unsigned char>::ToUnsignedA8R8G8B8(mesh.vert[i].C());
	}

	return vertexColors;
}

/**
 * @brief Get a #F Eigen vector of unsigned integers containing the values of the
 * face colors of a CMeshO. Each value is a packed ARGB color in a 32 bit
 * unsigned int (8 bits per component).
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F vector of unsigned integers (face colors)
 */
EigenVectorXui meshlab::faceColorArray(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequirePerFaceColor(mesh);

	EigenVectorXui faceColors(mesh.FN());

	for (int i = 0; i < mesh.FN(); i++) {
		faceColors(i) = vcg::Color4<unsigned char>::ToUnsignedA8R8G8B8(mesh.face[i].C());
	}

	return faceColors;
}

/**
 * @brief Get a #V Eigen vector of scalars containing the values of the
 * vertex quality of a CMeshO.
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V matrix of scalars (vertex quality)
 */
EigenVectorXm meshlab::vertexQualityArray(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);
	vcg::tri::RequirePerVertexQuality(mesh);

	EigenVectorXm qv(mesh.VN());
	for (int i = 0; i < mesh.VN(); i++) {
		qv(i) = mesh.vert[i].Q();
	}
	return qv;
}

/**
 * @brief Get a #F Eigen vector of scalars containing the values of the
 * face quality of a CMeshO.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F matrix of scalars (face quality)
 */
EigenVectorXm meshlab::faceQualityArray(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequirePerFaceQuality(mesh);

	EigenVectorXm qf(mesh.FN());
	for (int i = 0; i < mesh.FN(); i++) {
		qf(i) = mesh.face[i].Q();
	}
	return qf;
}

/**
 * @brief Get a #V*2 Eigen matrix of scalars containing the values of the
 * vertex texture coordinates of a CMeshO.
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V*2 matrix of scalars (vertex texture coordinates)
 */
EigenMatrixX2m meshlab::vertexTexCoordMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);
	vcg::tri::RequirePerVertexTexCoord(mesh);

	EigenMatrixX2m uv(mesh.VN(), 2);

	// per vertices uv
	for (int i = 0; i < mesh.VN(); i++) {
		uv(i, 0) = mesh.vert[i].T().U();
		uv(i, 1) = mesh.vert[i].T().V();
	}

	return uv;
}

/**
 * @brief Get a (#F*3)*2 Eigen matrix of scalars containing the values of the
 * wedge texture coordinates of a CMeshO. The matrix is organized as consecutive
 * #F triplets of uv coordinates.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #(#F*3)*2 matrix of scalars (wedge texture coordinates)
 */
EigenMatrixX2m meshlab::wedgeTexCoordMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequirePerFaceWedgeTexCoord(mesh);
	EigenMatrixX2m m(mesh.FN() * 3, 2);

	for (int i = 0; i < mesh.FN(); i++) {
		int base = i * 3;
		for (int j = 0; j < 3; j++) {
			m(base + j, 0) = mesh.face[i].WT(j).u();
			m(base + j, 1) = mesh.face[i].WT(j).v();
		}
	}
	return m;
}

/**
 * @brief Get a #V Eigen vector of booleans which are true if the corresponding
 * vertex is selected, false otherwise
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V vector of booleans
 */
EigenVectorXb meshlab::vertexSelectionArray(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);

	EigenVectorXb sel(mesh.VN());

	// per vertex selection
	for (int i = 0; i < mesh.VN(); i++) {
		sel(i) = mesh.vert[i].IsS();
	}

	return sel;
}

/**
 * @brief Get a #F Eigen vector of booleans which are true if the corresponding
 * face is selected, false otherwise
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F vector of booleans
 */
EigenVectorXb meshlab::faceSelectionArray(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);

	EigenVectorXb sel(mesh.FN());

	// per face selection
	for (int i = 0; i < mesh.FN(); i++) {
		sel(i) = mesh.face[i].IsS();
	}

	return sel;
}

/**
 * @brief Get a #F*3 Eigen matrix of integers containing the indices of the
 * adjacent faces for each face in a CMeshO.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F*3 matrix of integers (face-face adjacency)
 */
Eigen::MatrixX3i meshlab::faceFaceAdjacencyMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequireFFAdjacency(mesh);

	Eigen::MatrixX3i faceFaceMatrix(mesh.FN(), 3);

	for (int i = 0; i < mesh.FN(); i++) {
		for (int j = 0; j < 3; j++) {
			auto AdjF = mesh.face[i].FFp(j);
			if (AdjF == &mesh.face[i]) {
				faceFaceMatrix(i, j) = -1;
			}
			else {
				faceFaceMatrix(i, j) = mesh.face[i].FFi(j);
			}
		}
	}

	return faceFaceMatrix;
}

/**
 * @brief Get a #V Eigen vector of scalars containing the values of the
 * custom per-vertex attribute having the given name.
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V vector of scalars (custom scalar vertex attribute)
 */
EigenVectorXm
meshlab::vertexScalarAttributeArray(const CMeshO& mesh, const std::string& attributeName)
{
	vcg::tri::RequireVertexCompactness(mesh);
	CMeshO::ConstPerVertexAttributeHandle<Scalarm> attributeHandle =
		vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<Scalarm>(mesh, attributeName);
	if (vcg::tri::Allocator<CMeshO>::IsValidHandle(mesh, attributeHandle)) {
		EigenVectorXm attrVector(mesh.VN());
		for (unsigned int i = 0; i < (unsigned int) mesh.VN(); ++i) {
			attrVector[i] = attributeHandle[i];
		}
		return attrVector;
	}
	else {
		throw MLException(
			"No valid per vertex scalar attribute named " + QString::fromStdString(attributeName) +
			" was found.");
	}
}

/**
 * @brief Get a #V*3 Eigen matrix of scalars containing the values of the
 * custom per-vertex attribute having the given name.
 * The vertices in the mesh must be compact (no deleted vertices).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #V*3 matrix of scalars (custom Point3 vertex attribute)
 */
EigenMatrixX3m
meshlab::vertexVectorAttributeMatrix(const CMeshO& mesh, const std::string& attributeName)
{
	vcg::tri::RequireVertexCompactness(mesh);
	CMeshO::ConstPerVertexAttributeHandle<Point3m> attributeHandle =
		vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<Point3m>(mesh, attributeName);
	if (vcg::tri::Allocator<CMeshO>::IsValidHandle(mesh, attributeHandle)) {
		EigenMatrixX3m attrMatrix(mesh.VN(), 3);
		for (unsigned int i = 0; i < (unsigned int) mesh.VN(); ++i) {
			attrMatrix(i, 0) = attributeHandle[i][0];
			attrMatrix(i, 1) = attributeHandle[i][1];
			attrMatrix(i, 2) = attributeHandle[i][2];
		}
		return attrMatrix;
	}
	else {
		throw MLException(
			"No valid per vertex vector attribute named " + QString::fromStdString(attributeName) +
			" was found.");
	}
}

/**
 * @brief Get a #F Eigen vector of scalars containing the values of the
 * custom per-face attribute having the given name.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F vector of scalars (custom scalar face attribute)
 */
EigenVectorXm
meshlab::faceScalarAttributeArray(const CMeshO& mesh, const std::string& attributeName)
{
	vcg::tri::RequireFaceCompactness(mesh);
	CMeshO::ConstPerFaceAttributeHandle<Scalarm> attributeHandle =
		vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<Scalarm>(mesh, attributeName);
	if (vcg::tri::Allocator<CMeshO>::IsValidHandle(mesh, attributeHandle)) {
		EigenVectorXm attrMatrix(mesh.FN());
		for (unsigned int i = 0; i < (unsigned int) mesh.FN(); ++i) {
			attrMatrix[i] = attributeHandle[i];
		}
		return attrMatrix;
	}
	else {
		throw MLException(
			"No valid per face scalar attribute named " + QString::fromStdString(attributeName) +
			" was found.");
	}
}

/**
 * @brief Get a #F*3 Eigen matrix of scalars containing the values of the
 * custom per-face attribute having the given name.
 * The faces in the mesh must be compact (no deleted faces).
 * If the mesh is not compact, a vcg::MissingCompactnessException will be thrown.
 *
 * @param mesh: input mesh
 * @return #F*3 matrix of scalars (custom Point3 face attribute)
 */
EigenMatrixX3m
meshlab::faceVectorAttributeMatrix(const CMeshO& mesh, const std::string& attributeName)
{
	vcg::tri::RequireFaceCompactness(mesh);
	CMeshO::ConstPerFaceAttributeHandle<Point3m> attributeHandle =
		vcg::tri::Allocator<CMeshO>::GetPerFaceAttribute<Point3m>(mesh, attributeName);
	if (vcg::tri::Allocator<CMeshO>::IsValidHandle(mesh, attributeHandle)) {
		EigenMatrixX3m attrMatrix(mesh.FN(), 3);
		for (unsigned int i = 0; i < (unsigned int) mesh.FN(); ++i) {
			attrMatrix(i, 0) = attributeHandle[i][0];
			attrMatrix(i, 1) = attributeHandle[i][1];
			attrMatrix(i, 2) = attributeHandle[i][2];
		}
		return attrMatrix;
	}
	else {
		throw MLException(
			"No valid per face vector attribute named " + QString::fromStdString(attributeName) +
			" was found.");
	}
}
