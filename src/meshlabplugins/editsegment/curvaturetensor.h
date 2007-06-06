#include <vcg/container/simple_temporary_data.h>
#include <vcg/simplex/face/pos.h>
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
		Point3<double> T1;
		Point3<double> T2;
		double k1;
		double k2;
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

		void givens(double a, double b, double *c, double *s)
		{
			double tao;
			if (b == 0) {
				*c = 1;
				*s = 0;
			}
			else {
				if (abs(b)>abs(a)) {
					tao =  -a/b;
					*s = 1.0/sqrt(1.0+tao*tao);
					*c = *s * tao;
				}
				else {
					tao =  -b/a;
					*c = 1.0/sqrt(1.0 + tao * tao);
					*s = *c * tao;
				}
			}
		}

		void ComputePerVertexMatrix(VertexType & i, VertexType & j, Matrix33<double>& Mvi, Matrix33<double> & n_nMatrix, double wij) {
			Point3<double> ViVj;
			Point3<double> Tij;
			double kij = 0.0f;
			Matrix33<double> tempMatrix;

			Point3<ScalarType> tempViVj = i.P() - j.P();
			Point3<double> n = Point3<double>((double)i.N()[0], (double)i.N()[1], (double)i.N()[2]); 
			n = n.Normalize();
			ViVj = Point3<double>((double)tempViVj[0], (double)tempViVj[1], (double)tempViVj[2]);
			Tij = (n_nMatrix * ViVj) / Norm(n_nMatrix * ViVj);
			kij = (2 * (n * ViVj)) / Norm(ViVj);

			tempMatrix.ExternalProduct(Tij, Tij);
			Mvi += tempMatrix * kij * wij;
		}

	public:
		CurvatureTensor(MESH_TYPE * m, SimpleTempData<VertContainer, CurvData> *TDCurvP) {
			mesh = m;
			TDCurvPtr = TDCurvP;
		}

		~CurvatureTensor() {

		}

		void ComputeCurvatureTensor () {
			//updating per-vertex normals
			vcg::tri::UpdateNormals<MESH_TYPE>::PerVertex(*mesh);

			VertexIterator vi;
			for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {
				if (!vi->IsD() ) {

					Matrix33<double> Mvi;
					Mvi.SetZero();
					Matrix33<double> I;
					I.SetIdentity();

					Matrix33<double> n_nM;
					Point3<double> normal_d = Point3<double>((double)vi->N()[0], (double)vi->N()[1],(double)vi->N()[2]);
					normal_d = normal_d.Normalize();
					n_nM.ExternalProduct(normal_d, normal_d);
					n_nM = I - n_nM;				

					//Inizio Calcolo dei wij per tutti i vertici adiacenti
					FaceType* first_face = vi->VFp();
					vcg::face::Pos<FaceType> pos2(first_face, vi->VFi(), &(*vi));
					vector<double> wij_container;
					double totalDoubleAreaSize = 0;
					do {

						double doubleArea = vcg::DoubleArea<FaceType>(*pos2.F());
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

					/* controlla che la somma degli wij sia <=1
					float totalwij = 0.0f;
					for (int i = 0; i< (wij_container.size()); ++i) {
					totalwij +=	wij_container[i];
					}
					assert(totalwij <= 1);*/

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
					Point3<double> Wvi;
					Matrix33<double> Qvi;
					Matrix33<double> QviT;
					Matrix33<double> tempMatrix;

					Matrix33<double> A;
					Point3<double> E1 = Point3<double>(1,0,0);
					Point3<double> Nvi = Point3<double>( (double)(*vi).N()[0], (double)(*vi).N()[1], (double)(*vi).N()[2]);
					Nvi.Normalize();
					Point3<double> E1nNvi = E1 - Nvi;
					Point3<double> E1pNvi = E1 + Nvi;

					if (E1nNvi.Norm() > E1pNvi.Norm() ) Wvi = E1nNvi / E1nNvi.Norm();
					else Wvi = E1pNvi / E1pNvi.Norm();

					tempMatrix.ExternalProduct(Wvi, Wvi);

					Qvi.SetIdentity();
					Qvi -= tempMatrix * 2;
					QviT = Qvi;
					QviT.Transpose();

					A = QviT * Mvi * Qvi;

					//loop to set at 0 extremely small values
					/*for (int i = 0; i<3; ++i) {
						for (int j = 0; j<3; ++j) {
							if (abs(A[i][j]) < 0.000001) { 
								A[i][j] = 0.0; 
							}
						}
					}	*/

					double c;
					double s;

					givens(A[1][1], A[2][1], &c, &s);

					Point3<double> T1 = (A.GetColumn(1) * c) - (A.GetColumn(2) * s);
					Point3<double> T2 = (A.GetColumn(1) * s) + (A.GetColumn(2) * c);

					double k1 = 3*T1[1] - T2[2];
					double k2 = 3*T2[2] - T1[1];

					(*TDCurvPtr)[*vi].T1 = T1;
					(*TDCurvPtr)[*vi].T2 = T2;
					(*TDCurvPtr)[*vi].k1 = k1;
					(*TDCurvPtr)[*vi].k2 = k2;
				}
			}
		}


	};
}