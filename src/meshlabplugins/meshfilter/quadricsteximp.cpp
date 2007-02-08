/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
/****************************************************************************
  History
$Log$
Revision 1.2  2007/02/08 15:54:33  cignoni
Corrected ">>" into "> >" in double templates

Revision 1.1  2007/02/08 13:40:00  pirosu
Added Quadric Simplification(with textures) Filter


****************************************************************************/

#include "meshfilter.h"   
#include <QVector.h>
#include <QPair.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/local_optimization.h>
#include <vcg/complex/local_optimization/tri_edge_collapse_quadric.h>
#include <vcg/container/simple_temporary_data.h>
#include "algebra5.h"
#include "quadric5.h"

typedef	SimpleTempData<CMeshO::VertContainer, QVector<QPair<vcg::TCoord2<float>,Quadric5<double> > > > Quadric5Temp;
typedef	SimpleTempData<CMeshO::VertContainer, math::Quadric<double> > QuadricTemp;

using namespace vcg;
using namespace std;
using namespace vcg::tri;


class TriEdgeCollapseQuadricTexParameter
{
public:
	double	QualityThr; // all 
	double	BoundaryWeight;
	double	NormalThr;
	double	CosineThr;
	double	QuadricEpsilon;
	double	ScaleFactor;
	float   ExtraTCoordWeight;
	bool		UseArea;
	bool		UseVertexWeight;
	bool		NormalCheck;
	bool		QualityCheck;
	bool		OptimalPlacement;
	bool		MemoryLess;
	bool		ComplexCheck;
	bool		ScaleIndependent;
	//***********************
	bool		PreserveTopology; 
	bool		PreserveBoundary; 
	bool		MarkComplex;
	bool		FastPreserveBoundary; 
	bool		SafeHeapUpdate;
};



class QuadricTexHelper
		{
		public:

			QuadricTexHelper(){};


	      static void Init(){};

	  // it allocs the QPair for the vertex relativly to the texture coord parameter
	  static void Alloc(CVertexO *v,vcg::TCoord2<float> &coord)
	  {
		   QVector<QPair<vcg::TCoord2<float>,Quadric5<double> > > &qv = Vd(v);
		   Quadric5<double> *newq5 = new Quadric5<double>;
		   newq5->Zero();
		   vcg::TCoord2<float> *newcoord = new vcg::TCoord2<float>();
		   newcoord->u() = 0;
		   newcoord->v() = 0;
		   newcoord->u() += coord.u();
		   newcoord->v() += coord.v();

		   newq5->Sum3(Qd3(v),coord.u(),coord.v());

		   qv.push_back(*(new QPair<vcg::TCoord2<float>,Quadric5<double> >(*newcoord,*newq5)));
	  }

	  static void SumAll(CVertexO *v,vcg::TCoord2<float> &coord, Quadric5<double>& q)
	  {
		   QVector<QPair<vcg::TCoord2<float>,Quadric5<double> > > &qv = Vd(v);

		   for(int i = 0; i < qv.size(); i++)
		   {
			   vcg::TCoord2<float> &f = qv[i].first;
			   if((f.u() == coord.u()) && (f.v() == coord.v()))
				   qv[i].second += q;
			   else
				   qv[i].second.Sum3(Qd3(v),f.u(),f.v()); 
		   }
	  }

	  static bool Contains(CVertexO *v,vcg::TCoord2<float> &coord)
	  {
		   QVector<QPair<vcg::TCoord2<float>,Quadric5<double> > > &qv = Vd(v);

		   for(int i = 0; i < qv.size(); i++)
		   {
			   vcg::TCoord2<float> &f = qv[i].first;
			   if((f.u() == coord.u()) && (f.v() == coord.v()))
				   return true;
		   }
	   
		   return false;
	  }

	  static Quadric5<double> &Qd(CVertexO *v,vcg::TCoord2<float> &coord)
	  {
		   QVector<QPair<vcg::TCoord2<float>,Quadric5<double> > > &qv = Vd(v);

		   for(int i = 0; i < qv.size(); i++)
		   {
			   vcg::TCoord2<float> &f = qv[i].first;
			   if((f.u() == coord.u()) && (f.v() == coord.v()))
				   return qv[i].second;
		   }
	   
		   assert(0);
	  }
      static math::Quadric<double> &Qd3(CVertexO *v) {return TD3()[*v];}
	  static math::Quadric<double> &Qd3(CVertexO &v) {return TD3()[v];}
	 
	  static QVector<QPair<vcg::TCoord2<float>,Quadric5<double> > > &Vd(CVertexO *v){return (TD()[*v]);}
      static CVertexO::ScalarType W(CVertexO * /*v*/) {return 1.0;};
      static CVertexO::ScalarType W(CVertexO & /*v*/) {return 1.0;};
      static void Merge(CVertexO & /*v_dest*/, CVertexO const & /*v_del*/){};
      static  Quadric5Temp* &TDp() {static  Quadric5Temp *td; return td;}
      static  Quadric5Temp &TD() {return *TDp();}
      static  QuadricTemp* &TDp3() {static  QuadricTemp *td3; return td3;}
      static  QuadricTemp &TD3() {return *TDp3();}
		};



