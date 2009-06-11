/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef MESHCUTCURVATURE_H
#define MESHCUTCURVATURE_H

#include <vcg/container/simple_temporary_data.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/face/jumping_pos.h>

#include <vcg/complex/trimesh/base.h>
#include <vcg/simplex/vertex/base.h>
#include <vcg/simplex/face/base.h>
#include <vcg/space/point3.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/math/matrix.h>
#include <vcg/math/matrix33.h>

#include <float.h>

namespace vcg {

	class CurvData {
	public:
		Point3<float> T1;
		Point3<float> T2;
		float k1;
		float k2;
	};

	template <class MESH_TYPE> class CurvatureTensor {
		typedef typename MESH_TYPE::FaceIterator FaceIterator;
		typedef typename MESH_TYPE::VertexIterator VertexIterator;
		typedef typename MESH_TYPE::VertContainer VertContainer;
		typedef typename MESH_TYPE::VertexType VertexType;
		typedef typename MESH_TYPE::FaceType FaceType;
		typedef typename MESH_TYPE::CoordType CoordType;		
		typedef typename CoordType::ScalarType ScalarType;

	private:

		MESH_TYPE * mesh;
		SimpleTempData<VertContainer, CurvData> *TDCurvPtr;

	public:
		CurvatureTensor(MESH_TYPE * m, SimpleTempData<VertContainer, CurvData> *TDCurvP) {
			mesh = m;
			TDCurvPtr = TDCurvP;
		}

		~CurvatureTensor() {

		}

		typedef struct AdjVertex {
			VertexType * vert;
			float doubleArea;
			bool isBorder;
		};

		void ComputeCurvatureTensor() {
			vcg::tri::UpdateNormals<MESH_TYPE>::PerVertexNormalized(*mesh);

			VertexIterator vi;

			for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {

				if ( ! (*vi).IsD() && (*vi).VFp() != NULL) {

					VertexType * central_vertex = &(*vi);

					std::vector<float> weights;
					std::vector<AdjVertex> vertices;

					vcg::face::JumpingPos<FaceType> pos((*vi).VFp(), central_vertex);

					VertexType* firstV = pos.VFlip();
					VertexType* tempV;
					float totalDoubleAreaSize = 0.0f;

					if (((firstV->P()-central_vertex->P())^(pos.VFlip()->P()-central_vertex->P())).dot(central_vertex->N())<=0.0f)
					{
						pos.Set(central_vertex->VFp(), central_vertex);
						pos.FlipE();
						firstV = pos.VFlip();
					}	
					else pos.Set(central_vertex->VFp(), central_vertex);

					do 
					{
						pos.NextE();
						tempV = pos.VFlip();

						AdjVertex v;

						v.isBorder = pos.IsBorder();
						v.vert = tempV;			
						v.doubleArea = ((pos.F()->V(1)->P() - pos.F()->V(0)->P()) ^ (pos.F()->V(2)->P()- pos.F()->V(0)->P())).Norm();;
						totalDoubleAreaSize += v.doubleArea;

						vertices.push_back(v);						
					} 
					while(tempV != firstV);	

					for (int i = 0; i < vertices.size(); ++i) {
						if (vertices[i].isBorder) {
							weights.push_back(vertices[i].doubleArea / totalDoubleAreaSize);
						} else {
							weights.push_back(0.5f * (vertices[i].doubleArea + vertices[(i-1)%vertices.size()].doubleArea) / totalDoubleAreaSize);
						}
						assert(weights.back() < 1.0f);
					}

					Matrix33f Tp;
					for (int i = 0; i < 3; ++i)
						Tp[i][i] = 1.0f - powf(central_vertex->N()[i],2);
					Tp[0][1] = Tp[1][0] = -1.0f * (central_vertex->N()[0] * central_vertex->N()[1]);
					Tp[1][2] = Tp[2][1] = -1.0f * (central_vertex->N()[1] * central_vertex->N()[2]);
					Tp[0][2] = Tp[2][0] = -1.0f * (central_vertex->N()[0] * central_vertex->N()[2]);

					Matrix33f tempMatrix;
					Matrix33f M;
					M.SetZero();
					for (int i = 0; i < vertices.size(); ++i) {
						Point3f edge = (central_vertex->P() - vertices[i].vert->P());
						float curvature = (2.0f * (central_vertex->N().dot(edge)) ) / edge.SquaredNorm();
						Point3f T = (Tp*edge).normalized();
						tempMatrix.ExternalProduct(T,T);
						M += tempMatrix * weights[i] * curvature ;
					}

					Point3f W;
					Point3f e1(1.0f,0.0f,0.0f);
					if ((e1 - central_vertex->N()).SquaredNorm() > (e1 + central_vertex->N()).SquaredNorm())
						W = e1 - central_vertex->N();
					else 
						W = e1 + central_vertex->N();
					W.Normalize();

					Matrix33f Q;
					Q.SetIdentity();
					tempMatrix.ExternalProduct(W,W);
					Q -= tempMatrix * 2.0f;


					Matrix33f QtMQ = (Q.transpose() * M * Q);

					Point3f T1 = Q.GetColumn(1);
					Point3f T2 = Q.GetColumn(2);

					float s,c;
					// Gabriel Taubin hint and Valentino Fiorin impementation
					//float qt21 = QtMQ[2][1]; // unused var
					//float qt12 = QtMQ[1][2]; // unused var
					float alpha = QtMQ[1][1]-QtMQ[2][2];
					float beta  = QtMQ[2][1];

					float h[2];
					float delta = sqrtf(4.0f*powf(alpha, 2) +16.0f*powf(beta, 2));
					h[0] = (2.0f*alpha + delta) / (2.0f*beta);
					h[1] = (2.0f*alpha - delta) / (2.0f*beta);

					float t[2];
					float best_c, best_s;
					float min_error = FLT_MAX;
					for (int i=0; i<2; i++)
					{
						delta = sqrtf(powf(h[1], 2) + 4.0f);
						t[0] = (h[i]+delta) / 2.0f;
						t[1] = (h[i]-delta) / 2.0f;

						for (int j=0; j<2; j++)
						{
							float squared_t = powf(t[j], 2);
							float denominator = 1.0f + squared_t;
							s = (2.0f*t[j])		/ denominator;
							c = (1-squared_t) / denominator;

							float approximation = c*s*alpha + (powf(c, 2) - powf(s, 2))*beta;
							float angle_similarity = fabs(acosf(c)/asinf(s));
							float error = fabs(1.0f-angle_similarity)+fabs(approximation);
							if (error<min_error)
							{
								min_error = error;
								best_c = c;
								best_s = s;
							}
						}
					}
					c = best_c;
					s = best_s;

					vcg::ndim::MatrixMNf minor2x2 (2,2);
					vcg::ndim::MatrixMNf S (2,2);


					minor2x2[0][0] = QtMQ[1][1];
					minor2x2[0][1] = QtMQ[1][2];
					minor2x2[1][0] = QtMQ[2][1];
					minor2x2[1][1] = QtMQ[2][2];

					S[0][0] = S[1][1] = c;
					S[0][1] = s;
					S[1][0] = -1.0f * s;

					vcg::ndim::MatrixMNf StMS(S.transpose() * minor2x2 * S);

					float Principal_Curvature1 = (3.0f * StMS[0][0]) - StMS[1][1];
					float Principal_Curvature2 = (3.0f * StMS[1][1]) - StMS[0][0];

					Point3f Principal_Direction1 = T1 * c - T2 * s;
					Point3f Principal_Direction2 = T1 * s + T2 * c; 

					(*TDCurvPtr)[*vi].T1 = Principal_Direction1;
					(*TDCurvPtr)[*vi].T2 = Principal_Direction2;
					(*TDCurvPtr)[*vi].k1 = Principal_Curvature1;
					(*TDCurvPtr)[*vi].k2 = Principal_Curvature2;
				}
			}
		}
	};
}
#endif