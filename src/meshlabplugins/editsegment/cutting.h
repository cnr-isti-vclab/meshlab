#ifndef MESHCUTTING_H
#define MESHCUTTING_H

#ifdef max
#undef max
#endif

#include <iostream>
#include <fstream>
#include <cstdlib>

#include <queue>
#include <set>
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

	enum MarkType {
		U, //unmarked
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
			return (a.d < b.d);
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

		set<CuttingTriplet<VertexType>, MinTriplet<VertexType> > setQ; //usato come coda principale, serve a garantire l'ordinamento e allo stesso tempo a permettere la cancellazione di elementi in ordine sparso
		multimap<VertexType*,VertexType*> VertToV; //ogni vertice inserito nella coda principale ha una coppia qui che punta al vertice che ne ha provocato l'inserimento
		map<VertexType*,CuttingTriplet<VertexType> > VToTriplet; //ogni vertice ha una referenza alle triplette da esso inserite


		float ImprovedIsophoticDist(VertexType * p, VertexType * q) {
			float dist;
			float kpq = 0.0f;
			const float e = 2.71828182845904523536;

			const float W1 = 5.0f;
			const float W2 = 5.0f;

			Matrix33<float> n_nMatrix;
			Point3<float> ViVj = p->P() - q->P();
			Point3<float> Tij;

			Point3<float> n = p->N();
			n = n.Normalize();
			n_nMatrix.ExternalProduct(n, n);

			Tij = (n_nMatrix * ViVj).Normalize();

			float cos = (Tij * (*TDCurvPtr)[*p].T1.Normalize());
			cos *= cos;

			//k = k1 * cos^2(@) + k2 * sin^2(@); @ = angle between T1 and direction P->Q projected onto the plane N
			kpq = ((*TDCurvPtr)[*p].k1 * cos) + ((*TDCurvPtr)[*p].k2 * (1 - cos));

			if (kpq < 0)
				kpq = powf(e,fabs(kpq)) -1;

			dist = (p->P() - q->P()).Norm() + (W1 * (p->N() - q->N()).Norm()) + (W2 * kpq);

			return dist;
		}

		void AddNearestToQ(VertexType * v, std::ofstream & file) {

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
				tempTriplet.d = min_dist;
				switch((*TDMarkPtr)[v].Mark) {
					case iF: tempTriplet.m = F; break;
					case iB: tempTriplet.m = B; break;
					default : tempTriplet.m = (*TDMarkPtr)[v].Mark; break;
				}

				setQ.insert(tempTriplet);
				VertToV.insert(make_pair(tempTriplet.v, v));
				VToTriplet.insert(make_pair(v,tempTriplet));
				if (file) file << "inserita tripletta con distanza: " << tempTriplet.d << std::endl;
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
			int vertex_to_go = 0;
			int inputCounter = 0;

			for (vi=(*mesh).vert.begin(); vi!=(*mesh).vert.end(); ++vi) {
				if ( !vi->IsD() && (*TDMarkPtr)[*vi].Mark != iF && (*TDMarkPtr)[*vi].Mark != iB) {
					(*TDMarkPtr)[*vi].Mark = U;
					++vertex_to_go;
				} else {
					++inputCounter;
				}
			}

			//check if no input is given to prevent infinite loop.
			if (!inputCounter) return;

			std::ofstream file;
			file.open("editsegment.log");

			//Computing principal curvatures and directions for all vertices
			vcg::CurvatureTensor<MESH_TYPE>ct(mesh, TDCurvPtr);
			ct.ComputeCurvatureTensor();
			//now each vertex has principals curvatures and directions in its temp data

			if (file) file << "Inizializzazione da input." << std::endl;

			//second iteration on the marked vertex
			for (vi=(*mesh).vert.begin(); vi!=(*mesh).vert.end(); ++vi) {
				if ( !vi->IsD() && ((*TDMarkPtr)[*vi].Mark != U))
					AddNearestToQ(&(*vi),file);	
			}

			if (file) file << "Fine inizializzazione da input. Elementi aggiunti: " << setQ.size() << std::endl;

			while (vertex_to_go != 0) {
				//algorithm main loop

				if (setQ.empty()) {
					if (file) file << "Coda vuota. Re-Inizializzazione." << std::endl;
					for (vi=(*mesh).vert.begin(); vi!=(*mesh).vert.end(); ++vi) {
						if ( !vi->IsD() && ((*TDMarkPtr)[*vi].Mark != U))
							AddNearestToQ(&(*vi),file);	
					}
					if (setQ.empty()) break;
				}	else {

					CuttingTriplet<VertexType> tempTriplet;

					//prendo la tripletta con distanza minima

					tempTriplet = *(setQ.begin());
					assert((*TDMarkPtr)[tempTriplet.v].Mark == U);
					(*TDMarkPtr)[tempTriplet.v].Mark = tempTriplet.m;
					--vertex_to_go; 


					if (file) file << "Estratta tripletta con distanza: " << tempTriplet.d << std::endl;

					//prendo tutti i vertici che avevano inserito il vertice appena estratto
					vector<VertexType*> tempVertex;

					typedef typename multimap<VertexType*,VertexType*>::const_iterator MMI;
					pair<MMI,MMI> mm_range = VertToV.equal_range(tempTriplet.v);
					for (MMI mm_iter = mm_range.first; mm_iter != mm_range.second; ++mm_iter) {
						tempVertex.push_back(mm_iter->second);
					}

					VertToV.erase(tempTriplet.v);

					//rimuovo dalla coda tutte le triplette che sono state inserite dai vertici presi prima
					typename vector<VertexType*>::iterator tempV_iter;
					for (tempV_iter = tempVertex.begin(); tempV_iter != tempVertex.end(); tempV_iter++) {
						if (setQ.find(VToTriplet[*tempV_iter]) != setQ.end())
							setQ.erase(setQ.find(VToTriplet[*tempV_iter]));

						if (VToTriplet.find(*tempV_iter) != VToTriplet.end() ) 
							VToTriplet.erase(VToTriplet.find(*tempV_iter));
					}

					for (tempV_iter = tempVertex.begin(); tempV_iter != tempVertex.end(); tempV_iter++) {
						AddNearestToQ((*tempV_iter), file);	
					}

					AddNearestToQ(tempTriplet.v, file);	
					tempVertex.clear();
				}
			}

			VToTriplet.clear();
			setQ.clear();
			VertToV.clear();
			if (file) file.close();
		}

		void Colorize(bool selectForeground) {
			FaceIterator fi;
			VertexIterator vi;

			queue<FaceType*> edgeFaceQueue;

			for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {
				if ( (*TDMarkPtr)[(*vi)].Mark == F ) vi->C() = Color4b::Yellow;
				if ( (*TDMarkPtr)[(*vi)].Mark == B ) vi->C() = Color4b::White;
				if ( (*TDMarkPtr)[(*vi)].Mark == U ) vi->C() = Color4b::Green;
			}

			int bitflag = FaceType::NewBitFlag();
			int bitflag_2 = FaceType::NewBitFlag();

			int count;
			if (selectForeground) {

				for (fi = mesh->face.begin(); fi != mesh->face.end(); ++fi) {
					(*fi).ClearUserBit(bitflag);
					(*fi).ClearUserBit(bitflag_2);

					count = 0;
					for (int i = 0; i<3; ++i) {
						if ( (*TDMarkPtr)[(*fi).V(i)].Mark == F || (*TDMarkPtr)[(*fi).V(i)].Mark == iF	) ++count;
					}
					if (count == 3) {
						(*fi).SetS();
					}
					else if (count > 0) {
						edgeFaceQueue.push(&(*fi));
						(*fi).SetUserBit(bitflag);
					}	else 
						(*fi).ClearS();
				}

				while(!edgeFaceQueue.empty()) {
					FaceType * tmp_face = edgeFaceQueue.front();
					edgeFaceQueue.pop();
					float prod[3];
					float max_prod = 0.0f;
					int max_faceid = -1;
					for (int i=0; i<3; ++i) {
						prod[i] = (tmp_face->N().Normalize()) * (tmp_face->FFp(i)->N().Normalize());
						if (prod[i] > max_prod) {
							max_prod = prod[i];
							max_faceid = i;
						}  
					}


					if (!tmp_face->FFp(max_faceid)->IsUserBit(bitflag)) {
						//faccia certa
						if (tmp_face->FFp(max_faceid)->IsS()) {
							tmp_face->SetS();
						} else {
							tmp_face->ClearS();
						}
						tmp_face->ClearUserBit(bitflag);
					} else {
						//max faccia incerta
						//prendo la seconda
						float sec_prod = 0.0f;
						int sec_faceid = -1;
						for (int i = 0; i<3; ++i) {
							if (i != max_faceid && prod[i] > sec_prod && prod[i] > (max_prod - 0.001f) && !tmp_face->FFp(i)->IsUserBit(bitflag)) {
								sec_prod = prod[i];
								sec_faceid = i;
							}  
						}
						if (sec_prod != 0.0f) {
							if (tmp_face->FFp(sec_faceid)->IsS()) {
								tmp_face->SetS();
							} else {
								tmp_face->ClearS();
							}
							tmp_face->ClearUserBit(bitflag);
						} else {
							if (!tmp_face->IsUserBit(bitflag_2)) {
								edgeFaceQueue.push(tmp_face);
								tmp_face->SetUserBit(bitflag);
								tmp_face->SetUserBit(bitflag_2);
							}
						}
					}
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
			
			FaceType::DeleteBitFlag(bitflag_2);
			FaceType::DeleteBitFlag(bitflag);
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
			tri::UpdateColor<CMeshO>::VertexQuality(*mesh,H.Percentile(0.1f),H.Percentile(0.9f));
		}

		void Reset() {
			VertexIterator vi;

			for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {
				(*TDMarkPtr)[*vi].Mark = U;
				vi->C() = Color4b::White;
				vi->Q() = 0.0f;
			}

			FaceIterator fi;
			for (fi = mesh->face.begin(); fi != mesh->face.end(); ++fi) {
				(*fi).ClearS();
			}
		}
	};

}
#endif
