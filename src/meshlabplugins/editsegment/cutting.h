#ifndef MESHCUTTING_H
#define MESHCUTTING_H

#ifdef max
#undef max
#endif
#ifdef minor
#undef minor
#endif

#include <queue>
#include <math.h>
#include <limits>
#include <vcg/container/simple_temporary_data.h>
#include <vcg/simplex/face/pos.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/simplex/vertexplus/base.h>
#include <vcg/simplex/faceplus/base.h>
#include <vcg/space/point3.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <curvaturetensor.h>

#include <vcg/complex/trimesh/update/color.h>
#include <vcg/math/histogram.h>
#include <vcg/complex/trimesh/stat.h>




namespace vcg {

	enum MarkType {U, //unmarked
		F, //foreground
		B, //background
		iF, //inputForeground
		iB //inputBackground
	}; 

	class MarkData {
	public:
		MarkType Mark;
	};

	template <class VERTEX_TYPE> class CuttingTriplet {
	public:	
		VERTEX_TYPE *v;		
		float d;				//improved isophotic distance from nearest vertex v* in N
		MarkType m;			//marking label of v*
	};

	template <class VERTEX_TYPE> class MinTriplet {
	public:
		bool operator() (const CuttingTriplet<VERTEX_TYPE> & a, const CuttingTriplet<VERTEX_TYPE> & b) const {
			return (a.d > b.d);
		}
	};