template<class TriMeshType,class MYTYPE, class HelperType = tri::QInfoStandard<typename TriMeshType::VertexType>  >
class TriEdgeCollapseQuadricTex: public vcg::tri::TriEdgeCollapse< TriMeshType, MYTYPE> 
{
	typedef HelperType QH;
	typedef typename tri::TriEdgeCollapse<TriMeshType, MYTYPE>::HeapType HeapType;
	typedef typename tri::TriEdgeCollapse<TriMeshType, MYTYPE>::HeapElem HeapElem;
	typedef typename TriMeshType::FaceType FaceType;
	typedef typename TriMeshType::VertexType VertexType;


	public:

	inline TriEdgeCollapseQuadricTex(const EdgeType &p, int mark)
		//:TEC(p,i){}
	{
			this->localMark = mark;
			this->pos=p;
			this->_priority = ComputePriority();
	}

// puntatori ai vertici che sono stati messi non-w per preservare il boundary
	static std::vector<typename TriMeshType::VertexPointer>  & WV(){
      static std::vector<typename TriMeshType::VertexPointer> _WV; return _WV;
    }; 



	static TriEdgeCollapseQuadricTexParameter & Params(){static TriEdgeCollapseQuadricTexParameter p; return p;}

	    // Final Clean up after the end of the simplification process
    static void Finalize(TriMeshType &m,HeapType&h_ret)
    {
	  vcg::tri::UpdateFlags<TriMeshType>::FaceBorderFromVF(m);

      // If we had the boundary preservation we should clean up the writable flags
      if(Params().FastPreserveBoundary)
      {
        typename 	TriMeshType::VertexIterator  vi;
    	  for(vi=m.vert.begin();vi!=m.vert.end();++vi) 
          if(!(*vi).IsD()) (*vi).SetW();
      }
    	if(Params().PreserveBoundary)
      {
        typename 	std::vector<typename TriMeshType::VertexPointer>::iterator wvi;
        for(wvi=WV().begin();wvi!=WV().end();++wvi)
          if(!(*wvi)->IsD()) (*wvi)->SetW();
      }
    }

	static void SetDefaultParams(){
		Params().UseArea=true;
		Params().UseVertexWeight=false;
		Params().NormalCheck=false;
		Params().NormalThr=M_PI/2;
		Params().QualityCheck=true;
		Params().QualityThr=.1;
		Params().BoundaryWeight=.5;
		Params().OptimalPlacement=true;
		Params().ScaleIndependent=true;
		Params().ComplexCheck=false;
		Params().QuadricEpsilon =1e-15;
		Params().ScaleFactor=1.0;
		Params().ExtraTCoordWeight=0.0;

		Params().PreserveTopology = false;
	}


	inline static int matchVertexID(FaceType *f,VertexType *v)
	{
		if(f->V(0)==v)
			return 0;
		if(f->V(1)==v)
			return 1;
		if(f->V(2)==v)
			return 2;

		assert(0);
	}

