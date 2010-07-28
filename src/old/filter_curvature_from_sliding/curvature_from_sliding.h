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

#include<vcg/complex/trimesh/update/position.h>
#include<vcg/complex/trimesh/update/edges.h>
#include <vcg/complex/trimesh/closest.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/base.h>
#include <vcg/complex/trimesh/allocate.h>
#include <vcg/complex/trimesh/subset.h>
#include <vcg/space/index/grid_static_ptr.h>
#include <lib/levmar/lm.h>

#include "frame.h"

namespace vcg{
template <class MeshType>
class CurvatureFromSliding
{
typedef bool CallBackPos(const int pos, const char * str );

public:
	void Compute(MeshType & _mesh, typename MeshType::ScalarType   rad, CallBackPos *  cb = 0){

		vcg::tri::UpdateFlags<MeshType>::FaceProjection(_mesh);

		oPos = vcg::tri::Allocator<MeshType>::AddPerVertexAttribute<typename MeshType::CoordType>(patch,"oPpos");
		typename MeshType::VertexIterator vi;int i;
		mesh = &_mesh;
		vcg::Box3<typename MeshType::ScalarType> bbox = mesh->bbox;
		typename MeshType::ScalarType infl = bbox.Diag()*0.1;
		typename MeshType::CoordType bb(infl,infl,infl);
		bbox.min-=bb;
		bbox.max+=bb;

		grid.Set((*mesh).face.begin(),(*mesh).face.end(),bbox);
		for(i = 0, vi = mesh->vert.begin(); vi!= mesh->vert.end();++i, ++vi){
			v = &(*vi);
			Sample( rad);
			OnVertex();
			if(cb!=0)
				if ( i % (mesh->vert.size()/100) == 0)
					(*cb)(i / (mesh->vert.size()/100), "Computing Curvature Principal Direction from Sliding");
		}
		vcg::tri::Allocator<MeshType>::DeletePerVertexAttribute<typename MeshType::CoordType>(patch,oPos);
	}


private:
	typedef vcg::GridStaticPtr	<typename MeshType::FaceType, typename MeshType::ScalarType>		MeshGridType;
	MeshGridType grid;
	MeshType patch,*mesh;
	typename MeshType::VertexPointer v;
	typename MeshType::ScalarType ballsize;
	typename MeshType::PerVertexAttributeHandle<typename MeshType::CoordType> oPos;
 	typename MeshType::CoordType basedir ;

	static typename MeshType::FaceType * GetClosestFaceBase( MeshType & mesh,MeshGridType & gr,const typename MeshGridType::CoordType & _p, 
		const typename MeshGridType::ScalarType & _maxDist,typename MeshGridType::ScalarType & _minDist,
		typename MeshGridType::CoordType &_closestPt)
	{
		typedef typename MeshGridType::ScalarType ScalarType;
		typedef Point3<ScalarType> Point3x;
		typedef vcg::tri::FaceTmark<MeshType> MarkerFace;
		MarkerFace mf;
		mf.SetMesh(&mesh);
		vcg::face::PointDistanceBaseFunctor<ScalarType> PDistFunct;
		_minDist=_maxDist;
		return (gr.GetClosest(PDistFunct,mf,_p,_maxDist,_minDist,_closestPt));
	}