	template <class MESH_TYPE> class MeshCutting {

		typedef typename MESH_TYPE::FaceIterator FaceIterator;
		typedef typename MESH_TYPE::VertexIterator VertexIterator;
		typedef typename MESH_TYPE::VertContainer VertContainer;
		typedef typename MESH_TYPE::VertexType VertexType;
		typedef typename MESH_TYPE::FaceType FaceType;
		typedef typename MESH_TYPE::CoordType CoordType;		
		typedef typename MESH_TYPE::CoordType::ScalarType ScalarType;		

		typedef priority_queue<CuttingTriplet<VertexType>, vector<CuttingTriplet<VertexType> >, MinTriplet<VertexType> > TripletQueue;

	private:
		MESH_TYPE * mesh;
		SimpleTempData<VertContainer, MarkData> *TDMarkPtr;
		SimpleTempData<VertContainer, CurvData> *TDCurvPtr;
		TripletQueue Q;

		float ImprovedIsophoticDist(VertexType * p, VertexType * q) {
			float dist;
			double kpq = 0.0;
			const float e = 2.71828182845904523536;

			const float W1 = 0.1f;
			const float W2 = 0.9f;


			Matrix33<float> n_nMatrix;
			Point3<float> ViVj = p->P() - q->P();
			Point3<float> Tij;
			
			Point3<float> n = p->N();
			n = n.Normalize();
			n_nMatrix.ExternalProduct(n, n);
			
			Tij = (n_nMatrix * ViVj) / Norm(n_nMatrix * ViVj);

			float cos = (Tij * (*TDCurvPtr)[*p].T1) / (Tij.Norm() * ((*TDCurvPtr)[*p].T1).Norm());
			cos *= cos;

			//k = k1 * cos^2(@) + k2 * sin^2(@); @ = angle between T1 and direction P->Q projected onto the plane N
			kpq = ((*TDCurvPtr)[*p].k1 * cos) + ((*TDCurvPtr)[*p].k2 * (1 - cos));

			if (kpq < 0) {
				//kpq = pow(e,fabs(kpq)) - 1; //if kpq < 0 -> kpq = (e^|kpq|) - 1
				kpq = powf(e,fabs(kpq)) -1;
			}

			dist = (p->P() - q->P()).Norm() + W1 * (p->N() - q->N()).Norm() + W2 * kpq;

			return dist;
		}

		void AddNearestToQ(VertexType * v) {

			float dist = 0.0f;
			float min_dist = std::numeric_limits<float>::max();
			VertexType* nearestV=0;
			VertexType* tempV=0;
			vcg::face::VFIterator<FaceType> vi(v);

			FaceType * first_face = v->VFp();
			vcg::face::Pos<FaceType> pos(first_face, v->VFi(), v);
			pos.NextE();
			for (;pos.F() != first_face; pos.NextE()) {
				for (int i = 0; i < 3; ++i) {
					tempV = pos.F()->V(i);
					if (tempV->P() != v->P() && (*TDMarkPtr)[tempV].Mark == U) {
						//dist = vcg::SquaredDistance<ScalarType>(v->P(), tempV->P());
						dist = ImprovedIsophoticDist(v, tempV);
						if (dist < min_dist) {
							min_dist = dist;
							nearestV = tempV;
						}	
					}
				}
			}

			if (nearestV) {
				CuttingTriplet<VertexType> tempTriplet;
				tempTriplet.v = nearestV;
				tempTriplet.d = ImprovedIsophoticDist(v, nearestV);
				switch((*TDMarkPtr)[v].Mark) {
					case iF: tempTriplet.m = F; break;
					case iB: tempTriplet.m = B; break;
					default : tempTriplet.m = (*TDMarkPtr)[v].Mark; break;
				}
				Q.push(tempTriplet);
			}
		}


	public:

		MeshCutting(MESH_TYPE * ms) {
			mesh = ms;
			TDMarkPtr = new SimpleTempData<VertContainer, MarkData>((*mesh).vert);
			TDMarkPtr->Start(MarkData());
			TDCurvPtr = new SimpleTempData<VertContainer, CurvData>((*mesh).vert);
			TDCurvPtr->Start(CurvData());
		}

		~MeshCutting() {
			TDMarkPtr->Stop();
			TDCurvPtr->Stop();
		}

		inline void Mark(VertexType * v, MarkType m) {
			(*TDMarkPtr)[*v].Mark = m;
		}

		void MeshCut() {

			VertexIterator vi;
			int counter = 0;
			int inputCounter = 0;

			for (vi=(*mesh).vert.begin(); vi!=(*mesh).vert.end(); ++vi) {
				if ( !vi->IsD() && (*TDMarkPtr)[*vi].Mark != iF && (*TDMarkPtr)[*vi].Mark != iB) {
					(*TDMarkPtr)[*vi].Mark = U;
					++counter;
				} else {
					++inputCounter;
				}
			}

			//check if no input is given to prevent infinite loop.
			if (!inputCounter) return;

			//Computing principal curvatures and directions for all vertices
			vcg::CurvatureTensor<MESH_TYPE>ct(mesh, TDCurvPtr);
			ct.ComputeCurvatureTensor();
			//now each vertex has principals curvatures and directions in its temp data


			while (counter != 0) {
				//second iteration on the marked vertex
				for (vi=(*mesh).vert.begin(); vi!=(*mesh).vert.end(); ++vi) {
					if ( !vi->IsD() && ((*TDMarkPtr)[*vi].Mark != U))
						AddNearestToQ(&(*vi));	
				}

				//algorithm main loop
				CuttingTriplet<VertexType> tempTriplet;
				while(!Q.empty()) {
					tempTriplet = Q.top();
					Q.pop();
					if ( (*TDMarkPtr)[tempTriplet.v].Mark == U) {
						(*TDMarkPtr)[tempTriplet.v].Mark = tempTriplet.m;
						AddNearestToQ(tempTriplet.v);	
						--counter;
					}				
				}
			}
		}

		void Colorize(bool selectForeground) {
			FaceIterator fi;
			int count;
			if (selectForeground) {
				for (fi = mesh->face.begin(); fi != mesh->face.end(); ++fi) {
					count = 0;
					for (int i = 0; i<3; ++i) {
						if ( (*TDMarkPtr)[(*fi).V(i)].Mark == F || (*TDMarkPtr)[(*fi).V(i)].Mark == iF	) ++count;
					}
					if (count == 3) 
						(*fi).SetS();
					else 
						(*fi).ClearS();
				}
			} else {
				for (fi = mesh->face.begin(); fi != mesh->face.end(); ++fi) {
					count = 0;
					for (int i = 0; i<3; ++i) {
						if ( (*TDMarkPtr)[(*fi).V(i)].Mark == B || (*TDMarkPtr)[(*fi).V(i)].Mark == iB	) ++count;
					}
					if (count == 3) 
						(*fi).SetS();
					else 
						(*fi).ClearS();
				}
			}

		}

		//debugging function
		void ColorizeCurvature(bool gaussian) {
			vcg::CurvatureTensor<MESH_TYPE>ct(mesh, TDCurvPtr);
			ct.ComputeCurvatureTensor();
			
			VertexIterator vi;

			if (gaussian) { //gaussian
				for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {
					float gauss = (*TDCurvPtr)[*vi].k1 * (*TDCurvPtr)[*vi].k2; 
					vi->Q() = gauss;
				}
			} else { //mean
				for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {
					float mean = ((*TDCurvPtr)[*vi].k1 + (*TDCurvPtr)[*vi].k2) * 0.5f;
					vi->Q() = mean;
				}
			}

			Histogramf H;
			tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(*mesh,H);
			tri::UpdateColor<CMeshO>::VertexQuality(*mesh,H.Percentile(0.1),H.Percentile(0.9));
		}
	};

}
#endif
