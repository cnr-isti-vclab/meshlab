#ifndef MESHCUTTING_H
#define MESHCUTTING_H

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#include <iostream>
#include <fstream>
#include <cstdlib>

#include <vector>
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

#include <time.h>

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
			return (a.d > b.d);
		}
	};

	template <class VERTEX_TYPE> class SegmentHeap {
	private :
		vector<CuttingTriplet<VERTEX_TYPE> > _container;
		MinTriplet<VERTEX_TYPE> cmp;

	public :
		void push(CuttingTriplet<VERTEX_TYPE> element) {
			_container.push_back(element);
			push_heap(_container.begin(),_container.end(),cmp);
		}

		CuttingTriplet<VERTEX_TYPE> pop() {
			CuttingTriplet<VERTEX_TYPE> tmp = *(_container.begin());
			pop_heap(_container.begin(),_container.end(),cmp);
			_container.pop_back();
			return tmp;
		}

		void rebuild(){
			typename vector<CuttingTriplet<VERTEX_TYPE> >::iterator iter;
			int num_to_remove = 0;
			for (iter = _container.begin(); iter != _container.end(); ++iter) {
				if ((*iter).v->IMark() != U) {
					++num_to_remove;
					pop_heap(iter,_container.end(),cmp);
				}
			}
			_container.resize(_container.size() - num_to_remove);
		}

		int size() {
			return _container.size();
		}

		bool empty() {
			return _container.empty();
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

		//typedef priority_queue<CuttingTriplet<VertexType>, vector<CuttingTriplet<VertexType> >, MinTriplet<VertexType> > TripletQueue;
		typedef SegmentHeap<VertexType> TripletHeap;

	private:
		MESH_TYPE * mesh;
		SimpleTempData<VertContainer, CurvData> *TDCurvPtr;
		//TripletQueue Q;
		TripletHeap Q;
		float _normalWeight;
		float _curvatureWeight;

		float ImprovedIsophoticDist(VertexType * p, VertexType * q) {
			float dist;
			float kpq = 0.0f;
			const float e = 2.71828182845904523536;

			Matrix33<float> n_nMatrix;
			Point3<float> ViVj = p->P() - q->P();
			Point3<float> Tij;

			Point3<float> n = p->N();
			//n = n.Normalize();
			n_nMatrix.ExternalProduct(n, n);

			Tij = (n_nMatrix * ViVj).Normalize();

			float cos = (Tij * (*TDCurvPtr)[*p].T1.Normalize());
			cos *= cos;

			//k = k1 * cos^2(@) + k2 * sin^2(@); @ = angle between T1 and direction P->Q projected onto the plane N
			kpq = ((*TDCurvPtr)[*p].k1 * cos) + ((*TDCurvPtr)[*p].k2 * (1 - cos));

			if (kpq < 0) 
				//kpq = powf(e,fabs(kpq)) -1;
				//kpq = powf(kpq,2);
				kpq = powf(e,sqrt(fabs(kpq))) -1;
			dist = (p->P() - q->P()).Norm() + (_normalWeight * (p->N() - q->N()).Norm()) + (_curvatureWeight * kpq);

			return dist;
		}


		void AddNearestToQ(VertexType * v, std::ofstream & file) {

			float dist = 0.0f;
			float min_dist = std::numeric_limits<float>::max();

			vcg::face::JumpingPos<FaceType> pos(v->VFp(), v);

			VertexType* firstV = pos.VFlip();
			VertexType* nearestV=0;
			VertexType* tempV=0;

			do {
				pos.NextE();
				tempV = pos.VFlip();
				assert(tempV->P() != v->P());
				if (tempV->IMark() == U) {
					dist = ImprovedIsophoticDist(v, tempV);
					if (dist <= min_dist) {
						min_dist = dist;
						nearestV = tempV;
					}
				}
			} while(tempV != firstV);	

			if (nearestV) {
				CuttingTriplet<VertexType> tempTriplet;
				tempTriplet.d = min_dist;
				tempTriplet.v = nearestV;				
				switch(v->IMark()) {
					case iF: tempTriplet.m = F; break;
					case iB: tempTriplet.m = B; break;
					default : tempTriplet.m = (MarkType)v->IMark(); break;
				}
				Q.push(tempTriplet);
				if (file) file << "Inserzione: d=" << tempTriplet.d << std::endl;
			} else {
				if (file) file << "Nessun Elemento Inserito" << std::endl;
			}
		}

		void AddNeighborhoodNearestToQ(VertexType * v, std::ofstream & file) {
			vcg::face::JumpingPos<FaceType> pos(v->VFp(), v);
			VertexType* firstV = pos.VFlip();
			VertexType* tempV=0;

			do {
				pos.NextE();
				tempV = pos.VFlip();
				assert(tempV->P() != v->P());
				if (tempV->IMark() != U) {
					AddNearestToQ(tempV,file);
				}
			} while(tempV != firstV);	

		}

	public:

		MeshCutting(MESH_TYPE * ms) {
			mesh = ms;
			_normalWeight = 5.0f;
			_curvatureWeight = 5.0f;
			TDCurvPtr = new SimpleTempData<VertContainer, CurvData>((*mesh).vert);
			TDCurvPtr->Start(CurvData());
		}

		~MeshCutting() {
			TDCurvPtr->Stop();
		}

		inline void Mark(VertexType * v, MarkType m) {
			v->IMark() = m;
		}

		void MeshCut(float NormalWeight, float CurvatureWeight) {

			_normalWeight = NormalWeight;
			_curvatureWeight = CurvatureWeight;

			clock_t curvature_start_t;
			clock_t curvature_end_t;
			clock_t end_time;
			clock_t start_time = clock();

			VertexIterator vi;
			int vertex_to_go = 0;
			int inputCounter = 0;

			for (vi=(*mesh).vert.begin(); vi!=(*mesh).vert.end(); ++vi) {
				if ( !vi->IsD() && vi->IMark() != iF && vi->IMark() != iB) {
					vi->IMark() = U;
					++vertex_to_go;
				} else {
					++inputCounter;
				}
			}

			//check if no input is given to prevent infinite loop.
			if (!inputCounter) return;

			std::ofstream file;
			file.open("editsegment.log");

			curvature_start_t = clock();
			//Computing principal curvatures and directions for all vertices
			vcg::CurvatureTensor<MESH_TYPE>ct(mesh, TDCurvPtr);
			ct.ComputeCurvatureTensor();
			//now each vertex has principals curvatures and directions in its temp data
			curvature_end_t = clock();

			if (file) file << "Inizializzazione da input." << std::endl;

			//second iteration on the marked vertex
			for (vi=(*mesh).vert.begin(); vi!=(*mesh).vert.end(); ++vi) {
				if ( !vi->IsD() && (vi->IMark() != U))
					AddNearestToQ(&(*vi),file);	
			}

			if (file) file << "Fine inizializzazione da input. Elementi aggiunti: " << Q.size() << std::endl;

			int step_counter = 0;
			while (vertex_to_go != 0) {
				//algorithm main loop

				if (Q.empty()) {
					if (file) file << "Coda vuota. Re-Inizializzazione." << std::endl;
					for (vi=(*mesh).vert.begin(); vi!=(*mesh).vert.end(); ++vi) {
						if ( !vi->IsD() && (vi->IMark() != U))
							AddNearestToQ(&(*vi),file);	
					}
					if (Q.empty()) break;
				}	else {

					CuttingTriplet<VertexType> tempTriplet;

					//prendo la tripletta con distanza minima

					//tempTriplet = Q.top();
					//Q.pop();
					tempTriplet = Q.pop();

					//controlla se il vertice estratto è ancora valido o se è stato già marchiato in precedenza
					if (tempTriplet.v->IMark() == U) {
						tempTriplet.v->IMark() = tempTriplet.m;
						--vertex_to_go; 
						if (file) file << "Estrazione: d=" << tempTriplet.d << std::endl;
						AddNearestToQ(tempTriplet.v, file);	
						AddNeighborhoodNearestToQ(tempTriplet.v,file);
					} else {
						if (file) file << "Estrazione: Elemento nullo" << std::endl;
					}
					//++step_counter;
					//if (step_counter%2500 == 1) 
					//	Q.rebuild();
				}
			}
			end_time = clock();

			int total_time = end_time - start_time;
			int curvature_time = curvature_end_t - curvature_start_t;

			if (file) {
				file << std::endl << "tempo TOTALE impiegato: " << total_time << std::endl;
				file << std::endl << "tempo per calcolare la CURVATURA: " << curvature_time << std::endl;
				file.close();
			}
		}

		void Colorize(bool selectForeground, bool doRefine) {
			FaceIterator fi;
			VertexIterator vi;

			queue<FaceType*> edgeFaceQueue;

			for (vi = mesh->vert.begin(); vi != mesh->vert.end(); ++vi) {
				if ( vi->IMark() == F ) vi->C() = Color4b::Yellow;
				if ( vi->IMark() == B ) vi->C() = Color4b::LightGray;
				if ( vi->IMark() == U ) vi->C() = Color4b::Green;
			}

			int bitflag = FaceType::NewBitFlag();
			int bitflag_2 = FaceType::NewBitFlag();

			int count;
			if (selectForeground) {
				//select foreground
				for (fi = mesh->face.begin(); fi != mesh->face.end(); ++fi) {
					(*fi).ClearS();
					(*fi).ClearUserBit(bitflag);
					(*fi).ClearUserBit(bitflag_2);

					count = 0;
					for (int i = 0; i<3; ++i) {
						if ( (*fi).V(i)->IMark() == F || (*fi).V(i)->IMark() == iF	) ++count;
					}
					if (count == 3) {
						(*fi).SetS();
					}	else {
						if (doRefine) {
							if (count > 0) {
								edgeFaceQueue.push(&(*fi));
								(*fi).SetUserBit(bitflag);
							} else {
								(*fi).ClearS();
							}
						} else {
							(*fi).ClearS();
						}
					}
				}

				while(!edgeFaceQueue.empty()) {
					FaceType * tmp_face = edgeFaceQueue.front();
					edgeFaceQueue.pop();
					float prod[3];   
					float max_prod = std::numeric_limits<float>::min();
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
				//select background
				for (fi = mesh->face.begin(); fi != mesh->face.end(); ++fi) {
					(*fi).ClearUserBit(bitflag);
					(*fi).ClearUserBit(bitflag_2);
					count = 0;
					for (int i = 0; i<3; ++i) {
						if ( (*fi).V(i)->IMark() == B || (*fi).V(i)->IMark() == iB	) ++count;
					}
					if (count == 3) {
						(*fi).SetS();
					}	else {
						if (doRefine) {
							if (count > 0) {
								edgeFaceQueue.push(&(*fi));
								(*fi).SetUserBit(bitflag);
							} else {
								(*fi).ClearS();
							}
						} else {
							(*fi).ClearS();
						}
					}
				}

				while(!edgeFaceQueue.empty()) {
					FaceType * tmp_face = edgeFaceQueue.front();
					edgeFaceQueue.pop();
					float prod[3];   
					float max_prod = std::numeric_limits<float>::min();
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
				vi->IMark() = U;
				vi->C() = Color4b::LightGray;
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