	void Sample( typename MeshType::ScalarType   rad){

		MeshType::FaceType * f =0; 
		MeshType::ScalarType dist;

		std::vector<MeshType:: FaceType*> closests;
		std::vector<MeshType::ScalarType> distances;
		std::vector<MeshType::CoordType> points;
		MeshType::ScalarType distance;
		MeshType::CoordType point;

		typedef vcg::tri::FaceTmark<MeshType> MarkerFace;
		MarkerFace mf;
		mf.SetMesh(mesh);
		typedef vcg::face::PointDistanceBaseFunctor<typename MeshType::ScalarType> FDistFunct;
		(this->grid.GetInSphere/*<FDistFunct,MarkerFace,OBJPTRCONTAINER,DISTCONTAINER,POINTCONTAINER>*/
			(FDistFunct(),mf,  this->v->P(),rad,closests,distances,points));

		 patch.Clear();
		 vcg::tri::SubSet(patch,closests);

		 // sampling points
		 vcg::Point3f uu,vv;
		 vcg::GetUV( v->N(),uu,vv);

		 std::vector<vcg::Point3f > pts; // collection of sampling points

		 int ip = 0;
		 const int nc = 2;
		 int np[3]={ 4,8 };
		 float alpha,dalpha,r;
		 alpha = 0;r = rad/nc;
		 pts.push_back( v->P());
		 for(int  ic = 0;  ic<nc ;++ic){
			 r =(rad/nc)*(ic+1);
			 alpha = 0;
			 dalpha = 2*M_PI / np[ic];
			 for(int  ip = 0; ip < np[ic];++ip){
				 vcg::Point3f ps(cos(alpha)*r,sin(alpha)*r,0.0);
				 vcg::Point3f pp =  v->P() + uu*ps[0]  + vv * ps[1];
				 GetClosestFaceBase((*mesh),grid,pp,rad,distance,point);
				 pts.push_back(point);
				 alpha+=dalpha;
			 }
		 }

		 patch.Clear();

		 vcg::tri::Allocator<MeshType>::AddVertices(patch,pts.size());
		  for(int i  = 0; i < patch.vert.size(); ++i) {
			  patch.vert[i].P() = oPos[patch.vert[i]] =  pts[i];
		  }

		 vcg::tri::UpdateBounding<MeshType>::Box(patch);
		 this->ballsize = patch.bbox.Diag();

}