	inline int GetTexCoords(vcg::TCoord2<float> &tcoord0_1,vcg::TCoord2<float> &tcoord1_1,vcg::TCoord2<float> &tcoord0_2,vcg::TCoord2<float> &tcoord1_2)
	{
		int ncoords = 0;

		vcg::face::VFIterator<FaceType> vfi(this->pos.V(0));

		for(vfi.F() = this->pos.V(0)->VFp(), vfi.I() = this->pos.V(0)->VFi(); vfi.F()!=0; ++vfi )	// for all faces in v0		
			if(vfi.F()->V(0)==this->pos.V(1) || vfi.F()->V(1)==this->pos.V(1) || vfi.F()->V(2)==this->pos.V(1) ) // and v1
			{
				if(ncoords == 0)
				{
					tcoord0_1 = vfi.F()->WT(matchVertexID(vfi.F(),this->pos.V(0)));
					tcoord1_1 = vfi.F()->WT(matchVertexID(vfi.F(),this->pos.V(1)));
				}
				else
				{
					tcoord0_2 = vfi.F()->WT(matchVertexID(vfi.F(),this->pos.V(0)));
					tcoord1_2 = vfi.F()->WT(matchVertexID(vfi.F(),this->pos.V(1)));
					
					if(
						(tcoord0_1.u() == tcoord0_2.u()) &&
						(tcoord0_1.v() == tcoord0_2.v()) &&
						(tcoord1_1.u() == tcoord1_2.u()) &&
						(tcoord1_1.v() == tcoord1_2.v())
						)
						return 1;
					else
						return 2;
				}

				ncoords++;
			}

		return ncoords;
	}


		ScalarType ComputePriority()  {
			Quadric5<double> qsum1;
			Quadric5<double> qsum2;
			double min1[5];
			double min2[5];
			vcg::TCoord2<float> tcoord0_1;
			vcg::TCoord2<float> tcoord1_1;
			vcg::TCoord2<float> tcoord0_2;
			vcg::TCoord2<float> tcoord1_2;
			int ncoords;

			ncoords = GetTexCoords(tcoord0_1,tcoord1_1,tcoord0_2,tcoord1_2);

			return ComputeMinimalsAndPriority(min1,min2,qsum1,qsum2,tcoord0_1,tcoord1_1,tcoord0_2,tcoord1_2,ncoords);			
		}

		ScalarType ComputePriority(double vv[5],Quadric5<double> &qsum)
		{
			typename TriMeshType::VertexType * v[2];
			v[0] = this->pos.V(0);
			v[1] = this->pos.V(1);

			//// Move the two vertexe  into new position (storing the old ones)
			CoordType OldPos0=v[0]->P();
			CoordType OldPos1=v[1]->P();

			v[0]->P() = CoordType(vv[0],vv[1],vv[2]);
			//v[0]->P() = (v[0]->P()+v[1]->P())/2.0;
			v[1]->P()=v[0]->P();

			double QuadErr = qsum.Apply(vv); 

			//// Rescan faces and compute quality and difference between normals
			int i;

			double qt,   MinQual = 1e100;
			vcg::face::VFIterator<FaceType> x(this->pos.V(0));

			for(x.F() = v[0]->VFp(), x.I() = v[0]->VFi(),i=0; x.F()!=0; ++x )	// for all faces in v0		
				if(x.F()->V(0)!=v[1] && x.F()->V(1)!=v[1] && x.F()->V(2)!=v[1] )		// skip faces with v1
				{
					qt= QualityFace(*x.F());
					if(qt<MinQual) MinQual=qt;
				}
			for(x.F() = v[1]->VFp(), x.I() = v[1]->VFi(),i=0; x.F()!=0; ++x )		// for all faces in v1	
				if(x.F()->V(0)!=v[0] && x.F()->V(1)!=v[0] && x.F()->V(2)!=v[0] )			// skip faces with v0
				{
					qt= QualityFace(*x.F());
					if(qt<MinQual) MinQual=qt;
				}


			// All collapses involving triangles with quality larger than <QualityThr> has no penalty;
			if(MinQual>Params().QualityThr) MinQual=Params().QualityThr;  
					
			
			if(QuadErr<1e-15) QuadErr=1e-15;
			
			this->_priority = (ScalarType)(QuadErr / MinQual);
																								           
			//Rrestore old position of v0 and v1
			v[0]->P()=OldPos0;
			v[1]->P()=OldPos1;
			return this->_priority;
		}

