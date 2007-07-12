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

		void givens(float a, float b, float *c, float *s)
		{
			float tao;
			if (b == 0) {
				*c = 1;
				*s = 0;
			}
			else {
				if (fabs(b)>fabs(a)) {
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

		void ComputePerVertexMatrix(VertexType & i, VertexType & j, Matrix33<float>& Mvi, Matrix33<float> & n_nMatrix, float wij) {
			Point3<float> ViVj;
			Point3<float> Tij;
			float kij = 0.0f;
			Matrix33<float> tempMatrix;


			Point3<float> n = i.N();
			n = n.Normalize();
			ViVj = i.P() - j.P();
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
/* 
		//re-write, to be continued
		void newComputeCurvatureTensor() {
			vcg::tri::UpdateNormals<MESH_TYPE>::PerVertex(*mesh);

			VertexIterator vi;

			for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {
				
				if ( ! (*vi).IsD()) {
					std::vector<FaceType*> faces;
					std::vector<VertexType*> vertices;
					std::vector<float> weights;
					std::vector<AdjVertex> vertices;

					vcg::face::JumpingPos<FaceType> pos((*vi).VFp(), (*vi));
				
					VertexType* firstV = pos.VFlip();
					VertexType* tempV;
					float totalDoubleAreaSize = 0.0f;
					do 
					{
						pos.NextE();
						tempV = pos.VFlip();

						AdjVertex v;
						
						v.isBorder = pos.IsBorder()
						v.vertex = tempV;			
						v.doubleArea = pos.f->DoubleArea();
						totalDoubleAreaSize += v.doubleArea;

						vertices.push_back(v);						
					} 
					while(tempV != firstV);	
					
					std::vector<float> Weights;

					for (int i = 0; i<vertices.size(); ++i) {
						if (vertices[i].isBorder) {
							Weights.push_back(vertices[i].doubleArea /= totalDoubleAreaSize);
						} else {
							Weights.push_back((vertices[i].doubleArea + vertices[(i-1)%vertices.size()].doubleArea) / totalDoubleAreaSize);
						}
					}

				}
			}
		}

		*/
	};
}