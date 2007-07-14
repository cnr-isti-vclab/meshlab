#include <vcg/container/simple_temporary_data.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/simplex/face/jumping_pos.h>

#include <vcg/complex/trimesh/base.h>
#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/faceplus/base.h>
#include <vcg/space/point3.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/math/matrix.h>
#include <vcg/math/matrix33.h>

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
			vcg::tri::UpdateNormals<MESH_TYPE>::PerVertex(*mesh);

			VertexIterator vi;

			for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {

				if ( ! (*vi).IsD()) {

					VertexType * central_vertex = &(*vi);

					std::vector<float> weights;
					std::vector<float> curvatures;
					std::vector<AdjVertex> vertices;

					vcg::face::JumpingPos<FaceType> pos((*vi).VFp(), central_vertex);

					VertexType* firstV = pos.VFlip();
					VertexType* tempV;
					float totalDoubleAreaSize = 0.0f;

				/*	if (((firstV->P()-central_vertex->P())^(pos.VFlip()->P()-central_vertex->P()))*central_vertex->N()<=0.0f)
					{
						pos.Set(central_vertex->VFp(), central_vertex);
						pos.FlipE();
						firstV = pos.VFlip();
					}	
					else pos.Set(central_vertex->VFp(), central_vertex);*/

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

					for (int i = 0; i<vertices.size(); ++i) {
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
						curvatures.push_back(2.0f * ((central_vertex->N() * edge) / edge.SquaredNorm() ) );

						Point3f T = (Tp*edge).Normalize();
						tempMatrix.ExternalProduct(T,T);
						M += tempMatrix * weights[i] * curvatures[i] ;
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

					Matrix33f Qt(Q);
					Qt.Transpose();

					Matrix33f QtMQ = (Qt * M * Q);

					Point3f T1 = Q.GetColumn(1);
					Point3f T2 = Q.GetColumn(2);

					float s,c;
					// Gabriel Taubin hint and Valentino Fiorin impementation
					float qt21 = QtMQ[2][1];
					float qt12 = QtMQ[1][2];
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

					vcg::ndim::MatrixMNf minor (2,2);
					vcg::ndim::MatrixMNf S (2,2);


					minor[0][0] = QtMQ[1][1];
					minor[0][1] = QtMQ[1][2];
					minor[0][0] = QtMQ[2][1];
					minor[1][1] = QtMQ[2][2];

					S[0][0] = S[1][1] = c;
					S[0][1] = s;
					S[1][0] = -1.0f * s;

					vcg::ndim::MatrixMNf St (S);
					St.Transpose();					

					vcg::ndim::MatrixMNf StMS(St * minor * S);

					float Principal_Curvature1 = 3.0f * StMS[0][0] - StMS[1][1];
					float Principal_Curvature2 = 3.0f * StMS[1][1] - StMS[0][0];

					Point3f Principal_Direction1 = T1 * c - T2 * s;
					Point3f Principal_Direction2 = T1 * s + T2 * c; 

					(*TDCurvPtr)[*vi].T1 = Principal_Direction1;
					(*TDCurvPtr)[*vi].T2 = Principal_Direction2;
					(*TDCurvPtr)[*vi].k1 = Principal_Curvature1;
					(*TDCurvPtr)[*vi].k2 = Principal_Curvature2;
				}
			}
		}

		/* OLD VERSION
		void oldComputeCurvatureTensor () {
		//updating per-vertex normals
		vcg::tri::UpdateNormals<MESH_TYPE>::PerVertex(*mesh);

		VertexIterator vi;
		for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {
		if (!vi->IsD() ) {

		Matrix33<float> Mvi;
		Mvi.SetZero();
		Matrix33<float> I;
		I.SetIdentity();

		Matrix33<float> n_nM;
		//Point3<float> normal_d = Point3<double>((double)vi->N()[0], (double)vi->N()[1],(double)vi->N()[2]);
		Point3<float> normal = vi->N();
		normal = normal.Normalize();
		n_nM.ExternalProduct(normal, normal);
		n_nM = I - n_nM;				

		//Inizio Calcolo dei wij per tutti i vertici adiacenti
		FaceType* first_face = vi->VFp();
		vcg::face::Pos<FaceType> pos2(first_face, vi->VFi(), &(*vi));
		vector<float> wij_container;
		float totalDoubleAreaSize = 0;
		do {

		float doubleArea = vcg::DoubleArea<FaceType>(*pos2.F());
		totalDoubleAreaSize += doubleArea;
		wij_container.push_back(doubleArea);

		pos2.NextE();
		}  while (first_face != pos2.F());

		totalDoubleAreaSize *= 2;

		int last_wij = wij_container[wij_container.size()-1];
		for (int i = (wij_container.size()-1); i > 0; --i) {
		wij_container[i] += wij_container[i-1];
		wij_container[i] /= totalDoubleAreaSize;
		}
		wij_container[0] += last_wij;
		wij_container[0] /= totalDoubleAreaSize;

		//calcola la matrice Mvi per ogni vertice 
		vcg::face::Pos<FaceType> pos(first_face, vi->VFi(), &(*vi));
		pos.FlipV();
		pos.FlipE();
		int i = 0;
		ComputePerVertexMatrix(*vi, *(pos.V()), Mvi, n_nM, wij_container[i]);
		pos.FlipV();
		pos.FlipE();
		pos.FlipF();
		pos.FlipE();
		while (first_face != pos.F()) {
		++i;
		ComputePerVertexMatrix(*vi, *(pos.V()), Mvi, n_nM, wij_container[i]);
		pos.FlipV();
		pos.FlipE();
		pos.FlipF();
		pos.FlipE();					
		}


		//Mvi matrix ready for the vertex vi
		//calculate principal directions and curvature
		Point3f Wvi;
		Matrix33f Qvi;
		Matrix33f QviT;
		Matrix33f tempMatrix;

		Matrix33f A;
		Point3f E1(1.0,0.0,0.0);
		Point3f Nvi = (*vi).N();
		Nvi.Normalize();
		Point3f E1nNvi = E1 - Nvi;
		Point3f E1pNvi = E1 + Nvi;

		if (E1nNvi.Norm() > E1pNvi.Norm() ) Wvi = E1nNvi / E1nNvi.Norm();
		else Wvi = E1pNvi / E1pNvi.Norm();

		tempMatrix.ExternalProduct(Wvi, Wvi);

		Qvi.SetIdentity();
		Qvi -= tempMatrix * 2;
		QviT = Qvi;
		QviT.Transpose();

		A = QviT * Mvi * Qvi;

		float c;
		float s;

		givens(A[1][1], A[2][1], &c, &s);

		Point3<float> T1 = (A.GetColumn(1) * c) - (A.GetColumn(2) * s);
		Point3<float> T2 = (A.GetColumn(1) * s) + (A.GetColumn(2) * c);

		float k1 = 3*T1[1] - T2[2];
		float k2 = 3*T2[2] - T1[1];

		(*TDCurvPtr)[*vi].T1 = T1;
		(*TDCurvPtr)[*vi].T2 = T2;
		(*TDCurvPtr)[*vi].k1 = k1;
		(*TDCurvPtr)[*vi].k2 = k2;
		}
		}
		}
		*/

	};
}