		inline ScalarType ComputeMinimalsAndPriority(double dest_1[5],
									double dest_2[5],
									Quadric5<double> &qsum_1,
									Quadric5<double> &qsum_2,
									vcg::TCoord2<float> &tcoord0_1,
									vcg::TCoord2<float> &tcoord1_1,
									vcg::TCoord2<float> &tcoord0_2,
									vcg::TCoord2<float> &tcoord1_2,
									int ncoords
									)
		{
			double tmp1[5];
			double tmp2[5];
			ScalarType priority1;
			ScalarType priority2;

			tmp1[0] = this->pos.V(0)->P().X();
			tmp1[1] = this->pos.V(0)->P().Y();
			tmp1[2] = this->pos.V(0)->P().Z();
			tmp1[3] = tcoord0_1.u();
			tmp1[4] = tcoord0_1.v();

			tmp2[0] = this->pos.V(1)->P().X();
			tmp2[1] = this->pos.V(1)->P().Y();
			tmp2[2] = this->pos.V(1)->P().Z();
			tmp2[3] = tcoord1_1.u();
			tmp2[4] = tcoord1_1.v();


			assert(QH::Qd(this->pos.V(0),tcoord0_1).IsValid());
			assert(QH::Qd(this->pos.V(1),tcoord1_1).IsValid());

			qsum_1 = QH::Qd(this->pos.V(0),tcoord0_1);
			qsum_1 += QH::Qd(this->pos.V(1),tcoord1_1);

			ComputeMinimal(dest_1,tmp1,tmp2,qsum_1);
			priority1 = ComputePriority(dest_1,qsum_1);
			
			if(ncoords < 2)
				return priority1*(1 + (Params().ExtraTCoordWeight)*(QH::Vd(this->pos.V(0)).size()+ QH::Vd(this->pos.V(1)).size() - 2));


			tmp1[3] = tcoord0_2.u();
			tmp1[4] = tcoord0_2.v();

			tmp2[3] = tcoord1_2.u();
			tmp2[4] = tcoord1_2.v();

			assert(QH::Qd(this->pos.V(0),tcoord0_2).IsValid());
			assert(QH::Qd(this->pos.V(1),tcoord1_2).IsValid());

			qsum_2 = QH::Qd(this->pos.V(0),tcoord0_2);
			qsum_2 += QH::Qd(this->pos.V(1),tcoord1_2);

			ComputeMinimal(dest_2,tmp1,tmp2,qsum_2);
			priority2 = ComputePriority(dest_2,qsum_2);

			if(priority1 > priority2)
			{
				ComputeMinimalWithGeoContraints(dest_2,tmp1,tmp2,qsum_2,dest_1);
				priority2 = ComputePriority(dest_2,qsum_2);
			}
			else
			{
				ComputeMinimalWithGeoContraints(dest_1,tmp1,tmp2,qsum_1,dest_2);
				priority1 = ComputePriority(dest_1,qsum_1);
			}


			this->_priority = max(priority1, priority2)*(1 + (Params().ExtraTCoordWeight)*(QH::Vd(this->pos.V(0)).size()+QH::Vd(this->pos.V(1)).size() - 2));
		
			return this->_priority;
		}

		inline void ComputeMinimal(double vv[5],double v0[5],double v1[5], Quadric5<double> qsum)
		{	
			bool rt=qsum.Minimum(vv);
				if(!rt) { // if the computation of the minimum fails we choose between the two edge points and the middle one.
			
					vv[0] = (v0[0] + v1[0])/2;
					vv[1] = (v0[1] + v1[1])/2;
					vv[2] = (v0[2] + v1[2])/2;
					vv[3] = (v0[3] + v1[3])/2;
					vv[4] = (v0[4] + v1[4])/2;

					double qvx=qsum.Apply(vv);
					double qv0=qsum.Apply(v0);
					double qv1=qsum.Apply(v1);

					
					if(qv0<qvx)
					{
						vv[0] = v0[0];
						vv[1] = v0[1];
						vv[2] = v0[2];
						vv[3] = v0[3];
						vv[4] = v0[4];
					}

					if(qv1<qvx && qv1<qv0)
					{
						vv[0] = v1[0];
						vv[1] = v1[1];
						vv[2] = v1[2];
						vv[3] = v1[3];
						vv[4] = v1[4];
					}
				}
				 
		}