	void OnVertex(){

		static int m = 0;
		static double *fvec  = NULL;
	
		const int n = 2;

		if(patch.vert.size()>m){
			if (fvec!=NULL) {free(fvec); fvec = NULL;}
			fvec  = (double*)malloc(patch.vert.size()*sizeof(double));
		}
		m =  patch.vert.size();

		if(m==0) return;

		double  x[2];
		memset(&fvec[0],0,m*sizeof(double)); 
		memset(x,0,n*sizeof(double)); 

		x[0] = x[1] = 0.0; 

		MeshType::CoordType d1,d2;
		vcg::GetUV(v->N(),basedir,d2);
		vcg::Matrix44f tmp;

		double info[9];
		/* I: opts[0-4] = minim. options [\mu, \epsilon1, \epsilon2, \epsilon3, \delta]. Respectively the
		* scale factor for initial \mu, stopping thresholds for ||J^T e||_inf, ||Dp||_2 and ||e||_2 and
		* the step used in difference approximation to the jacobian. Set to NULL for defaults to be used.
		* If \delta<0, the jacobian is approximated with central differences which are more accurate
		* (but slower!) compared to the forward differences employed by default. 
		*/
		  double opts[5] = {
			   LM_INIT_MU
			  ,LM_STOP_THRESH
			  ,LM_STOP_THRESH
			  ,LM_STOP_THRESH
			  ,-0.01//LM_DIFF_DELTA//
		  };
		  
		  dlevmar_dif(&CurvatureFromSliding::eval,x,&fvec[0],n,m,50,opts,info,NULL,NULL,this);

		tmp.SetRotateRad(x[0],v->N());
		v->PD1() = tmp * basedir;
		v->K1() =  x[1];
	 
		//printf("las err %f\n",maxerr);


		float c = x[1];

		x[0] = x[0] + M_PI * 0.5;
		tmp.SetRotateRad(x[0],v->N());
		v->PD2() = tmp * basedir;
		memset(&fvec[0],0,m*sizeof(double)); 
		
		dlevmar_dif(&CurvatureFromSliding::eval_only_k,&x[1],&fvec[0],1,m,50,NULL,info,NULL,NULL,this);

		tmp.SetRotateRad(x[0],v->N());
		
		v->K2() = x[1];

		printf("%f %f \n",v->K1(),v->K2());
		if(fabs((float)v->K1())> fabs((float)v->K2())){
			std::swap(v->PD2(),v->PD1());
			std::swap(v->K2(),v->K1());
		}

}
static void eval( double *x,double *p,  int n,int m, void *data){
// x[0] --> angle 
// x[1] --> curvature
	CurvatureFromSliding<MeshType>  * cfs  = (CurvatureFromSliding<MeshType> *)data;
	typename MeshType::VertexPointer v = cfs->v;
	MeshType &  patch = cfs->patch;
	MeshType *  mesh = cfs->mesh;
	MeshGridType & grid = cfs->grid;
	typename MeshType::CoordType basedir =cfs->basedir;
	typename MeshType::ScalarType ballsize = cfs->ballsize;
	typename MeshType::PerVertexAttributeHandle<typename MeshType::CoordType> &oPos=cfs->oPos;

	typename  MeshType::CoordType d1;
	vcg::Matrix44f rot,rt,rot90;
	rot.SetRotateRad(x[0],v->cN());
	d1 = rot * basedir;
	Frame<float>::BuildRotoTranslation(v->cP(),v->cN(),d1,x[1],  ballsize*0.1,rt);

	float tot_dist = 0.0;
			// trasform the mesh
		 for(int i  = 0; i < patch.vert.size(); ++i) 
			 patch.vert[i].P() = rt * oPos[ patch.vert[i]];

			typename MeshType::FaceType *  f = 0;
			float  dist_upper_bound = patch.bbox.Diag();
			
			float dist;
			vcg::Point3f normf,bestq,ip;
			// sampling
			for(int i = 0 ; i < patch.vert.size(); ++i){	
				f=NULL;
				if(grid.bbox.IsIn(patch.vert[i].P())){
					dist = dist_upper_bound;
				f = GetClosestFaceBase(*mesh, grid, patch.vert[i].P(), dist_upper_bound, dist,  bestq);
				}
				else f=NULL;
				if(f==NULL)
					p[i] = 0;
				else
					p[i] = dist;
				tot_dist+=p[i];
	}
 }


static void eval_only_k( double *x,double *p,  int n,int m, void *data){
// *x  --> curvature
	vcg::Matrix44f rt;
	typename MeshType::CoordType d1;
	CurvatureFromSliding<MeshType>  * cfs  = (CurvatureFromSliding<MeshType> *)data;
	typename MeshType::VertexPointer v = cfs->v;
	MeshType &  patch = cfs->patch;
	MeshType *  mesh = cfs->mesh;
	MeshGridType & grid = cfs->grid;
	typename MeshType::ScalarType ballsize = cfs->ballsize;
	typename MeshType::CoordType basedir =cfs->basedir;
	typename MeshType::PerVertexAttributeHandle<typename MeshType::CoordType> &oPos=cfs->oPos;

	Frame<float>::BuildRotoTranslation(	v->cP(),
										v->cN(),
										v->PD2(),
										*x,
										ballsize*0.1,
										rt);

	float tot_dist = 0.0;
			// trasforma la mesh
		 for(int i  = 0; i < patch.vert.size(); ++i) 
			 patch.vert[i].P() = rt * oPos[patch.vert[i]];

			typename MeshType::FaceType *  f = 0;
			float  dist_upper_bound = patch.bbox.Diag();
			
			float dist;
			vcg::Point3f normf,bestq,ip;
			// sampling
			for(int i = 0 ; i < patch.vert.size(); ++i){	
				f=NULL;
				if(grid.bbox.IsIn(patch.vert[i].P())){
				dist  = dist_upper_bound;
				f = GetClosestFaceBase(* mesh , grid, patch.vert[i].P(), dist_upper_bound, dist,  bestq);
				}
				else f=NULL;
				if(f==NULL)
					p[i] = dist_upper_bound;
				else
					p[i] = dist;
				tot_dist+=p[i];
	}

 }

};// end of class

}// end of subspace