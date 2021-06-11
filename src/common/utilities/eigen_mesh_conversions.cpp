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

#include "eigen_mesh_conversions.h"
#include "../mlexception.h"

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
 * @return a CMeshO made of the given components
 */
CMeshO meshlab::meshFromMatrices(
		const EigenMatrixX3m& vertices,
		const Eigen::MatrixX3i& faces,
		const EigenMatrixX3m& vertexNormals,
		const EigenMatrixX3m& faceNormals,
		const EigenVectorXm& vertexQuality,
		const EigenVectorXm& faceQuality)
{
	CMeshO m;
	if (vertices.rows() > 0) {
		//add vertices and their associated normals and quality if any
		std::vector<CMeshO::VertexPointer> ivp(vertices.rows());

		bool hasVNormals = vertexNormals.rows() > 0;
		bool hasVQuality = vertexQuality.rows() > 0;
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
		CMeshO::VertexIterator vi =
				vcg::tri::Allocator<CMeshO>::AddVertices(m, vertices.rows());
		for (unsigned int i = 0; i < vertices.rows(); ++i, ++vi) {
			ivp[i] = &*vi;
			vi->P() = CMeshO::CoordType(vertices(i,0), vertices(i,1), vertices(i,2));
			if (hasVNormals) {
				vi->N() = CMeshO::CoordType(
							vertexNormals(i,0),
							vertexNormals(i,1),
							vertexNormals(i,2));
			}
			if (hasVQuality) {
				vi->Q() = vertexQuality(i);
			}
		}

		//add faces and their associated normals and quality if any

		bool hasFNormals = faceNormals.rows() > 0;
		bool hasFQuality = faceQuality.rows() > 0;
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
		CMeshO::FaceIterator fi =
				vcg::tri::Allocator<CMeshO>::AddFaces(m, faces.rows());
		for (unsigned int i = 0; i < faces.rows(); ++i, ++fi) {
			for (unsigned int j = 0; j < 3; j++){
				if ((unsigned int)faces(i,j) >= ivp.size()) {
					throw MLException(
							"Error while creating mesh: bad vertex index " +
							QString::number(faces(i,j)) + " in face " +
							QString::number(i) + "; vertex " + QString::number(j) + ".");
				}
			}
			fi->V(0)=ivp[faces(i,0)];
			fi->V(1)=ivp[faces(i,1)];
			fi->V(2)=ivp[faces(i,2)];

			if (hasFNormals){
				fi->N() = CMeshO::CoordType(
							faceNormals(i,0),
							faceNormals(i,1),
							faceNormals(i,2));
			}
			if (hasFQuality) {
				fi->Q() = faceQuality(i);
			}
		}
		if (!hasFNormals){
			vcg::tri::UpdateNormal<CMeshO>::PerFace(m);
		}
		if (!hasVNormals){
			vcg::tri::UpdateNormal<CMeshO>::PerVertex(m);
		}
	}
	else {
		throw MLException("Error while creating mesh: Vertex matrix is empty.");
	}

	return m;
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
	for (int i = 0; i < mesh.VN(); i++){
		for (int j = 0; j < 3; j++){
			vert(i,j) = mesh.vert[i].cP()[j];
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
	for (int i = 0; i < mesh.FN(); i++){
		for (int j = 0; j < 3; j++){
			faces(i,j) = (int)vcg::tri::Index(mesh,mesh.face[i].cV(j));
		}
	}

	return faces;
}

EigenMatrixX3m meshlab::vertexNormalMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);

	// create eigen matrix of vertex normals
	EigenMatrixX3m vertexNormals(mesh.VN(), 3);

	// per vertices normals
	for (int i = 0; i < mesh.VN(); i++){
		for (int j = 0; j < 3; j++){
			vertexNormals(i,j) = mesh.vert[i].cN()[j];
		}
	}

	return vertexNormals;
}

EigenMatrixX3m meshlab::faceNormalMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);

	// create eigen matrix of face normals
	EigenMatrixX3m faceNormals(mesh.FN(), 3);

	// per face normals
	for (int i = 0; i < mesh.FN(); i++){
		for (int j = 0; j < 3; j++){
			faceNormals(i,j) = mesh.face[i].cN()[j];
		}
	}

	return faceNormals;
}

EigenMatrixX3m meshlab::vertexColorMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);
	EigenMatrixX3m vertexColors(mesh.VN(), 4);

	for (int i = 0; i < mesh.VN(); i++){
		for (int j = 0; j < 4; j++){
			vertexColors(i,j) = mesh.vert[i].C()[j] / 255.0;
		}
	}

	return vertexColors;
}

EigenMatrixX3m meshlab::faceColorMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequirePerFaceColor(mesh);

	EigenMatrixX3m faceColors(mesh.FN(), 4);

	for (int i = 0; i < mesh.FN(); i++){
		for (int j = 0; j < 4; j++){
			faceColors(i,j) = mesh.face[i].C()[j] / 255.0;
		}
	}

	return faceColors;
}

Eigen::Matrix<unsigned int, Eigen::Dynamic, 1> meshlab::vertexColorArray(
		const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);
	Eigen::Matrix<unsigned int, Eigen::Dynamic, 1> vertexColors(mesh.VN());

	for (int i = 0; i < mesh.VN(); i++){
		vertexColors(i) =
			vcg::Color4<unsigned char>::ToUnsignedA8R8G8B8(mesh.vert[i].C());
	}

	return vertexColors;
}

Eigen::Matrix<unsigned int, Eigen::Dynamic, 1> meshlab::faceColorArray(
		const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequirePerFaceColor(mesh);

	Eigen::Matrix<unsigned int, Eigen::Dynamic, 1> faceColors(mesh.FN());

	for (int i = 0; i < mesh.FN(); i++){
		faceColors(i) =
			vcg::Color4<unsigned char>::ToUnsignedA8R8G8B8(mesh.face[i].C());
	}

	return faceColors;
}

EigenVectorXm meshlab::vertexQualityArray(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);
	vcg::tri::RequirePerVertexQuality(mesh);

	EigenVectorXm qv(mesh.VN());
	for (int i = 0; i < mesh.VN(); i++){
		qv(i) = mesh.vert[i].cQ();
	}
	return qv;
}

EigenVectorXm meshlab::faceQualityArray(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequirePerFaceQuality(mesh);

	EigenVectorXm qf(mesh.FN());
	for (int i = 0; i < mesh.FN(); i++){
		qf(i) = mesh.face[i].cQ();
	}
	return qf;
}

EigenMatrixX2m meshlab::vertexTexCoordMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireVertexCompactness(mesh);
	vcg::tri::RequirePerVertexTexCoord(mesh);

	EigenMatrixX2m uv (mesh.VN(), 2);

	// per vertices uv
	for (int i = 0; i < mesh.VN(); i++) {
		uv(i,0) = mesh.vert[i].cT().U();
		uv(i,1) = mesh.vert[i].cT().V();
	}

	return uv;
}

EigenMatrixX2m meshlab::wedgeTexCoordMatrix(const CMeshO& mesh)
{
	vcg::tri::RequireFaceCompactness(mesh);
	vcg::tri::RequirePerVertexTexCoord(mesh);
	EigenMatrixX2m m(mesh.FN()*3, 2);

	for (int i = 0; i < mesh.FN(); i++) {
		int base = i * 3;
		for (int j = 0; j < 3; j++){
			m(base+j, 0) = mesh.face[i].WT(j).u();
			m(base+j, 1) = mesh.face[i].WT(j).v();
		}
	}
	return m;
}