		inline void ComputeMinimalWithGeoContraints(double vv[5],double v0[5],double v1[5], Quadric5<double> qsum, double geo[5])
		{
 			bool rt=qsum.MinimumWithGeoContraints(vv,geo);
				if(!rt) { // if the computation of the minimum fails we choose between the two edge points and the middle one.
			 
					vv[0] = geo[0];
					vv[1] = geo[1];
					vv[2] = geo[2];
					vv[3] = (v0[3] + v1[3])/2;
					vv[4] = (v0[4] + v1[4])/2;

					double qvx=qsum.Apply(vv);

					vv[3] = v0[3];
					vv[4] = v0[4];

					double qv0=qsum.Apply(vv);

					vv[3] = v1[3];
					vv[4] = v1[4];

					double qv1=qsum.Apply(v1);

					vv[3] = (v0[3] + v1[3])/2;
					vv[4] = (v0[4] + v1[4])/2;
					
					if(qv0<qvx)
					{
						vv[3] = v0[3];
						vv[4] = v0[4];
					}

					if(qv1<qvx && qv1<qv0)
					{
						vv[3] = v1[3];
						vv[4] = v1[4];
					}
				}
				
		}

	static void InitQuadric(TriMeshType &m)
	{
		typename TriMeshType::FaceIterator pf;
		int j;
		HelperType::Init(); 
			
		for(pf=m.face.begin();pf!=m.face.end();++pf)
			if( !(*pf).IsD() && (*pf).IsR() )
				if((*pf).V(0)->IsR() &&(*pf).V(1)->IsR() &&(*pf).V(2)->IsR())
						{
							Quadric5<double> q;
							q.byFace(*pf,QH::Qd3((*pf).V(0)),QH::Qd3((*pf).V(1)),QH::Qd3((*pf).V(2)));

							for(j=0;j<3;++j)
							  if( (*pf).V(j)->IsW())
							  {
								  if(!HelperType::Contains((*pf).V(j),(*pf).WT(j)))
								  {
									HelperType::Alloc((*pf).V(j),(*pf).WT(j));
								  }

								  HelperType::SumAll((*pf).V(j),(*pf).WT(j),q);
							  }

						}

		typename TriMeshType::VertexIterator pv;
		for(pv=m.vert.begin();pv!=m.vert.end();++pv)
		{
			if(QH::Vd(&*pv).size() > 1)
			{
				
				for(int i = 0; i < QH::Vd(&*pv).size(); i++)
				{
					Quadric5<double> &q =  QH::Vd(&*pv)[i].second;
					math::Quadric<double> &q3 =  QH::Qd3(*pv);
					j = 1;

				}
			}

		} 

	}

		static void Init(TriMeshType &m,HeapType&h_ret){

		typename 	TriMeshType::VertexIterator  vi;
		typename 	TriMeshType::FaceIterator  pf;

		EdgeType av0,av1,av01;

			vcg::tri::UpdateTopology<TriMeshType>::VertexFace(m);
			vcg::tri::UpdateFlags<TriMeshType>::FaceBorderFromFF(m);

			InitQuadric(m);

		// Initialize the heap with all the possible collapses 
			  for(vi=m.vert.begin();vi!=m.vert.end();++vi) 
					if((*vi).IsRW())
							{
									vcg::face::VFIterator<FaceType> x;
									for( x.F() = (*vi).VFp(), x.I() = (*vi).VFi(); x.F()!=0; ++ x){
										x.V1()->ClearV();
										x.V2()->ClearV();
								  }
									for( x.F() = (*vi).VFp(), x.I() = (*vi).VFi(); x.F()!=0; ++x )
					{
										assert(x.F()->V(x.I())==&(*vi));
										if((x.V0()<x.V1()) && x.V1()->IsRW() && !x.V1()->IsV()){
													x.V1()->SetV();
													h_ret.push_back(HeapElem(new MYTYPE(EdgeType(x.V0(),x.V1()),TriEdgeCollapse< TriMeshType,MYTYPE>::GlobalMark() )));
													}
										if((x.V0()<x.V2()) && x.V2()->IsRW()&& !x.V2()->IsV()){
													x.V2()->SetV();
													h_ret.push_back(HeapElem(new MYTYPE(EdgeType(x.V0(),x.V2()),TriEdgeCollapse< TriMeshType,MYTYPE>::GlobalMark() )));
												}
									}
					}	
		make_heap(h_ret.begin(),h_ret.end());
	}

  inline  void UpdateHeap(HeapType & h_ret)
  {
		this->GlobalMark()++;
		VertexType *v[2];
		v[0]= this->pos.V(0);
        v[1]= this->pos.V(1);	
		v[1]->IMark() = this->GlobalMark();

		// First loop around the remaining vertex to unmark visited flags
		vcg::face::VFIterator<FaceType> vfi(v[1]);	
		
		while (!vfi.End()){
			vfi.V1()->ClearV();
			vfi.V2()->ClearV();
			++vfi;
		}

		// Second Loop 
		vfi = face::VFIterator<FaceType>(v[1]);	
		while (!vfi.End())
		{
		assert(!vfi.F()->IsD()); 
			  for (int j=0;j<3;j++) 
				{
					
					if( !(vfi.V1()->IsV()) && vfi.V1()->IsRW())
					{
						vfi.V1()->SetV();
						
						h_ret.push_back(HeapElem(new MYTYPE(EdgeType(vfi.V0(),vfi.V1()), this->GlobalMark())));
						std::push_heap(h_ret.begin(),h_ret.end());
					}
				
					if(  !(vfi.V2()->IsV()) && vfi.V2()->IsRW())
					{
						vfi.V2()->SetV();

						h_ret.push_back(HeapElem(new MYTYPE(EdgeType(vfi.V0(),vfi.V2()),this->GlobalMark())));
						std::push_heap(h_ret.begin(),h_ret.end());
					}
				}
	      ++vfi;
		} 
  }

  void Execute(TriMeshType &m)
  {
	Quadric5<double> qsum1;
	Quadric5<double> qsum2;
	double min1[5];
	double min2[5];
	vcg::TCoord2<float> tcoord0_1;
	vcg::TCoord2<float> tcoord1_1;
	vcg::TCoord2<float> tcoord0_2;
	vcg::TCoord2<float> tcoord1_2;
	vcg::TCoord2<float> *newtcoord1;
	vcg::TCoord2<float> *newtcoord2;
	QVector<QPair<vcg::TCoord2<float>,Quadric5<double> > > *qv = new QVector<QPair<vcg::TCoord2<float>,Quadric5<double> > >();
	int ncoords;
	typename TriMeshType::VertexType * v[2];
	v[0] = this->pos.V(0);
	v[1] = this->pos.V(1);

	math::Quadric<double> qsum3 = QH::Qd3(v[0]);
	qsum3 += QH::Qd3(v[1]);

	ncoords = GetTexCoords(tcoord0_1,tcoord1_1,tcoord0_2,tcoord1_2);

	ComputeMinimalsAndPriority(min1,min2,qsum1,qsum2,tcoord0_1,tcoord1_1,tcoord0_2,tcoord1_2,ncoords);			

	 CoordType newPos(min1[0],min1[1],min1[2]); /* it's the same as min2[0],min2[1],min2[2] since the geometrical
												constraint has been imposed during the re-computation of the other minimal */

		
	 int FaceDel=DoCollapse(this->pos, newPos ); // v0 is deleted and v1 take the new position

	 m.fn-=FaceDel;

	 --m.vn;
	 

	vcg::TCoord2<float> *newtcoord;
	Quadric5<double> *newq;
	
	
	newq = new Quadric5<double>;
	newtcoord = new vcg::TCoord2<float>();
	
	newtcoord->u() = (float)min1[3];
	newtcoord->v() = (float)min1[4];
	newtcoord1 = newtcoord;
	*newq = qsum1;

	qv->push_back(*(new QPair<vcg::TCoord2<float>,Quadric5<double> >(*newtcoord,*newq)));

	if(ncoords > 1)
	{
		newq = new Quadric5<double>;
		newtcoord = new vcg::TCoord2<float>();
		
		newtcoord->u() = min2[3];
		newtcoord->v() = min2[4];
		newtcoord2 = newtcoord;
		*newq = qsum2;

		qv->push_back(*(new QPair<vcg::TCoord2<float>,Quadric5<double> >(*newtcoord2,*newq)));
	}


	vcg::face::VFIterator<FaceType> vfi(v[1]);	
	while (!vfi.End())
	{
		vcg::TCoord2<float>& tcoords = vfi.F()->WT(matchVertexID(vfi.F(),v[1]));

		if(
			((tcoords.u() == tcoord0_1.u()) && (tcoords.v() == tcoord0_1.v())) ||
			((tcoords.u() == tcoord1_1.u()) && (tcoords.v() == tcoord1_1.v()))
			)
		{
			tcoords.u() = newtcoord1->u();
			tcoords.v() = newtcoord1->v();
		}
		else if(
			(ncoords > 1) && 
			(
			((tcoords.u() == tcoord0_2.u()) && (tcoords.v() == tcoord0_2.v())) ||
			((tcoords.u() == tcoord1_2.u()) && (tcoords.v() == tcoord1_2.v()))
			)
			)
		{
			tcoords.u()= newtcoord2->u();
			tcoords.v()= newtcoord2->v();
		}
		else
		{
			newq = new Quadric5<double>();
			newtcoord = new vcg::TCoord2<float>();
			
			*newtcoord = tcoords;

			if(QH::Contains(v[0],tcoords))
			{
				*newq = QH::Qd(v[0],tcoords);
				newq->Sum3(QH::Qd3(v[1]),tcoords.u(),tcoords.v());
			}
			else if(QH::Contains(v[1],tcoords))
			{
				*newq = QH::Qd(v[1],tcoords);
				newq->Sum3(QH::Qd3(v[0]),tcoords.u(),tcoords.v());
			}
			else
				assert(0);
			
			qv->push_back(*(new QPair<vcg::TCoord2<float>,Quadric5<double> >(*newtcoord,*newq)));
		}

		++vfi; 
	}
	QH::Qd3(v[1]) = qsum3;
	QH::Vd(v[1]) = *qv;
		
  }

};

class MyTriEdgeCollapseQTex: public TriEdgeCollapseQuadricTex< CMeshO, MyTriEdgeCollapseQTex, QuadricTexHelper > {
						public:
						typedef  TriEdgeCollapseQuadricTex< CMeshO,  MyTriEdgeCollapseQTex, QuadricTexHelper> TECQ;
            typedef  CMeshO::VertexType::EdgeType EdgeType;
            inline MyTriEdgeCollapseQTex(  const EdgeType &p, int i) :TECQ(p,i){}
};

void QuadricTexSimplification(CMeshO &cm,int  TargetFaceNum, float QualityThr, float extratexw, CallBackPos *cb)
{
	math::Quadric<double> QZero;
	QZero.Zero();
	QuadricTemp TD3(cm.vert);
	QuadricTexHelper::TDp3()=&TD3;

	TD3.Start(QZero);

	
	QVector <QPair<vcg::TCoord2<float>,Quadric5<double> > > qv;

     Quadric5Temp TD(cm.vert);
    QuadricTexHelper::TDp()=&TD;
    TD.Start(qv);


	  MyTriEdgeCollapseQTex::SetDefaultParams();

	  MyTriEdgeCollapseQTex::Params().QualityThr=QualityThr;
	  
	  

	MyTriEdgeCollapseQTex::Params().ExtraTCoordWeight = extratexw;





  vcg::LocalOptimization<CMeshO> DeciSession(cm);
	cb(1,"Initializing simplification");
	DeciSession.Init<MyTriEdgeCollapseQTex>();

	DeciSession.SetTargetSimplices(TargetFaceNum);
	DeciSession.SetTimeBudget(0.1f);
	int startFn=cm.fn;
  
	int faceToDel=cm.fn-TargetFaceNum;
	
	while( DeciSession.DoOptimization() && cm.fn>TargetFaceNum )
	{
    char buf[256];
    sprintf(buf,"Simplifing heap size %i ops %i\n",DeciSession.h.size(),DeciSession.nPerfmormedOps);
	   cb(100-100*(cm.fn-TargetFaceNum)/(faceToDel), buf);
	};

	DeciSession.Finalize<MyTriEdgeCollapseQTex>();
  

}